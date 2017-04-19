#pragma once

#include <errno.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <string>
#include "address.pb.h"
#include <limits>

using namespace std;

class Client {
public:
    Client();
    Client(int num);
    ~Client();

    void run();
    int numOfPersons;

protected:
    virtual void create();
    virtual void close_socket();
    void echo();
    bool send_request(string);
    bool get_response();
    void generate_proto_msg();

    int server_;
    int buflen_;
    char* buf_;
};
