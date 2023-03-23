#include "Buffer.h"
#include "util.h"
#include <string>
#include <iostream>

Buffer::Buffer(){}

Buffer::Buffer(std::string _buf) : buf(_buf){}

Buffer::~Buffer(){}

void Buffer::append(const char* _str, int _size){
    for(int i = 0; i < _size; ++ i){
        if(_str[i] == '\0') break;
        buf.push_back(_str[i]);
    }
}

void Buffer::append(const char* _str){
    buf.append(_str);
}

ssize_t Buffer::size(){
    return buf.size();
}

const char* Buffer::c_str(){
    return buf.c_str();
}

void Buffer::clear(){
    buf.clear();
}

void Buffer::getline(){
    buf.clear();
    std::getline(std::cin, buf);
}

void Buffer::SetBuf(const char* _buf){
    buf.clear();
    buf.append(_buf);
}