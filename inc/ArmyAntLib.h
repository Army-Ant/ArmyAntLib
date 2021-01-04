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

#ifndef ARMYANT_LIB_H_2016_3_19
#define ARMYANT_LIB_H_2016_3_19

#include "AADefine.h"
#include "AA_start.h"

#include <cstring>

namespace ArmyAnt {

namespace Infos {

// define the version class to get the library version
// 定义关于版本的结构，用于获取本库的版本
// TODO: 是否需要附加宏以支持编译时判断库版本？
union ARMYANTLIB_API Version
{
public:
	uint32 ver;
	struct ARMYANTLIB_API VerChar
	{
		uint8 majorVer;		// 大版本
		uint8 minorVer;		// 小版本
		uint8 patchVer;		// 补丁
		uint8 buildVer;		// 构建

		VerChar(uint8 majorVer, uint8 minorVer, uint8 patchVer, uint8 buildVer);
	}sver;

	Version(uint32 ver);
	Version(uint8 majorVer, uint8 minorVer, uint8 patchVer, uint8 buildVer);

public:


public:
	// The version of this library and other referenced libraries
	// 获取本库或者所引用的第三方库的版本号
	// Get this library's version without param
	// 若要获取本库版本号，请无参调用
	// Avaliable param values: boost python
	// 可选参数：boost, python
	static const Version GetVersion(const char* obj = nullptr);
};

// define the runtime enviroment infomation singleton class
// 定义该单例用于获取关于运行时环境的有关信息
class ARMYANTLIB_API Enviroment
{
public:
	enum class OS : int16
	{
		// Big class
		Unix = 0b00000000,
		Windows = 0b00000001,

		// Medium class
		// For Unix
		Linux = 0b0000000000000100,
		BSD = 0b0000000000001000,
		Solaris = 0b0000000000001100,
		Minix = 0b0000000000010000,
		OpenServer = 0b0000000000010100,
		QNX = 0b0000000000011000,
		// For Windows
		WindowsClassic = 0b0000000000000101,
		WindowsPhone = 0b0000000000001001,
		WindowsServer = 0b0000000000001101,

		// Small class
		// For Linux
		Ubuntu = 0b0000000000100100,
		Debian = 0b0000000001000100,
		ChromeOS = 0b0000000001100100,
		Fedora = 0b0000000010000100,
		YunOS = 0b0000000010100100,
		RedHat = 0b0000000011000100,
		Android = 0b0000000011100100,
		CentOS = 0b0000000100000100,
		OpenSUSE = 0b0000000100100100,
		Oracle = 0b0000000101000100,
		Deepin = 0b0000000101100100,
		ArchLinux = 0b0000000110000100,
		Gentoo = 0b0000000110100100,
		Mandriva = 0b0000000111000100,
		TurboLinux = 0b0000000111100100,
		Xandros = 0b0000001000000100,
		// For BSD
		FreeBSD = 0b0000000000101000,
		OpenBSD = 0b0000000001001000,
		NetBSD = 0b0000000001101000,
		AIX = 0b0000000010001000,
		macOS = 0b0000000010101000,
		iOS = 0b0000000011001000,
	};

	enum class BITS : int8
	{
		_8bits,
		_16bits,
		_32bits,
		_64bits,
		_128bits
	};

	enum class DevLanguage : int8
	{
		Unknown,
		Assembly_Intel,
		Assembly_AMD,
		Assembly_ARM,
		COBOL,
		Fortrain,
		Basic,
		C,
		SmartTalk,
		Matlab,
		Perl,
		Shell,
		CPlusPlus,
		VisualBasic,
		Batch,
		Java,
		JavaScript,
		PHP,
		ObjectiveC,
		CSharp,
		Python,
		Lua,
		Ruby,
		Go,
		FSharp,
		Swift,
		ActionScript,
		Delphi,
		R,
		D,
		Groovy,
		Lisp,
		Scala,
		Ada
	};

	static OS GetOSType();
	static Version GetOSVersion();
	static BITS GetOSBits();
};

} // namespace Infos

namespace Utils {

// math utilities
// TODO: 在Math类工具完成时，应当将这些函数转移到Math模块
namespace Math{
ARMYANTLIB_API double GetRandom(double min, double max);
ARMYANTLIB_API bool GetRoll(double percent);
ARMYANTLIB_API uint32 GetMultiRoll(double percent);
}

}

} // namespace ArmyAnt

#endif // ARMYANT_LIB_H_2016_3_19