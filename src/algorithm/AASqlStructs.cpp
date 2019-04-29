/* Copyright (c) 2015 ArmyAnt
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

#include <vector>

#include "../../inc/AASqlStructs.h"
#include "../../inc/AAClassPrivateHandle.hpp"


#define AA_SQL_EXPRESS_HANDLE_MANAGER ClassPrivateHandleManager<SqlExpress, SqlExpress_inner>::getInstance()
#define AA_SQL_CLAUSE_HANDLE_MANAGER ClassPrivateHandleManager<SqlClause, SqlClause_inner>::getInstance()

namespace ArmyAnt{

String SqlStructHelper::getDataTypeName(SqlFieldType type){
    switch(type){
        case SqlFieldType::Null:
            return "null";
        case SqlFieldType::MySql_BIT:
            return "bit";
        case SqlFieldType::MySql_CHAR:      // TODO: mysql所有项目以及部分SqlServer项目都应当对括号内的长度限制作处理
            return "char(255)";
        case SqlFieldType::MySql_VARCHAR:    //
            return "varchar(1024)";
        case SqlFieldType::MySql_GEOMETRY:
            return "geometry";
        case SqlFieldType::MsSqlServer_text:
            return "text";
        case SqlFieldType::MySql_BINARY:    //
            return "binary(4096)";
        case SqlFieldType::MySql_VARBINARY:    //
            return "varbinary(32768)";
        case SqlFieldType::MySql_LONGVARCHAR:    //
            return "varchar(32768)";
        case SqlFieldType::MySql_LONGVARBINARY:    //
            return "varbinary(262144)";
        case SqlFieldType::MySql_JSON:
            return "json";
        case SqlFieldType::MySql_ENUM:
            return "enum()";    // TODO: 需要对mysql枚举作特殊处理,下面set同
        case SqlFieldType::MySql_SET:
            return "set";
        case SqlFieldType::MySql_TINYINT:
            return "tinyint";
        case SqlFieldType::MySql_SMALLINT:
            return "smallint";
        case SqlFieldType::MySql_MEDIUMINT:
            return "mediumint";
        case SqlFieldType::MySql_INT:
            return "int";
        case SqlFieldType::MySql_BIGINT:
            return "bigint";
        case SqlFieldType::MySql_FLOAT:
            return "float";
        case SqlFieldType::MySql_DOUBLE:
            return "double";
        case SqlFieldType::MySql_DEMICAL:
            return "demical";
        case SqlFieldType::MySql_DATE:
        case SqlFieldType::MsSqlServer_date:
            return "date";
        case SqlFieldType::MySql_DATETIME:
        case SqlFieldType::MsSqlServer_datetime:
            return "datetime";
        case SqlFieldType::MySql_TIMESTAMP:
        case SqlFieldType::MsSqlServer_timestamp:
            return "timestamp";
        case SqlFieldType::MySql_TIME:
        case SqlFieldType::MsSqlServer_time:
            return "time";
        case SqlFieldType::MySql_YEAR:
            return "year";
        case SqlFieldType::MsAccess_Currency:
            return "Currency";
        case SqlFieldType::MsAccess_AutoNumber:
            return "AutoNumber";
        case SqlFieldType::MsAccess_YesNo:
            return "Yes/No";
        case SqlFieldType::MsAccess_Hyperlink:
            return "Hyperlink";
        case SqlFieldType::MsAccess_Text:// = MySql_Varchar,
            return "Text";
        case SqlFieldType::MsAccess_Memo:// = MySql_Text,
            return "Memo";
        case SqlFieldType::MsAccess_Byte:// = MySql_TinyInt,
            return "Byte";
        case SqlFieldType::MsAccess_Integer:// = MySql_SmallInt,
            return "Integer";
        case SqlFieldType::MsAccess_Long:// = MySql_Int,
            return "Long";
        case SqlFieldType::MsAccess_Single:// = MySql_Float,
            return "Single";
        case SqlFieldType::MsAccess_Double:// = MySql_Double,
            return "Double";
        case SqlFieldType::MsAccess_DateTime:// = MySql_DateTime,
            return "Date/Time";
        case SqlFieldType::MsAccess_OleObject:// = MySql_LongBlob,
            return "Ole Object";
        case SqlFieldType::MsAccess_LookupWizard:// = MySql_Enum,
            return "Lookup Wizard";
        case SqlFieldType::MsSqlServer_char:
            return "char(8000)";
        case SqlFieldType::MsSqlServer_varchar:
            return "varchar(max)";
        case SqlFieldType::MsSqlServer_nchar:
            return "nchar(4000)";
        case SqlFieldType::MsSqlServer_nvarchar:
            return "nvarchar(4000)";
        case SqlFieldType::MsSqlServer_ntext:
            return "ntext";
        case SqlFieldType::MsSqlServer_bit:
            return "bit";
        case SqlFieldType::MsSqlServer_binary:
            return "binary(8000)";
        case SqlFieldType::MsSqlServer_varbinary:
            return "varbinary(max)";
        case SqlFieldType::MsSqlServer_image:
            return "image";
        case SqlFieldType::MsSqlServer_tinyint:
            return "tinyint";
        case SqlFieldType::MsSqlServer_smallint:
            return "smallint";
        case SqlFieldType::MsSqlServer_int:
            return "int";
        case SqlFieldType::MsSqlServer_bigint:
            return "bigint";
        case SqlFieldType::MsSqlServer_decimal:
            return "demical";
        case SqlFieldType::MsSqlServer_numeric:
            return "numeric";
        case SqlFieldType::MsSqlServer_smallmoney:
            return "smallmoney";
        case SqlFieldType::MsSqlServer_money:
            return "money";
        case SqlFieldType::MsSqlServer_float:
            return "float";
        case SqlFieldType::MsSqlServer_real:
            return "real";
        case SqlFieldType::MsSqlServer_datetime2:
            return "datetime2";
        case SqlFieldType::MsSqlServer_smalldatetime:
            return "smalldatetime";
        case SqlFieldType::MsSqlServer_datetimeoffset:
            return "datetimeoffset";
        case SqlFieldType::MsSqlServer_sql_variant:
            return "sql_variant";
        case SqlFieldType::MsSqlServer_uniqueidentifier:
            return "uniqueidentifier";
        case SqlFieldType::MsSqlServer_xml:
            return "xml";
        case SqlFieldType::MsSqlServer_cursor:
            return "cursor";
        case SqlFieldType::MsSqlServer_table:
            return "table";
        case SqlFieldType::MsExcel_Normal:
            return "excel";
        default:
            return "";
    }
}

SqlField::SqlField()
    :head(nullptr), value(""){}
SqlField::SqlField(const String& value, const SqlFieldHead* head)
    : head(head), value(value){}

SqlField::~SqlField(){}

bool SqlField::setValue(const String& v){
    value = v;
    return true;
}

const String& SqlField::getValue() const{
    return value;
}

const SqlFieldHead* SqlField::getHead() const{
    return head;
}

SqlRow::SqlRow(const SqlRow& copied)
    :length(copied.length), fields(nullptr){
    if(length <= 0)
        return;
    fields = new SqlField[length];
    for(uint32 i = 0; i < length; ++i){
        fields[i] = copied.fields[i];
    }
}

SqlRow::SqlRow(SqlRow&& moved)
    :length(moved.length), fields(moved.fields){
    moved.length = 0;
    moved.fields = nullptr;
}

SqlRow::~SqlRow(){
    Fragment::AA_SAFE_DELALL(fields);
}

uint32 SqlRow::size() const{
    return length;
}

const SqlField& SqlRow::operator[](int32 index)const{
    return const_cast<SqlRow*>(this)->operator[](index);
}

SqlField& SqlRow::operator[](int32 index){
    if(index < 0)
        index += length;
    if(index >= length || index < 0)
        throw nullptr;
    return fields[index];
}

SqlColumn::SqlColumn(const SqlColumn & copied)
    :fields(nullptr), indexes(nullptr), length(copied.length){
    if(length <= 0)
        return;
    fields = new SqlField[length];
    indexes = new uint32[length];
    for(uint32 i = 0; i < length; ++i){
        fields[i] = copied.fields[i];
        indexes[i] = copied.indexes[i];
    }
}

SqlColumn::SqlColumn(SqlColumn && moved)
    :fields(moved.fields), indexes(moved.indexes), length(moved.length){
    moved.fields = nullptr;
    moved.length = 0;
    moved.indexes = nullptr;
}

SqlColumn::~SqlColumn(){
    Fragment::AA_SAFE_DELALL(indexes);
    Fragment::AA_SAFE_DELALL(fields);
}

uint32 SqlColumn::size() const{
    return length;
}

const SqlFieldHead* SqlColumn::getHead(uint32 index) const{
    return operator[](index).getHead();
}

const SqlField& SqlColumn::operator[](int32 index) const{
    return const_cast<SqlColumn*>(this)->operator[](index);
}

SqlField& SqlColumn::operator[](int32 index){
    if(index < 0)
        index += length;
    if(index >= length || index < 0)
        throw nullptr;
    return fields[index];
}

SqlTable::SqlTable(const SqlTable & copied)
    :_width(copied._width), _height(copied._height), heads(nullptr), fields(nullptr){
    if(_width > 0){
        heads = new SqlFieldHead[_width];
        for(uint32 i = 0; i < _width; ++i){
            heads[i] = copied.heads[i];
        }
        if(_height > 0){
            fields = new SqlField * [_height];
            for(uint32 i = 0; i < _height; ++i){
                fields[i] = new SqlField[_width];
                for(uint32 n = 0; n < _width; ++n){
                    fields[i][n] = copied.fields[i][n];
                }
            }
        }
    }
}

SqlTable::SqlTable(SqlTable && moved)
    :_width(moved._width), _height(moved._height), heads(moved.heads), fields(moved.fields){
    moved.heads = nullptr;
    moved.fields = nullptr;
    moved._width = 0;
    moved._height = 0;
}

SqlTable& SqlTable::operator=(const SqlTable & copied){
    Fragment::AA_SAFE_DELALL(heads);
    for(uint32 i = 0; i < _height; ++i){
        Fragment::AA_SAFE_DELALL(fields[i]);
    }
    Fragment::AA_SAFE_DELALL(fields);
    _width = copied._width;
    _height = copied._height;
    if(_width > 0){
        heads = new SqlFieldHead[_width];
        for(uint32 i = 0; i < _width; ++i){
            heads[i] = copied.heads[i];
        }
        if(_height > 0){
            fields = new SqlField * [_height];
            for(uint32 i = 0; i < _height; ++i){
                fields[i] = new SqlField[_width];
                for(uint32 n = 0; n < _width; ++n){
                    fields[i][n] = copied.fields[i][n];
                }
            }
        }
    }
    return *this;
}

SqlTable& SqlTable::operator=(SqlTable && moved){
    _width = moved._width;
    _height = moved._height;
    heads = moved.heads;
    fields = moved.fields;
    moved._width = 0;
    moved._height = 0;
    moved.heads = nullptr;
    moved.fields = nullptr;
    return *this;
}

SqlTable::~SqlTable(){
    Fragment::AA_SAFE_DELALL(heads);
    for(uint32 i = 0; i < _height; ++i){
        Fragment::AA_SAFE_DELALL(fields[i]);
    }
    Fragment::AA_SAFE_DELALL(fields);
}

uint32 SqlTable::size() const{
    return _width * _height;
}

uint32 SqlTable::width() const{
    return _width;
}

uint32 SqlTable::height() const{
    return _height;
}

const SqlFieldHead* SqlTable::getHead(int32 index) const{
    if(index < 0)
        index += _width;
    if(index >= _width || index < 0)
        throw nullptr;
    return heads + index;
}

SqlRow SqlTable::operator[](int32 index){
    if(index < 0)
        index += _height;
    if(index >= _height || index < 0 || _width <= 0)
        throw nullptr;
    char tmp[sizeof(SqlRow)] = "";
    SqlRow ret = *reinterpret_cast<SqlRow*>(tmp);
    ret.length = _width;
    ret.fields = new SqlField[_width];
    for(uint32 i = 0; i < _width; ++i){
        ret.fields[i] = fields[index][i];
    }
    return ret;
}

const SqlField& SqlTable::operator()(int32 rowIndex, int32 colIndex)const{
    return const_cast<SqlTable*>(this)->operator()(rowIndex, colIndex);
}

SqlField& SqlTable::operator()(int32 rowIndex, int32 colIndex){
    if(rowIndex < 0)
        rowIndex += _height;
    if(colIndex < 0)
        colIndex += _width;
    if(rowIndex >= _height || rowIndex < 0 || colIndex >= _width || colIndex < 0)
        throw nullptr;
    return fields[rowIndex][colIndex];
}

SqlColumn SqlTable::operator()(std::nullptr_t, int32 colIndex){
    if(colIndex < 0)
        colIndex += _width;
    if(colIndex >= _width || colIndex < 0 || _height <= 0)
        throw nullptr;
    char tmp[sizeof(SqlColumn)] = "";
    SqlColumn ret = *reinterpret_cast<SqlColumn*>(tmp);
    ret.length = _height;
    ret.fields = new SqlField[_height];
    ret.indexes = new uint32[_height];
    for(uint32 i = 0; i < _height; ++i){
        ret.fields[i] = fields[i][colIndex];
        ret.indexes[i] = i;
    }
    return ret;
}

const SqlColumn SqlTable::operator()(std::nullptr_t, int32 colIndex)const{
    return const_cast<SqlTable*>(this)->operator()(nullptr, colIndex);
}

SqlTable::SqlTable(const SqlFieldHead * heads, uint32 width, uint32 height)
    :_width(width), _height(height), heads(nullptr), fields(nullptr){
    if(width > 0){
        this->heads = new SqlFieldHead[width];
        for(uint32 i = 0; i < width; ++i){
            this->heads[i] = heads[i];
        }
        if(_height > 0){
            fields = new SqlField * [_height];
            for(uint32 i = 0; i < _height; ++i){
                fields[i] = new SqlField[width];
                for(uint32 n = 0; n < _width; ++n){
                    fields[i][n].head = heads + n;
                    fields[i][n].value = "";
                }
            }
        }
    }
}


/********************* Sql Express ************************************/

class SqlExpress_inner{
    std::vector<String> expresses;
};

SqlExpress::SqlExpress(const String & str)
    :type(SqlOperatorType::none){
    AA_SQL_EXPRESS_HANDLE_MANAGER.GetHandle(this);
    AAAssert(pushValue(str), );
}

SqlExpress::~SqlExpress(){
    delete AA_SQL_EXPRESS_HANDLE_MANAGER.ReleaseHandle(this);
}

bool SqlExpress::pushValue(const String & value){
    // TODO
    return false;
}


/********************* Sql Clause *************************************/

class SqlClause_inner{
    std::vector<SqlExpress> expresses;
};


SqlClause::SqlClause(const String & str)
    :type(SqlClauseType::Null){
    AA_SQL_CLAUSE_HANDLE_MANAGER.GetHandle(this);
    AAAssert(pushExpress(SqlExpress(str)));
}

SqlClause::~SqlClause(){
    delete AA_SQL_CLAUSE_HANDLE_MANAGER.ReleaseHandle(this);
}

bool SqlClause::pushExpress(const SqlExpress & value){
    // TODO
    return false;
}

}


#undef AA_SQL_EXPRESS_HANDLE_MANAGER
#undef AA_SQL_CLAUSE_HANDLE_MANAGER
