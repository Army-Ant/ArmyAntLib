/*
 * Copyright (c) 2015 ArmyAnt
 * 版权所有 (c) 2015 ArmyAnt
 *
 * Licensed under the BSD License, Version 2.0 (the License);
 * 本软件使用BSD协议保护, 协议版本:2.0
 * you may not use this file except in compliance with the License.
 * 使用本开源代码文件的内容, 视为同意协议
 * You can read the license content in the file "LICENSE" at the root of this project
 * 您可以在本项目的根目录找到名为"LICENSE"的文件, 来阅读协议内容
 * You may also obtain a copy of the License at
 * 您也可以在此处获得协议的副本:
 *
 *     http://opensource.org/licenses/BSD-3-Clause
 *
 * Unless required by applicable law or agreed to in writing, software distributed under the License is distributed on an AS IS BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * 除非法律要求或者版权所有者书面同意,本软件在本协议基础上的发布没有任何形式的条件和担保,无论明示的或默许的.
 * See the License for the specific language governing permissions and limitations under the License.
 * 请在特定限制或语言管理权限下阅读协议
 * This file is the internal source file of this project, is not contained by the closed source release part of this software
 * 本文件为内部源码文件, 不会包含在闭源发布的本软件中
 */

#include "../base/base.hpp"
#include "../../inc/AAString.h"
#include "../../inc/AASocket.h"
#include "../../inc/AAClassPrivateHandle.hpp"

#include <map>
#include <queue>
#include <thread>
#include <mutex>
#include <memory>
#include <boost/asio.hpp>
#include <boost/beast.hpp>

#ifdef OS_WINDOWS
#include <WinSock2.h>
#include <in6addr.h>
#include <ws2tcpip.h>
#else
#include <list>
#include <netinet/in.h>
#endif


#define AA_HANDLE_MANAGER ClassPrivateHandleManager<Socket, Socket_Private>::getInstance()

#define AA_HANDLE_MANAGER_WEBSOCKET ClassPrivateHandleManager<Socket, WebSocket_Private>::getInstance()


namespace ArmyAnt{

/**************** Source file private functions **************************/

// 将socket的ipv4地址转换为ArmyAnt::IPAddr_v4
inline static IPAddr_v4 toAAAddr(in_addr addr){
#ifdef OS_WINDOWS
	return IPAddr_v4(addr.S_un.S_addr);
#else
	return IPAddr_v4(addr.s_addr);
#endif
}

// 将socket的ipv6地址转换为ArmyAnt::IPAddr_v6
inline static IPAddr_v6 toAAAddr(in6_addr addr){
#if defined OS_WINDOWS
	return IPAddr_v6(addr.u.Word);
#elif defined OS_BSD
	return IPAddr_v6(addr.__u6_addr.__u6_addr16);
#else
	return IPAddr_v6(addr.__in6_u.__u6_addr16);
#endif
}

// 将socket的地址信息(不确定版本)转换为ArmyAnt::IPAddr(通用)
inline static IPAddr& toAAAddr(addrinfo addr){
	if(addr.ai_family == AF_INET6){
		static auto ret = toAAAddr(*reinterpret_cast<in6_addr*>(&addr));
		return ret;
	} else{
		static auto ret = toAAAddr(*reinterpret_cast<in_addr*>(&addr));
		return ret;
	}
}

// 将ArmyAnt::IPAddr_v4转换成socket的ipv4地址结构
inline static in_addr toCAddr(const IPAddr_v4&addr){
	in_addr ret;
#ifdef OS_WINDOWS
	ret.S_un.S_addr = addr.getNum();
#else
	ret.s_addr = addr.getNum();
#endif
	return ret;
}

// 将ArmyAnt::IPAddr_v6转换成socket的ipv6地址结构
inline static in6_addr toCAddr(const IPAddr_v6&addr){
	in6_addr ret;
	for(uint8 i = 0; i < 8; i++){
#if defined OS_WINDOWS
		ret.u.Word[i] = addr.getWord(i);
#elif defined OS_BSD
		ret.__u6_addr.__u6_addr16[i] = addr.getWord(i);
#else
		ret.__in6_u.__u6_addr16[i] = addr.getWord(i);
#endif
	}
	return ret;
}

// 将ArmyAnt::IPAddr(通用)转换成socket的ip不确定版本的地址结构
inline static addrinfo toCAddr(const IPAddr& addr){
	addrinfo ret;
	if(addr.getIPVer() == 4){
		in_addr a = toCAddr(static_cast<const IPAddr_v4>(addr));
		ret = *reinterpret_cast<addrinfo*>(&a);
	} else{
		in6_addr a = toCAddr(static_cast<const IPAddr_v6>(addr));
		ret = *reinterpret_cast<addrinfo *>(&a);
	}
	return ret;
}

// 将boost的ip地址,转换成ArmyAnt::IPAddr
inline static IPAddr& toAAAddr(boost::asio::ip::address addr){
	if(addr.is_v4()){
		static auto ret = IPAddr_v4(addr.to_v4().to_ulong());
		return ret;
	} else{
		auto bts = addr.to_v6().to_bytes();
		uint8 ip[16];
		for(int i = 0; i < 16; i++)
			ip[i] = bts[i];
		static auto ret = IPAddr_v6(ip);
		return ret;
	}
}

// 将ArmyAnt::IPAddr转换成boost的ip地址
inline static boost::asio::ip::address toBoostAddr(const IPAddr& addr){
	if(addr.getIPVer() == 4)
		return boost::asio::ip::address_v4(static_cast<IPAddr_v4>(addr).getNum());
	std::array<uint8, 16>wds;
	for(uint8 i = 0; i < wds.size(); i++){
		wds[i] = static_cast<IPAddr_v6>(addr).getByte(i);
	}
	return boost::asio::ip::address_v6(wds);
}

// 判断本机内存是否为大端, 这将决定IP地址结构的存储和发送方式
inline static bool getLocalMachineIsBigEnding(){
	union{
		char c;
		uint16 s = 1;
	} u;
	return u.c != 1;
}

/************* Source for class IPAddr and its derived classes ************/


IPAddr::IPAddr(){}

IPAddr::IPAddr(const IPAddr &){}

IPAddr & IPAddr::operator=(const char * ipStr){
	parseFromString(ipStr);
	return *this;
}

IPAddr & IPAddr::operator=(const IPAddr &){
	return *this;
}

IPAddr::~IPAddr(){}

bool IPAddr::operator==(const char * value)const{
	return String(value) == getStr();
}

bool IPAddr::operator!=(const char * value)const{
	return !operator==(value);
}

bool IPAddr::operator==(const IPAddr & value) const{
	if(getIPVer() == 4)
		return static_cast<IPAddr_v4>(*this) == value;
	return  static_cast<IPAddr_v6>(*this) == value;
}

bool IPAddr::operator!=(const IPAddr & value) const{
	return !operator==(value);
}

IPAddr::operator IPAddr_v4(){
	return *static_cast<IPAddr_v4*>(this);
}

IPAddr::operator const IPAddr_v4() const{
	return *static_cast<const IPAddr_v4*>(this);
}

IPAddr::operator IPAddr_v6(){
	return *static_cast<IPAddr_v6*>(this);
}

IPAddr::operator const IPAddr_v6() const{
	return *static_cast<const IPAddr_v6*>(this);
}

IPAddr * IPAddr::create(const char * ipStr){
	try{
		return static_cast<IPAddr*>(new IPAddr_v4(ipStr));
	} catch(std::invalid_argument){
		return static_cast<IPAddr*>(new IPAddr_v6(ipStr));
	}
}

IPAddr * IPAddr::clone(const IPAddr & value){
	{
		if(value.getIPVer() == 4)
			return static_cast<IPAddr*>(new IPAddr_v4(value));
		return static_cast<IPAddr*>(new IPAddr_v6(value));
	}
}



IPAddr_v4::IPAddr_v4(const char * ipStr)
	: IPAddr(){
	parseFromString(ipStr);
}

IPAddr_v4::IPAddr_v4(uint32 ipNum)
	: IPAddr(){
	uint8* chars = reinterpret_cast<uint8*>(&ipNum);
	if(getLocalMachineIsBigEnding()){
		net = chars[0];
		host = chars[1];
		lh = chars[2];
		impNum = chars[3];
	} else{
		net = chars[3];
		host = chars[2];
		lh = chars[1];
		impNum = chars[0];
	}
}

IPAddr_v4::IPAddr_v4(uint8 net, uint8 host, uint8 lh, uint8 impNum)
	: IPAddr(){
	this->net = net;
	this->host = host;
	this->lh = lh;
	this->impNum = impNum;
}

IPAddr_v4::IPAddr_v4(const IPAddr_v4 & value)
	: IPAddr(), net(value.net), host(value.host), lh(value.lh), impNum(value.impNum){}

IPAddr_v4 & IPAddr_v4::operator=(const char * ipStr){
	IPAddr::operator=(ipStr);
	return*this;
}

IPAddr_v4 & IPAddr_v4::operator=(uint32 ipNum){
	uint8* chars = reinterpret_cast<uint8*>(&ipNum);
	if(getLocalMachineIsBigEnding()){
		net = chars[0];
		host = chars[1];
		lh = chars[2];
		impNum = chars[3];
	} else{
		net = chars[3];
		host = chars[2];
		lh = chars[1];
		impNum = chars[0];
	}
	return*this;
}

IPAddr_v4 & IPAddr_v4::operator=(const IPAddr_v4 & value){
	IPAddr::operator=(value);
	net = value.net;
	host = value.host;
	lh = value.lh;
	impNum = value.impNum;
	return*this;
}

IPAddr_v4::~IPAddr_v4(){}

const char * IPAddr_v4::getStr() const{
	static char ret[32];
	memset(ret, 0, 32);
	sprintf(ret, "%d.%d.%d.%d", net, host, lh, impNum);
	return ret;
}

uint8 IPAddr_v4::getIPVer() const{
	return uint8(4);
}

uint32 IPAddr_v4::getNum() const{
	uint32 ret = 0;
	if(getLocalMachineIsBigEnding()){
		ret = *reinterpret_cast<const uint32*>(this);
	} else{
		uint8 chars[4] = {impNum, lh, host, net};
		ret = *reinterpret_cast<const uint32*>(chars);
	}
	return ret;
}

uint8 IPAddr_v4::getHost() const{
	return host;
}

bool IPAddr_v4::setHost(uint8 _host){
	host = _host;
	return true;
}

uint8 IPAddr_v4::getNet() const{
	return net;
}

bool IPAddr_v4::setNet(uint8 _net){
	net = _net;
	return true;
}

uint8 IPAddr_v4::getImpNumber() const{
	return impNum;
}

bool IPAddr_v4::setImpNumber(uint8 _impNum){
	impNum = _impNum;
	return true;
}

uint8 IPAddr_v4::getLH() const{
	return lh;
}

bool IPAddr_v4::setLH(uint8 _lh){
	lh = _lh;
	return true;
}

bool IPAddr_v4::operator==(const char * value)const{
	return IPAddr::operator==(value);
}

bool IPAddr_v4::operator!=(const char * value)const{
	return IPAddr::operator!=(value);
}

bool IPAddr_v4::operator==(uint32 ipNum)const{
	return getNum() == ipNum;
}

bool IPAddr_v4::operator!=(uint32 ipNum)const{
	return !operator==(ipNum);
}

bool IPAddr_v4::operator==(const IPAddr_v4 & value)const{
	return getNum() == value.getNum();
}

bool IPAddr_v4::operator!=(const IPAddr_v4 & value)const{
	return !operator==(value);
}

IPAddr_v4&IPAddr_v4::localhost(){
	static IPAddr_v4 ret((unsigned char)127, (unsigned char)0, (unsigned char)0, (unsigned char)1);
	return ret;
}

void IPAddr_v4::parseFromString(const char * str){
	//若为空字符串,则将地址复位
	if(str == nullptr || strlen(str) == 0){
		net = uint8(0);
		host = uint8(0);
		lh = uint8(0);
		impNum = uint8(0);
	} else{
		// 分解ip地址字符串格式, 分解失败则抛出异常
		int _host, _net, _impNum, _lh;
		if(sscanf(str, "%d.%d.%d.%d", &_net, &_host, &_lh, &_impNum) < 4)
			throw std::invalid_argument("Wrong IPv4 address string format!");
		if(_host > 255 || _host < 0 || _net>255 || _net < 0 || _impNum>255 || _impNum < 0 || _lh>255 || _lh < 0)
			throw std::invalid_argument("Wrong IPv4 address string number (one or more number is larger than 255 or less than 0)!");
		net = uint8(_net);
		host = uint8(_host);
		lh = uint8(_lh);
		impNum = uint8(_impNum);
	}
}

IPAddr_v6::IPAddr_v6(std::nullptr_t)
	:IPAddr(){}

IPAddr_v6::IPAddr_v6(const char * ipStr)
	: IPAddr(){
	parseFromString(ipStr);
}

IPAddr_v6::IPAddr_v6(uint16 words[8])
	: IPAddr(){
	if(words == nullptr)
		memset(bytes, 0, 16);
	else
		for(int i = 0; i < 8; i++){
			char* chars = reinterpret_cast<char*>(words + i);
			if(getLocalMachineIsBigEnding()){
				bytes[i * 2] = chars[0];
				bytes[i * 2 + 1] = chars[1];
			} else{
				bytes[i * 2] = chars[1];
				bytes[i * 2 + 1] = chars[0];
			}
		}
}

IPAddr_v6::IPAddr_v6(uint8 bytes[16])
	:IPAddr(){
	if(bytes == nullptr)
		memset(bytes, 0, 16);
	else
		for(int i = 0; i < 16; i++){
			this->bytes[i] = bytes[i];
		}
}

IPAddr_v6::IPAddr_v6(const IPAddr_v6 & value)
	:IPAddr(){
	for(int i = 0; i < 16; i++){
		bytes[i] = value.bytes[i];
	}
}

IPAddr_v6 & IPAddr_v6::operator=(const char * ipStr){
	IPAddr::operator=(ipStr);
	return*this;
}

IPAddr_v6 & IPAddr_v6::operator=(const IPAddr_v6 & value){
	IPAddr::operator=(value);
	for(int i = 0; i < 16; i++){
		bytes[i] = value.bytes[i];
	}
	return*this;
}

IPAddr_v6::~IPAddr_v6(){}

const char * IPAddr_v6::getStr() const{
	static char ret[128];
	memset(ret, 0, 32);
	sprintf(ret, "%d:%d:%d:%d:%d:%d:%d:%d", getWord(0), getWord(1), getWord(2), getWord(3), getWord(4), getWord(5), getWord(6), getWord(7));
	return ret;
}

uint8 IPAddr_v6::getIPVer() const{
	return uint8(6);
}

uint8 IPAddr_v6::getByte(uint8 index) const{
	AAAssert(index < 16, uint8(0));
	return bytes[index];
}

bool IPAddr_v6::setByte(uint8 index, uint8 value){
	AAAssert(index < 16, false);
	bytes[index] = value;
	return true;
}

uint16 IPAddr_v6::getWord(uint8 index) const{
	AAAssert(index < 8, uint16(0));
	uint16 ret = 0;
	if(getLocalMachineIsBigEnding()){
		ret = *reinterpret_cast<const uint16*>(bytes + index * 2);
	} else{
		uint8 chars[2] = {bytes[index * 2 + 1], bytes[index * 2]};
		ret = *reinterpret_cast<const uint16*>(chars);
	}
	return ret;
}

bool IPAddr_v6::setWord(uint8 index, uint16 value){
	AAAssert(index < 8, false);
	uint8*chars = reinterpret_cast<uint8*>(&value);
	if(getLocalMachineIsBigEnding()){
		bytes[index * 2] = chars[0];
		bytes[index * 2 + 1] = chars[1];
	} else{
		bytes[index * 2] = chars[1];
		bytes[index * 2 + 1] = chars[0];
	}
	return true;
}

bool IPAddr_v6::operator==(const char * value) const{
	return IPAddr::operator==(value);
}

bool IPAddr_v6::operator!=(const char * value) const{
	return IPAddr::operator!=(value);
}

bool IPAddr_v6::operator==(const IPAddr_v6 & value) const{
	for(int i = 0; i < 16; i++){
		if(bytes[i] != value.bytes[i])
			return false;
	}
	return true;
}

bool IPAddr_v6::operator!=(const IPAddr_v6 & value) const{
	return !operator==(value);
}

uint16 IPAddr_v6::operator[](int index)const{
	AAAssert(index < 8 && index >= 0, getWord(index));
	return getWord(uint8(index));
}

IPAddr_v6&IPAddr_v6::localhost(){
	static IPAddr_v6 ret("0:0:0:0:0:0:0:1");
	return ret;
}

void IPAddr_v6::parseFromString(const char * str){
	//若为空字符串,则将地址复位
	if(str == nullptr || strlen(str) == 0){
		memset(bytes, 0, 16);
	} else{
		// 分解ip地址字符串格式, 分解失败则抛出异常
		// TODO: 目前仅支持标准结构的ipv6地址字符串, 考虑添加简化方式的地址字符串解析
		int words[8] = {0};
		if(sscanf(str, "%d:%d:%d:%d:%d:%d:%d:%d", words, words + 1, words + 2, words + 3, words + 4, words + 5, words + 6, words + 7) < 8)
			throw std::invalid_argument("Wrong IPv6 address string format!");
		for(int i = 0; i < 8; i++)
			if(words[i] > 65535 || words[i] < 0)
				throw std::invalid_argument("Wrong IPv6 address string number (one or more number is larger than 65535 or less than 0)!");
		for(int i = 0; i < 8; i++)
			setWord(i, uint16(words[i]));
	}
}


SocketException::SocketException(ErrorType type, const char * message, int code)
	: type(type), message(message), code(code){}

SocketException::SocketException(const SocketException & value)
	: type(value.type), message(value.message), code(value.code){}

SocketException::SocketException(SocketException && moved)
	: type(moved.type), message(moved.message), code(moved.code){}

/***************** Defination for private data structs ********************/

// 代表一个TCP连接的socket套接字数据
struct TCP_Socket_Datas{
	TCP_Socket_Datas();
	TCP_Socket_Datas(std::shared_ptr <boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> webs, const IPAddr* addr, uint16 port, const IPAddr* localAddr, uint16 localport);
	TCP_Socket_Datas(std::shared_ptr<boost::asio::ip::tcp::socket> s, const IPAddr* addr, uint16 port, const IPAddr* localAddr, uint16 localport);
	virtual ~TCP_Socket_Datas();

	void setSocket(std::nullptr_t);
	void setSocket(boost::asio::ip::tcp::socket*socket);
	void setSocket(boost::beast::websocket::stream<boost::asio::ip::tcp::socket>* unsharedSocket);
	boost::asio::ip::tcp::socket* getSocket()const;
	bool isShared()const;
	std::shared_ptr < boost::asio::ip::tcp::socket> getSharedSocket()const;
	std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> getWebSocket()const;
	void closeSocket(bool noReset = false);

	IPAddr* addr = nullptr;			// 对方ip地址
	uint16 port = 0;				// 对方端口
	IPAddr* localAddr = nullptr;	// 我方使用的ip地址
	uint16 localport = 0;			// 我方使用的端口
	boost::asio::strand<boost::asio::io_context::executor_type>* strand;

private:
	std::shared_ptr<boost::asio::ip::tcp::socket> s;	//boost的socket连接对象
	std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> webs;	//boost的websocket连接对象

	AA_FORBID_COPY_CTOR(TCP_Socket_Datas);
	AA_FORBID_ASSGN_OPR(TCP_Socket_Datas);
};

// Socket类的私有数据
struct Socket_Private{
	Socket_Private();
	virtual ~Socket_Private();

	void onTCPSendingResponse(uint32 index, std::shared_ptr<uint8> buffer, boost::asio::ip::tcp::socket*s, boost::system::error_code err, std::size_t size, uint64 realSize);

	uint32 maxBufferLen = 65530;		// 接收数据的buffer的最大长度
	boost::asio::io_service localService;
	std::shared_ptr<std::thread> localServiceThread = nullptr;

	Socket::SendingResp asyncResp = nullptr;
	void* asyncRespUserData = nullptr;
	uint32 asyncRespTimes = 0;

	struct ErrorInfo{
		SocketException err;
		const IPAddr* addr;
		uint16 port;
		String functionName;
		ErrorInfo(SocketException err, const IPAddr&addr, uint16 port, String functionName)
			:err(err), addr(IPAddr::create(addr.getStr())), port(port), functionName(functionName){}
		ErrorInfo(const ErrorInfo& value)
			:err(value.err), addr(IPAddr::create(value.addr->getStr())), port(value.port), functionName(value.functionName){}
		ErrorInfo(ErrorInfo&& temp)
			:err(temp.err), addr(temp.addr), port(temp.port), functionName(temp.functionName){
			temp.addr = nullptr;
		}
		~ErrorInfo(){ AA_SAFE_DEL(addr); }
	};
	std::mutex errorThreadMutex;
	std::queue<ErrorInfo> errorInfos;
	bool isErrorThreadRunning = false;
	std::thread* errorReportThread = nullptr;
	Socket::ErrorInfoCall errorReportCallBack = nullptr;
	void* errorReportUserData = nullptr;
	static void errorThreading(Socket_Private*self);
	void startErrorReportThread();
	void endErrorReportThread();
	void reportError(SocketException err, IPAddr&addr, uint16 port, String functionName);

	bool isAsync = false;				// 是否异步监听, 对于服务器, 总是true
	bool isListening = false;			// 是否正在监听

	AA_FORBID_COPY_CTOR(Socket_Private);
	AA_FORBID_ASSGN_OPR(Socket_Private);
};

// TCPServer类的私有数据
struct TCPServer_Private : public Socket_Private{
	TCPServer_Private(int32 maxClientNum) :Socket_Private(), maxClientNum(maxClientNum), acceptor(localService){};
	virtual ~TCPServer_Private();

	bool start(uint16 port, bool ipv6);
	bool startWeb(uint16 port, bool ipv6);
	bool stop(uint32 waitTime);

	void onConnectShared(std::shared_ptr<boost::asio::ip::tcp::socket> s, boost::system::error_code err);
	void onConnectUnshared(std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> s, boost::system::error_code err);
	void onReceivedShared(std::shared_ptr<boost::asio::ip::tcp::socket> s, uint32 index, boost::system::error_code err, std::size_t size, std::shared_ptr<uint8>);
	void onReceivedUnshared(std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> s, uint32 index, boost::system::error_code err, std::size_t size, std::shared_ptr<boost::beast::multi_buffer> buffer);

	bool givenUpClient(uint32 index);
	bool givenUpClient(const IPAddr& addr, uint16 port);
	bool givenUpAllClients();

	int getIndexByAddrPort(const IPAddr&clientAddr, uint16 port);

	boost::asio::ip::tcp::acceptor acceptor;		// 连接接受对象

	uint16 serverPort = 0;
	int32 maxClientNum = 0;

	Socket::ServerConnectCall connectCallBack = nullptr;	// 收到连接时的回调
	void* connetcCallData = nullptr;
	Socket::ServerLostCall lostCallBack = nullptr;
	void* lostCallData = nullptr;
	Socket::ServerGettingCall gettingCallBack = nullptr;	// 接受信息时的回调
	void* gettingCallData = nullptr;	// 接受信息时的回调要传递的额外数据

	std::map<uint32, TCP_Socket_Datas*> clients;
	std::mutex clientMutex;

private:

	AA_FORBID_COPY_CTOR(TCPServer_Private);
	AA_FORBID_ASSGN_OPR(TCPServer_Private);
};

struct TCPClient_Private : public Socket_Private, public TCP_Socket_Datas{
	TCPClient_Private() :Socket_Private(), TCP_Socket_Datas(){};
	virtual ~TCPClient_Private();

	bool connectServer(uint16 port, bool isAsync, TCPClient::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::asio::ip::tcp::socket* socket);
	bool connectServer(uint16 port, bool isAsync, TCPClient::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::beast::websocket::stream<boost::asio::ip::tcp::socket>* socket);
	bool disconnectServer(uint32 waitTime);

	void onAsyncConnect(bool needHandshake, Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err);
	void onConnect(Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err);
	void onReceivedShared(Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err, std::size_t size, std::shared_ptr<uint8> buffer);
	void onReceivedUnshared(Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err, std::size_t size, std::shared_ptr<boost::beast::multi_buffer> buffer);

	Socket::ClientLostCall lostCallBack = nullptr;
	void* lostCallData = nullptr;
	Socket::ClientGettingCall gettingCallBack = nullptr;	// 接受信息时的回调
	void* gettingCallData = nullptr;	// 接受信息时的回调要传递的额外数据

private:
	bool connectServer(uint16 port, bool isAsync, bool needHandshake, TCPClient::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData);

	AA_FORBID_COPY_CTOR(TCPClient_Private);
	AA_FORBID_ASSGN_OPR(TCPClient_Private);
};

struct UDPSingle_Private : public Socket_Private{
	UDPSingle_Private();

	boost::asio::ip::udp::socket* s = nullptr;	//boost的socket连接对象
	IPAddr* localAddr = nullptr;
	uint16 localPort = 0;
	boost::asio::ip::udp::endpoint recver;
	std::list<boost::asio::mutable_buffer> recvbuffer;		//接受信息的buffer
	std::list<boost::asio::mutable_buffer> sendBuffer;	//发送信息的buffer

	Socket::UDPGettingCall gettingCallBack = nullptr;	// 接受信息时的回调
	void* gettingCallData = nullptr;	// 接受信息时的回调要传递的额外数据

	AA_FORBID_COPY_CTOR(UDPSingle_Private);
	AA_FORBID_ASSGN_OPR(UDPSingle_Private);
};

struct WebSocket_Private{
	WebSocket_Private(boost::asio::io_service&localService);
	WebSocket_Private(boost::asio::ip::tcp::acceptor&acceptor);

	std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> stream;
};


/******************* Source for private data structs **********************/


TCP_Socket_Datas::TCP_Socket_Datas() :webs(nullptr), s(nullptr), addr(nullptr), port(0), localAddr(nullptr), localport(0), strand(nullptr){}

TCP_Socket_Datas::TCP_Socket_Datas(std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> webs, const IPAddr * addr, uint16 port, const IPAddr * localAddr, uint16 localport)
	:webs(webs), s(nullptr), addr(IPAddr::clone(*addr)), port(port), localAddr(IPAddr::clone(*localAddr)), localport(localport), strand(new boost::asio::strand<boost::asio::io_context::executor_type>(webs->get_executor())){}

TCP_Socket_Datas::TCP_Socket_Datas(std::shared_ptr<boost::asio::ip::tcp::socket> s, const IPAddr * addr, uint16 port, const IPAddr* localAddr, uint16 localport)
	: webs(nullptr), s(s), addr(IPAddr::clone(*addr)), port(port), localAddr(IPAddr::clone(*localAddr)), localport(localport), strand(nullptr){}

TCP_Socket_Datas::~TCP_Socket_Datas(){
	if(s != nullptr){
		boost::system::error_code err;
		if(s->is_open()){
			s->shutdown(s->shutdown_both, err);
		}
		if(!err)
			s->cancel(err);
		if(!err){
			s->close(err);
		}
	}
	AA_SAFE_DEL(addr);
	AA_SAFE_DEL(localAddr);
	AA_SAFE_DEL(strand);
}

void TCP_Socket_Datas::setSocket(std::nullptr_t){
	s.reset();
	webs.reset();
}

void TCP_Socket_Datas::setSocket(boost::asio::ip::tcp::socket * socket){
	if(s.get() != socket)
		s.reset(socket);
}

void TCP_Socket_Datas::setSocket(boost::beast::websocket::stream<boost::asio::ip::tcp::socket>* websocket){
	if(webs.get() != websocket)
		webs.reset(websocket);
}

boost::asio::ip::tcp::socket * TCP_Socket_Datas::getSocket() const{
	if(webs == nullptr)
		return s.get();
	return &webs->next_layer();
}

bool TCP_Socket_Datas::isShared() const{
	return webs == nullptr;
}

std::shared_ptr<boost::asio::ip::tcp::socket> TCP_Socket_Datas::getSharedSocket() const{
	return s;
}

std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> TCP_Socket_Datas::getWebSocket() const{
	return webs;
}

void TCP_Socket_Datas::closeSocket(bool noReset){
	boost::system::error_code err;
	if(s != nullptr && s->is_open())
		s->close(err);
	if(webs != nullptr && webs->is_open())
		webs->close(boost::beast::websocket::close_reason("Kicked out"), err);
	if(!noReset){
		s.reset();
		webs.reset();
	}
}

void Socket_Private::onTCPSendingResponse(uint32 index, std::shared_ptr<uint8>buffer, boost::asio::ip::tcp::socket*s, boost::system::error_code err, std::size_t size, uint64 realSize){
	if(asyncResp != nullptr && asyncResp(size, asyncRespTimes++, index, buffer.get(), realSize, asyncRespUserData) && err)
		s->async_write_some(boost::asio::buffer(buffer.get(), realSize), std::bind(&Socket_Private::onTCPSendingResponse, this, index, buffer, s, std::placeholders::_1, std::placeholders::_2, realSize));
}

Socket_Private::Socket_Private() :localService(){
	startErrorReportThread();
}
Socket_Private::~Socket_Private(){
	endErrorReportThread();
}

void Socket_Private::errorThreading(Socket_Private*self){
	while(self->isErrorThreadRunning){
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
		self->errorThreadMutex.lock();
		while(!self->errorInfos.empty()){
			auto errorInfo = self->errorInfos.front();
			if(self->errorReportCallBack != nullptr)
				self->errorReportCallBack(errorInfo.err, *errorInfo.addr, errorInfo.port, errorInfo.functionName, self->errorReportUserData);
			self->errorInfos.pop();
		}
		self->errorThreadMutex.unlock();
	}
}

void Socket_Private::startErrorReportThread(){
	isErrorThreadRunning = true;
	errorReportThread = new std::thread(std::bind(errorThreading, this));
}

void Socket_Private::endErrorReportThread(){
	isErrorThreadRunning = false;
	errorReportThread->join();
	AA_SAFE_DEL(errorReportThread);
	errorThreadMutex.lock();
	while(!errorInfos.empty())
		errorInfos.pop();
	errorThreadMutex.unlock();
}

void Socket_Private::reportError(SocketException err, IPAddr&addr, uint16 port, String functionName){
	if(errorReportCallBack != nullptr && isErrorThreadRunning){
		errorThreadMutex.lock();
		errorInfos.push(ErrorInfo(err, addr, port, functionName));
		errorThreadMutex.unlock();
	} else{
		throw err;
	}
}

TCPServer_Private::~TCPServer_Private(){}

bool TCPServer_Private::start(uint16 port, bool ipv6){
	std::shared_ptr<IPAddr> ip = nullptr;
	if(ipv6){
		std::shared_ptr<IPAddr> ip6(new IPAddr_v6(nullptr));
		ip = ip6;
	} else{
		std::shared_ptr<IPAddr> ip4(new IPAddr_v4(0, 0, 0, 0));
		ip = ip4;
	}

	if(isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The server has started");
		reportError(ex, *ip, port, "StartServer");
		return false;
	}

	// 服务器总是异步
	isAsync = true;
	serverPort = port;
	// 打开连接接收器
	auto protocol = ipv6 ? boost::asio::ip::tcp::v6() : boost::asio::ip::tcp::v4();
	try{
		auto endP = boost::asio::ip::tcp::endpoint(protocol, port);
		acceptor.open(protocol);
		acceptor.bind(endP);
		// 开始监听
		acceptor.listen(maxClientNum);
		// 连接套接字
		std::shared_ptr<boost::asio::ip::tcp::socket> s(new boost::asio::ip::tcp::socket(localService));
		acceptor.async_accept(*s, std::bind(&TCPServer_Private::onConnectShared, this, s, std::placeholders::_1));
		localServiceThread = std::shared_ptr<std::thread>(new std::thread([this, ip, port](){
			boost::system::error_code err;
			localService.reset();
			localService.run(err);
			auto code = err.value();
			auto message = err.message();
			SocketException ex(SocketException::ErrorType::SystemError, message.c_str(), code);
			reportError(ex, *ip, port, "StartServer localServiceThread");
		}));
	} catch(boost::system::system_error e){
		auto code = e.code().value();
		auto message = e.code().message();
		SocketException ex(SocketException::ErrorType::SystemError, message.c_str(), code);
		reportError(ex, *ip, port, "StartServer");
		return false;
	}

	isListening = true;
	return true;
}

bool ArmyAnt::TCPServer_Private::startWeb(uint16 port, bool ipv6){
	std::shared_ptr<IPAddr> ip = nullptr;
	if(ipv6){
		std::shared_ptr<IPAddr> ip6(new IPAddr_v6(nullptr));
		ip = ip6;
	} else{
		std::shared_ptr<IPAddr> ip4(new IPAddr_v4(0, 0, 0, 0));
		ip = ip4;
	}

	if(isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The server has started");
		reportError(ex, *ip, port, "StartServer");
		return false;
	}

	// 服务器总是异步
	isAsync = true;
	serverPort = port;
	// 打开连接接收器
	auto protocol = ipv6 ? boost::asio::ip::tcp::v6() : boost::asio::ip::tcp::v4();
	try{
		auto endP = boost::asio::ip::tcp::endpoint(protocol, port);
		acceptor.open(protocol);
		acceptor.bind(endP);
		// 开始监听
		acceptor.listen(maxClientNum);
		// 连接套接字
		std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> s(new boost::beast::websocket::stream<boost::asio::ip::tcp::socket>{localService});
		acceptor.async_accept(s->next_layer(), std::bind(&TCPServer_Private::onConnectUnshared, this, s, std::placeholders::_1));
		localServiceThread = std::shared_ptr<std::thread>(new std::thread([this, ip, port](){
			boost::system::error_code err;
			localService.reset();
			localService.run(err);
			auto code = err.value();
			auto message = err.message();
			SocketException ex(SocketException::ErrorType::SystemError, message.c_str(), code);
			reportError(ex, *ip, port, "StartServer localServiceThread");
		}));
	} catch(boost::system::system_error e){
		auto code = e.code().value();
		auto message = e.code().message();
		SocketException ex(SocketException::ErrorType::SystemError, message.c_str(), code);
		reportError(ex, *ip, port, "StartServer");
		return false;
	}

	isListening = true;
	return true;
}

bool TCPServer_Private::stop(uint32 waitTime){
	isListening = false;
	localService.stop();
	if(localServiceThread != nullptr)
		localServiceThread->join();
	if(acceptor.is_open()){
		acceptor.cancel();
		if(!givenUpAllClients()){
			auto ip4 = IPAddr_v4(0, 0, 0, 0);
			auto ip6 = IPAddr_v6(nullptr);
			IPAddr*ip = &ip6;
			SocketException ex(SocketException::ErrorType::SocketStatueError, "GivenUpAllClients failed");
			reportError(ex, *ip, serverPort, "StopServer");
			return false;
		}
		acceptor.close();
	}
	serverPort = 0;
	return true;
}

void TCPServer_Private::onConnectShared(std::shared_ptr<boost::asio::ip::tcp::socket> s, boost::system::error_code err){
	std::shared_ptr<boost::asio::ip::tcp::socket> news(new boost::asio::ip::tcp::socket(localService));
	acceptor.async_accept(*news, std::bind(&TCPServer_Private::onConnectShared, this, news, std::placeholders::_1));
	if(!err){
		uint32 index = 0;
		clientMutex.lock();
		for(uint32 i = 0; i < clients.size() + 1; i++)
			if(clients.find(i) == clients.end()){
				clients.insert(std::pair<uint32, TCP_Socket_Datas*>(i, new TCP_Socket_Datas(s, &toAAAddr(s->remote_endpoint().address()), s->remote_endpoint().port(), &toAAAddr(s->local_endpoint().address()), s->local_endpoint().port())));
				index = i;
				break;
			}
		auto cl = clients.find(index)->second;
		if(connectCallBack == nullptr || connectCallBack(index, connetcCallData)){
			auto buffer = std::shared_ptr<uint8>(new uint8[maxBufferLen]);
			s->async_read_some(boost::asio::buffer(buffer.get(), maxBufferLen), std::bind(&TCPServer_Private::onReceivedShared, this, s, index, std::placeholders::_1, std::placeholders::_2, buffer));
		} else{
			givenUpClient(index);
		}
		clientMutex.unlock();
	}
}

void TCPServer_Private::onConnectUnshared(std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> s, boost::system::error_code err){
	std::shared_ptr<boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> news(new boost::beast::websocket::stream<boost::asio::ip::tcp::socket>{localService});
	acceptor.async_accept(news->next_layer(), std::bind(&TCPServer_Private::onConnectUnshared, this, news, std::placeholders::_1));
	if(err)
		auto msg = err.message();
	else{
		s->async_accept([this, s](boost::system::error_code err){
			if(err)
				auto msg = err.message();
			else{
				clientMutex.lock();
				uint32 index = 0;
				auto clientData = new TCP_Socket_Datas(s, &toAAAddr(s->next_layer().remote_endpoint().address()), s->next_layer().remote_endpoint().port(), &toAAAddr(s->next_layer().local_endpoint().address()), s->next_layer().local_endpoint().port());
				for(uint32 i = 0; i < clients.size() + 1; i++)
					if(clients.find(i) == clients.end()){
						clients.insert(std::pair<uint32, TCP_Socket_Datas*>(i, clientData));
						index = i;
						break;
					}
				auto cl = clients.find(index)->second;
				if(connectCallBack == nullptr || connectCallBack(index, connetcCallData)){
					auto buffer = std::shared_ptr<boost::beast::multi_buffer>(new boost::beast::multi_buffer(maxBufferLen));
					s->async_read(*buffer, boost::asio::bind_executor(*clientData->strand, std::bind(&TCPServer_Private::onReceivedUnshared, this, s, index, std::placeholders::_1, std::placeholders::_2, buffer)));
				} else{
					givenUpClient(index);
				}
				clientMutex.unlock();
			}
		});
	}
}

void TCPServer_Private::onReceivedShared(std::shared_ptr<boost::asio::ip::tcp::socket> s, uint32 index, boost::system::error_code err, std::size_t size, std::shared_ptr<uint8> buffer){
	clientMutex.lock();
	auto client = clients.find(index)->second;
	clientMutex.unlock();

	if(!err){
		if(size > 0){
			gettingCallBack(index, buffer.get(), size, gettingCallData);
			memset(buffer.get(), 0, maxBufferLen);
		}
	} else{
		auto v = err.value();
		auto m = err.message();
		SocketException e(SocketException::ErrorType::SystemError, m.c_str(), v);
		reportError(e, *client->addr, client->port, "onReceived");
		switch(v){
			case boost::asio::error::eof:
			case boost::asio::error::connection_aborted:
			case boost::asio::error::connection_reset:
				lostCallBack(index, lostCallData);
				return;
		}
	}
	memset(buffer.get(), 0, maxBufferLen);
	s->async_read_some(boost::asio::buffer(buffer.get(), maxBufferLen), std::bind(&TCPServer_Private::onReceivedShared, this, s, index, std::placeholders::_1, std::placeholders::_2, buffer));
}

void TCPServer_Private::onReceivedUnshared(std::shared_ptr < boost::beast::websocket::stream<boost::asio::ip::tcp::socket>> s, uint32 index, boost::system::error_code err, std::size_t size, std::shared_ptr<boost::beast::multi_buffer> buffer){
	clientMutex.lock();
	auto client = clients.find(index)->second;
	clientMutex.unlock();
	if(!err){
		if(size > 0){
			std::stringstream strstr;
			strstr << boost::beast::buffers(buffer->data());
			gettingCallBack(index, strstr.str().c_str(), size, gettingCallData);
		}
	} else{
		auto v = err.value();
		auto m = err.message();
		SocketException e(SocketException::ErrorType::SystemError, m.c_str(), v);
		reportError(e, *client->addr, client->port, "onReceived");
		switch(v){
			case boost::asio::error::eof:
			case boost::asio::error::connection_aborted:
			case boost::asio::error::connection_reset:
				lostCallBack(index, lostCallData);
				return;
		}
	}
	buffer.reset(new boost::beast::multi_buffer(maxBufferLen));
	s->async_read(*buffer, boost::asio::bind_executor(*client->strand, std::bind(&TCPServer_Private::onReceivedUnshared, this, s, index, std::placeholders::_1, std::placeholders::_2, buffer)));
}

bool TCPServer_Private::givenUpClient(uint32 index){
	clientMutex.lock();
	auto cl = clients.find(index);
	if(cl == clients.end()){
		clientMutex.unlock();
		return false;
	}
	delete cl->second;
	clients.erase(cl);
	clientMutex.unlock();
	return true;
}

bool TCPServer_Private::givenUpClient(const IPAddr&clientAddr, uint16 port){
	auto ret = getIndexByAddrPort(clientAddr, port);
	if(ret < 0)
		return false;
	return givenUpClient(ret);
}

bool TCPServer_Private::givenUpAllClients(){
	clientMutex.lock();
	auto i = clients.begin();
	while(i != clients.end()){
		i->second->closeSocket();
		delete i->second;
		i = clients.erase(i);
	}
	clientMutex.unlock();
	return clients.empty();
}

int TCPServer_Private::getIndexByAddrPort(const IPAddr & clientAddr, uint16 port){
	clientMutex.lock();
	for(auto i = clients.begin(); i != clients.end(); i++){
		if(*i->second->addr == clientAddr && i->second->port == port){
			clientMutex.unlock();
			return i->first;
		}
	}
	clientMutex.unlock();
	return -1;
}

TCPClient_Private::~TCPClient_Private(){
	setSocket(nullptr);
}

bool TCPClient_Private::connectServer(uint16 localPort, bool async, TCPClient::ClientConnectCall asyncConnectCallBack, void * asyncConnectCallData, boost::asio::ip::tcp::socket* socket){
	if(isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The server has connected");
		reportError(ex, *addr, port, "ConnectServer");
		return false;
	}
	isAsync = async;
	setSocket(socket);
	return connectServer(localPort, async, false, asyncConnectCallBack, asyncConnectCallData);
}

bool TCPClient_Private::connectServer(uint16 localPort, bool async, TCPClient::ClientConnectCall asyncConnectCallBack, void * asyncConnectCallData, boost::beast::websocket::stream<boost::asio::ip::tcp::socket>* socket){
	if(isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The server has connected");
		reportError(ex, *addr, port, "ConnectServer");
		return false;
	}
	isAsync = async;
	setSocket(socket);
	return connectServer(localPort, async, true, asyncConnectCallBack, asyncConnectCallData);
}

bool TCPClient_Private::connectServer(uint16 localPort, bool async, bool needHandshake, TCPClient::ClientConnectCall asyncConnectCallBack, void * asyncConnectCallData){
	if(async){
		getSocket()->async_connect(boost::asio::ip::tcp::endpoint(toBoostAddr(*addr), port), std::bind(&TCPClient_Private::onAsyncConnect, this, needHandshake, asyncConnectCallBack, asyncConnectCallData, std::placeholders::_1));
	} else{
		boost::system::error_code err;
		localService.reset();
		getSocket()->connect(boost::asio::ip::tcp::endpoint(toBoostAddr(*addr), port), err);
		if(err){
			auto code = err.value();
			auto msg = err.message();
			SocketException ex(SocketException::ErrorType::SystemError, msg.c_str(), code);
			reportError(ex, *addr, port, "ConnectServer");
			disconnectServer(20000);
			return false;
		}
		if(needHandshake){
			getWebSocket()->handshake((String("ws://") + addr->getStr() + ":" + String(port)).c_str(), "/", err);
			if(err){
				auto code = err.value();
				auto msg = err.message();
				SocketException ex(SocketException::ErrorType::SystemError, msg.c_str(), code);
				reportError(ex, *addr, port, "ConnectServer");
				disconnectServer(20000);
				return false;
			}
		}
		onConnect(asyncConnectCallBack, asyncConnectCallData, err);
	}
	return true;
}

bool TCPClient_Private::disconnectServer(uint32 waitTime){
	if(getSocket() != nullptr){
		boost::system::error_code err;
		if(getSocket()->is_open()){
			getSocket()->shutdown(getSocket()->shutdown_both, err);
		}
		if(!err){
			getSocket()->cancel(err);
		} else{
			auto msg = err.message();
		}
		closeSocket(true);
	}
	localService.stop();
	if(localServiceThread != nullptr && localServiceThread->joinable() && localServiceThread->get_id() != std::this_thread::get_id()){
		localServiceThread->join();
	}
	isListening = false;
	AA_SAFE_DEL(addr);
	AA_SAFE_DEL(localAddr);
	return true;
}

void TCPClient_Private::onAsyncConnect(bool needHandshake, Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err){
	if(err){
		asyncConnectCallBack(!!err, asyncConnectCallData);
		getSocket()->cancel(err);
		if(!err){
			boost::system::error_code closeErr;
			if(getSocket()->is_open()){
				getSocket()->shutdown(getSocket()->shutdown_both, closeErr);
			}
			if(!closeErr)
				getSocket()->cancel(closeErr);
			if(!closeErr){
				getSocket()->close(closeErr);
			}
			setSocket(nullptr);
		}
	} else{
		if(needHandshake){
			getWebSocket()->handshake((addr->getStr() + String(":") + String(port)).c_str(), "/", err);
			if(err){
				auto code = err.value();
				auto msg = err.message();
				SocketException ex(SocketException::ErrorType::SystemError, msg.c_str(), code);
				reportError(ex, *addr, port, "onAsyncConnect");
				disconnectServer(20000);
				return;
			}
		}
		onConnect(asyncConnectCallBack, asyncConnectCallData, err);
	}
}

void TCPClient_Private::onConnect(Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err){
	isListening = true;
	localAddr = IPAddr::clone(toAAAddr(getSocket()->local_endpoint().address()));
	localport = getSocket()->local_endpoint().port();
	if(isShared()){
		auto buffer = std::shared_ptr<uint8>(new uint8[maxBufferLen]);
		memset(buffer.get(), 0, maxBufferLen);
		getSocket()->async_read_some(boost::asio::buffer(buffer.get(), maxBufferLen), std::bind(&TCPClient_Private::onReceivedShared, this, asyncConnectCallBack, asyncConnectCallData, std::placeholders::_1, std::placeholders::_2, buffer));
	} else{
		auto buffer = std::shared_ptr<boost::beast::multi_buffer>(new boost::beast::multi_buffer(maxBufferLen));
		getWebSocket()->async_read(*buffer, boost::asio::bind_executor(*strand, std::bind(&TCPClient_Private::onReceivedUnshared, this, asyncConnectCallBack, asyncConnectCallData, std::placeholders::_1, std::placeholders::_2, buffer)));
	}
	localServiceThread = std::shared_ptr<std::thread>(new std::thread([this](){
		boost::system::error_code err;
		localService.reset();
		localService.run(err);
		auto v = err.value();
		auto m = err.message();
		SocketException e(SocketException::ErrorType::SystemError, m.c_str(), v);
		reportError(e, *addr, port, "onConnect localServiceThread");
		switch(v){
			case boost::asio::error::eof:
			case boost::asio::error::connection_aborted:
			case boost::asio::error::connection_reset:
				if(getSocket() != nullptr){
					if(getSocket()->is_open()){
						getSocket()->shutdown(getSocket()->shutdown_both, err);
					}
					if(!err)
						getSocket()->cancel(err);
					if(!err){
						getSocket()->close(err);
					}
				}
				localService.stop();
				isListening = false;
				setSocket(nullptr);
				// 此处不进行lostCallback回调
				return;
		}
	}));
}

void TCPClient_Private::onReceivedShared(Socket::ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData, boost::system::error_code err, std::size_t size, std::shared_ptr<uint8> buffer){
	if(!err){
		if(size > 0){
			gettingCallBack(buffer.get(), size, gettingCallData);
		}
	} else{
		auto v = err.value();
		auto m = err.message();
		SocketException e(SocketException::ErrorType::SystemError, m.c_str(), v);
		reportError(e, *addr, port, "onReceived");
		switch(v){
			case boost::asio::error::eof:
			case boost::asio::error::connection_aborted:
			case boost::asio::error::connection_reset:
				if(getSocket() != nullptr){
					if(getSocket()->is_open()){
						getSocket()->shutdown(getSocket()->shutdown_both, err);
					}
					if(!err)
						getSocket()->cancel(err);
					if(!err){
						getSocket()->close(err);
					}
				}
				localService.stop();
				isListening = false;
				setSocket(nullptr);
				lostCallBack(lostCallData);
				return;
		}
	}
	memset(buffer.get(), 0, size);
	getSocket()->async_read_some(boost::asio::buffer(buffer.get(), maxBufferLen), std::bind(&TCPClient_Private::onReceivedShared, this, asyncConnectCallBack, asyncConnectCallData, std::placeholders::_1, std::placeholders::_2, buffer));
}

void TCPClient_Private::onReceivedUnshared(Socket::ClientConnectCall asyncConnectCallBack, void * asyncConnectCallData, boost::system::error_code err, std::size_t size, std::shared_ptr<boost::beast::multi_buffer> buffer){
	if(!err){
		if(size > 0){
			std::stringstream strstr;
			strstr << boost::beast::buffers(buffer->data());
			gettingCallBack(strstr.str().c_str(), size, gettingCallData);
		}
	} else{
		auto v = err.value();
		auto m = err.message();
		SocketException e(SocketException::ErrorType::SystemError, m.c_str(), v);
		reportError(e, *addr, port, "onReceived");
		switch(v){
			case boost::asio::error::eof:
			case boost::asio::error::connection_aborted:
			case boost::asio::error::connection_reset:
				if(getSocket() != nullptr){
					if(getSocket()->is_open()){
						getSocket()->shutdown(getSocket()->shutdown_both, err);
					}
					if(!err)
						getSocket()->cancel(err);
					if(!err){
						getSocket()->close(err);
					}
				}
				localService.stop();
				isListening = false;
				setSocket(nullptr);
				lostCallBack(lostCallData);
				return;
		}
	}
	buffer = std::shared_ptr<boost::beast::multi_buffer>(new boost::beast::multi_buffer(maxBufferLen));
	getWebSocket()->async_read(*buffer, boost::asio::bind_executor(*strand, std::bind(&TCPClient_Private::onReceivedUnshared, this, asyncConnectCallBack, asyncConnectCallData, std::placeholders::_1, std::placeholders::_2, buffer)));
}

UDPSingle_Private::UDPSingle_Private()
	:Socket_Private(){

}

WebSocket_Private::WebSocket_Private(boost::asio::io_service & localService) :stream(new boost::beast::websocket::stream<boost::asio::ip::tcp::socket>{localService}){}

WebSocket_Private::WebSocket_Private(boost::asio::ip::tcp::acceptor&acceptor) :stream(new boost::beast::websocket::stream<boost::asio::ip::tcp::socket>{acceptor.get_executor().context()}){}

/*********************** Source for class Socket **************************/


Socket::Socket(void*innerType){
	AA_HANDLE_MANAGER.GetHandle(this, reinterpret_cast<Socket_Private*>(innerType));
}

Socket::~Socket(void){
	delete AA_HANDLE_MANAGER.ReleaseHandle(this);
}

bool Socket::setMaxIOBufferLen(uint32 len){
	AA_HANDLE_MANAGER[this]->maxBufferLen = len;
	return true;
}

bool Socket::setSendingResponseCallBack(SendingResp sendingRespCB, void * pUser){
	AA_HANDLE_MANAGER[this]->asyncResp = sendingRespCB;
	AA_HANDLE_MANAGER[this]->asyncRespUserData = pUser;
	return true;
}

bool Socket::setErrorReportCallBack(ErrorInfoCall errorReportCB, void * pUser){
	AA_HANDLE_MANAGER[this]->errorReportCallBack = errorReportCB;
	AA_HANDLE_MANAGER[this]->errorReportUserData = pUser;
	return true;
}

IPAddr_v4 Socket::getLocalIPv4Addr(int index){
	if(index < 0){
		return IPAddr_v4(127, 0, 0, 1);
	}
	//获取主机名称

	char tszHost[256];
	gethostname(tszHost, 256);
	addrinfo* hostaddrinfo;
	//获取主机地址列表
	getaddrinfo(tszHost, "80", nullptr, &hostaddrinfo);
	//按索引选择
	while(index != 0){
		if(hostaddrinfo == nullptr)
			continue;
		hostaddrinfo = hostaddrinfo->ai_next;
		while(hostaddrinfo->ai_family != AF_INET)
			hostaddrinfo = hostaddrinfo->ai_next;
		index--;
	}
	if((hostaddrinfo != nullptr) && (hostaddrinfo->ai_family == AF_INET))
		return toAAAddr(((sockaddr_in*)(hostaddrinfo->ai_addr))->sin_addr);

	return IPAddr_v4(127, 0, 0, 1);
}

IPAddr_v6 Socket::getLocalIpv6Addr(int index){
	if(index < 0){
		return IPAddr_v6("0:0:0:0:0:0:0:0");
	}
	//获取主机名称

	char tszHost[256];
	gethostname(tszHost, 256);
	addrinfo* hostaddrinfo;
	//获取主机地址列表
	getaddrinfo(tszHost, "80", nullptr, &hostaddrinfo);
	//按索引选择
	while(index != 0){
		if(hostaddrinfo == nullptr)
			continue;
		hostaddrinfo = hostaddrinfo->ai_next;
		while(hostaddrinfo->ai_family != AF_INET)
			hostaddrinfo = hostaddrinfo->ai_next;
		index--;
	}
	if((hostaddrinfo != nullptr) && (hostaddrinfo->ai_family == AF_INET))
		return toAAAddr(*reinterpret_cast<in6_addr*>(&((sockaddr_in*)(hostaddrinfo->ai_addr))->sin_addr));

	return IPAddr_v6("0:0:0:0:0:0:0:0");
}


/******************* Source for class TCPServer ************************/


TCPServer::TCPServer(int32 maxConnNum)
	:Socket(new TCPServer_Private(maxConnNum)){
	//auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
}

TCPServer::~TCPServer(void){
	stop(0xffffffff);
}

bool TCPServer::setGettingCallBack(Socket::ServerGettingCall recvCB, void*pUser){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	hd->gettingCallBack = recvCB;
	hd->gettingCallData = pUser;
	return true;
}

bool TCPServer::setConnectCallBack(ServerConnectCall connectCB, void * pUser){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	hd->connectCallBack = connectCB;
	hd->connetcCallData = pUser;
	return true;
}

bool TCPServer::setDisconnectCallBack(ServerLostCall disconnCB, void * pUser){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	hd->lostCallBack = disconnCB;
	hd->lostCallData = pUser;
	return true;
}

bool TCPServer::setMaxConnNum(int32 maxClientNum){
	static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this])->maxClientNum = maxClientNum;
	return true;
}

bool TCPServer::setMaxIOBufferLen(uint32 len){
	if(isStarting())
		return false;
	return Socket::setMaxIOBufferLen(len);
}

bool TCPServer::start(uint16 port, bool ipv6){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->start(port, ipv6);
}

bool TCPServer::stop(uint32 waitTime){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->stop(waitTime);
}

bool TCPServer::givenUpClient(uint32 index){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->givenUpClient(index);
}

bool TCPServer::givenUpClient(const IPAddr& addr, uint16 port){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->givenUpClient(addr, port);
}

bool TCPServer::givenUpAllClients(){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->givenUpAllClients();
}

mac_uint TCPServer::send(uint32 index, void * data, uint64 len, bool isAsync){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	hd->clientMutex.lock();
	auto cl = hd->clients.find(index);
	if(cl == hd->clients.end()){
		hd->clientMutex.unlock();
		return 0;
	}

	auto buffer = std::shared_ptr<uint8>(new uint8[len]);
	memcpy(buffer.get(), data, len);
	if(!isAsync){
		auto ret = cl->second->getSocket()->send(boost::asio::buffer(buffer.get(), len));
		hd->clientMutex.unlock();
		return ret;
	} else{
		hd->asyncRespTimes = 0;
		cl->second->getSocket()->async_write_some(boost::asio::buffer(buffer.get(), len), std::bind(&Socket_Private::onTCPSendingResponse, AA_HANDLE_MANAGER[this], index, buffer, cl->second->getSocket(), std::placeholders::_1, std::placeholders::_2, len));
		hd->clientMutex.unlock();
		return 0;
	}
}

int TCPServer::getMaxConnNum() const{
	return static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this])->maxClientNum;
}

int TCPServer::getNowConnNum() const{
	// WARNING : If the number is larger than INT_MAX
	return int(static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this])->clients.size());
}

TCPServer::IPAddrInfo TCPServer::getClientByIndex(int index) const{
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	auto hdcls = hd->clients;
	hd->clientMutex.lock();
	auto ret = hdcls.find(index);
	if(ret != hdcls.end()){
		TCPServer::IPAddrInfo result = {ret->second->addr,ret->second->port,ret->second->localAddr,ret->second->localport};
		hd->clientMutex.unlock();
		return result;
	}
	hd->clientMutex.unlock();
	return{nullptr,0,nullptr,0};
}
void TCPServer::getAllClients(TCPServer::IPAddrInfo* ref) const{
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	auto hdcls = hd->clients;
	int index = 0;
	hd->clientMutex.lock();
	for(auto i = hdcls.begin(); i != hdcls.end(); ++i){
		ref[index++] = {i->second->addr,i->second->port,i->second->localAddr,i->second->localport};
	}
	hd->clientMutex.unlock();
}

int TCPServer::getIndexByAddrPort(const IPAddr & clientAddr, uint16 port){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->getIndexByAddrPort(clientAddr, port);
}

bool TCPServer::isStarting()const{
	return AA_HANDLE_MANAGER[this]->isListening;
}


/******************* Source for class TCPClient ************************/


TCPClient::TCPClient()
	:Socket(new TCPClient_Private()){}

TCPClient::~TCPClient(){}

bool TCPClient::setServerAddr(const IPAddr & addr){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The server has connected");
		hd->reportError(ex, *hd->addr, hd->port, "SetServerAddr");
		return false;
	}
	hd->addr = IPAddr::clone(addr);
	return true;
}

bool TCPClient::setServerPort(uint16 port){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The server has connected");
		hd->reportError(ex, *hd->addr, hd->port, "SetServerPort");
		return false;
	}
	hd->port = port;
	return port != 0;
}

bool TCPClient::setLostServerCallBack(ClientLostCall disconnCB, void * pUser){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	hd->lostCallBack = disconnCB;
	hd->lostCallData = pUser;
	return true;
}

bool TCPClient::setGettingCallBack(Socket::ClientGettingCall recvCB, void*pUser){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	hd->gettingCallBack = recvCB;
	hd->gettingCallData = pUser;
	return true;
}

bool TCPClient::connectServer(uint16 port, bool isAsync, ClientConnectCall asyncConnectCallBack, void* asyncConnectCallData){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	auto protocol = hd->addr->getIPVer() == 6 ? boost::asio::ip::tcp::v6() : boost::asio::ip::tcp::v4();
	try{
		return hd->connectServer(port, isAsync, asyncConnectCallBack, asyncConnectCallData, new boost::asio::ip::tcp::socket(hd->localService, boost::asio::ip::tcp::endpoint(protocol, port)));
	} catch(boost::system::system_error err){
		auto msg = err.what();
		return false;
	}
}

bool TCPClient::disconnectServer(uint32 waitTime){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->disconnectServer(waitTime);
}

mac_uint TCPClient::send(const void * pBuffer, size_t len, bool isAsync){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	if(!hd->isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "Have not connected to the server");
		hd->reportError(ex, *hd->addr, hd->port, "TCPClient::Send");
		return false;
	}
	if(pBuffer == nullptr || len <= 0){
		SocketException ex(SocketException::ErrorType::InvalidArgument, "Buffer error");
		hd->reportError(ex, *hd->addr, hd->port, "TCPClient::Send");
		return false;
	}
	auto buffer = std::shared_ptr<uint8>(new uint8[len]);
	memcpy(buffer.get(), pBuffer, len);
	if(!isAsync){
		auto ret = hd->getSocket()->send(boost::asio::buffer(buffer.get(), len));
		return ret;
	} else{
		hd->asyncRespTimes = 0;
		hd->getSocket()->async_write_some(boost::asio::buffer(buffer.get(), len), std::bind(&Socket_Private::onTCPSendingResponse, AA_HANDLE_MANAGER[this], 0, buffer, hd->getSocket(), std::placeholders::_1, std::placeholders::_2, len));
		return len;
	}
}

const IPAddr & TCPClient::getServerAddr() const{
	return *static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this])->addr;
}

uint16 TCPClient::getServerPort() const{
	return static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this])->port;
}

const IPAddr & TCPClient::getLocalAddr() const{
	return *static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this])->localAddr;
}

uint16 TCPClient::getLocalPort() const{
	return static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this])->localport;
}

bool TCPClient::isConnection() const{
	return static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this])->isListening;
}


/******************* Source for class UDPSingle ************************/


UDPSilgle::UDPSilgle()
	:Socket(new UDPSingle_Private()){}

UDPSilgle::~UDPSilgle(){
	auto hd = static_cast<UDPSingle_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->s->is_open()){
		boost::system::error_code err;
		if(hd->s->is_open()){
			hd->s->shutdown(hd->s->shutdown_both, err);
		}
		if(!err)
			hd->s->cancel(err);
		if(!err){
			hd->s->close(err);
		}
		AA_SAFE_DEL(hd->s);
	}
}

bool UDPSilgle::setGettingCallBack(Socket::UDPGettingCall recvCB, void*pUser){
	auto hd = static_cast<UDPSingle_Private*>(AA_HANDLE_MANAGER[this]);
	hd->gettingCallBack = recvCB;
	hd->gettingCallData = pUser;
	return true;
}

bool UDPSilgle::startListening(bool isIPv4){
	auto hd = static_cast<UDPSingle_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "The listening has started");
		hd->reportError(ex, *hd->localAddr, hd->localPort, "UDPSilgle::StartListening");
		return false;
	}
	hd->isAsync = true;
	AA_SAFE_DEL(hd->s);
	hd->s = new boost::asio::ip::udp::socket(hd->localService);
	try{
		if(!hd->s->is_open())
			hd->s->open(isIPv4 ? boost::asio::ip::udp::v4() : boost::asio::ip::udp::v6());
		hd->localAddr = IPAddr::clone(toAAAddr(hd->s->local_endpoint().address()));
		hd->localPort = hd->s->local_endpoint().port();
	} catch(boost::system::system_error e){
		auto code = e.code().value();
		auto msg = e.code().message();
		SocketException ex(SocketException::ErrorType::SystemError, msg.c_str(), code);
		hd->reportError(ex, *hd->localAddr, hd->localPort, "UDPSilgle::StartListening");
		return false;
	}
	hd->s->async_receive_from(hd->recvbuffer, hd->recver, [hd](boost::system::error_code err, std::size_t){
		auto senderAddr = IPAddr::clone(toAAAddr(hd->recver.address())); uint16 senderPort = hd->recver.port(); 			if(!err)
			while(!hd->recvbuffer.empty()){
				auto bfn = *hd->recvbuffer.begin();
				auto bfnp = boost::asio::buffer_cast<uint8*>(bfn);
				auto sz = boost::asio::buffer_size(bfn);
				hd->gettingCallBack(*senderAddr, senderPort, bfnp, sz, hd->gettingCallData);
				hd->recvbuffer.pop_front();
			} else switch(err.value()){
				default:
					break;
			}
	});

	return true;
}

mac_uint UDPSilgle::send(const IPAddr & addr, uint16 port, void * data, size_t len, bool isAsync){
	auto hd = static_cast<UDPSingle_Private*>(AA_HANDLE_MANAGER[this]);
	if(!hd->isListening)
		return 0;
	try{
		if(!hd->s->is_open())
			hd->s->open(addr.getIPVer() == 4 ? boost::asio::ip::udp::v4() : boost::asio::ip::udp::v6());
		hd->localAddr = IPAddr::clone(toAAAddr(hd->s->local_endpoint().address()));
		hd->localPort = hd->s->local_endpoint().port();
	} catch(boost::system::error_code){
		return 0;
	}

	if(!isAsync){
		hd->sendBuffer.clear();
		hd->sendBuffer.push_back(boost::asio::mutable_buffer(data, size_t(len)));
		return hd->s->send_to(hd->sendBuffer, boost::asio::ip::udp::endpoint(toBoostAddr(addr), port));
	}

	std::shared_ptr<std::function<void(boost::system::error_code err, std::size_t sz)>> respF;
	std::shared_ptr<std::function<void(boost::system::error_code err, std::size_t sz)>>&respF2 = respF;
	std::shared_ptr<uint32> n(new uint32(0));
	respF = std::shared_ptr<std::function<void(boost::system::error_code err, std::size_t sz)>>(new std::function<void(boost::system::error_code err, std::size_t sz)>([hd, &addr, port, &n, len, data, &respF2](boost::system::error_code err, std::size_t sz){
		if(!err)
			hd->sendBuffer.clear();
		if(hd->asyncResp != nullptr && hd->asyncResp(sz, (*n)++, 0, data, len, hd->asyncRespUserData) && err)
			hd->s->async_send_to(hd->sendBuffer, boost::asio::ip::udp::endpoint(toBoostAddr(addr), port), *respF2);
	}));
	hd->s->async_send_to(hd->sendBuffer, boost::asio::ip::udp::endpoint(toBoostAddr(addr), port), *respF);
	return 0;
}

bool UDPSilgle::stopListening(uint32 waitTime){
	auto hd = static_cast<UDPSingle_Private*>(AA_HANDLE_MANAGER[this]);
	hd->isListening = false;
	boost::system::error_code err;
	if(hd->s->is_open()){
		hd->s->shutdown(hd->s->shutdown_both, err);
	}
	if(!err)
		hd->s->cancel(err);
	if(!err){
		hd->s->close(err);
	}
	AA_SAFE_DEL(hd->s);
	return !err;
}

bool UDPSilgle::isListening() const{
	return static_cast<UDPSingle_Private*>(AA_HANDLE_MANAGER[this])->isListening;
}
TCPWebSocketServer::TCPWebSocketServer(int32 maxConnNum):TCPServer(maxConnNum){
}

TCPWebSocketServer::~TCPWebSocketServer(){
}

bool TCPWebSocketServer::start(uint16 port, bool ipv6){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->startWeb(port, ipv6);
}

bool TCPWebSocketServer::stop(uint32 waitTime){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->stop(waitTime);
}

mac_uint TCPWebSocketServer::send(uint32 index, void * data, uint64 len, bool isAsync){
	auto hd = static_cast<TCPServer_Private*>(AA_HANDLE_MANAGER[this]);
	hd->clientMutex.lock();
	auto cl = hd->clients.find(index);
	if(cl == hd->clients.end()){
		hd->clientMutex.unlock();
		return 0;
	}

	auto buffer = std::shared_ptr<uint8>(new uint8[len]);
	memcpy(buffer.get(), data, len);
	if(!isAsync){
		boost::beast::error_code err;
		auto ret = cl->second->getWebSocket()->write(boost::asio::buffer(buffer.get(), len), err);
		hd->clientMutex.unlock();
		if(err){
			hd->reportError(SocketException(SocketException::ErrorType::SystemError, err.message().c_str(), err.value()), *cl->second->addr, cl->second->port, "TCPWebSocketServer::send");
		}
		return ret;
	} else{
		hd->asyncRespTimes = 0;
		cl->second->getWebSocket()->async_write(boost::asio::buffer(buffer.get(), len), std::bind(&Socket_Private::onTCPSendingResponse, AA_HANDLE_MANAGER[this], index, buffer, cl->second->getSocket(), std::placeholders::_1, std::placeholders::_2, len));
		hd->clientMutex.unlock();
		return 0;
	}
}

TCPWebSocketClient::TCPWebSocketClient():TCPClient(){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	auto webHd = new WebSocket_Private(hd->localService);
	AA_HANDLE_MANAGER_WEBSOCKET.GetHandle(this, webHd);
	hd->strand = new boost::asio::strand<boost::asio::io_context::executor_type>(webHd->stream->get_executor());
}

TCPWebSocketClient::~TCPWebSocketClient(){
	delete AA_HANDLE_MANAGER_WEBSOCKET.ReleaseHandle(this);
}

bool TCPWebSocketClient::connectServer(uint16 port, bool isAsync, ClientConnectCall asyncConnectCallBack, void * asyncConnectCallData){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	auto webHd = AA_HANDLE_MANAGER_WEBSOCKET[this];
	return hd->connectServer(port, isAsync, asyncConnectCallBack, asyncConnectCallData, webHd->stream.get());
}

bool TCPWebSocketClient::disconnectServer(uint32 waitTime){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	auto webHd = AA_HANDLE_MANAGER_WEBSOCKET[this];
	boost::beast::error_code err;
	boost::beast::websocket::teardown(boost::beast::websocket::role_type::client, webHd->stream->next_layer(), err);
	if(webHd->stream->is_open()){
		webHd->stream->close(boost::beast::websocket::close_reason("Disconnect"));
	}
	return hd->disconnectServer(waitTime);
}

mac_uint TCPWebSocketClient::send(const void * pBuffer, size_t len, bool isAsync){
	auto hd = static_cast<TCPClient_Private*>(AA_HANDLE_MANAGER[this]);
	if(!hd->isListening){
		SocketException ex(SocketException::ErrorType::SocketStatueError, "Have not connected to the server");
		hd->reportError(ex, *hd->addr, hd->port, "TCPClient::Send");
		return false;
	}
	if(pBuffer == nullptr || len <= 0){
		SocketException ex(SocketException::ErrorType::InvalidArgument, "Buffer error");
		hd->reportError(ex, *hd->addr, hd->port, "TCPClient::Send");
		return false;
	}
	auto buffer = std::shared_ptr<uint8>(new uint8[len]);
	memcpy(buffer.get(), pBuffer, len);
	if(!isAsync){
		auto ret = hd->getWebSocket()->write(boost::asio::buffer(buffer.get(), len));
		return ret;
	} else{
		hd->asyncRespTimes = 0;
		hd->getWebSocket()->async_write(boost::asio::buffer(buffer.get(), len), std::bind(&Socket_Private::onTCPSendingResponse, AA_HANDLE_MANAGER[this], 0, buffer, hd->getSocket(), std::placeholders::_1, std::placeholders::_2, len));
		return len;
	}
}

}

#undef AA_HANDLE_MANAGER
