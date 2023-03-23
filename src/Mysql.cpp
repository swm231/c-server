#include "Mysql.h"
#include "util.h"
#include <vector>
#include <string>
#include <iostream>

#define MAX_SQL 1024

Mysql::Mysql() : mysql(mysql_init(NULL)){
    errif(mysql ==  NULL, "mysql init error");
}

Mysql::~Mysql(){
    if(mysql != NULL)
        mysql_close(mysql);
}

bool Mysql::InitSql(std::string host, std::string user, std::string passwd, std::string db_name){
    mysql = mysql_real_connect(mysql, host.c_str(), user.c_str(), passwd.c_str(), db_name.c_str(), 0, NULL, 0);
    errif(mysql == NULL, mysql_error(mysql));
    return true;
}

bool Mysql::ExeSql(std::string sql){
    errif(mysql_query(mysql, sql.c_str()), mysql_error(mysql));
    result = mysql_store_result(mysql);
    if(result){

    }
    return true;
}

bool Mysql::Insert(const Account* acc){
    char sql[MAX_SQL];
    sprintf(sql, "INSERT into accounts (name, passwd) values('%s', '%s');", acc->name_.c_str(), acc->passwd_.c_str());
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return false;
    }
    return true;
}

bool Mysql::Delete(const Account* acc){
    char sql[MAX_SQL];
    sprintf(sql, "DELETE from accounts where name = '%s';", acc->passwd_.c_str());
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return false;
    }
    return true;
}

bool Mysql::Modify(const Account* acc){
    char sql[MAX_SQL];
    sprintf(sql, "UPDATE accounts SET passwd = '%s', sockfd = '%d'"
                "where name = '%s';", acc->passwd_.c_str(), -1, acc->name_.c_str());
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return false;
    }
    return true;
}

ssize_t Mysql::Check(const Account* acc){
    char sql[MAX_SQL];
    sprintf(sql, "SELECT * from accounts where name = '%s';", acc->name_.c_str());
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return -1;
    }
    result = mysql_store_result(mysql);
    if(result){
        int n = mysql_num_rows(result);
        if(n == 0)  return 2; //  用户不存在
        row = mysql_fetch_row(result);
        if(acc->passwd_ != row[1]) return 1;
        return 0;
    }
    return 2;
}

bool Mysql::Fd_Modify(const Account* acc, int fd){
    char sql[MAX_SQL];
    sprintf(sql, "UPDATE accounts SET sockfd = '%d'"
                "where name = '%s';", fd, acc->name_.c_str());
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return false;
    }
    return true;
}

std::vector<std::string> Mysql::LookList(const Account* acc){
    char sql[MAX_SQL];
    sprintf(sql, "SELECT * from relationship where name1 = '%s';", acc->name_.c_str());
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
    }
    result = mysql_store_result(mysql);
    std::vector<std::string> v;
    if(result){
        int n = mysql_num_rows(result);
        for(int i = 0; i < n; ++ i){
            row = mysql_fetch_row(result);
            v.push_back(row[1]);
        }
    }
    return v;
}

bool Mysql::Find(const char* str){
    char sql[MAX_SQL];
    sprintf(sql, "SELECT * from accounts where name = '%s';", str);
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return -1;
    }
    result = mysql_store_result(mysql);
    if(result){
        if (mysql_num_rows(result)) return true;
        return false;
    }
    return false;
}

void Mysql::AddShip(Account* acc, const char* str){
    char sql[MAX_SQL];
    sprintf(sql, "INSERT into relationship (name1, name2) values('%s', '%s');", acc->name_.c_str(), str);
    if(mysql_query(mysql, sql))
        errif(true, mysql_error(mysql));
}

int Mysql::FindFd(const char* str){
    char sql[MAX_SQL];
    sprintf(sql, "SELECT * from accounts where name = '%s';", str);
    if(mysql_query(mysql, sql)){
        errif(true, mysql_error(mysql));
        return -1;
    }
    result = mysql_store_result(mysql);
    std::vector<std::string> v;
    if(result){
        int n = mysql_num_rows(result);
        if(n == 0) return 0;
        row = mysql_fetch_row(result);
        
        return (int)(*row[2]) - '0';
    }
    return 0;
}