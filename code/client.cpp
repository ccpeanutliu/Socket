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

    //socket的建立
    int sockfd = 0;
    sockfd = socket(AF_INET , SOCK_STREAM , 0);

    if (sockfd == -1){
        printf("Fail to create a socket.");
    }

    //socket的連線

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;

    //localhost test
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(9990);


    int err = connect(sockfd,(struct sockaddr *)&info,sizeof(info));
    if(err==-1){
        printf("Connection error\n");
        return 0;
    }

    char buffer[B_SIZE] = {};

    bool tmp = 1;
    int rec = 0;
    string login, input_buf, port, message;

    recv(sockfd,buffer,sizeof(buffer),0);
    printf("%s",buffer);

    //Send a message to server
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
        recv(sockfd,buffer,sizeof(buffer),0);

        printf("\n*** Response from server *** \n\n");
        printf("%s\n", buffer);
        printf("*** over ***\n\n");

        if(strstr(buffer, "AUTH") != NULL)
            tmp = 1;
    }
    bool exit = 0;
    while(!tmp)
    {
        memset(buffer,'\0',sizeof(buffer));
        message.clear();
        login.clear();
        printf("Enter 'l' to  List online member, 'e' to Exit:\n");
        cin >> login;
//        cout << "\n-------" << login << "--------\n";
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
        recv(sockfd,buffer,sizeof(buffer),0);

        printf("\n*** Response from server *** \n\n");

        //printf("%s",buffer);
        cout << buffer;
        if(strstr(buffer,"Bye") != NULL)
        {
            cout << "\n\n*** over ***\n\n";
            break;
        }
        cout << "\n*** over ***\n\n";
	    //if(exit)
	      //  break;
    }
    
    printf("close Socket\n");
    close(sockfd);
    return 0;
}
