/*	*
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
 
#ifndef BASE_HPP_2015_11_11
#define BASE_HPP_2015_11_11

#include "../../inc/AADefine.h"

/*
#ifdef DEBUG
#define BOOST_ENABLE_ASSERT_DEBUG_HANDLER
#else
#define BOOST_ENABLE_ASSERT_HANDLER
#endif
*/

// 使用boost的断言工具来进行库开发的debug
#include <boost/assert.hpp>
#ifdef DEBUG
#undef AAAssert
#define AAAssert(x,y) BOOST_ASSERT((x))
#endif

// 不允许boost使用自动指针
#define BOOST_NO_AUTO_PTR
// 输出boost链接信息以排查问题
#define BOOST_LIB_DIAGNOSTIC


#ifdef OS_WINDOWS
// boost需要定义WINNT
#ifndef _WIN32_WINNT
#define _WIN32_WINNT 0x0600
#endif

#endif

using namespace ArmyAnt::Fragment;

#ifndef AA_USE_PYTHON
#define AA_USE_PYTHON 1
#endif

#endif // BASE_HPP_2015_11_11
