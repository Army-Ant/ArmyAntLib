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

#ifndef AA_SQL_STRUCTS_H_2017_4_1
#define AA_SQL_STRUCTS_H_2017_4_1

#include "AAString.h"

namespace ArmyAnt
{

// 数据库字段类型, 不同的数据库, 类型不同
enum class SqlFieldType : uint8
{
	Unknown,
    Null,
	UpdateResult,

	MySql_BIT,
    MySql_CHAR,
    MySql_VARCHAR,
    MySql_ENUM,
    MySql_SET,
    MySql_TINYINT,
    MySql_SMALLINT,
    MySql_MEDIUMINT,
    MySql_INT,
    MySql_BIGINT,
    MySql_FLOAT,
    MySql_DOUBLE,
	MySql_DEMICAL,
	MySql_NUMERIC,
    MySql_DATE,
	MySql_DATETIME,
    MySql_TIMESTAMP,
    MySql_TIME,
	MySql_YEAR,
	MySql_GEOMETRY,
	MySql_BINARY,
	MySql_VARBINARY,
	MySql_LONGVARCHAR,
	MySql_LONGVARBINARY,
	MySql_JSON,

    MsAccess_Currency,
    MsAccess_AutoNumber,
    MsAccess_YesNo,
    MsAccess_Hyperlink,
    MsAccess_Text,// = MySql_Varchar,
    MsAccess_Memo,// = MySql_Text,
    MsAccess_Byte,// = MySql_TinyInt,
    MsAccess_Integer,// = MySql_SmallInt,
    MsAccess_Long,// = MySql_Int,
    MsAccess_Single,// = MySql_Float,
    MsAccess_Double,// = MySql_Double,
    MsAccess_DateTime,// = MySql_DateTime,
    MsAccess_OleObject,// = MySql_LongBlob,
    MsAccess_LookupWizard,// = MySql_Enum,

    MsSqlServer_char,
    MsSqlServer_varchar,
    MsSqlServer_text,
    MsSqlServer_nchar,
    MsSqlServer_nvarchar,
    MsSqlServer_ntext,
    MsSqlServer_bit,
    MsSqlServer_binary,
    MsSqlServer_varbinary,
    MsSqlServer_image,
    MsSqlServer_tinyint,
    MsSqlServer_smallint,
    MsSqlServer_int,
    MsSqlServer_bigint,
    MsSqlServer_decimal,
    MsSqlServer_numeric,
    MsSqlServer_smallmoney,
    MsSqlServer_money,
    MsSqlServer_float,
    MsSqlServer_real,
    MsSqlServer_datetime,
    MsSqlServer_datetime2,
    MsSqlServer_smalldatetime,
    MsSqlServer_date,
    MsSqlServer_time,
    MsSqlServer_datetimeoffset,
    MsSqlServer_timestamp,
    MsSqlServer_sql_variant,
    MsSqlServer_uniqueidentifier,
    MsSqlServer_xml,
    MsSqlServer_cursor,
    MsSqlServer_table,

    MsExcel_Normal
};

// 条件语句类型
enum class SqlClauseType
{
    Null,
    Where,
    OrderBy,
    Top
};

// 运算符类型
enum class SqlOperatorType{
    none,
    is,
    like,
    in,
    between,
    _and,
    _or,
    alias,
    innerJoin,
    leftJoin,
    rightJoin,
    fullJoin
};

// 帮助工具类
class ARMYANTLIB_API SqlStructHelper{
public:
	// 根据字段类型获取该类型在sql语句中的名称
    static String getDataTypeName(SqlFieldType type);
};

// 表头信息结构
struct ARMYANTLIB_API SqlFieldHead
{
    uint32 length;
    String catalogName;
    String columnName;
    SqlFieldType type;
    bool allowNull;
    bool autoIncrease;
};

// 单字段类
class ARMYANTLIB_API SqlField
{
public:
    SqlField();
    SqlField(const String&value, const SqlFieldHead*head);
    ~SqlField();

public:
    bool setValue(const String&value);
    const String&getValue()const;
    const SqlFieldHead*getHead()const;

private:
	friend class SqlTable;
	friend class ISqlClient;
    const SqlFieldHead* head;
    String value;
};

// 一条数据类
class ARMYANTLIB_API SqlRow
{
public:
    SqlRow(const SqlRow&copied);
    SqlRow(SqlRow&&moved);
    ~SqlRow();

public:
    uint32 size()const;
	const SqlField&operator[](int32 index)const;
	SqlField&operator[](int32 index);

private:
    friend class ISqlClient;
    friend class SqlTable;
    SqlField* fields;
    uint32 length;
};

// 一列数据类
class ARMYANTLIB_API SqlColumn
{
public:
    SqlColumn(const SqlColumn&copied);
    SqlColumn(SqlColumn&&moved);
    ~SqlColumn();

public:
    uint32 size()const;
    const SqlFieldHead*getHead(uint32 index)const;
    const SqlField&operator[](int32 index)const;
	SqlField&operator[](int32 index);

private:
    friend class ISqlClient;
    friend class SqlTable;
    SqlField* fields;
    uint32* indexes;
    uint32 length;
};

// 数据库信息结构
struct ARMYANTLIB_API SqlDatabaseInfo
{
    String name;
    String server;
    String charset;
    String sortRule;
};

// 表信息结构
struct ARMYANTLIB_API SqlTableInfo
{
    String tableName;
    String engine;
    String comment;
    SqlDatabaseInfo* parentDatabase;
};

// 表类
class ARMYANTLIB_API SqlTable : protected SqlTableInfo 
{
public:
    SqlTable(const SqlTable&copied);
    SqlTable(SqlTable&&moved);
	SqlTable& operator=(const SqlTable&copied);
	SqlTable& operator=(SqlTable&&moved);
    ~SqlTable();

public:
    uint32 size()const;
    uint32 width()const;
    uint32 height()const;
    const SqlFieldHead*getHead(int32 index)const;
	SqlRow operator[](int32 index);
	const SqlField&operator()(int32 rowIndex, int32 colIndex)const;
	SqlField&operator()(int32 rowIndex, int32 colIndex);
    SqlColumn operator()(std::nullptr_t, int32 colIndex);
    const SqlColumn operator()(std::nullptr_t, int32 colIndex)const;

private:
    friend class ISqlClient;
	SqlTable(const SqlFieldHead* heads, uint32 width, uint32 height);
    uint32 _width;
    SqlFieldHead* heads;
    uint32 _height;
    SqlField** fields;
};

// SQL表达式类
class ARMYANTLIB_API SqlExpress{
public:
    SqlExpress(const String&str="");
    ~SqlExpress();

public:
    bool pushValue(const String& value);
    bool pushValues(const String*values, uint32 num);
    bool removeValue(uint32 index);
    bool clear();

public:
    SqlOperatorType type;
};

// SQL条件语句类
class ARMYANTLIB_API SqlClause
{
public:
    SqlClause(const String&str = "");
    ~SqlClause();

public:
    bool pushExpress(const SqlExpress& value);
    bool pushExpresses(const SqlExpress*values, uint32 num);
    bool removeExpress(uint32 index);
    bool clear();

public:
    SqlClauseType type;
};

}

#endif // AA_SQL_STRUCTS_H_2017_4_1
