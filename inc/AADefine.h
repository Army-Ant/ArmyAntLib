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

#ifndef A_A_DEFINE_H_2015_11_11
#define A_A_DEFINE_H_2015_11_11

#include "C_AADefine.h"
#include "AAFragment.h"

#ifdef OS_BSD
// Xcode编译发现需要此头
#include <cstddef>
#endif

// 添加在类中, 禁止默认的拷贝构造函数(需要自定义拷贝构造函数时请勿使用)
#define AA_FORBID_COPY_CTOR(typen) public: typen(typen&)=delete
// 添加在类中, 禁止默认的拷贝赋值运算符(需要重载拷贝赋值运算符时请勿使用)
#define AA_FORBID_ASSGN_OPR(typen) public: typen& operator=(typen&)=delete
// 添加在类中, 将类变为静态抽象类,禁止创建类的任何实例，也禁止继承
#define AA_FORBID_CREATE_OB(typen) public: virtual void Forbid_Create___##typen##_____Real____Virtual__Function()=0

#endif // A_A_DEFINE_H_2015_11_11
