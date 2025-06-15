#include<iostream>
#include<unistd.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>
#include<thread>
#include<vector>
#include<errno.h>
#include<map>
#include<set>
#include<mutex>
std::map<std::string,std::set<int>> registry;
std::mutex topic_mutex; // handle concurrency 
void handleClient(int );
int main(){
    const char *port_p = std::getenv("BROKER_PORT");
    if(!port_p){
        std::cerr<<"Environment variables not set\n";
        return 0;
    }
    const int port = atoi(port_p);
    int s = socket(AF_INET,SOCK_STREAM,0);
    sockaddr_in address;
    address.sin_family=AF_INET;
    address.sin_port=htons(port);
    address.sin_addr.s_addr=INADDR_ANY;
    bind(s,(struct sockaddr*)&address,sizeof(address));
    listen(s,1000);
    std::vector<std::thread> clients;
    std::cout<<"Server started waiting for connections\n";
    while(1){
        int cS = accept(s,nullptr,nullptr);
        if(cS<0){
            std::cerr<<"Accept failed\n";
            continue;
        }
        std::cout<<"New client connected\n";
        clients.emplace_back(handleClient,cS);
    } 
    for(auto &it:clients){
        if(it.joinable()){
            it.join();
        }
    }
    close(s);
}


void handleClient(int clientSocket){
    char buf[1024]={0};
    while(true){
        memset(buf,0,sizeof(buf));
        int count = recv(clientSocket,buf,sizeof(buf),0);
        if(count==0){
            std::cout<<"Client closed connection\n";
            break;
        }else if(count<0){
            if(errno==ECONNRESET){
                std::cout<<"Client terminated connection\n";
            }else{
                std::cout<<"Error receiving data: "<<strerror(errno)<<"\n";
            }
            break;
        }
        // std::cout<<"Message recieved from client: "<<buf<<"\n";
        std::string text(buf);
        // std::cout<<text<<"\n";
        text.erase(text.find_last_not_of("\r\n") + 1);
        // std::cout<<text<<"\n";
        if(text.starts_with("SUBSCRIBE")){
            std::string topic = text.substr(10);
            {
                std::lock_guard<std::mutex> lock(topic_mutex);
                registry[topic].insert(clientSocket);
            }
            std::cout << "Client subscribed to topic: " << topic << "\n";
            std::string response = "[broker] Subscription Successful\n";
            send(clientSocket,response.c_str(),response.length(),0);
        }else if(text.starts_with("PUBLISH")){
            auto index = text.find('\n');
            if(index==std::string::npos){
                std::cout<<"Invalid PUBLISH format\n";
                std::string response = "[broker] Invalid PUBLISH format please try again\n";
                send(clientSocket,response.c_str(),response.length(),0);
                continue;
            } 
            std::string topic = text.substr(0,index).substr(8);
            std::string publish_text = text.substr(index+1);
            std::lock_guard<std::mutex> lock(topic_mutex);
            if(registry.find(topic)!=registry.end()){
                for(int clientSocketId:registry[topic]){
                    // std::string response = publish_text+'\n';
                    std::string response = "[" + topic + "] " + publish_text + "\n";
                    send(clientSocketId,response.c_str(),response.length(),0);
                }
                std::cout<<"Published successfully\n";
            }else{
                std::string response = "No subsciptions to the topic\n";
                send(clientSocket,response.c_str(),response.length(),0);
                continue;
            }
            std::string response = "Publish request Successful\n";
            send(clientSocket,response.c_str(),response.length(),0);
        }else{
            std::cout<<"Invalid Command\n";
            std::string response = "Invalid command\n";
            send(clientSocket,response.c_str(),response.length(),0);
        }
    }
    {
        std::lock_guard<std::mutex> lock(topic_mutex);
        for(auto &[topic, subs] : registry){
            subs.erase(clientSocket);
        }
    }
    close(clientSocket);
}