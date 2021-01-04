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
#include "../../inc/ArmyAntLib.h"
#include "../../inc/AAString.h"
#include <boost/random.hpp>
#include <boost/version.hpp>

namespace ArmyAnt {

namespace Infos {

Version::VerChar::VerChar(uint8 majorVer, uint8 minorVer, uint8 patchVer, uint8 buildVer)
	:majorVer(majorVer), minorVer(minorVer), patchVer(patchVer), buildVer(buildVer)
{
}

Version::Version(uint32 ver)
	: ver(ver)
{
}

Version::Version(uint8 majorVer, uint8 minorVer, uint8 patchVer, uint8 buildVer)
	: sver(majorVer, minorVer, patchVer, buildVer)
{
}

const Version Version::GetVersion(const char* obj)
{
	String objstr = obj;
	if(objstr == "boost")
		return Version(BOOST_VERSION / 100000, BOOST_VERSION / 100 % 1000, BOOST_VERSION % 100, 0);
	else if(objstr == "python")
		return Version(3, 5, 1, 0);
	else if(objstr == "")	// Self library's version
		return Version(0, 0, 0, 2);
	else return Version(0, 0, 0, 0);
}

Enviroment::BITS Enviroment::GetOSBits()
{
#if defined _32BIT
	return Enviroment::BITS::_32bits;
#elif defined _64BIT
	return Enviroment::BITS::_64bits;
#endif
}

} // namespace Infos

namespace Utils {

	double Math::GetRandom(double min, double max)
	{
		boost::uniform_real<double> ret(min, max);
		boost::random::mt19937_64 eng(time_t(0));
		return ret(eng);
	}

	bool Math::GetRoll(double percent)
	{
		return GetRandom(0,100)<percent;
	}

	uint32 Math::GetMultiRoll(double percent)
	{
		uint32 ret = 0;
		if (percent >= 100)
			return AA_UINT32_MAX;
		while (GetRoll(percent)) {
			ret++;
		}
		return ret;
	}


}

} // namespace ArmyAnt
