#include<stdio.h>
#include<string.h>	//strlen
#include<stdlib.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include<iostream>
#include<pthread.h> //for threading , link with lpthread
#include <openssl/rsa.h>
#include <openssl/pem.h>
#include <iomanip>
#include <fstream>
#include <string>

using namespace std;

#define MAX_CLIENT 10

void *connection_handler(void *);

typedef struct user user;

typedef struct ip ip;

struct user {
	char name[50];
	char ip[10] = {"127.0.0.1"};
	int port = 0;
	bool online = false;
	int acc_balance = 10000;
};

struct ip
{
	char value[50];
};

int user_count = 0;
struct user users[MAX_CLIENT];
struct ip temp_ip[MAX_CLIENT]; 

int main(int argc , char *argv[])
{
	int socket_desc , new_socket , c , *new_sock;
	struct sockaddr_in server , client;
	char *message;
	int port_num = 0;
	
	cout << "Please enter port number:";
	cin >> port_num;

	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( port_num );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		puts("bind failed");
		return 1;
	}
	puts("bind done");
	
	//Listen
	listen(socket_desc , MAX_CLIENT);
	
	//Accept and incoming connection
	puts("Waiting for incoming connections...");
	c = sizeof(struct sockaddr_in);
	while( (new_socket = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)) )
	{
		user_count++;

		puts("Connection accepted");
		strcpy(temp_ip[user_count].value, inet_ntoa(client.sin_addr));
		
		//Reply to the client
		message = "Hello Client , I have received your connection. \n";
		write(new_socket , message , strlen(message));
		
		pthread_t sniffer_thread;
		new_sock = (int *) malloc(1);
		*new_sock = new_socket;
		
		if( pthread_create( &sniffer_thread , NULL ,  connection_handler , (void*) new_sock) < 0)
		{
			perror("could not create thread");
			return 1;
		}
		
		//Now join the thread , so that we dont terminate before the thread
		//pthread_join( sniffer_thread , NULL);
		puts("Handler assigned");
	}
	
	if (new_socket<0)
	{
		perror("accept failed");
		return 1;
	}
	
	return 0;
}

/*
 * This will handle connection for each client
 * */
void *connection_handler(void *socket_desc)
{
	//Get the socket descriptor
	int sock = *(int*)socket_desc;
	int read_size, user_num = user_count;
	char *message , client_message[2000], username[100], server_message[100];
	int user_id = -1;
	
	//Receive a message from client
	while( (read_size = recv(sock , client_message , sizeof(client_message) , 0)) > 0 )
	{
		//Send the message back to client
		if(strstr(client_message, "REGISTER#") != NULL)
		{
            memset(server_message, '\0', sizeof(server_message));
            bool repeat = 0;

			strtok(client_message, "#");
			strcpy(client_message, strtok(NULL, "#"));
			client_message[strlen(client_message)-1] = '\0';
			cout << client_message;

			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if(strcmp(users[i].name, client_message) == 0 || user_id != -1)
				{
					strcpy(server_message, "210 FAIL");
					strcat(server_message, "\n");
					cout << server_message << endl;
					write(sock , server_message , strlen(server_message));
					repeat = 1;
				}
			}

			if(repeat == 1)
				continue;

			strcpy(users[user_num].name, client_message);
			cout << users[user_num].name << endl;

			strcpy(server_message, "100 OK");
			strcat(server_message, "\n");
			cout << server_message << endl;
			write(sock , server_message , strlen(server_message));
			memset(client_message, '\0', sizeof(client_message));
		}
		else if(strstr(client_message, "List") != NULL)
		{
			int online_num = 0;
			memset(server_message, '\0', sizeof(server_message));

			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if (users[i].online == 1)
				{
					online_num++;
				}
			}

			cout << user_id;

			if(user_id == -1)
			{
				strcpy(server_message, "FAILED, You are offline");
				write(sock , server_message , strlen(server_message));
				cout << server_message;
				continue;
			}

			char temp[20];
			sprintf(temp, "%d", users[user_id].acc_balance);
			strcpy(server_message, temp);
			strcat(server_message, "\n");
			sprintf(temp, "%d", online_num);
			strcat(server_message, temp);
			strcat(server_message, "\n");
			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if (users[i].online == 1)
				{
					strcat(server_message, users[i].name);
					strcat(server_message, "#");
					strcat(server_message, users[i].ip);
					strcat(server_message, "#");
					sprintf(temp, "%d", users[i].port);
					strcat(server_message, temp);
					strcat(server_message, "\n");
				}
			}
			cout << server_message << endl;
			write(sock , server_message , strlen(server_message));

			memset(client_message, '\0', sizeof(client_message));
		}
		else if(strstr(client_message, "Exit") != NULL)
		{
			memset(server_message, '\0', sizeof(server_message));

			if(user_id == -1)
			{
				strcpy(server_message, "FAILED, You are offline");
				write(sock , server_message , strlen(server_message));
				continue;
			}

			users[user_id].online = 0;
			users[user_id].port = 0;
			user_id = -1;
			strcpy(server_message, "Bye");
			strcat(server_message, "\n");
			write(sock , server_message , strlen(server_message));

			memset(client_message, '\0', sizeof(client_message));
		}
		else if(strstr(client_message, "TRANS#") != NULL)
		{
			memset(server_message, '\0', sizeof(server_message));
			strtok(client_message, "#");
			strcpy(client_message, strtok(NULL, "#"));
			client_message[strlen(client_message)-1] = '\0';
			cout << client_message << endl;		
			bool valid = 0;

			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if(strcmp(users[i].name, client_message) == 0 //&& user_id != i
				 && users[i].online == 1)
				{
					valid = 1;
					strcpy(server_message, users[i].ip);
					char temp[20];
					sprintf(temp, "%d", users[i].port);
					strcat(server_message, "#");
					strcat(server_message, temp);
					write(sock , server_message , strlen(server_message));	
					cout << server_message << endl;				
					break;
				}
			}

			if(valid == 0)
			{	
				write(sock , "230FAILED", strlen("230FAILED"));
				cout << "230 Failed" << endl;	
			}
		}
		else if(strstr(client_message, "#") != NULL)
		{
			char port[5];

			memset(server_message, '\0', sizeof(server_message));
			strcpy(client_message, strtok(client_message, "#"));
			strcpy(port, strtok(NULL, "#"));

			int online_num = 1;
			bool valid = 0;

			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if (users[i].online == 1)
				{
					online_num++;
				}
			}

			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if(strcmp(users[i].name, client_message) == 0 && user_id == -1
				 && users[i].online == 0)
				{
					valid = 1;
					user_id = i;
					users[i].online = 1;
					users[i].port = atoi(port);
					strcpy(users[i].ip, temp_ip[user_num].value);
					break;
				}
			}

			if(valid == 0)
			{
				strcpy(server_message, "220 AUTH_FAIL");
				strcat(server_message, "\n");
				write(sock , server_message , strlen(server_message));
				continue;
			}

			char temp[20];
			sprintf(temp, "%d", users[user_id].acc_balance);
			strcpy(server_message, temp);
			strcat(server_message, "\n");
			sprintf(temp, "%d", online_num);
			strcat(server_message, temp);
			strcat(server_message, "\n");
			for(int i=0 ; i<=MAX_CLIENT ; i++)
			{
				if (users[i].online == 1)
				{
					strcat(server_message, users[i].name);
					strcat(server_message, "#");
					strcat(server_message, users[i].ip);
					strcat(server_message, "#");
					sprintf(temp, "%d", users[i].port);
					strcat(server_message, temp);
					strcat(server_message, "\n");
				}
			}
			cout << server_message << endl;
			write(sock , server_message , strlen(server_message));
			memset(client_message, '\0', sizeof(client_message));
		}
		else
		{
//**************Encryption***************
				cout << client_message << endl;

                FILE *payer, *payee;
                RSA *payerRSA = nullptr, *payeeRSA = nullptr;
                if((payer = fopen("payer_pub.pem","r")) == NULL) {
                    cout << "payer Error" << endl;
                    exit(-1);
                }
                if((payee = fopen("payee_pub.pem","r")) == NULL) {
                    cout << "payee Error" << endl;
                    exit(-1);
                }
                // 初始化算法庫
                OpenSSL_add_all_algorithms();
                // 從 .pem 格式讀取公私鑰
                if((payerRSA = PEM_read_RSA_PUBKEY(payer, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read payer error" << endl;
                }
                fclose(payer);
                if((payeeRSA = PEM_read_RSA_PUBKEY(payee, NULL,NULL,NULL)) == NULL) { 
                    cout << "Read payee error" << endl;
                }
                fclose(payee);
                int rsa_len = RSA_size(payerRSA); // 幫你算可以加密 block 大小，字數超過要分開加密
                
                const unsigned char * src = (const unsigned char *)client_message; //  測試的明文
                // 要開空間來存放加解密結果，型態要改成 unsigned char *

                cerr << endl << strlen(client_message) << endl;

                unsigned char * dec1 = (unsigned char *)malloc(rsa_len);
                unsigned char * dec2 = (unsigned char *)malloc(rsa_len);
                if(RSA_public_decrypt(rsa_len, src, dec1, payeeRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "dec1 error" << endl;
                }
                cerr << "dec1: " << dec1 << endl;
                cerr << "len1: " << strlen((const char*) dec1) << endl;
                if(RSA_public_decrypt(rsa_len, dec1, dec2, payerRSA, RSA_PKCS1_PADDING) < 0) {
                    cout << "dec2 error" << endl;
                }
                // 加密後就會變成一堆奇怪字元
                cerr << "dec2: " << dec2 << endl;
                
                cerr << "len2: " << strlen((const char*) dec2) << endl;
                // 因為是它的函式 new 出來的東東，需要用他的函式釋放記憶體
                RSA_free(payerRSA);
                RSA_free(payeeRSA);

//***************Encryption********************
                memset(client_message, '\0', sizeof(client_message));

		}
	}
	
	if(read_size == 0)
	{
		puts("Client disconnected");
		fflush(stdout);
	}
	else if(read_size == -1)
	{
		perror("recv failed");
	}
		
	//Free the socket pointer
	free(socket_desc);
	
	return 0;
}