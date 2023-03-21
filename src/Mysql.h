#ifndef _MYSQL_H_
#define _MYSQL_H_

#include <mysql/mysql.h>
#include <string>

class Account{
public:
    std::string name_;
    std::string passwd_;
};

class Mysql{
private:
    MYSQL* mysql;
    MYSQL_RES* result;
    MYSQL_ROW row;
public:
    Mysql();
    ~Mysql();

    bool InitSql(std::string,std::string,std::string,std::string);
    bool ExeSql(std::string);

    bool Insert(Account&&);
    bool Delete(Account&&);
    bool Modify(Account&&);
    bool Check(Account&&);
};

#endif