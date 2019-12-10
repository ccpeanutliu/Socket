#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>
#define B_SIZE 512

using namespace std;

int main(int argc , char *argv[])
{

    if(argc < 3)
    {
        cout << "\n Usage: ./client.out <ip> <port>\n";
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
    info.sin_family = PF_INET; // ipv4

    //localhost test
    info.sin_addr.s_addr = inet_addr(argv[1]); //ip address
    info.sin_port = htons(x);

    //socket描述符, socket信息, info大小
    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error\n");
        return 0;
    }

    char buffer[B_SIZE] = {}; // string receive from server

    bool tmp = 1; // login or not
    int rec; 
    string login, input_buf, port, message; // method, name, port, message send to server.

    rec = recv(sockfd,buffer,sizeof(buffer),0);
    //printf("%s",buffer);
    cout << buffer;
    // cin and send message
    while(tmp)
    {
        memset(buffer,'\0',sizeof(buffer));
        message.clear();

        login.clear();
        input_buf.clear();
        port.clear();

        printf("Enter 'r' to Register, 'q' to Exit, 'l' to Login:\n");
        cin >> login;
        if(login == "r")
        {
            printf("Enter your name to register: ");
            cin >> input_buf;
            message = "REGISTER#" + input_buf + "\n";
        }
        else if(login == "q")
        {    
            printf("Bye\n");
            break;
        }

        else if(login == "l")
        {
            printf("Enter your name: \n");
            cin >> input_buf;
            printf("Enter your port: \n");
            cin >> port;
            message = input_buf + "#" + port + "\n";
            tmp = 0;
        }
        else
            printf("***Please follow the rule****\n");

        char send_m[B_SIZE];
        send(sockfd,strcpy(send_m,message.c_str()),sizeof(message),0);
        rec = recv(sockfd,buffer,sizeof(buffer),0);

        if(strstr(buffer, "AUTH") != NULL)
            tmp = 1;

        printf("\n*** Response from server *** \n\n");
        //printf("%s\n", buffer);
        //int count = 0;
        if(login == "l")
            for(int i = 0; i < 2; i++){
                if(strstr(buffer, "AUTH") != NULL)
                {
                    cout << buffer << "\n";    
                    break;
                }
                if(i != 0)
                {
                    rec = recv(sockfd,buffer,sizeof(buffer),0);
                }
                cout << buffer;
                if(strstr(buffer, "#") != NULL)
                {
                    cout << "\n";
                    break;
                }
                memset(buffer,'\0',sizeof(buffer));
            }
        if(login != "l")
            cout << buffer << "\n";
        printf("*** over ***\n\n");

        
    }
    bool exit = 0;
    while(!tmp)
    {
        memset(buffer,'\0',sizeof(buffer));
        message.clear();
        login.clear();
        printf("Enter 'l' to  List online member, 'e' to Exit:\n");
        cin >> login;
        if(login == "l"){
            message = "List\n";
        }
        else if(login == "e"){
	        exit = 1;
	        message = "Exit\n";
        }
        else
            printf("***Please follow the rule****\n");

        char send_m[B_SIZE];
        send(sockfd,strcpy(send_m,message.c_str()),sizeof(message),0);
        

        printf("\n*** Response from server *** \n\n");
        //cout << "\n\n-----" << rec << "----\n\n";
        //printf("%s",buffer);

        //rec = recv(sockfd,buffer,sizeof(buffer),0);
        if(login == "l")
            for(int i = 0; i < 2; i++){
                rec = recv(sockfd,buffer,sizeof(buffer),0);
                if(rec > 1)
                    cout << buffer;
                if(strstr(buffer, "#") != NULL)
                    break;
                
                memset(buffer,'\0',sizeof(buffer));
            }
        if(login != "l")
        {
            rec = recv(sockfd,buffer,sizeof(buffer),0);
            cout << buffer;
        }
        if(strstr(buffer,"Bye") != NULL)
        {
            cout << "\n\n*** over ***\n\n";
            break;
        }
        cout << "\n*** over ***\n\n";
    }
    
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
