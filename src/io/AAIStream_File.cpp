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
#include "../../inc/AAIStream_File.h"
#include "AAIStream_Private.hxx"
#include "../../inc/AAClassPrivateHandle.hpp"
#include "../../inc/AAString.h"
#include <list>
#include <mutex>

#ifdef OS_WINDOWS
#include <windows.h>
#undef CopyFile
#undef DeleteFile

#elif defined OS_UNIX // ifdef OS_WINDOWS
#define _FILE_OFFSET_BITS 64
#include <sys/stat.h>
#include <fcntl.h>
#include <memory.h>
#include <sys/errno.h>
#include <unistd.h>
#include <sys/types.h>

#ifdef OS_ANDROID

#elif defined OS_MACOS  // ifdef OS_ANDROID

#endif // ifdef OS_ANDROID    elif defined OS_MACOS

#endif // ifdef OS_WINDOWS    elif defined OS_UNIX


#define AA_HANDLE_MANAGER ClassPrivateHandleManager<IStream, IStream_Private>::getInstance()

namespace ArmyAnt {

//封装类私有成员的保护器
class IStream_File_Private : public IStream_Private
{
public:
	IStream_File_Private() {};
	~IStream_File_Private()	{};

	//是否在打开时不允许文件不存在
	bool nocreate = true;
	//是否在打开时不允许文件已存在
	bool noexist = false;
	//文件名
	String name = "";
	//文件指针,
	FILE*file = nullptr;
	//读写锁
	std::mutex mutex;

	inline int Fseek(int64 offset, int whence)
	{
		return Fseek(file, offset, whence);
	}

	static inline int Fseek(FILE*file, int64 offset, int whence)
	{
#if defined OS_WINDOWS && !defined _CMAKE
		return _fseeki64(file, offset, whence);
#else
		return fseek(file, offset, whence);
#endif
	}

	static inline int64 GetFPos(fpos_t pos){
#if defined OS_WINDOWS || defined OS_BSD
        return pos;
#else
        return pos.__pos;
#endif
	}
    static inline void SetPos(fpos_t&pos, int64 value) {
#if defined OS_WINDOWS || defined OS_BSD
		pos = value;
#else
		pos.__pos = value;
#endif
	}

private:
	AA_FORBID_COPY_CTOR(IStream_File_Private);
	AA_FORBID_ASSGN_OPR(IStream_File_Private);
};


/******************************** Class File *********************************************************/

File::File()
	:StaticStream()
{
    AA_HANDLE_MANAGER.GetHandle(this, new IStream_File_Private());
}

File::~File()
{
}

bool File::SetStreamMode(bool nocreate /*= true*/, bool noexist /*= false*/)
{
	//不能两者同时为false
	if(nocreate&&noexist)
		return false;
	//保存设定
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	hd->nocreate = nocreate;
	hd->noexist = noexist;
	return true;
}

bool File::Open(const char* filepath)
{
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	//type没有重置，说明本流尚未关闭
	if(hd->file != nullptr)
		return false;
	AAAssert(filepath != nullptr, false);
	//根据exist和created的允许设定，判断文件存在情况
	bool fexist = IsFileExist(filepath);
	if(hd->nocreate&&!fexist)
		return false;
	if(hd->noexist&&fexist)
		return false;
	hd->mutex.lock();
	//打开文件，并保存文件名
#if defined OS_WINDOWS
	if(fopen_s(&hd->file, filepath, "rb+") != 0){
		if(hd->nocreate){
			hd->mutex.unlock();
			return false;
		}
		if(fopen_s(&hd->file, filepath, "wb") != 0){
			hd->mutex.unlock();
			return false;
		}
		fclose(hd->file);
		if(fopen_s(&hd->file, filepath, "rb+") != 0){
			hd->mutex.unlock();
			return false;
		}
	}
#else
	hd->file = fopen(filepath, "rb+");
	if(hd->file == nullptr){
		if(hd->nocreate){
			hd->mutex.unlock();
			return false;
		}
		hd->file = fopen(filepath, "wb");
		if(hd->file == nullptr){
			hd->mutex.unlock();
			return false;
		}
		fclose(hd->file);
		hd->file = fopen(filepath, "rb+");
		if(hd->file == nullptr){
			hd->mutex.unlock();
			return false;
		}
	}
#endif
	hd->name = filepath;
	hd->mutex.unlock();
	return true;
}


/*
bool File::Open(const char* pipename, const char*pipePath, const char*pipeServer / *= "."* /)
{
	auto hd = handleManager[this];
	if(hd->type != StreamType::None)
		return false;
	AAAssert(pipename != nullptr, false);
	//打开命名管道
	hd->name = String("\\\\") + pipeServer + "\\pipe\\" + pipePath + pipename;
	hd->file = fopen(hd->name.c_str(), "wb+");
	if(hd->file == nullptr)
	{
		if(hd->nocreate || 0 == (hd->pipeHandle =

#ifdef OS_WINDOWS

		   CreateNamedPipeA(hd->name.c_str(), PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED, PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE | PIPE_WAIT, PIPE_UNLIMITED_INSTANCES - 1, 0, 0, 0, 0)
#else
		   mkfifo(hd->name.c_str(), O_RDWR | O_CREAT)
#endif
		   )|| nullptr == (hd->file = fopen(hd->name.c_str(), "wb+")))
		{
			hd->name = "";
			return false;
		}
		std::function<void(ArmyAnt::IStream_File_Private*)> func = hd->NamePipeReader;
		hd->pipeReader = new std::thread(func,&(*hd));
	}
	hd->type = StreamType::NamePipe;
	if(hd->file != nullptr&&hd->noexist)
		Close();
	return hd->file != nullptr;
}
*/

bool File::Close()
{
	bool ret = true;
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	if (hd->file == nullptr)
		return true;
	//根据相应的类型进行关闭
	hd->mutex.lock();
	ret = 0 == fclose(hd->file);
	hd->mutex.unlock();
	hd->file = nullptr;
	hd->name = "";
	return ret;
}

bool File::IsOpened(bool dynamicCheck/* = true*/)
{
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	if(hd->file == nullptr)
		return false;
	else if(!dynamicCheck)
		return true;

	//动态检查是否打开，防止意外断开
	uint8 c;
	try
	{
		return 0 < (Read(static_cast<void*>(&c), uint32(1), 0));
	}
	catch(std::exception)
	{
		return false;
	}
}

int64 File::GetLength() const
{
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	//根据类型获取长度
	hd->mutex.lock();
	fpos_t nowpos;
	IStream_File_Private::SetPos(nowpos, 0);
	fgetpos(hd->file, &nowpos);
	//fseek(hd->file, 0, SEEK_END);
	hd->Fseek(0, SEEK_END);
	fpos_t pos;
	fgetpos(hd->file, &pos);
	hd->Fseek(IStream_File_Private::GetFPos(nowpos), SEEK_SET);
	auto ret = IStream_File_Private::GetFPos(pos);
	hd->mutex.unlock();
	return ret;
}

int64 File::GetPos() const
{
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	//根据类型获取当前读写位置
	fpos_t ret;
	fgetpos(hd->file, &ret);
	return IStream_File_Private::GetFPos(ret);
}

bool File::IsEndPos() const
{
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	return feof(hd->file) != 0;
}

bool File::MoveTo(int64 pos){
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	auto len = GetLength();
	if(pos < 0)
		pos = len + pos + 1;
	if(pos < 0)
		pos = 0;
	else if(pos > len)
		pos = len;
	hd->mutex.lock();
	hd->Fseek(pos, SEEK_SET);
	hd->mutex.unlock();
	return true;
}

const char* File::GetSourceName() const
{
	return	static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this])->name.c_str();
}

int64 File::Read(void*buffer, uint32 len /*= Constant::c_uint32Max*/, int64 pos /*= Constant::c_uint64Max*/)
{
	AAAssert(buffer != nullptr, int64(0));
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	//记录当前位置。如果参数制定了要开始读取的位置，则读取过后要返回到原位置
	hd->mutex.lock();
	fpos_t now;
	fgetpos(hd->file, &now);
	hd->Fseek(0, SEEK_END);
	fpos_t wholelen;
	fgetpos(hd->file, &wholelen);
	//标记是否在读取结束后返回初始位置
	bool isCurPos = false;
	if(pos == AA_UINT64_MAX)
	{
		isCurPos = true;
		pos = IStream_File_Private::GetFPos(now);
	}
	hd->Fseek(pos, SEEK_SET);
	auto readedLen = min(IStream_File_Private::GetFPos(wholelen), len);
	auto realReaded = fread(buffer, 1, readedLen, hd->file);
	if(!isCurPos)
		hd->Fseek(IStream_File_Private::GetFPos(now), SEEK_SET);
	hd->mutex.unlock();
	return int64(realReaded);
}

int64 File::Read(void*buffer, uint8 endtag, int64 maxlen/* = FILE_SHORT_POS_END*/)
{
	AAAssert(buffer != nullptr, int64(0));
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	int64 len = 0;
	fpos_t wholeLen;
	hd->mutex.lock();
	IStream_File_Private::SetPos(wholeLen, GetLength());
	size_t sz = 0;
	uint8 a = 0;
	//根据类型进行读取。磁盘文件、命名管道和串口可以统一调用库函数
	//逐字读取
	for(auto nowpos = GetPos(); nowpos < IStream_File_Private::GetFPos(wholeLen) || len == maxlen; nowpos++)
	{
		sz = fread(&a, 1, 1, hd->file);
		if(a == endtag || sz < 1)
			return len;
		else
		{
			static_cast<uint8*>(buffer)[len++] = a;
		}
	}
	hd->mutex.unlock();
	return len;
}

int64 File::Write(const void*buffer, int64 len /*= 0*/)
{
	AAAssert(buffer != nullptr, int64(0));
	auto hd = static_cast<IStream_File_Private*>(AA_HANDLE_MANAGER[this]);
	//如果len参数没有传入，则写内存到流，直至遇到0，这相当于写入字符串至流
	if(len == 0)
		while(static_cast<const uint8*>(buffer)[len] != 0)
			len++;

	int64 writeLen = 0;
	hd->mutex.lock();

#ifdef OS_WINDOWS
	// So much posix old OS file reading and writing operation need to call the "fseek" function between the nearly fwrite and fread,
	// The modern Unix system do not have this problem any more, but Windows still do
	hd->Fseek(0L, 1);
#endif
	fflush(hd->file);
	writeLen = fwrite(buffer, 1, len, hd->file);
	fflush(hd->file);
#ifdef OS_WINDOWS
	hd->Fseek(0L, 1);
#endif

	hd->mutex.unlock();
	return writeLen;
}

bool File::IsEmpty() const
{
	return const_cast<File*>(this)->IsOpened(false);
}

bool File::CopyFile(const char*srcPath, const char*dstPath)
{
	//只读打开源文件和目标文件，前者应当成功，后者应当失败
	auto srcfp = fopen(srcPath, "rb");
	auto dstfp = fopen(dstPath, "rb");
	if(srcfp == nullptr || dstfp != nullptr)
		return false;
	//创建目标文件
	dstfp = fopen(dstPath, "wb");
	if(dstfp == nullptr)
		return false; 
	//开辟拷贝文件的临时内存，先尝试申请1MB
	uint32 memlen = 1048576;
	char*cpmem = new char[memlen];
	//如果申请失败，则尝试申请1kB
	if(cpmem == nullptr)
	{
		memlen = 1024;
		cpmem = new char[memlen];
		AAAssert(cpmem != nullptr, false);
	}
	//检测源文件的长度
	fpos_t flen;
	IStream_File_Private::Fseek(srcfp, 0, SEEK_END);
	fgetpos(srcfp, &flen);
	IStream_File_Private::Fseek(srcfp, 0, SEEK_SET);
	//拷贝
	for(int64 i = 0; i < IStream_File_Private::GetFPos(flen); i+=memlen)
	{
		auto readed = fread(cpmem, memlen, 1, srcfp);
		fwrite(cpmem, readed, 1, dstfp);
		fflush(dstfp);
	}
	//保存并关闭文件
	fclose(srcfp);
	fclose(dstfp);
	//释放内存
	AA_SAFE_DELALL(cpmem);
	return true;
}

bool File::MoveOrRenameFile(const char*srcPath, const char*dstPath)
{
	return 0 == rename(srcPath, dstPath);
}

bool File::DeleteFile(const char*path)
{
	return 0 == remove(path);
}

bool File::IsFileExist(const char*path)
{
	//尝试只读打开文件
	auto ret = fopen(path, "rb");
	if(ret != nullptr)
	{
		fclose(ret);
		return true;
	}
	return false;
}

File* File::GetStream(mac_uint handle)
{
	return const_cast<File*>(static_cast<const File*>(AA_HANDLE_MANAGER.GetSourceByHandle(reinterpret_cast<IStream_Private*>(handle))));
}

bool File::operator^=(const char* filename)
{
	File fp;
	if(!fp.Open(filename))
		return false;
	//统计要读取的长度
	auto flen = GetLength()- GetPos();
	//申请内存拷贝临时区1MB
	uint32 memlen = 1048576;
	char*cpmem = new char[memlen];
	//申请失败，则尝试申请1kB
	if(cpmem == nullptr)
	{
		memlen = 1024;
		cpmem = new char[memlen];
		AAAssert(cpmem != nullptr, false);
	}
	//进行数据拷贝
	for(int64 i = 0; i < flen; i += memlen)
	{
		int64 readed = fp.Read(cpmem, memlen);
		if(0 == readed)
		{
			fp.Close();
			return false;
		}
		if(0 == Write(cpmem, readed))
		{
			fp.Close();
			return false;
		}
	}
	fp.Close();
	return true;
}

} // namespace ArmyAnt

#undef AA_HANDLE_MANAGER
