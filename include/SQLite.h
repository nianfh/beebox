
#ifndef __SQLITE_H__
#define __SQLITE_H__

#include <string>

using namespace std;

struct sqlite3;
struct sqlite3_stmt;

namespace beebox
{

class CSQLite
{
public:
    CSQLite();
    ~CSQLite();

    bool open(string filePath);
    void close();
    bool create(string filePath);

    bool select(string sql, string& recordInJson);
    bool execute(string sql);

    string getError();

private:
    string getValue(sqlite3_stmt* res, int columnIndex);

    sqlite3* m_db;
};

}

#endif // __SQLITE_H__
