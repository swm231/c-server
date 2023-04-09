#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>
#include <memory>

class Buffer{
private:
public:
    std::string buf;
    Buffer();
    Buffer(std::string);
    ~Buffer();

    void append(const char*, int);
    void append(const char*);
    ssize_t size();
    const char* c_str();
    void clear();
    void getline();
    void SetBuf(const char*);
};

#endif