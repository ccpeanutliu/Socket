#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string>
#include <iostream>
using namespace std;
#define B_SIZE 512


int main(int argc , char *argv[])
{

    if(argc < 3)
    {
        printf("\n Usage: ./newClient.out <ip> <port>\n");
        return 0;
    }
    int x = atoi(argv[2]);

    //socket的建立
    int sockfd = 0;
    // AF_INET: 使用ipv4
    // SOCK_STREAM: 使用tcp
    sockfd = socket(AF_INET , SOCK_STREAM , 0); 

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    /*

    struct sockaddr_in {
    short            sin_family;   // AF_INET,因為這是IPv4;
    unsigned short   sin_port;     // 儲存port No
    struct in_addr   sin_addr;     // 參見struct in_addr
    char             sin_zero[8];  // Not used, must be zero 
    };
    
    */
    bzero(&info,sizeof(info));// 初始化
    info.sin_family = AF_INET; // ipv4

    //localhost test
    info.sin_addr.s_addr = inet_addr(argv[1]); //ip address
    info.sin_port = htons(x);

    //socket描述符, socket信息, info大小
    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error\n");
        return 0;
    }

    char buffer[B_SIZE];
    memset(&buffer[0] , 0 , sizeof(buffer));
    recv(sockfd , buffer , B_SIZE, 0);
    cout << buffer;
    bool isLogin = false;
    string login_name;
    while(1)
    {
        cout << "What you ganna do?\n";
        if(!isLogin)
        {
            cout << "1. Register.\n";
            cout << "2. Login.\n";
            cout << "4. Exit.\n";
        }
        else
        {
            cout << "3. List money and online member.\n";
            cout << "4. Exit.\n";
        }
        cout << "Your choice: ";
            
        int method = 0;
        while(1)
        {
            cin >> method;
            if(method <= 5 && method > 0)
                break;
            else
                cout << "please follow my tips.\n";
        }

        
        char sendbuf[B_SIZE];
        if(method == 1 && !isLogin)
        {
            string tmp;
            cout << "What's your name?\n";
            cin >> tmp;
            strcpy(sendbuf, "REGISTER#");
			strcat(sendbuf, tmp.c_str());
			strcat(sendbuf, "\n");
            
        }
        else if(method == 2 && !isLogin)
        {
            string tmp;
            cout << "What's your name?\n";
            cin >> sendbuf;
            login_name.assign(sendbuf);
            strcat(sendbuf, "#");
            cout << "What's your port?\n";
            cin >> tmp;
            strcat(sendbuf, tmp.c_str());
            strcat(sendbuf, "\n");
            isLogin = true;
        }
        else if(method == 3 && isLogin)
        {
            strcpy(sendbuf, "List\n");
        }
        else if(method == 4)
        {
            if(!isLogin)
            {
                cout << "Bye\n";
                break;
            }
            strcpy(sendbuf,"Exit\n");
            send(sockfd,sendbuf,B_SIZE,0);  
            memset(&buffer[0],0,B_SIZE);
            for(int j = 0; j < 2; j++)
                recv(sockfd,buffer,sizeof(buffer),0);
            cout << buffer;
            break;
        }
        else if(method == 5)
        {
            string name;
            string text;
            cout << "who do you want to contact?";
            cout << "name: ";
            cin >> name;
            strcpy(sendbuf, login_name.c_str());
            strcat(sendbuf, "#");
            cout << "Your text: ";
            cin >> text;
            strcat(sendbuf, text.c_str());
            strcat(sendbuf, "#");
            strcat(sendbuf, name.c_str());
            strcat(sendbuf, "\n");
        }
        else
        {
            cout << "\nSomething wrong!!!\n";
            continue;
        }
        send(sockfd,sendbuf,B_SIZE,0);
        memset(&buffer[0],0,sizeof(buffer));
        for(int j = 0; j < 2; j++)
            recv(sockfd,buffer,B_SIZE,0);
        
        cout << "\n-----From server-----\n" << buffer << "\n";
        string judge;
        judge.assign(buffer);
        if(judge.find("AUTH_FAIL") != string::npos)
        {    
            isLogin = false;
            login_name.clear();
        }
    }
    
    
    cout << "close Socket\n";
    close(sockfd);
    return 0;
}
