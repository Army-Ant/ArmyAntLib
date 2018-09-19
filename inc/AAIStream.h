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

#ifndef AA_I_STREAM_H_2016_5_11
#define AA_I_STREAM_H_2016_5_11

#include "AADefine.h"
#include "AA_start.h"
#include <cstddef>

namespace ArmyAnt {


enum class StreamType : uint8
{
	None,
	File,
	Memory,
	NamePipe,
	ComData,
	Network		//网络通信功能尚未开发
};


class ARMYANTLIB_API IStream
{
public:
	IStream();
	virtual ~IStream();

public:

	/** 按名称字符串打开流， 不同的流有不同的字符串格式要求 
	  */
	virtual bool Open(const char*src) = 0;

	/** 关闭流
	  */
	virtual bool Close() = 0;

	/** 检验流是否打开中
	  * @ param = "dynamicCheck" : 是否进行实时检查，当该参数为默认或者true时，在非联网状态下，将尝试读取流，联网状态下，将尝试ping目标地址
	  */
	virtual bool IsOpened(bool dynamicCheck = true) = 0;

	/** 获取流源的名称，即调用Open时输入的字符串参数
	  * @ return : 流源的名称
	  */
	virtual const char* GetSourceName() const = 0;

	/** 判断流是否为静态，静态流代表文件、内存这一类数据存储器；非静态流则是网络、管道等一些实时传输的数据通信流
	  * @ return : 静态返回true，非静态返回false
	  */
	virtual bool IsStatic() const = 0;

	/** 检验当前流的类型
	  * @ return : 返回流的类型
	  */
	virtual StreamType GetType() const = 0;

public:

	/**	根据url创建流，需要手动释放内存。
		url由约定俗成的格式组成，具体规则如下：
		file:// 开头，代表磁盘文件，后跟文件路径
		memory:// 开头，代表内存，后跟内存src字符串
		
	  */
	static IStream* Create(const char*url);

	/** 根据流的类型和其对应的src字符串创建相应的流，需要手动释放内存
	  */
	static IStream* Create(StreamType type, const char*src);

	static IStream* GetStream(mac_uint handle);

	AA_FORBID_ASSGN_OPR(IStream);
	AA_FORBID_COPY_CTOR(IStream);
};


/** 静态流，表示对已存在的数据的读写流，包括磁盘文件，内存等等，这种流具有如下特点：
		读取不分同步和异步，不需要监听读取
		读与写使用同一个流缓存，同一个位置指针
		可以在原有数据的任意处插写入数据
  */
class ARMYANTLIB_API StaticStream : public IStream
{
public:
	StaticStream();
	virtual ~StaticStream();

public:
	/** 检查流的长度
	  */
	virtual int64 GetLength() const = 0;

	/** 检查当前读写指针的位置
	  */
	virtual int64 GetPos() const = 0;

	/** 读写指针是否已到流末尾
	  */
	virtual bool IsEndPos() const = 0;

	/* 将读写指针移动到指定位置
	 * @ param = "pos" : 从流开头算起，要移动到的位置。 若为负数, 则从尾部算起, 如-1为移动到结尾
	 */
	virtual bool MoveTo(int64 pos) = 0;

	/* 将读写指针向后移动指定量的字节数
	 * @ param = "pos" : 要移动的字节量, 若为负数, 则向前移动。
	 */
	virtual bool MoveBy(int64 pos);

	/** 读取流中的数据
	  * @ param = "buffer" : 要将数据保存到的位置
	  * @ param = "len" : 要读取的最大长度
	  * @ param = "pos" : 要读取的开始位置，不传此参数则从当前位置就地读取
	  * @ return : 读取到的实际长度，如果为0，可能发生了错误
	  */
	virtual int64 Read(void*buffer, uint32 len = AA_UINT32_MAX, int64 pos = AA_UINT64_MAX) = 0;

	/** 读取流中的数据
	  * @ param = "buffer" : 要将数据保存到的位置
	  * @ param = "endtag" : 读取到此值的字节数据时，停止
	  * @ param = "maxlen" : 要读取的最大长度
	  * @ return : 读取到的实际长度，如果为0，可能发生了错误
	  */
	virtual int64 Read(void*buffer, uint8 endtag, int64 maxlen = AA_UINT64_MAX) = 0;

	/** 将数据写入流
	  * @ param = "buffer" : 要写入的数据所在的位置
	  * @ param = "len" : 要写入的长度，不传此参数，则当遇到数据中的0值（字符串结尾）时停止写入
	  * @ return : 写入的实际长度，如果为0，可能发生了错误
	  */
	virtual int64 Write(const void*buffer, int64 len = 0) = 0;

	/** 判断流是否为空
	  */
	virtual bool IsEmpty() const = 0;

public:

	/** 将流内容写入指定位置，写入部分为从当前指针处到文件结尾，如果在到达结尾前已经写入的长度达到FILE_SHORT_POS_END，则停止
	  * @ param = "buffer" : 要写入的区域
	  */
	virtual StaticStream&operator>>(void*buffer) final;

	/** 将目标字符串内容写入流，该内容遇到0数据（字符串结尾）时停止写入
	  * @ param = "buffer" : 要写入的区域
	  */
	virtual StaticStream&operator<<(void*buffer) final;

	/** 判断流是否为空
	  */
	virtual bool operator==(std::nullptr_t) const final;

	/** 判断流是否不为空
	  */
	virtual bool operator!=(std::nullptr_t) const final;

public:
	virtual bool IsStatic() const final { return true; }

	static StaticStream* GetStream(mac_uint handle);

	AA_FORBID_ASSGN_OPR(StaticStream);
	AA_FORBID_COPY_CTOR(StaticStream);
};


class ARMYANTLIB_API DynamicStream : public IStream
{
public:
	DynamicStream();
	virtual ~DynamicStream();

public:

public:
	virtual bool IsStatic() const final { return false; }

	AA_FORBID_ASSGN_OPR(DynamicStream);
	AA_FORBID_COPY_CTOR(DynamicStream);
};

} // namespace ArmyAnt

#endif // AA_I_STREAM_H_2016_5_11
