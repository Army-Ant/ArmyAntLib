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

#ifndef AALOG_H_20180524
#define AALOG_H_20180524

#include <iostream>
#include "AADefine.h"
#include "AA_start.h"

namespace ArmyAnt{

class ARMYANTLIB_API Logger{
public:
	enum class AlertLevel : uint8{
		Verbose = 0,
		Debug = 1,
		Info = 2,
		Import = 3,
		Warning = 4,
		Error = 5,
		Fatal = 6
	};
	static const char*convertLevelToString(AlertLevel level);

public:
	Logger(const char* logFilePath = nullptr);
	~Logger();

public:
	// Output log to std::cout
	void setConsoleLevel(AlertLevel level = AlertLevel::Import);
	AlertLevel getConsoleLevel()const;

	// Output log to a disk file
	bool setLogFile(const char* path);
	const char*getLogFilePath()const;
	void setFileLevel(AlertLevel level = AlertLevel::Verbose);
	AlertLevel getFileLevel()const;

	// Output log to a user-defined ostream
	void setUserStream(std::ostream* stream);
	std::ostream*getUserStream();
	void setUserStreamLevel(AlertLevel level = AlertLevel::Debug);
	AlertLevel getUserStreamLevel()const;

public:
	bool pushLog(const char* content, AlertLevel level, const char*tag = nullptr);
	bool pushLogOnlyInConsole(const char* content, AlertLevel level, const char*tag = nullptr);
	bool pushLogOnlyInFile(const char* content, AlertLevel level, const char*tag = nullptr);

protected:
	bool pushLogToConsole(const char* wholeContent);
	bool pushLogToFile(const char* wholeContent);
	bool pushLogToUserStream(const char* wholeContent);


	AA_FORBID_COPY_CTOR(Logger);
	AA_FORBID_ASSGN_OPR(Logger);
};

} // namespace ArmyAntServer 


#endif // AALOG_H_20180524