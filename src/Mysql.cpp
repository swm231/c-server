#include "Mysql.h"
#include "util.h"
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
    sprintf(sql, "UPDATE accounts SET passwd = '%s'"
                "where name = '%s';", acc->passwd_.c_str(), acc->name_.c_str());
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