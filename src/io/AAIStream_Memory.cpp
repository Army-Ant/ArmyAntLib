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

#include "../../inc/AAIStream_Memory.h"
#include "AAIStream_Private.hxx"

#ifndef OS_WINDOWS
#include <cstdlib>
#endif

#define AA_HANDLE_MANAGER ClassPrivateHandleManager<IStream, IStream_Private>::getInstance()

namespace ArmyAnt {

class IStream_Memory_Private : public IStream_Private
{
public:
	IStream_Memory_Private() {}
	~IStream_Memory_Private() {}

public:
	void* mem = nullptr;
	int64 len = 0;
	int64 pos = 0;
	bool needFree = false;
};

Memory::Memory()
	:StaticStream()
{
    AA_HANDLE_MANAGER.GetHandle(this, new IStream_Memory_Private());
}

Memory::~Memory()
{
}

bool Memory::Open(const char * src)
{
	// TODO: Fill the memory string openning function
	return false;
}

bool Memory::Open(uint32 len)
{
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	if(len <= 0)
		return false;
	hd->mem = malloc(len);
	if(hd->mem == nullptr)
		return false;
	hd->len = len;
	hd->needFree = true;
	return true;
}

bool Memory::Open(mac_uint memaddr, int64 len)
{
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	//type没有重置，说明本流尚未关闭
	if(hd->mem != nullptr)
		return false;
	AAAssert(memaddr != 0, false);
	//保存内存地址
	hd->mem = reinterpret_cast<void*>(memaddr);
	//测试内存能否读写
	try
	{
		uint8 a = (static_cast<uint8*>(hd->mem))[0];
		char*p = static_cast<char*>(hd->mem);
		p[0] = 1;
		p[0] = 2;
		p[0] = a;
		a = (static_cast<uint8*>(hd->mem))[len - 1];
		p[len - 1] = 1;
		p[len - 1] = 2;
		p[len - 1] = a;
	}
	//如果不能读写，则取消打开该流
	catch(std::exception&)
	{
		hd->mem = nullptr;
		return false;
	}
	hd->len = len;
	return true;
}

bool Memory::Open(void* memaddr, int64 len)
{
	return Open(reinterpret_cast<mac_uint>(memaddr), len);
}

bool Memory::Close()
{
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->needFree)
		free(hd->mem);
	hd->mem = nullptr;
	hd->len = 0;
	hd->needFree = false;
	return true;
}

bool Memory::IsOpened(bool)
{
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->mem != nullptr && hd->len > 0;
}

int64 Memory::GetLength() const
{
	return static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this])->len;
}

int64 Memory::GetPos() const
{
	return static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this])->pos;
}

bool Memory::IsEndPos() const
{
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	return hd->pos == hd->len && hd->len > 0;
}

bool Memory::MoveTo(int64 pos)
{
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->len <= 0)
		return false;
	if(hd->pos > hd->len)
		hd->pos = hd->len;
	else
		hd->pos = pos;
	return true;
}

const char * Memory::GetSourceName() const
{
	static bool lock = false;
	static char name[16] = "";
	if(lock)
		return nullptr;
	lock = true;
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	//保存内存地址、长度等信息
#ifdef _32BIT
	sprintf(name, "%X", mac_uint(hd->mem));
#else
	sprintf(name, "%X%X", uint32(reinterpret_cast<mac_uint>(hd->mem) / AA_UINT32_MAX), uint32(reinterpret_cast<mac_uint>(hd->mem) % AA_UINT32_MAX));
#endif
	lock = false;
	return name;
}

void * Memory::GetMemory()
{
	return static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this])->mem;
}

const void * Memory::GetMemory() const
{
	return const_cast<Memory*>(this)->GetMemory();
}

int64 Memory::Read(void * buffer, uint32 len, int64 pos)
{
	AAAssert(buffer != nullptr, int64(0));
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	bool isCurPos = false;
	if(pos == AA_UINT64_MAX)
	{
		isCurPos = true;
		pos = hd->pos;
	}
	if(pos >= hd->len)
		return false;
	//内存拷贝
	int64 reallen = Fragment::min(hd->len - pos, len);
	memcpy(static_cast<char*>(buffer), static_cast<char*>(hd->mem) + pos, uint32(reallen));
	if(isCurPos)
		hd->pos += reallen;
	return reallen;
}

int64 Memory::Read(void * buffer, uint8 endtag, int64 maxlen)
{
	AAAssert(buffer != nullptr, int64(0));
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	//逐字读取
	for(auto nowPos = hd->pos; ; nowPos++)
	{
		if((nowPos < hd->len && static_cast<uint8*>(hd->mem)[nowPos] == endtag) || nowPos - hd->pos == maxlen)
		{
			auto alllen = int64(nowPos - hd->pos);
			memcpy(static_cast<char*>(buffer), static_cast<char*>(hd->mem) + hd->pos, alllen);
			hd->pos = nowPos;
			return alllen;
		}
	}
}

int64 Memory::Write(const void * buffer, int64 len)
{
	AAAssert(buffer != nullptr, int64(0));
	auto hd = static_cast<IStream_Memory_Private*>(AA_HANDLE_MANAGER[this]);
	//如果len参数没有传入，则写内存到流，直至遇到0，这相当于写入字符串至流
	if(len == 0)
		while(static_cast<const uint8*>(buffer)[len] != 0)
			len++;

	int64 writeLen = 0;
	writeLen = int64(Fragment::min(len, hd->len - hd->pos));
	memcpy(static_cast<uint8*>(hd->mem) + hd->pos, buffer, writeLen);
	hd->pos += writeLen;
	return writeLen;
}

bool Memory::IsEmpty() const
{
	return const_cast<Memory*>(this)->IsOpened();
}

}

#undef AA_HANDLE_MANAGER
