//
//  main.c
//  socket client
//
//  Created by kuoloading on 2019/12/4.
//  Copyright © 2019 kuoloading. All rights reserved.
//
#include <iostream>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <cmath>
#include <string>
#define BUFF_SIZE  1024
using namespace std;

int main(int argc , char *argv[]){
//    string input;
    int sock; /* socket descriptor*/
    int recved; //儲存回傳接收到了多少個位元組
    string box;
//    char message = 0;  // 存要傳的訊息
    
    sock = socket(PF_INET, SOCK_STREAM, 0);
    if(sock == -1){
        cout<< "error happened! \n";
        return 0;
    }
    /* create an endpoint for communication;
    int socket(int domain, int type, int
    protocol);*/
//    SOCK_STREAM , SOCK_DGRAM , SOCK_RAW;
    

    struct sockaddr_in info;
    bzero(&info,sizeof(info));
    info.sin_family = PF_INET;  //
    info.sin_addr.s_addr = inet_addr("127.0.0.1");
    info.sin_port = htons(2048); // 儲存port No

    int err = connect(sock, (struct sockaddr*)&info, sizeof(info));
        if( err == -1 ){
            cout<< "Connection error\n";
            return 0;
        }
   
    char receiveMessage[BUFF_SIZE] = {};
    
    if ((recved = recv(sock,receiveMessage,sizeof(char)*BUFF_SIZE,0)) < 0){
        }
    cout << receiveMessage;
    // receiving message
    
//    char message[BUFF_SIZE] = {};
    bzero(receiveMessage,sizeof(char)*BUFF_SIZE);
    while(1){
        string input,regis,portnum;
        
        ///先output connection accepted
        box.clear();
        
        input.clear();
        regis.clear();
        portnum.clear();
        bzero(receiveMessage,sizeof(char)*BUFF_SIZE);
//        memset(&receiveMessage, '\0', sizeof(receiveMessage));
        cout <<" Welcome! Please choose a option:\n Register:[RES]\n Login:[LOG]\n List:[LS]\n Exit:[EXIT]\n";
        getline(cin,box);
//      cin >> box;
        if(box == "RES" ){
            cout <<"Please enter your name:\n";
//            string input;
            getline(cin,input);
            regis = "REGISTER#" + input + "\n";
            char tab2[1024];
//            strcpy(tab2, regis.c_str());
            send(sock,strcpy(tab2, regis.c_str()),sizeof(regis),0);
            
            //        char receiveMessage[100] = {};
            //           send(sock,message,sizeof(message),0);
//            if ((recved = recv(sock,receiveMessage,sizeof(char)*BUFF_SIZE,0)) < 0){
//                    cout<< "recv failed, with received bytes = " << recved <<"\n";
//                    break;
//                }
//            else if (recved == 0){
//                cout<< "<end>\n";
//                break;
//            }
//            cout << receiveMessage;
        }
        else if(box == "LOG" ){
            cout <<"Please enter your name:\n";
//             string input;
            getline(cin,input);
            cout <<"Please enter portNum:\n";
//            string portnum;
            getline(cin,portnum);
            regis = input +"#"+ portnum + "\n";
            char tab2[1024];
            //            strcpy(tab2, regis.c_str());
            send(sock,strcpy(tab2, regis.c_str()),sizeof(regis),0);
            //        char receiveMessage[100] = {};
            //           send(sock,message,sizeof(message),0);
//            if ((recved = recv(sock,receiveMessage,sizeof(char)*BUFF_SIZE,0)) < 0){
//                cout<< "recv failed, with received bytes = " << recved <<"\n";
//                break;
//                }
//            else if (recved == 0){
//                cout<< "<end>\n";
//                break;
//            }
//            cout << receiveMessage;
        }
        else if(box == "LS" ){
            string list ="List\n";
            char tab2[1024];
            send(sock,strcpy(tab2,list.c_str()),sizeof(list),0);
            //        char receiveMessage[100] = {};
            //           send(sock,message,sizeof(message),0);
//            if ((recved = recv(sock,receiveMessage,sizeof(char)*BUFF_SIZE,0)) < 0){
//                    cout<< "recv failed, with received bytes = " << recved <<"\n";
//                    break;
//                }
//            else if (recved == 0){
//                cout<< "<end>\n";
//                break;
//            }
//            cout << receiveMessage;
        }
        else if(box == "EXIT"){
            string Exit ="Exit\n";
            char tab2[1024];
            send(sock,strcpy(tab2,Exit.c_str()),sizeof(Exit),0);
            //        char receiveMessage[100] = {};
            //           send(sock,message,sizeof(message),0);
//            if ((recved = recv(sock,receiveMessage,sizeof(char)*BUFF_SIZE,0)) < 0){
//                    cout<< "recv failed, with received bytes = " << recved <<"\n";
//                    break;
//                }
//                else if (recved == 0){
//                    cout<< "<end>\n";
//                    break;
//                }
//                cout << receiveMessage;
                break;
        }
        else{
            cout <<"Please enter correct option! \n";
            continue;
        }
//        send(sock,box,sizeof(box),0);
//        bzero(receiveMessage,sizeof(char)*BUFF_SIZE);
////        char receiveMessage[100] = {};
////           send(sock,message,sizeof(message),0);
        if ((recved = recv(sock,receiveMessage,sizeof(char)*BUFF_SIZE,0)) < 0){
            cout<< "recv failed, with received bytes = " << recved <<"\n";
                   break;
               }
        else if (recved == 0){
            cout<< "<end>\n";
            break;
        }
        cout << receiveMessage;
//        printf("%d:%s\n",recved,receiveMessage);
        
//        for(int i=0; i< sizeof(box); i++){
//            box[i]={};
//        }
//        recved = 0;
//        for(int i=0; i< sizeof(receiveMessage); i++){
//            receiveMessage[i]={};
//        }
    }
    cout<<"\nclose Socket\n";
    close(sock);
    return 0;
}

    
    
    
    
 
