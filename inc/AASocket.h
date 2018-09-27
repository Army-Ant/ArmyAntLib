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
 */

#ifndef AA_SOCKET_H_2016_3_28
#define AA_SOCKET_H_2016_3_28

#include <functional>

#include "AA_start.h"
#include "AADefine.h"
#include "AAString.h"

namespace ArmyAnt {

class IPAddr_v4;
class IPAddr_v6;

class ARMYANTLIB_API IPAddr
{
public:
	typedef IPAddr_v4 V4;
	typedef IPAddr_v6 V6;

public:
	IPAddr();
	IPAddr(const IPAddr&value);
	virtual IPAddr&operator=(const char* ipStr);
	IPAddr&operator=(const IPAddr&value);
	virtual ~IPAddr();

public:
	virtual const char*getStr()const = 0;
	virtual uint8 getIPVer()const = 0;
	virtual bool operator==(const char*value)const;
	virtual bool operator!=(const char*value)const;
	bool operator==(const IPAddr&value)const;
	bool operator!=(const IPAddr&value)const;

public:
	operator IPAddr_v4 ();
	operator const IPAddr_v4 ()const;
	operator IPAddr_v6 ();
	operator const IPAddr_v6 ()const;

public:
	static IPAddr* create(const char*ipStr);
	static IPAddr* clone(const IPAddr& value);

protected:
	virtual void parseFromString(const char* str) = 0;
};

class ARMYANTLIB_API IPAddr_v4 : public IPAddr
{
public:
	IPAddr_v4(const char* ipStr);
	IPAddr_v4(uint32 ipNum);
	IPAddr_v4(uint8 net, uint8 host, uint8 lh, uint8 impNum);
	IPAddr_v4(const IPAddr_v4&value);
	IPAddr_v4&operator=(const char* ipStr)override;
	IPAddr_v4&operator=(uint32 ipNum);
	IPAddr_v4&operator=(const IPAddr_v4&value);
	~IPAddr_v4();

public:
	const char*getStr()const override;
	uint8 getIPVer()const override;
	uint32 getNum()const;
	uint8 getNet()const;
	bool setNet(uint8 _net);
	uint8 getHost()const;
	bool setHost(uint8 _host);
	uint8 getLH()const;
	bool setLH(uint8 _lh);
	uint8 getImpNumber()const;
	bool setImpNumber(uint8 _impNum);

public:
	bool operator==(const char*value)const override;
	bool operator!=(const char*value)const override;
	bool operator==(uint32 ipNum)const;
	bool operator!=(uint32 ipNum)const;
	bool operator==(const IPAddr_v4&value)const;
	bool operator!=(const IPAddr_v4&value)const;

public:
	static IPAddr_v4&localhost();

protected:
	virtual void parseFromString(const char* str)override;

private:
	uint8 net, host, lh, impNum;
};

class ARMYANTLIB_API IPAddr_v6 : public IPAddr
{
public:
	IPAddr_v6(std::nullptr_t);
	IPAddr_v6(const char* ipStr);
	IPAddr_v6(uint16 words[8]);
	IPAddr_v6(uint8 bytes[16]);
	IPAddr_v6(const IPAddr_v6&value);
	IPAddr_v6&operator=(const char* ipStr)override;
	IPAddr_v6&operator=(const IPAddr_v6&value);
	~IPAddr_v6();

public:
	const char*getStr()const override;
	uint8 getIPVer()const override;
	uint8 getByte(uint8 index)const;
	bool setByte(uint8 index, uint8 value);
	uint16 getWord(uint8 index)const;
	bool setWord(uint8 index, uint16 value);

public:
	bool operator==(const char*value)const override;
	bool operator!=(const char*value)const override;
	bool operator==(const IPAddr_v6&value)const;
	bool operator!=(const IPAddr_v6&value)const;
	uint16 operator[](int index)const;

public:
	static IPAddr_v6&localhost();

protected:
	virtual void parseFromString(const char* str)override;

private:
	uint8 bytes[16];
};

class ARMYANTLIB_API SocketException{
public:
	enum class ErrorType{
		Unknown,
		SystemError,
		InvalidArgument,
		SocketStatueError,
		CommandError
	};

public:
	SocketException(ErrorType type, const char* message, int code = 0);
	SocketException(const SocketException& value);
	SocketException(SocketException&& moved);

public:
	ErrorType type;
	String message;
	int code;
};

//作为通信操作的公共基类
class ARMYANTLIB_API Socket
{
	// 回调函数原型定义，请在实现任何同步回调函数体时注意效率，不要延时太长，否则会导致网络阻塞甚至丢失数据
	// 异步不受此限制, 但是异步回调太长会影响数据处理效率, 因为同一个Socket一次只能处理一个回调
	// 因此建议回调中仅接收数据, 数据处理建议在另一线程中
public:
	//TCP服务器得到连接回调，参数分别为客户端IPv4, 客户端端口号, 用户传入参数, 服务器的客户端索引, 返回true表示接受连接, 返回false表示拒绝连接
	typedef std::function<bool(uint32 clientIndex, void*pUser)> ServerConnectCall;
	//TCP服务器收到数据回调，参数分别为对方IPv4，对方端口号，数据包，数据包容量大小（不是数据包大小），用户传入参数
	typedef std::function<void(uint32 clientIndex, uint8*data, mac_uint datalen, void*pUser)> ServerGettingCall;
	//TCP服务器丢失客户端连接回调，参数分别为客户端IPv4，客户端端口号，用户传入参数
	typedef std::function<void(uint32 clientIndex, void*pUser)> ServerLostCall;

	//TCP客户端连接回调，参数分别为连接是否成功, 用户传入参数, 请勿在此函数中调用重新连接
	typedef std::function<void(bool isSucceed, void*pUser)> ClientConnectCall;
	//TCP客户端收到数据回调，参数分别为对方IPv4，对方端口号，数据包，数据包容量大小（不是数据包大小），用户传入参数
	typedef std::function<void(uint8*data, mac_uint datalen, void*pUser)> ClientGettingCall;
	//TCP客户端断开连接回调，参数为用户传入参数, 请勿在此函数中调用重新连接
	typedef std::function<void(void*pUser)> ClientLostCall;

	//UDP收到数据回调，参数分别为对方IPv4，对方端口号，数据包，数据包容量大小（不是数据包大小），用户传入参数
	typedef std::function<void(const IPAddr&addr, uint16 port, uint8*data, mac_uint datalen, void*pUser)> UDPGettingCall;

	//异步发送回执
	typedef std::function<bool(mac_uint sendedSize, uint32 retriedTimes, uint32 index, void*sendedData, uint64 len, void* pUser)> SendingResp;
	//socket连接及连通时错误信息回调, 参数为 异常体, 对方地址, 对方端口, 出错的函数名
	typedef std::function<void(const SocketException&err, const IPAddr&addr, uint16 port, String functionName, void*pUser)> ErrorInfoCall;

public:
	Socket(void*innerType/* new Socket_Private()*/);
	virtual ~Socket(void);

public:
	enum class ProtocolType :uint8
	{
#include "AASocket_ProtocolTypes.txt"
	};
	struct IPAddrInfo
	{
		const IPAddr* clientAddr;
		uint16 clientPort;
		const IPAddr* serverAddr;
		uint16 serverPort;
	};


public:
	virtual bool setMaxIOBufferLen(uint32 len = 65530);
	//设定异步发送回执回调
	bool setSendingResponseCallBack(SendingResp sendingRespCB, void*pUser = nullptr);
	//设定错误报告回调. 如不设, 则出错时会抛出异常
	bool setErrorReportCallBack(ErrorInfoCall errorReportCB, void*pUser = nullptr);

public:
	//获取本机IP地址
	static IPAddr_v4 getLocalIPv4Addr(int index = 0);
	static IPAddr_v6 getLocalIpv6Addr(int index = 0);

	AA_FORBID_COPY_CTOR(Socket);
	AA_FORBID_ASSGN_OPR(Socket);
};

//用于TCP服务器的操作类
class ARMYANTLIB_API TCPServer :public Socket
{
public:
	//默认构造函数，参数为：
	// nMaxConnNum：最大允许同时连接的数量
	TCPServer(int32 maxConnNum = 65536);

	//析构函数不应负责关闭服务器，请调用者务必遵守调用规则,自行调用关闭服务器的函数,以此规范代码层次结构
	virtual ~TCPServer(void);

public:
	//以下是连接前的一些设定

	//设定收到信息回调
	virtual bool setGettingCallBack(ServerGettingCall recvCB, void*pUser = nullptr);
	//设定收到连接回调
	virtual bool setConnectCallBack(ServerConnectCall connectCB, void*pUser = nullptr);
	//设定断开连接回调
	virtual bool setDisconnectCallBack(ServerLostCall disconnCB, void*pUser = nullptr);
	//设定可同时接受的最大连接数,如果传入参数不是正数，会返回false
	virtual bool setMaxConnNum(int32 maxClientNum);

	virtual bool setMaxIOBufferLen(uint32 len = 65530) override;

public:
	//以下是连接和实际收发操作

	//开启服务器，开始监听并运行回调
	virtual bool start(uint16 port, bool ipv6 = false);
	//停止服务器，关闭套接字和所有传入连接,输入参数为最大等待时间，超过此时限就强制关闭线程
	virtual bool stop(uint32 waitTime);
	//断开客户端
	virtual bool givenUpClient(uint32 index);
	virtual bool givenUpClient(const IPAddr& addr, uint16 port);
	virtual bool givenUpAllClients();
	//向指定索引的客户端发送数据
	virtual mac_uint send(uint32 index, void*data, uint64 len, bool isAsync = true);

public:
	//以下是获取状态

	//获取最大可连接的客户端数
	virtual int getMaxConnNum() const;
	//获取当前连接的客户端数
	virtual int getNowConnNum() const;
	//根据索引获取对应客户端的基本信息
	virtual IPAddrInfo getClientByIndex(int index) const;
	//获取当前连接的客户端列表
	virtual void getAllClients(IPAddrInfo* ref) const;
	//根据地址和端口号获取客户端索引
	virtual int getIndexByAddrPort(const IPAddr& clientAddr, uint16 port);
	//服务器是否正在监听
	virtual bool isStarting() const;

	AA_FORBID_COPY_CTOR(TCPServer);
	AA_FORBID_ASSGN_OPR(TCPServer);
};


// 用于TCP客户端的操作类
class ARMYANTLIB_API TCPClient : public Socket
{
public:

	//默认构造函数
	TCPClient();
	//析构函数不应负责关闭服务器，请调用者务必遵守调用规则,自行调用关闭服务器的函数,以此规范代码层次结构
	virtual ~TCPClient();

public:
	//以下是连接前的一些设定

	//设置服务器地址,不可以是INADDR_ANY
	virtual bool setServerAddr(const IPAddr& addr);
	//设定服务器端口,如果传入端口号为0，会返回false
	virtual bool setServerPort(uint16 port);
	//设定断开连接回调
	virtual bool setLostServerCallBack(ClientLostCall disconnCB, void*pUser);
	//设定收到信息回调
	virtual bool setGettingCallBack(ClientGettingCall recvCB, void*pUser = nullptr);

public:
	//以下是连接和实际收发操作

	//连接服务器
	virtual bool connectServer(uint16 port, bool isAsync, ClientConnectCall asyncConnectCallBack = nullptr, void* asyncConnectCallData = nullptr);
	//断开连接
	virtual bool disconnectServer(uint32 waitTime);
	//向服务器发送消息
	virtual mac_uint send(const void*pBuffer, size_t len, bool isAsync = false);

public:
	//以下是获取状态

	//获取服务器IP地址
	virtual const IPAddr& getServerAddr() const;
	//获取服务器端口
	virtual uint16 getServerPort() const;
	virtual const IPAddr& getLocalAddr()const;
	virtual uint16 getLocalPort()const;
	//是否在连接状态
	virtual bool isConnection() const;

	AA_FORBID_COPY_CTOR(TCPClient);
	AA_FORBID_ASSGN_OPR(TCPClient);
};


//用于UDP收发操作的类
class ARMYANTLIB_API UDPSilgle : public Socket
{
public:

	//默认构造函数
	UDPSilgle();
	virtual ~UDPSilgle();

public:
	//设定收到信息回调
	bool setGettingCallBack(UDPGettingCall recvCB, void*pUser = nullptr);

	//以下是实际收发操作

	//开始监听
	bool startListening(bool isIPv4 = true);
	//向指定地址和端口发送（无须初始化套接字）
	mac_uint send(const IPAddr& addr, uint16 port, void*data, size_t len, bool isAsync = false);
	//关闭收发端口套接字
	bool stopListening(uint32 waitTime);

public:
	//以下是获取状态

	//UDP是否正在监听
	bool isListening() const;

	AA_FORBID_COPY_CTOR(UDPSilgle);
	AA_FORBID_ASSGN_OPR(UDPSilgle);
};

// 用于WebSocket的服务器类
class ARMYANTLIB_API TCPWebSocketServer : public TCPServer{
public:
	TCPWebSocketServer(int32 maxConnNum = 65536);
	virtual ~TCPWebSocketServer();

public:
	//开启服务器，开始监听并运行回调
	virtual bool start(uint16 port, bool ipv6 = false) override;
	//停止服务器，关闭套接字和所有传入连接,输入参数为最大等待时间，超过此时限就强制关闭线程
	virtual bool stop(uint32 waitTime) override;
	//断开客户端
	//virtual bool givenUpClient(uint32 index) override;
	//virtual bool givenUpClient(const IPAddr& addr, uint16 port) override;
	//virtual bool givenUpAllClients() override;
	//向指定索引的客户端发送数据
	//virtual mac_uint send(uint32 index, void*data, uint64 len, bool isAsync = true) override;

};

class ARMYANTLIB_API TCPWebSocketClient : public TCPClient{
public:
	TCPWebSocketClient();
	virtual ~TCPWebSocketClient();

public:
	virtual bool connectServer(uint16 port, bool isAsync, ClientConnectCall asyncConnectCallBack = nullptr, void* asyncConnectCallData = nullptr) override;
	virtual bool disconnectServer(uint32 waitTime) override;
	//virtual mac_uint send(const void*pBuffer, size_t len, bool isAsync = false) override;
	
};

} // namespace ArmyAnt

#endif // AA_SOCKET_H_2016_3_28