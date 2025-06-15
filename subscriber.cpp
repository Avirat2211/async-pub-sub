#include<iostream>
#include<unistd.h>
#include<arpa/inet.h>
#include<string.h>
#include<thread>
#include<vector>
void receive(int s);
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
    if(inet_pton(AF_INET,broker_ip_p,&address.sin_addr)<=0){
        std::cerr << "Invalid address/ Address not supported "<<strerror(errno)<<"\n";
        return 1;
    }
    if(connect(s,(sockaddr *)&address,sizeof(address))<0){
        std::cerr<<"Connection to broker failed: "<<strerror(errno)<<"\n";
        return 0;
    }
    std::cout<<"Connection to broker Successful\n";
    std::cout<<"'exit' to stop\n";
    std::cout<<"'SUBSCRIBE' to subscribe to a new topic\n";
    // std::vector<std::thread> subscriptions;
    std::thread receiver(receive,s);
    while(1){
        std::string option;
        std::cin>>option;
        if(option=="SUBSCRIBE"){
            std::cout<<"Enter topic: ";
            std::string topic;
            std::cin>>topic;
            if(topic=="exit")break;
            std::string token = "SUBSCRIBE "+topic;
            send(s,token.c_str(),token.size(),0);
            std::cout<<"Subsciption request sent\n";
        } else if(option=="exit"){
            break;
        } else{
            std::cout<<"Enter a valid command\n";
        }
    }    
    close(s);
    receiver.detach();
}
void receive(int s){
    char buf[1024];
    while (1) {
        memset(buf, 0, sizeof(buf));
        int count = recv(s, buf, sizeof(buf), 0);
        if(count <= 0){
            std::cout << "Disconnected from broker\n";
            break;
        }
        std::cout << buf;
    }
}