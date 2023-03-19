#ifndef _BUFFER_H_
#define _BUFFER_H_

#include <string>

class Buffer{
private:
    std::string buf;
public:
    Buffer();
    ~Buffer();

    void append(const char*, int);
    ssize_t size();
    const char* c_str();
    void clear();
    void getlen();
};

#endif