#include<stdio.h>
#include<stdlib.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <arpa/inet.h>
#include <fcntl.h> // for open
#include <unistd.h> // for close
#include <pthread.h>
#include <string>

#include <iostream>

#define BUFF_SIZE 1024

using namespace std;
int count = 0;
char client_message[BUFF_SIZE];
string port_arr[500];
string name_arr[500];
//char buffer[BUFF_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socketThread(void *arg)
{
  int newSocket = *((int *)arg);
  string message;
  char buffer[BUFF_SIZE] = "Hello, welcome!\n";
  cout << "Connect to client.\n";
  send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
  while(1){
    message.clear();
    memset(buffer,'\0',sizeof(char)*BUFF_SIZE);
    memset(client,'\0',sizeof(char)*BUFF_SIZE);
    recv(newSocket, client_message , 2000 , 0);
    // Send message to the client socket 
    pthread_mutex_lock(&lock);
    //char *message = malloc(sizeof(client_message)+20);
    
    //strcpy(message,"Hello Client : ");
    //message = "Hello Client";
    message.assign(client_message);
    //strcat(message,client_message);
    //strcat(message,"\n");
    if(strstr(client_message, "REGISTER#") != NULL){
      bool already = 0;
      string name(message.substr(9));
      for(int i = 0; i < count; i++){
        if(name_arr[i].compare(name) == 0){
          already = 1;
          break;
        }
      }
      if(already)
      {
        message = "210 AUTH\n";
        strcpy(buffer,message.c_str());
        send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
      }
      else
      {
        message = "100 OK\n";
        strcpy(buffer,message.c_str());
        send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
        name_arr[count] = name;
        count ++;
      }
    }
    else if(strstr(client_message, "#") != NULL)
    {
      bool already = 0;
      string port;
      string name(message.substr(0,message.find("#")));
      for(int i = 0; i < count; i++){
        if(name_arr[i].compare(name) == 0){
          already = 1;
          break;
        }
      }
      if(already){
        port = message.substr(message.find("#")+1);
        port_arr[count] = port;
      }
    }


    //strcpy(buffer,message);
    //free(message);
    send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
    pthread_mutex_unlock(&lock);
    sleep(1);

  }
  printf("Exit socketThread \n");
  close(newSocket);
  pthread_exit(NULL);
}
int main(){
  int serverSocket, newSocket;
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;
  //Create the socket. 
  serverSocket = socket(PF_INET, SOCK_STREAM, 0);
  // Configure settings of the server address struct
  // Address family = Internet 
  serverAddr.sin_family = AF_INET;
  //Set port number, using htons function to use proper byte order 
  serverAddr.sin_port = htons(7799);
  //Set IP address to localhost 
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  //Set all bits of the padding field to 0 
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
  //Bind the address struct to the socket 
  bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
  //Listen on the socket, with 40 max connection requests queued 
  if(listen(serverSocket,50)==0)
    printf("Listening\n");
  else
    printf("Error\n");
    pthread_t tid[60];
    int i = 0;
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        addr_size = sizeof serverStorage;
        newSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
        //for each client request creates a thread and assign the client request to it to process
       //so the main thread can entertain next request
        if( pthread_create(&tid[i], NULL, socketThread, &newSocket) != 0 )
           printf("Failed to create thread\n");
        if( i >= 50)
        {
          i = 0;
          while(i < 50)
          {
            pthread_join(tid[i++],NULL);
          }
          i = 0;
        }
    }
  return 0;
}