#include "client.h"

Client::Client() {
    // setup variables
    buflen_ = 1024;
    buf_ = new char[buflen_+1];
}

Client::~Client() {
}

void Client::run() {
    // connect to the server and run echo program
    create();
    generate_proto_msg();
    //echo();
}

void
Client::create() {
}

void
Client::close_socket() {
}

void
Client::generate_proto_msg() {

    tutorial::AddressBook address_book;
    tutorial::Person *person = address_book.add_people();
    person->set_name("foobar");
    person->set_id(1);
    person->set_email("foo@bar.com");
    tutorial::Person::PhoneNumber* phone_number = person->add_phones();
    phone_number->set_number("1234");
    phone_number->set_type(tutorial::Person::MOBILE);

    std::string data;
    address_book.SerializeToString(&data);
    
    // send request
    bool success = send_request(data);
        // break if an error occurred
    if (not success)
      cout<<"Error sending proto packet!"<<endl;
    // get a response
    success = get_response();
    // break if an error occurred
    if (not success)
      cout<<"Error receving proto packet!"<<endl;
    close_socket();
}


void
Client::echo() {
    string line;
    
    // loop to handle user interface
    while (getline(cin,line)) {
        // append a newline
        line += "\n";
        // send request
        bool success = send_request(line);
        // break if an error occurred
        if (not success)
            break;
        // get a response
        success = get_response();
        // break if an error occurred
        if (not success)
            break;
    }
    close_socket();
}

bool
Client::send_request(string request) {
    // prepare to send request
    const char* ptr = request.c_str();
    int nleft = request.length();
    int nwritten;
    // loop to be sure it is all sent
    while (nleft) {
        if ((nwritten = send(server_, ptr, nleft, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
                continue;
            } else {
                // an error occurred, so break out
                perror("write");
                return false;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

bool
Client::get_response() {
    string response = "";
    // read until we get a newline
    while (response.find("\n") == string::npos) {
        int nread = recv(server_,buf_,1024,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
            return "";
        }
        // be sure to use append in case we have binary data
        response.append(buf_,nread);
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
    tutorial::AddressBook addr_book;
    addr_book.ParseFromString(response);
    cout<<"Got response for person name: "<<addr_book.people(0).name()<<endl;
    //cout << response;
    return true;
}