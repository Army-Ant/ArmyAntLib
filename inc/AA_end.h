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

#ifndef A_A_END_2015_11_11
#define A_A_END_2015_11_11

#if !defined ARMYANTLIB_EXPORTS && ! defined AA_DONOT_AUTO_LINK_

#if defined DEBUG || defined _DEBUG
#define AA_IS_DEBUG "_d"
#else 
#define AA_IS_DEBUG
#endif

#if defined _64BIT && defined _arm
#define AA_TARGET_MACHINE "_arm64"
#elif defined _64BIT
#define AA_TARGET_MACHINE "_64"
#elif defined _arm
#define AA_TARGET_MACHINE "_arm"
#else
#define AA_TARGET_MACHINE ""
#endif

#if defined AA_USE_STATIC
#define AA_LIB_TYPE "_s"
#else
#define AA_LIB_TYPE
#endif

#ifdef OS_WINDOWS
#pragma comment(lib,"lib\\ArmyAntLib" AA_LIB_TYPE AA_IS_DEBUG AA_TARGET_MACHINE ".lib")
#elif defined OS_BSD // OS_WINDOWS

#else // OS_WINDOWS
#include <dlfcn.h>

namespace ArmyAnt {

// Load library automatically

inline const auto&importArmyAnt() 
{
	struct ArmyAnt 
	{
		ArmyAnt()
		{
			ptr = dlopen("libArmyAnt" AA_LIB_TYPE AA_IS_DEBUG AA_TARGET_MACHINE ".so", RTLD_NOW);
		}
		~ArmyAnt()
		{
			dlclose(ptr);
		}
		void*ptr = nullptr;
	};
	static ArmyAnt ret; 
	return ret.ptr; 
}; 

}
#endif // OS_WINDOWS

#undef AA_IS_DEBUG
#undef AA_TARGET_MACHINE
#undef AA_LIB_TYPE

#endif //  !defined ARMYANTLIB_EXPORTS && ! defined AA_DONOT_AUTO_LINK_

#endif // A_A_END_2015_11_11
