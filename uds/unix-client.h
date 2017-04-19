#pragma once

#include <sys/un.h>

#include "client.h"

class UnixClient : public Client {

public:
    UnixClient();
    UnixClient(int numOfPersons);
    ~UnixClient();

protected:
    void create();
    void close_socket();

private:
    const char* socket_name_;
};
