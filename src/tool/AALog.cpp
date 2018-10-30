/* Copyright (c) 2015 ArmyAnt
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

#include "../../inc/AALog.h"
#include "../../inc/AATimeUtilities.h"

#include <queue>
#include <thread>

#include "../../inc/AAClassPrivateHandle.hpp"
#include "../../inc/AAString.h"
#include "../../inc/AAIStream_File.h"


#define AA_HANDLE_MANAGER ArmyAnt::ClassPrivateHandleManager<Logger, Logger_Private>::getInstance()

namespace ArmyAnt{

class Logger_Private{
public:
	Logger_Private() :mutex(), logFileWriteQueue(), threadEnd(false), logFileWriteThread(&Logger_Private::update, this){}
	~Logger_Private(){
		threadEnd = true;
		logFileWriteThread.join();
	}

	void update();

	static ArmyAnt::String getWholeContent(const char * content, Logger::AlertLevel level, const char * tag);

	Logger::AlertLevel consoleLevel = Logger::AlertLevel::Import;
	ArmyAnt::File logFile;
	Logger::AlertLevel fileLevel = Logger::AlertLevel::Verbose;
	std::ostream* userStream = nullptr;
	Logger::AlertLevel userStreamLevel = Logger::AlertLevel::Debug;

	ArmyAnt::String logFileName;
	std::mutex mutex;
	std::queue<ArmyAnt::String> logFileWriteQueue;
	bool threadEnd = false;
	std::thread logFileWriteThread;
};

ArmyAnt::String Logger_Private::getWholeContent(const char * content, Logger::AlertLevel level, const char * tag){
	ArmyAnt::String timeString = "[ " + TimeUtilities::getTimeStamp() + " ] ";
	ArmyAnt::String tagString = "[ " + ArmyAnt::String(tag) + " ] ";
	ArmyAnt::String wholeContent = timeString + tagString + "[ " + Logger::convertLevelToString(level) + " ] " + content;
	return wholeContent;
}

void Logger_Private::update(){
	while(true){
		mutex.lock();
		bool isEmpty = logFileWriteQueue.empty();
		if(!isEmpty){
			if(logFile.IsOpened())
				logFile.Close();
			if(!logFile.Open(logFileName)){
				std::this_thread::sleep_for(std::chrono::microseconds(1));
				continue;
			}
			while(!logFileWriteQueue.empty()){
				auto msg = logFileWriteQueue.front();

				logFile.MoveTo(-1);
				bool ret = logFile.Write(msg);
				if(ret != 0){
					ret = logFile.Write("\n");
				} else{

				}

				logFileWriteQueue.pop();
			}
			logFile.Close();
			mutex.unlock();
		} else{
			mutex.unlock();
			if(threadEnd){
				break;
			} else{
				std::this_thread::sleep_for(std::chrono::microseconds(1));
			}
		}
	}
}

const char * Logger::convertLevelToString(AlertLevel level){
	switch(level){
		case AlertLevel::Verbose:
			return "Verbose";
		case AlertLevel::Debug:
			return "Debug";
		case AlertLevel::Info:
			return "Info";
		case AlertLevel::Import:
			return "Import";
		case AlertLevel::Warning:
			return "Warning";
		case AlertLevel::Error:
			return "Error";
		case AlertLevel::Fatal:
			return "Fatal";
	}
	return "Unknown";
}

Logger::Logger(const char* logFilePath){
	AA_HANDLE_MANAGER.GetHandle(this);
	setLogFile(logFilePath);
}

Logger::~Logger(){
	AA_HANDLE_MANAGER.ReleaseHandle(this);
}

void Logger::setConsoleLevel(Logger::AlertLevel level){
	AA_HANDLE_MANAGER[this]->consoleLevel = level;
}

Logger::AlertLevel Logger::getConsoleLevel()const{
	return AA_HANDLE_MANAGER[this]->consoleLevel;
}

bool Logger::setLogFile(const char* path){
	if(path == nullptr)
		return false;
	auto hd = AA_HANDLE_MANAGER[this];
	hd->logFileName = path;
	hd->mutex.lock();
	hd->logFile.SetStreamMode(false, false);
	auto ret = hd->logFile.Open(path);
	hd->logFile.Close();
	hd->mutex.unlock();
	return ret;
}
const char*Logger::getLogFilePath()const{
	return AA_HANDLE_MANAGER[this]->logFile.GetSourceName();
}

void Logger::setFileLevel(Logger::AlertLevel level){
	AA_HANDLE_MANAGER[this]->fileLevel = level;
}

Logger::AlertLevel Logger::getFileLevel()const{
	return AA_HANDLE_MANAGER[this]->fileLevel;
}

void Logger::setUserStream(std::ostream* stream){
	AA_HANDLE_MANAGER[this]->userStream = stream;
}

std::ostream*Logger::getUserStream(){
	return AA_HANDLE_MANAGER[this]->userStream;
}

void Logger::setUserStreamLevel(Logger::AlertLevel level){
	AA_HANDLE_MANAGER[this]->userStreamLevel = level;
}

Logger::AlertLevel Logger::getUserStreamLevel()const{
	return AA_HANDLE_MANAGER[this]->userStreamLevel;
}

bool Logger::pushLog(const char * content, Logger::AlertLevel level, const char * tag){
	ArmyAnt::String wholeContent = Logger_Private::getWholeContent(content, level, tag);
	bool ret = true;
	if(level >= AA_HANDLE_MANAGER[this]->consoleLevel){
		ret = pushLogToConsole(wholeContent.c_str());
	}
	if(level >= AA_HANDLE_MANAGER[this]->fileLevel){
		ret = ret && pushLogToFile(wholeContent.c_str());
	}
	if(level >= AA_HANDLE_MANAGER[this]->userStreamLevel){
		ret = ret && pushLogToUserStream(wholeContent.c_str());
	}
	return ret;
}

bool Logger::pushLogOnlyInConsole(const char * content, AlertLevel level, const char * tag){
	ArmyAnt::String wholeContent = Logger_Private::getWholeContent(content, level, tag);
	bool ret = true;
	if(level >= AA_HANDLE_MANAGER[this]->consoleLevel){
		ret = pushLogToConsole(wholeContent.c_str());
	}
	return ret;
}

bool Logger::pushLogOnlyInFile(const char * content, AlertLevel level, const char * tag){
	ArmyAnt::String wholeContent = Logger_Private::getWholeContent(content, level, tag);
	bool ret = true;
	if(level >= AA_HANDLE_MANAGER[this]->fileLevel){
		ret = ret && pushLogToFile(wholeContent.c_str());
	}
	return ret;
}

bool Logger::pushLogToConsole(const char * wholeContent){
	std::cout << wholeContent << std::endl;
	return true;
}

bool Logger::pushLogToFile(const char * wholeContent){
	auto hd = AA_HANDLE_MANAGER[this];
	hd->mutex.lock();
	hd->logFileWriteQueue.push(wholeContent);
	hd->mutex.unlock();
	return true;
}

bool Logger::pushLogToUserStream(const char * wholeContent){
	if(AA_HANDLE_MANAGER[this]->userStream == nullptr)
		return true;
	if(AA_HANDLE_MANAGER[this]->userStream->bad())
		return false;
	*(AA_HANDLE_MANAGER[this]->userStream) << wholeContent << "\n";
	return true;
}

} // namespace ArmyAntServer 

#undef AA_HANDLE_MANAGER