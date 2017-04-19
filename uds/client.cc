#include "client.h"

#define PACKET_SIZE 1024000

Client::Client() {
    // setup variables
  buflen_ = PACKET_SIZE;
  buf_ = new char[buflen_+1];
}

Client::Client(int num) {
    // setup variables
  buflen_ = PACKET_SIZE;
  buf_ = new char[buflen_+1];
  this->numOfPersons = num;
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

  cout<<"numOfPersons="<<this->numOfPersons<<"\t";
    tutorial::AddressBook address_book;
    for(int cnt=0; cnt < this->numOfPersons; cnt++) {
          
      tutorial::Person *person = address_book.add_people();
      //string name;
      //cout<<"Enter person name: ";
      //    getline(cin, name);
      person->set_name("foobar");
      person->set_id(cnt);
      person->set_email("foo@bar.com");
      tutorial::Person::PhoneNumber* phone_number = person->add_phones();
      phone_number->set_number("1234");
      phone_number->set_type(tutorial::Person::MOBILE);

    }

    std::string data_tmp;
    address_book.SerializeToString(&data_tmp);
    //data_tmp = data_tmp + 
    // cout<<"data size: "<<data_tmp.length()<<endl;

    double minValue = std::numeric_limits<int>::max();
    double avgValue = 0.0;

    struct timespec requestStart, requestEnd;
    
    int counter = 0;
    while(counter < 100) {
      clock_gettime(CLOCK_MONOTONIC, &requestStart);

      std::string data;
      address_book.SerializeToString(&data);

      // send request
      bool success = send_request(data);
      // break if an error occurred
      if (not success)
	cout<<"Error sending proto packet!"<<endl;

      //cout<<"debug"<<endl;
      // get a response
      success = get_response();

      // End measurement
      clock_gettime(CLOCK_MONOTONIC, &requestEnd);

      if (not success)
	cout<<"Error receving proto packet!"<<endl;

      // Calculate time it took
      double accum = ( requestEnd.tv_sec - requestStart.tv_sec )*1000000000.0
	+ ( requestEnd.tv_nsec - requestStart.tv_nsec );

      if(accum < minValue) minValue = accum;
      avgValue += accum;

      //cout<<"Iteration: "<<counter<<endl;
      counter++;
    }

    printf("avg_us = %lf, counter=%d minValue=%lf total=%lf avg=%lf\n", (avgValue/counter)/2000, counter, minValue, avgValue, avgValue/counter);
    // break if an error occurred
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
  //int message_length = request.length();
   char numstr[21] = {'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a', 'a','a','a'};
    sprintf(numstr, "%lu", request.length());
    numstr[20] = 0;
    //printf("numstr = %s\n", numstr);
    string newreq = numstr + request;
    const char* ptr = request.c_str();
    int nleft = request.length();
    int nwritten = PACKET_SIZE;

    if((nwritten=send(server_, numstr, 21*sizeof(char), 0)) < 0) {
      printf("Size sending failed\n");
     }
    // loop to be sure it is all sent
    //while (nwritten == PACKET_SIZE ) {
    while(nleft) {
      int size = PACKET_SIZE;
      if (nleft < PACKET_SIZE) size = nleft;
      if ((nwritten = send(server_, ptr, size, 0)) < 0) {
            if (errno == EINTR) {
                // the socket call was interrupted -- try again
	      perror("prblem");
	      return false;
                //continue;
            } else {
                // an error occurred, so break out
                perror("write");
	      return true;
            }
        } else if (nwritten == 0) {
            // the socket is closed
            return false;
        }
      //cout<<"nleft="<<nleft<<endl;
        nleft -= nwritten;
        ptr += nwritten;
    }
    return true;
}

bool
Client::get_response() {
    string response = "";
    int nread = PACKET_SIZE;
    int read_size = 0;
    char numstr[21];
    int ret = 0;
    if((ret = recv(server_,numstr, 21*sizeof(char),0)) < 0) {
      printf("Reading size failed!\n");
      exit(5);
    }
    int data_size = atoi(numstr);
    // printf("Got data_size = %s int=%d\n", numstr, data_size);
    // read until we get a newline
    //while (response.find("\n") != string::npos) {
    while(read_size < data_size) {
      // while(nread == PACKET_SIZE) {
      int to_read = PACKET_SIZE;
      if((data_size-read_size) < PACKET_SIZE) to_read = data_size - read_size;

      nread = recv(server_,buf_,to_read,0);
      //cout << "nread="<< nread<<endl;
        if (nread < 0) {
            if (errno == EINTR)
                // the socket call was interrupted -- try again
                continue;
            else
                // an error occurred, so break out
                return "";
        } else if (nread == 0) {
            // the socket is closed
	    break;
            //return "";
        }
        // be sure to use append in case we have binary data
        response.append(buf_,nread);
	read_size+=nread;
    }
    // a better client would cut off anything after the newline and
    // save it in a cache
    // cout<<"Got response of size: "<<response.length()<<" nread="<< nread<<endl;
    tutorial::AddressBook addr_book;
    addr_book.ParseFromString(response);
    // cout<<"Got response for person name: "<<addr_book.people(0).name()<<endl;
    //cout << response;
    return true;
}
