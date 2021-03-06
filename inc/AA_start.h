﻿/*  
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

#ifndef AA_START_H_2015_11_11
#define AA_START_H_2015_11_11

#ifdef _WIN32

#ifndef AA_USE_STATIC

#if defined ARMYANTLIB_EXPORTS
#define ARMYANTLIB_API __declspec(dllexport)
#else
#define ARMYANTLIB_API __declspec(dllimport)
#endif

#else // AA_USE_STATIC -> true

#define ARMYANTLIB_API

#endif // AA_USE_STATIC

#else // _WIN32 -> _UNIX

#define ARMYANTLIB_API

#endif // _WIN32

// Defination macros comments:

//	language setting:
//		_cplusplus:
//	debug settings:
//		DEBUG:
//		NDEBUG:
//		AA_IS_DEBUG:
//	target machine bit setting:
//		_32BIT:
//		_64BIT:
//	target machine type setting:
//		_x86:
//		_arm:
//	used library type setting
//		AA_USE_STATIC:
//	target OS type setting:
//		OS_WINDOWS:
//			OS_WP8:
//			OS_WINDOWS_SERVER:
//			OS_WINDOWS_OLD:
//		OS_UNIX:
//			OS_LINUX:
//				OS_DEBIAN:
//				OS_UBUNTU:
//				OS_CENTOS:
//				OS_FEDORA:
//				OS_ORACLE:
//				OS_CHROMEOS:
//				OS_ANDROID:
//				OS_DEEPINOS:
//				OS_YUNOS:
//				OS_REDHAT:
//				OS_SUSE:
//			OS_BSD:
//				OS_FREEBSD:
//				OS_MACOS:
//				OS_IOS:
//	compiler and build-tool setting:
//		_MSVC:
//		_GNUC:
//		_MAKEFILE:
//		_CMAKE:
//		_LLVM:
//	The macros which will be added in future:
//		



#endif // AA_START_H_2015_11_11
