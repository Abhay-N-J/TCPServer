#pragma once

#include <string>
#include <queue>
#include "data_store.hpp"

class Reader {
    private:
        int sock;
        std::queue<std::string> list;
        
    public:
        Reader(int sock);
        void split(const char* s, int len);
        std::string next();
        std::string trim(const std::string&);
};

void cleanup(int);
std::string trim(std::string);
std::string read_msg(int, DataStore*, bool*, Reader*);
std::string write_msg(DataStore*, bool*, Reader*);
void count_msg(int, DataStore*, bool*, Reader*);
std::string delete_msg(int, DataStore*, bool*, Reader*);
int init_server(int);
void main_loop(int);

