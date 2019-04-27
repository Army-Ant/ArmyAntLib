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

#include "../../inc/AAIStream.h"
#include "AAIStream_Private.hxx"
#include <boost/spirit/home/support/detail/endian/endian.hpp>

#define AA_HANDLE_MANAGER ClassPrivateHandleManager<IStream, IStream_Private>::getInstance()

namespace ArmyAnt {

IStream::IStream()
{
}

IStream::~IStream()
{
    delete AA_HANDLE_MANAGER.ReleaseHandle(this);
}

StaticStream::StaticStream()
	:IStream()
{
}

StaticStream::~StaticStream()
{
}

DynamicStream::DynamicStream()
	: IStream()
{
}

DynamicStream::~DynamicStream()
{
}

StaticStream& StaticStream::operator>>(void*buffer)
{
	Read(buffer);
	return *this;
}

StaticStream& StaticStream::operator<<(void*buffer)
{
	Write(buffer);
	return *this;
}


bool StaticStream::operator==(std::nullptr_t) const
{
	return IsEmpty();
}

bool StaticStream::operator!=(std::nullptr_t) const
{
	return !IsEmpty();
}

StaticStream * StaticStream::GetStream(mac_uint handle)
{
	auto ret = AA_HANDLE_MANAGER.GetSourceByHandle(reinterpret_cast<IStream_Private*>(handle));
	if(!ret->IsStatic())
		return nullptr;
	return const_cast<StaticStream*>(static_cast<const StaticStream*>(ret));
}

bool IStream::IsLittleEnding(){
    int32 a = 0x11100100;
    return boost::spirit::detail::load_little_endian<int32, 4>(&a) == a;
}

IStream * IStream::GetStream(mac_uint handle)
{
	return 	const_cast<IStream*>(AA_HANDLE_MANAGER.GetSourceByHandle(reinterpret_cast<IStream_Private*>(handle)));
}

bool StaticStream::MoveBy(int64 pos){
	return MoveTo(pos + GetPos());
}

}

#undef AA_HANDLE_MANAGER