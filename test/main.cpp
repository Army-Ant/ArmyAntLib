#include "main.h"

#include <iostream>
#include <string>
#include <map>


void jsonTest()
{
	auto st = new File();
	st->Open("testJson.json");
	char buf[32768] = "";
	st->Read(buf);
	std::cout << "file read ok" << std::endl;
	JsonUnit*j = JsonUnit::create(buf);
	/*
	std::cout << "parsed ok, type:" << uint32(uint8(j->getType())) << std::endl;
	j->toJsonString(buf);
	*/
	JsonUnit::release(j);
	std::cout << buf << std::endl; st->Close();
	delete st;
}

bool onServerConnected(const ArmyAnt::IPAddr&clientAddr, uint16 clientPort, void*pThis) {
	std::cout << "[ Callback ] Server get client connected, ip: " << clientAddr.getStr() << " , port: " << clientPort << std::endl;
	return true;
}

void onServerDisonnected(const ArmyAnt::IPAddr& clientAddr, uint16 clientPort, void*pThis) {
	std::cout << "[ Callback ] Server get client disconnected, ip: " << clientAddr.getStr() << " , port: " << clientPort << std::endl;
}

void onServerReceived(const ArmyAnt::IPAddr&addr, uint16 port, uint8*data, mac_uint datalen, void*pThis) {
	std::cout << "[ Callback ] Server received client data, ip: " << addr.getStr() << " , port: " << port << std::endl;
	std::cout << "[ Callback ] Data length: " << datalen << " , content: " << reinterpret_cast<const char*>(data) << std::endl;
}

void TCPServerTest() {
	ArmyAnt::TCPServer coreServer;
	//coreServer.setConnectCallBack(onServerConnected);
	//coreServer.setDisconnectCallBack(onServerDisonnected);
	//coreServer.setGettingCallBack(onServerReceived);

	try {
		auto ret = coreServer.start(4744);
		std::cout << "Server created " << (ret ? "successful" : "failure");
	}
	catch (ArmyAnt::SocketException e) {
		auto msg = e.message.c_str();
		std::cout << "Server created failure, error code: " << msg;
	}
	int exitcode = 0;
	std::cin >> exitcode;
	while (exitcode != 200) {
		std::cin >> exitcode;
	}
	coreServer.stop(10000);
}


bool gt_macMapTest(){
    std::map<int, const char*> intMap;
    if(intMap.find(1) == intMap.end())
        intMap.insert(std::make_pair(1,"the first one"));
    intMap.insert(std::make_pair(2,"the second one"));
    std::map<const char*, const char*> ptrMap;
    if(ptrMap.find("1") == ptrMap.end())
        ptrMap.insert(std::make_pair("1", "the first one"));
    ptrMap.insert(std::make_pair("2", "the second one"));
    return true;
}

static bool onConnected(const ArmyAnt::IPAddr&clientAddr, uint16 clientPort, void*pUser) {
	std::cout << "[ Callback ] Client connect server successful !" << std::endl;
	return true;
}

static bool onLostServer(void*pData) {
	std::cout << "[ Callback ] Client lost the connection to server" << std::endl;
	return false;
}

static void onReceived(const ArmyAnt::IPAddr&addr, uint16 port, uint8*data, mac_uint datalen, void*pUser) {
	std::cout << "[ Callback ] Client get message from server, content:" << std::endl;
	std::cout << reinterpret_cast<char*>(data) << std::endl;
}

static bool onResponse(mac_uint sendedSize, uint32 retriedTimes, int32 index, void*data, uint64 len, void* pUser) {
	if (sendedSize == 0) {
		std::cout << "[ Callback ] Client sended failed, will not try again." << std::endl;
		return false;
	}
	std::cout << "[ Callback ] Client get the response from server, content:" << std::endl;
	std::cout << static_cast<char*>(data) << std::endl;
	return false;
}


std::map<std::string, ArmyAnt::TCPClient*> clientLists;
bool createTCPClient(std::string index) {
	auto ret = clientLists.find(index);
	if (ret != clientLists.end())
		return false;
	clientLists.insert(std::make_pair(index, new ArmyAnt::TCPClient()));
	return true;
}

bool deleteTCPClient(std::string index) {
	auto ret = clientLists.find(index);
	if (ret == clientLists.end())
		return false;
	delete ret->second;
	clientLists.erase(ret);
	return true;
}

bool startTCPClient(std::string index, uint16 port) {
	auto ret = clientLists.find(index);
	if (ret == clientLists.end())
		return false;
	if (ret->second->isConnection())
		return false;

	// ArmyAnt::IPAddr_v4 ip(127, 0, 0, 1);
	ArmyAnt::IPAddr_v4 ip(192, 168, 15, 1);
	ret->second->setServerAddr(ip);
	ret->second->setServerPort(8086);
	ret->second->setLostServerCallBack(onLostServer, ret->second);
	//ret->second->setGettingCallBack(onReceived, ret->second);
	//return ret->second->connectServer(port, false, onConnected, ret->second);
	return false;
}

bool stopTCPClient(std::string index) {
	auto ret = clientLists.find(index);
	if (ret == clientLists.end())
		return false;
	if (ret->second->isConnection()) {
		return ret->second->disconnectServer(10000);
	}
	return false;
}

bool sendToServer(std::string index, std::string content) {
	auto ret = clientLists.find(index);
	if (ret == clientLists.end())
		return false;
	ret->second->setSendingResponseCallBack(onResponse, ret->second);
	if (ret->second->isConnection()) {
		return ret->second->send(content.c_str(), content.length(), true) > 0;
	}
	return false;
}

void exit() {
	auto i = clientLists.begin();
	while (!clientLists.empty()) {
		if (i->second->isConnection()) {
			i->second->disconnectServer(10000);
		}
		delete i->second;
		i = clientLists.erase(i);
	}
}


int tcpClientTest()
{
	std::cout << "Welcome to the test client !" << std::endl;
	std::cout << "Please enter the command:" << std::endl;
	std::cout << "	1 : createTCPClient" << std::endl;
	std::cout << "	2 : startTCPClient" << std::endl;
	std::cout << "	3 : sendToServer" << std::endl;
	std::cout << "	4 : stopTCPClient" << std::endl;
	std::cout << "	5 : deleteTCPClient" << std::endl;
	std::cout << "	exit : exit" << std::endl << std::endl;
	std::string str_com;
	std::cin >> str_com;

	std::string index;
	std::string content;
	uint16 port;
	while (str_com != "exit") {
		index = "";
		content = "";

		if (str_com == "1") {
			std::cout << "Please name the TCP client: ";
			std::cin >> index;
			while (index.empty()) {
				std::cout << "Please name the TCP client: ";
				std::cin >> index;
			}
			std::cout << "TCP client created " << (createTCPClient(index) ? "successful" : "failure") << std::endl << std::endl;
		}
		else if (str_com == "2") {
			std::cout << "Please enter the name of the TCP client you want to start: ";
			std::cin >> index;
			while (index.empty()) {
				std::cout << "Please enter the name of the TCP client you want to start: ";
				std::cin >> index;
			}
			std::cout << "Please input the port you want to stand for the client: ";
			std::cin >> port;
			while (port == 0) {
				std::cout << "Please input the port you want to stand for the client: ";
				std::cin >> port;
			}
			std::cout << "TCP client started " << (startTCPClient(index, port) ? "successful" : "failure") << std::endl << std::endl;
		}
		else if (str_com == "3") {
			std::cout << "Please enter the name of the TCP client you want to send from: ";
			std::cin >> index;
			while (index.empty()) {
				std::cout << "Please enter the name of the TCP client you want to send from: ";
				std::cin >> index;
			}
			std::cout << "Please input the content you want to send: ";
			std::cin >> content;
			while (content.empty()) {
				std::cout << "Please input the content you want to send: ";
				std::cin >> content;
			}
			std::cout << "TCP client send message to server " << (sendToServer(index, content) ? "successful" : "failure") << std::endl << std::endl;
		}
		else if (str_com == "4") {
			std::cout << "Please enter the name of the TCP client you want to stop: ";
			std::cin >> index;
			while (index.empty()) {
				std::cout << "Please enter the name of the TCP client you want to stop: ";
				std::cin >> index;
			}
			std::cout << "TCP client stopped " << (stopTCPClient(index) ? "successful" : "failure") << std::endl << std::endl;
		}
		else if (str_com == "5") {
			std::cout << "Please enter the name of the TCP client you want to delete: ";
			std::cin >> index;
			while (index.empty()) {
				std::cout << "Please enter the name of the TCP client you want to delete: ";
				std::cin >> index;
			}
			std::cout << "TCP client deleted " << (deleteTCPClient(index) ? "successful" : "failure") << std::endl << std::endl;
			break;
		}
		else {
			std::cout << "Unknown command: " << str_com << ", please retry." << std::endl;
		}

		std::cout << "Please enter the next command:" << std::endl;
		std::cin >> str_com;
	}

	std::cout << "Test program exit ..." << std::endl;
	exit();
	return 0;
}



int main(int argc, char**argv, char**env_vars)
{
	jsonTest();
	getchar();
	getchar();
	return 0;
}