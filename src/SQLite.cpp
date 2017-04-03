
#include "SQLite.h"

#include <sqlite3/sqlite3.h>

#include <stdio.h>
#include <vector>

namespace beebox
{

CSQLite::CSQLite()
{
    m_db = NULL;
}

CSQLite::~CSQLite()
{
	close();
}

bool CSQLite::open(string filePath)
{
    if (m_db)
    {
        close();
    }

    return sqlite3_open_v2(filePath.c_str(), &m_db, SQLITE_OPEN_READWRITE, NULL)
           == SQLITE_OK ? true : false;
}

void CSQLite::close()
{
    sqlite3_close(m_db);
}

bool CSQLite::create(string filePath)
{
    return sqlite3_open_v2(filePath.c_str(), &m_db,
                           SQLITE_OPEN_READWRITE |
                           SQLITE_OPEN_CREATE,
                           NULL)
           == SQLITE_OK ? true : false;
}

bool CSQLite::select(string sql, string& recordInJson)
{
    sqlite3_stmt* res = NULL;
    const char* unused = NULL;
	recordInJson.clear();

    int rc = sqlite3_prepare_v2(m_db, sql.c_str(), sql.size(), &res, &unused);
    if (rc != SQLITE_OK)
    {
        return false;
    }

    int columnCount = sqlite3_column_count(res);
    vector<string> fieldNameList;
    for (int i=0; i<columnCount; ++i)
    {
        const char* name = sqlite3_column_name(res, i);
        string _name = "\"" ;
        _name += name;
        _name += "\":";
        fieldNameList.push_back(_name);
    }

    recordInJson += "[";

    rc = sqlite3_step(res);
    while(rc == SQLITE_ROW)
    {
        for(int i=0; i<columnCount; i++)
        {
            if (i == 0)
            {
                recordInJson += "{";
            }

			string val = getValue(res, i);
			recordInJson += fieldNameList[i];
			recordInJson += val.empty() ? "null" : val;

            if (i < columnCount - 1)
            {
                recordInJson += ",";
            }
            else
            {
                recordInJson += "}";
            }
        }

        rc = sqlite3_step(res);
        if (rc == SQLITE_ROW)
        {
            recordInJson += ",";
        }
    }

    recordInJson += "]";

    sqlite3_finalize(res);

	if (recordInJson.size() < 3)
	{
		recordInJson.clear();
	}

    return true;
}

bool CSQLite::execute(string sql)
{
    printf("########## sql: %s\n", sql.c_str());
    char* errMsg = NULL;
    int rc = sqlite3_exec(m_db, sql.c_str(), 0, 0, &errMsg);
    if (rc != SQLITE_OK)
    {
        printf("SQL error:%s\n", errMsg);
        return false;
    }
    else
    {
        printf("SQL Execute OK!\n");
    }

    return true;
}

string CSQLite::getError()
{
    return sqlite3_errmsg(m_db);
}

string CSQLite::getValue(sqlite3_stmt* res, int columnIndex)
{
    const unsigned char* pStr = NULL;
    int type =  sqlite3_column_type(res, columnIndex);
    string out;
    char buffer[256] = {0};

    switch(type)
    {
    case SQLITE_INTEGER:
        sprintf(buffer, "%d", sqlite3_column_int(res, columnIndex));
        out = buffer;
        break;
    case SQLITE_FLOAT:
        sprintf(buffer, "%f", sqlite3_column_double(res, columnIndex));
        out = buffer;
        break;
    case SQLITE3_TEXT:
        pStr = sqlite3_column_text(res, columnIndex);
        if (pStr)
        {
            out = "\"";
            out += (char*)pStr;
            out += "\"";
        }
        break;
    case SQLITE_BLOB:
        //sqlite3_column_bytes(res, 0);
        //(void*) sqlite3_column_blob(res, 0);
        out = "blob";
        break;
    case SQLITE_NULL:
        out = "";
        break;
    default:
        out = "";
        break;
    };

    return out;
}

} // namespace beebox
