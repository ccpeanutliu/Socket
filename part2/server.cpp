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
#include <vector>
#include <iostream>

#define BUFF_SIZE 1024

using namespace std;
int count = 0;
int online = 0;
char client_message[BUFF_SIZE];
string port_arr[500];
string name_arr[500];
vector <string> online_member;
//char buffer[BUFF_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;

void * socketThread(void *arg)
{
    int newSocket = *((int *)arg);
    string message, save, ip_addr, port, loginAs;
    char buffer[BUFF_SIZE] = "Hello, welcome!\n";
    cout << "Connect to client.\n";
    send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
    bool tmp = 1;
    while(tmp)
    {
        message.clear();
        memset(buffer,'\0',sizeof(char)*BUFF_SIZE);
        memset(client_message,'\0',sizeof(char)*BUFF_SIZE);
        recv(newSocket, client_message , sizeof(char)*BUFF_SIZE , 0);
        // Send message to the client socket 
        pthread_mutex_lock(&lock);
    
        message.assign(client_message);
        
        cout << message;

        if(message.find("REGISTER#") != string::npos)
        {
            bool already = 0;
            string name(message.substr(9,message.find("\n")-9));
            for(int i = 0; i < count; i++)
            {
                if(name_arr[i].compare(name) == 0)
                {
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
        else if(message.find("#") != string::npos)
        {
            bool already = 0;
            string name(message.substr(0,message.find("#")));
            //cout << name << "...";
            for(int i = 0; i < count; i++)
            {
                if(name_arr[i].compare(name) == 0)
                {
                    save = name_arr[i];
                    already = 1;
                    break;
                }
            }

        //cout << already;
            if(already)
            {
                port = message.substr(message.find("#")+1,message.find("\n")-message.find("#")-1);
                port_arr[count] = port;
                tmp = 0;
                ip_addr = "127.0.0.1";
                loginAs = save + "#" + ip_addr + "#" + port + "\n";
                online_member.push_back(save + "#" + ip_addr + "#" + port + "\n");
                online ++;
            }
        }
        //cout << message;
        //strcpy(buffer,message);
        //free(message);
        //send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
        pthread_mutex_unlock(&lock);
        //sleep(1);
    }
    int first_log = 0;
    int logout;
    while(!tmp)
    {

        memset(buffer,'\0',sizeof(buffer));
        memset(client_message,'\0',sizeof(char)*BUFF_SIZE);
        message.clear();
        if(first_log == 0)
        {
            message = "10000\n";
            message += "numbers of online client:";
            message += to_string(online);
            message += "\n";
            for(int i = 0; i < online; i++)
            {
                message += online_member[i];
            }
            strcpy(buffer,message.c_str());
            send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
            first_log ++;
        }
        else
        {
            recv(newSocket, client_message , sizeof(char)*BUFF_SIZE , 0);
            pthread_mutex_lock(&lock);
            message.assign(client_message);
            if(message.compare("List\n") == 0)
            {
                message.clear();
                message = "10000\n";
                message += "numbers of online client:";
                message += to_string(online);
                message += "\n";
                for(int i = 0; i < online; i++)
                {
                    message += online_member[i];
                }
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
            }
            else if(message.compare("Exit\n") == 0)
            {
                message.clear();
                message = "Bye\n";
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
                tmp = 0;
                for(int i = 0; i < online; i++)
                {
                    if(online_member[i] == loginAs)
                    {
                        logout = i;
                    }
                }
                online_member.erase(online_member.begin()+logout);
                break;
            }
        }
        pthread_mutex_unlock(&lock);
    }

    printf("Exit socketThread \n");
    online --;
    pthread_mutex_unlock(&lock);
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