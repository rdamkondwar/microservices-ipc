#include "server.h"
#include <limits>

#define PACKET_SIZE 1024

Server::Server() {
    // setup variables
  buflen_ = PACKET_SIZE;
    buf_ = new char[buflen_+1];
}

Server::~Server() {
    delete buf_;
}

void
Server::run() {
    // create and run the server
    create();
    serve();
}

void
Server::create() {
}

void
Server::close_socket() {
}

void
Server::serve() {
    // setup client
    int client;
    struct sockaddr_in client_addr;
    socklen_t clientlen = sizeof(client_addr);
      // accept clients
    while ((client = accept(server_,(struct sockaddr *)&client_addr,&clientlen)) > 0) {
        handle(client);
    }
    close_socket();
}

void
Server::handle(int client) {
  // loop to handle all requests
  int counter = 0;
  while (1) {
    // get a request
    string request = get_request(client);

    // break if client is done or an error occurred
    if (request.empty())
      break;

    // Deserialize the request
    tutorial::AddressBook addr_book;
    addr_book.ParseFromString(request);
    // cout<<"Got response for person name: "<<addr_book.people(0).name()<<endl;

    counter++;
    // cout<<"Counter = "<< counter;
    string newrequest;
    addr_book.SerializeToString(&newrequest);
    //data_tmp = data_tmp + 
    // cout<<"data size: "<<newrequest.length()<<endl;

    // send response
    bool success = send_response(client,newrequest);
    // break if an error occurred
    if (not success)
      break;

  }
    close(client);
}

string
Server::get_request(int client) {
    string request = "";
    int nread = PACKET_SIZE;
    int read_size = 0;
    int data_size = 0;
    char numstr[21];
    int ret = 0;
    if((ret = recv(client,numstr, 21*sizeof(char),0)) < 0) {
      printf("Reading size failed!\n");
      exit(5);
    }
    
    
    data_size = atoi(numstr);
    // printf("Got data_size = %s int=%d\n", numstr, data_size);
    // read until we get a newline
    //while (request.find("\n") != string::npos) {
    
    //while(nread == PACKET_SIZE) {
    while(read_size < data_size) {
      int to_read = PACKET_SIZE;
      if((data_size-read_size) < PACKET_SIZE) to_read = data_size - read_size;
        nread = recv(client,buf_,to_read,0);
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else {
                // an error occurred, so break out
	      request.append(buf_, nread);
	      break;
	    }
	      // return "";
        } else if (nread == 0) {
            // the socket is closed
            //return "";
	    break;
        }
        // be sure to use append in case we have binary data
        request.append(buf_,nread);
	read_size += nread;
    }

    if(read_size > 0) {
      // std::cout<<"Got response of size: "<<request.length()<<" nread="<< nread<<endl;
      // tutorial::AddressBook addr_book;
      // addr_book.ParseFromString(request);
      // cout<<"Got response for person name: "<<addr_book.people(0).name()<<endl;
    }
    // a better server would cut off anything after the newline and
    // save it in a cache
    return request;
}

bool
Server::send_response(int client, string response) {
  // prepare to send response
  const char* ptr = response.c_str();
  int nleft = response.length();
  int nwritten;

  char numstr[21] = {'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a'};
  sprintf(numstr, "%lu", response.length());
  numstr[20] = 0;
  //printf("numstr = %s\n", numstr);

  if((nwritten=send(client, numstr, 21*sizeof(char), 0)) < 0) {
    printf("Size sending failed\n");
  }
  
  // loop to be sure it is all sent
  while (nleft) {
    int size = PACKET_SIZE;
    if (nleft < PACKET_SIZE) size = nleft;
    if ((nwritten = send(client, ptr, size, 0)) < 0) {
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
