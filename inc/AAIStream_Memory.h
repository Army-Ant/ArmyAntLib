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
	*/

#ifndef AA_I_STREAM_MEMORY_H_2016_5_11
#define AA_I_STREAM_MEMORY_H_2016_5_11

#include "AAIStream.h"
#include <cstring>

namespace ArmyAnt {

class ARMYANTLIB_API Memory : public StaticStream
{
public:
	Memory();
	~Memory();

public:

	/** 打开内存
	  * @ param = "src" : 要打开的内存地址以及长度信息字符串
	  */
	virtual bool Open(const char* src) override;

	/** 新打开一段指定长度的内存
	  */
	bool Open(uint32 len);

	/* 按内存地址数据打开内存或共享内存
	* @ param = "memaddr" : 要内存地址编号
	* @ param = "len" : 可读写的内存块总大小
	*/
	bool Open(mac_uint memaddr, int64 len);

	/* 按内存指针打开内存或共享内存
	* @ param = "memaddr" : 要内存起始指针
	* @ param = "len" : 可读写的内存块总大小
	*/
	bool Open(void* memaddr, int64 len);

	/* 关闭流，任何类型的流都要用此方式关闭
	*/
	virtual bool Close() override;

	/* 检验流是否打开中
	* @ param = "dynamicCheck" : 此参数在此内存流类中无实际意义
	*/
	virtual bool IsOpened(bool dynamicCheck = true) override;

	/* 检验当前流的类型
	* @ return : 返回流的类型
	*/
	virtual StreamType GetType() const final { return StreamType::File; };

	/* 检查流的长度
	*/
	virtual int64 GetLength() const override;

	/* 检查当前读写指针的位置
	*/
	virtual int64 GetPos() const override;

	/* 读写指针是否已到流末尾
	*/
	virtual bool IsEndPos() const override;

	/* 将读写指针移动到指定位置
	* @ param = "pos" : 从流开头算起，要移动到的位置。 若为负数, 则从尾部算起, 如-1为移动到结尾
	*/
	virtual bool MoveTo(int64 pos) override;

	/* 获取流源的文件名，对于文件指其路径名称，对于管道则是管道全名，对于串口则是串口名，对于内存和共享内存，则是内存地址号，对于网络，则是其域名或者ip地址和端口
	*/
	virtual const char* GetSourceName() const override;

	void* GetMemory();
	const void* GetMemory()const;

	/* 读取流中的数据
	* @ param = "buffer" : 要将数据保存到的位置
	* @ param = "len" : 要读取的最大长度
	* @ param = "pos" : 要读取的开始位置，不传此参数则从当前位置就地读取
	* @ return : 读取到的实际长度，如果为0，可能发生了错误
	*/
	virtual int64 Read(void*buffer, uint32 len = AA_UINT32_MAX, int64 pos = AA_UINT64_MAX)override;

	/* 读取流中的数据
	* @ param = "buffer" : 要将数据保存到的位置
	* @ param = "endtag" : 读取到此值的字节数据时，停止
	* @ param = "maxlen" : 要读取的最大长度
	* @ return : 读取到的实际长度，如果为0，可能发生了错误
	*/
	virtual int64 Read(void*buffer, uint8 endtag, int64 maxlen = AA_UINT64_MAX)override;

	/* 将数据写入流
	* @ param = "buffer" : 要写入的数据所在的位置
	* @ param = "len" : 要写入的长度，不传此参数，则当遇到数据中的0值（字符串结尾）时停止写入
	* @ return : 写入的实际长度，如果为0，可能发生了错误
	*/
	virtual int64 Write(const void*buffer, int64 len = 0)override;

	virtual bool IsEmpty()const override;

public:
	AA_FORBID_ASSGN_OPR(Memory);
	AA_FORBID_COPY_CTOR(Memory);
};


} // namespace ArmyAnt

#endif // AA_I_STREAM_MEMORY_H_2016_5_11