#ifndef _MYSQL_H_
#define _MYSQL_H_

#include <mysql/mysql.h>
#include <string>
#include <vector>
#include <memory>

class Account{
public:
    std::string name_;
    std::string passwd_;
    Account():name_(""), passwd_(""){};
    ~Account(){
        name_.clear();
        passwd_.clear();
    }

    void Set_Name(const char* _str){
        name_.clear();
        name_.append(_str);
    }
    void Set_Pass(const char* _str){
        passwd_.clear();
        passwd_.append(_str);
    }
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

    bool Insert(const Account*);
    bool Delete(const Account*);
    bool Modify(const Account*);
    ssize_t Check(const Account*);
    bool Fd_Modify(const Account*, int);

    std::vector<std::string> LookList(const Account*);
    std::vector<int> GetOnlFd();
    bool Find(const char*);
    void AddShip(Account*, const char*);
    int FindFd(const char*);
};

#endif