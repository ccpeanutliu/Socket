#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <iostream>
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
    char message[100] = {};
    char buffer[5000] = {};
    char input_buf[100] = {};
    char port[100] = {};

    bool tmp = 1;
    char login = 'N';

    recv(sockfd,buffer,sizeof(buffer),0);
    printf("%s",buffer);

    //Send a message to server
    while(tmp)
    {
        memset(buffer,'\0',sizeof(buffer));
        memset(message,'\0',sizeof(message));
        
        //cout << "\n" << tmp << "\n";

        login = 'N';
        memset(input_buf,'\0',sizeof(input_buf));
        memset(port,'\0',sizeof(input_buf));
        printf("Enter 'R' to Register, 'Q' to Exit, 'L' to Login: ");
        cin >> login;
        if(login == 'R')
        {
            printf("Enter your name to register: ");
            scanf("%s", input_buf);
            strcpy(message, "REGISTER#");
            strcat(message, input_buf);
        }
        else if(login == 'Q')
            printf("Bye\n");

        else if(login == 'L')
        {
            printf("Enter your name: ");
            scanf("%s", input_buf);
            printf("Enter your port: ");
            scanf("%s", port);
            strcpy(message, input_buf);
            strcat(message,"#");
            strcat(message, port);
            tmp = 0;
        }
        else
            printf("***Please follow the rule****\n");

        strcat(message,"\n");
        send(sockfd,message,sizeof(message),0);
        recv(sockfd,buffer,sizeof(buffer),0);

        printf("\n*** Response from server *** \n\n");
        printf("%s\n", buffer);
        printf("*** over ***\n\n");

        if(strstr(buffer, "AUTH_FAIL") != NULL)
            tmp = 1;
    }
    while(!tmp)
    {
        memset(buffer,'\0',sizeof(buffer));
        memset(message,'\0',sizeof(message));

        printf("Enter 'L' to  List online member, 'E' to Exit: ");
        cin >> login;
        if(login == 'L')
            strcpy(message, "List");

        else if(login == 'E')
            strcpy(message, "Exit");

        else
            printf("***Please follow the rule****\n");

        strcat(message,"\n");
        send(sockfd,message,sizeof(message),0);
        recv(sockfd,buffer,sizeof(buffer),0);

        printf("\n*** Response from server *** \n\n");

        printf("%s",buffer);
        
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