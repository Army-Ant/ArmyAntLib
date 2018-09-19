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
#include "../../inc/AAClassPrivateHandle.hpp"
#include "../../inc/AAIStream_File.h"
#include <mutex>


#define AA_HANDLE_MANAGER ClassPrivateHandleManager<LoggerBase, LoggerBase_Private>::getInstance()

namespace ArmyAnt{

    class LoggerBase_Private {
    public:
        LoggerBase_Private();
        ~LoggerBase_Private();

        LoggerBase::Contents setting;
        String filepath = "";
        File file;
        bool isInited = false;
        std::mutex mutex;

        AA_FORBID_ASSGN_OPR(LoggerBase_Private);
        AA_FORBID_COPY_CTOR(LoggerBase_Private);
    };

	const LoggerBase::Contents& LoggerBase::Contents::defaultSetting() {
		static LoggerBase::Contents ret;
		return ret;
	}

    LoggerBase_Private::LoggerBase_Private() {
        file.SetStreamMode(false, false);
    }

    LoggerBase_Private::~LoggerBase_Private() {
    }

    LoggerBase::LoggerBase() {
        AA_HANDLE_MANAGER.GetHandle(this, new LoggerBase_Private());
    }

    LoggerBase::~LoggerBase() {
        auto pr = AA_HANDLE_MANAGER[this];
        pr->mutex.lock();
        AA_HANDLE_MANAGER.ReleaseHandle(this);
        pr->mutex.unlock();
        delete pr;
    }

    bool LoggerBase::initLog(String filepath, const Contents& setting) {
        auto hd = AA_HANDLE_MANAGER[this];
        hd->mutex.lock();
        if(hd->isInited){
            hd->mutex.unlock();
            return false;
        }
        hd->filepath = filepath;
        hd->setting = setting;
        if(!hd->file.Open(filepath.c_str())){
            hd->mutex.unlock();
            return false;
        }
        hd->file.Close();
        hd->isInited = true;
        hd->mutex.unlock();
        return true;
    }

    bool LoggerBase::pushLog(String date, String time, String function, String file, String line, String tag, String level, String log) {
        auto hd = AA_HANDLE_MANAGER[this];
        if (!hd->isInited){
            hd->mutex.unlock();
            return false;
        }
        String logStr = "";
        if (hd->setting.datetime)
            logStr += '[' + date + ' ' + time + ']';
        if (hd->setting.tag)
            logStr += '[' + tag + ']';
        logStr += '[' + level + ']';
        if (hd->setting.fileAndLine)
            logStr += '[' + file + ' ' + line + ']';
        if (hd->setting.function)
            logStr += function + ':';
        logStr += log + hd->setting.lineEnding;
        bool ret = hd->file.Open(hd->filepath.c_str());
        if (!hd->setting.warningIfFileFailed){
            if(ret) {
                ret = hd->file.Write(logStr.c_str(), logStr.size());
                hd->file.Close();
            }
        }else if(hd->setting.warningIfFileFailed) {
            if (ret) {
                ret = hd->file.Write(logStr.c_str(), logStr.size());
                if(!ret)
                    std::cout << "Failed to write last log to the log file! File path: " << hd->filepath.c_str() << std::endl;
                hd->file.Close();
            } else
                std::cout << "Failed to open log file! File path: " << hd->filepath.c_str() << std::endl;
        }
        if (hd->setting.toConsole || (hd->setting.toConsoleIfFileFailed && !ret))
            std::cout << logStr.c_str();
        hd->mutex.unlock();
        return ret;
    }
}

#undef AA_HANDLE_MANAGER