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
//string port_arr[500];

//string name_arr[500];
//char buffer[BUFF_SIZE];
pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
string get_client_ip;

struct socketAndIP{
    int socket;
    string ip;
};

struct clientinfo{
    string name;
    int money;
    string ipAddr;
    string portnum;
};

vector <struct clientinfo> onlineinfo;
vector <struct clientinfo> allinfo;

void * socketThread(void *arg)
{ 
    int newSocket = *((int *)arg);
    string message, save, port, loginAs;
    int save_money = 0;
    string ip_addr = get_client_ip;
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
                if((allinfo[i].name).compare(name) == 0)
                {
                    already = 1;
                    break;
                }
            }
            if(already)
            {
                message = "210 AUTH (ACCOUNT ALREADY REGISTER)\n";
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
            }
            else
            {
                message = "100 OK\n";
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
                struct clientinfo tmp;
                tmp.name = name;
                tmp.money = 10000;
                allinfo.push_back(tmp);
                //name_arr[count] = name;
                count ++;
            }
        }
        else if(message.find("#") != string::npos)
        {
            int already = 0;
            string name(message.substr(0,message.find("#")));
            //cout << name << "...";
            for(int i = 0; i < count; i++)
            {
                if((allinfo[i].name).compare(name) == 0)
                {
                    save = allinfo[i].name;
                    save_money = allinfo[i].money;
                    already = 1;
                    break;
                }
            }
            for(int i = 0; i < online; i++)
            {
                if(onlineinfo[i].name == name)
                {
                    already = -1;
                    break;
                }
            }

        //cout << already;
            if(already == 1)
            {
                port = message.substr(message.find("#")+1,message.find("\n")-message.find("#")-1);
                //port_arr[count] = port;
                tmp = 0;

                struct clientinfo thisinfo;
                thisinfo.name = save;
                thisinfo.money = save_money;
                thisinfo.ipAddr = ip_addr;
                thisinfo.portnum = port;
                onlineinfo.push_back(thisinfo);
                online ++;
            }
            else if(already == -1)
            {
                message = "220 AUTH_FAIL\nerror: ACCOUNT ALREADY LOGIN\n";
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
            }
            else
            {
                message = "220 AUTH_FAIL\nerror: ACCOUNT DOESN'T EXIST\n";
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
            }
        }
        else
        {
            message.clear();
            message = "'REGISTER#[name]' to register or '[name]#[port]' to login.\n";
            strcpy(buffer,message.c_str());
            send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
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
            //message = to_string(save_money);
            for(int i = 0; i < count; i++)
            {
                if(allinfo[i].name == save)
                {
                    message = to_string(allinfo[i].money);
                    break;
                }
            }
            message += "\nnumbers of online client:";
            message += to_string(online);
            message += "\n";
            for(int i = 0; i < online; i++)
            {
                message += onlineinfo[i].name;
                message += "#";
                message += onlineinfo[i].ipAddr;
                message += "#";
                message += onlineinfo[i].portnum;
                message += "\n";
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
            cout << message;
            if(message.compare("List\n") == 0)
            {
                message.clear();
                //message = to_string(save_money);
                for(int i = 0; i < count; i++)
                {
                    if(allinfo[i].name == save)
                    {
                        message = to_string(allinfo[i].money);
                        break;
                    }
                }
                message += "\nnumbers of online client:";
                message += to_string(online);
                message += "\n";
                for(int i = 0; i < online; i++)
                {
                    //message += online_member[i];
                    message += onlineinfo[i].name;
                    message += "#";
                    message += onlineinfo[i].ipAddr;
                    message += "#";
                    message += onlineinfo[i].portnum;
                    message += "\n";
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
                    if(onlineinfo[i].name == save)
                        logout = i;
                }
                //online_member.erase(online_member.begin()+logout);
                onlineinfo.erase(onlineinfo.begin()+logout);
                break;
            }
            else if(message.find("Tran#") != string::npos)
            {
                
                cout << "message -> " << message << endl;
                string contact;
                contact = message.substr(5,message.find("\n")-1);
                cout << contact;
                string contactip;
                string contactport;
                int exist = 0;
                for(int i = 0; i < online; i++)
                {
                    if(onlineinfo[i].name+"\n" == contact)
                    {
                        contactip = onlineinfo[i].ipAddr;
                        contactport = onlineinfo[i].portnum;
                        exist = 1;
                        break;
                    }
                }
                if(exist)
                {
                    strcpy(buffer, contactip.c_str());
                    strcat(buffer, "#");
                    strcat(buffer, contactport.c_str());
                    strcat(buffer, "\n");
                    cout << buffer;
                    send(newSocket, buffer, BUFF_SIZE, 0);
                }
                else
                {
                    memset(buffer,'\0',sizeof(buffer));
                    strcpy(buffer, "250 NOT_ONLINE\n");
                    cout << buffer;
                    send(newSocket, buffer, BUFF_SIZE, 0);
                }
            }
            else if(message.find("#") != string::npos)
            {
                string sender, receiver, tmp;
                int tran_money;
                tmp = message.substr(message.find("#")+1);
                sender = message.substr(0,message.find("#"));
                receiver = tmp.substr(0,tmp.find("#"));
                tmp = tmp.substr(tmp.find("#")+1);
                tran_money = atoi(tmp.c_str());
                cout << "----start transaction----\n";
                cout << "sender -> " << sender << endl;
                cout << "receiver-> " << receiver << endl;
                cout << "amount of money -> " << tran_money << endl;
                cout << "----end transaction----\n";
                for(int i = 0; i < online; i++)
                {
                    if(onlineinfo[i].name == sender)
                    {
                        onlineinfo[i].money -= tran_money;
                    }
                    if(onlineinfo[i].name == receiver)
                    {
                        onlineinfo[i].money += tran_money;
                    }
                }
                for(int i = 0; i < count; i++)
                {
                    if(allinfo[i].name == sender)
                    {
                        allinfo[i].money -= tran_money;
                    }
                    if(allinfo[i].name == receiver)
                    {
                        allinfo[i].money += tran_money;
                    }
                }
                //cout << "name: " << save << "\nmoney: " << save_money << endl;
                
            }
            else{
                message.clear();
                message = "Please enter 'List' to List the online member or 'Exit' to leave.\n";
                strcpy(buffer,message.c_str());
                send(newSocket,buffer,sizeof(char)*BUFF_SIZE,0);
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

int main(int argc, char *argv[]){
    if(argc < 2)
    {
        cout << "./getip_server.o [port].\n";
        return 0;
    }

    int serverSocket, newSocket;
    struct sockaddr_in serverAddr;
    struct sockaddr_storage serverStorage;
    socklen_t addr_size;
    //Create the socket. 
    int server_port = atoi(argv[1]);
    serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    serverAddr.sin_family = PF_INET;
    serverAddr.sin_port = htons(server_port);
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
    bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));
    
    
    
    if(listen(serverSocket,50)==0)
        printf("Listening\n");
    else
        printf("Error\n");
    pthread_t tid[60];
    int i = 0;
    while(1)
    {
        //Accept call creates a new socket for the incoming connection
        //為新的連接創一個新的socket
        struct sockaddr_in clientAddress;
        socklen_t clientLen = sizeof(clientAddress);
        newSocket = accept(serverSocket, (struct sockaddr *) &clientAddress, &clientLen);
        
        if(getpeername(newSocket, (sockaddr *) &clientAddress, &clientLen))
            cout << "Failed to get client ip";
        else
            get_client_ip = inet_ntoa(clientAddress.sin_addr);
        //for each client request creates a thread and assign the client request to it to process
        //每一個新連結的client, 都幫它創一個thread
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

