#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
int main(){
    const char *broker_ip_p = std::getenv("BROKER_IP");
    const char *port_p = std::getenv("BROKER_PORT");
    if(!broker_ip_p || !port_p){
        std::cerr<<"Environment varirables not set\n";
        return 0;
    }
    const int port = atoi(port_p);
    int s = socket(AF_INET,SOCK_STREAM,0);
    if(s<0){
        std::cerr<<"Socket creation failed\n";
        return 0;
    }
    sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    if (inet_pton(AF_INET, broker_ip_p, &address.sin_addr) <= 0) {
        std::cerr << "Invalid address/ Address not supported\n";
        return 1;
    }
    if(connect(s,(sockaddr*)&address,sizeof(address))<0){
        std::cerr<<"Connection to broker failed: "<<strerror(errno)<<"\n";
        return 0;
    }

    std::cout<<"Connection to broker Successful\n";
    std::cout<<"'exit' to stop\n";
    while(1){
        std::cout<<"Enter topic: ";
        std::string topic;
        std::getline(std::cin,topic);
        if(topic=="exit")break;
        std::cout<<"Enter message: ";
        std::string message;
        std::getline(std::cin,message);
        if(message=="exit")break;
        std::string token = "PUBLISH "+topic+'\n'+message;
        send(s,token.c_str(),token.size(),0);
        char buff[1024]={0};
        int count = recv(s,buff,sizeof(buff),0);
        if(count){
            std::cout<<buff<<"\n";
        }
    }
    close(s);
}