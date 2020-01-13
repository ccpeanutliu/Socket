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
#include <sys/wait.h>
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
    int my_port;

    // for transaction
    string ipaddr2;
    int port2;

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
            cout << "5. Transaction with someone\n";
            cout << "6. Accept someone's transaction\n";
        }
        cout << "Your choice: ";
            
        int method = 0;
        while(1)
        {
            cin >> method;
            if(method <= 6 && method > 0)
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
            my_port = atoi(tmp.c_str());
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
        // transaction ~~~
        else if(method == 5)
        {
            int status;
            if(fork() == 0)
            {
                string name;
                string rcvmsg;
                cout << "who do you want to transaction?\n";
                cout << "name: ";
                cin >> name;
                memset(sendbuf,'\0',sizeof(sendbuf));
                strcat(sendbuf, "Tran#");
                strcat(sendbuf, name.c_str());
                strcat(sendbuf, "\n");
                //cout << sendbuf;
                send(sockfd,sendbuf,B_SIZE,0);
                if(1)
                {
                    memset(&buffer[0],0,sizeof(buffer));
                    for(int j = 0; j < 2; j++)
                        recv(sockfd,buffer,B_SIZE,0);
                    cout << "\n-----From server-----\n" << buffer << "\n";
                    rcvmsg.assign(buffer);
                }
                if(rcvmsg.find("250") == string::npos)
                {
                    ipaddr2 = rcvmsg.substr(0,rcvmsg.find("#"));
                    string tmp;
                    tmp = rcvmsg.substr(rcvmsg.find("#")+1, rcvmsg.find("\n")-1);
                    port2 = atoi(tmp.c_str());
                }
                else
                    continue;

                
                cout << "Please enter the cash you want to transaction: ";
                string money;
                cin >> money;
                int sockfd2 = socket(AF_INET , SOCK_STREAM , 0);
                if (sockfd2 == -1){
                    cout << "Fail to create a socket.";
                    return 0;
                }
                struct sockaddr_in info2;
                bzero(&info,sizeof(info2));
                info2.sin_family = PF_INET;
                info2.sin_addr.s_addr = inet_addr(ipaddr2.c_str());
                info2.sin_port = htons(port2);
                int err = connect(sockfd2,(struct sockaddr *)&info2,sizeof(info2));
                if(err == -1){
                    cout << "Connection error";
                    return 0;
                }
                memset(sendbuf, '\0', sizeof(sendbuf));
                strcpy(sendbuf, login_name.c_str());
                strcat(sendbuf, "#");
                strcat(sendbuf, money.c_str());
                strcat(sendbuf, "\n");

                send(sockfd2, sendbuf, B_SIZE, 0);
                close(sockfd2);
            }
            else
                wait(&status);
            continue;
        }
        else if(method == 6)
        {
            int status, fd[2];
            pipe(fd);
            if(fork() == 0)
            {
                int socket_desc2 , new_socket2 , c2;
                struct sockaddr_in server2, client2;
                char *message2;
                
                //Create socket
                socket_desc2 = socket(AF_INET , SOCK_STREAM , 0);
                if (socket_desc2 == -1)
                {
                    printf("Could not create socket");
                }
                
                //Prepare the sockaddr_in structure
                server2.sin_family = AF_INET;
                server2.sin_addr.s_addr = INADDR_ANY;
                cout << "my_port -> " << my_port << endl;
                server2.sin_port = htons(my_port);
                
                //Bind
                if( bind(socket_desc2,(struct sockaddr *)&server2 , sizeof(server2)) < 0)
                {
                    puts("bind failed");
                    continue;
                    return 1;
                }
                //puts("bind done");
                listen(socket_desc2 , 3);
                
                //Accept and incoming connection
                puts("Waiting for incoming connections...");
                c2 = sizeof(struct sockaddr_in);
                new_socket2 = accept(socket_desc2, (struct sockaddr *)&client2, (socklen_t*)&c2);
                if (new_socket2<0)
                {
                    perror("accept failed");
                    return 1;
                }
                memset(buffer,'\0',sizeof(buffer));
                recv(new_socket2, buffer, B_SIZE, 0);
                string rcvmsg(buffer);
                int tran_money;
                //cout << buffer;

                close(fd[0]);
                close(fd[1]);
                close(new_socket2);
                //puts("Connection accepted");
                memset(sendbuf,'\0',sizeof(sendbuf));
                strcpy(sendbuf, (rcvmsg.substr(0,rcvmsg.find("#"))).c_str());
                strcat(sendbuf, "#");
                strcat(sendbuf, login_name.c_str());
                strcat(sendbuf, "#");
                strcat(sendbuf, (rcvmsg.substr(rcvmsg.find("#")+1)).c_str());
                cout << sendbuf << endl;
                send(sockfd, sendbuf, B_SIZE, 0);
            }
            continue;
        }

        // transaction~~~
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
