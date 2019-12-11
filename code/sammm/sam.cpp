#include <iostream>
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <arpa/inet.h> 
#include <string.h> 

using namespace std;

int main(int argc, char const *argv[])
{
	//socket的建立
  int sockfd = 0;
  sockfd = socket(AF_INET , SOCK_STREAM , 0);

  if (sockfd == -1){
      printf("Fail to create a socket.");
  }

  //socket的連線

  struct sockaddr_in info;
  // bzero(&info,sizeof(info));
  info.sin_family = PF_INET;

  //localhost test
  char ips[16] = "140.112.107.45";
  int ports = 8888;
  // cout << "Please enter server's ip: ";
  // cin >> ips;
  // cout << "Please enter serer's port: ";
  // cin >> ports;
  info.sin_addr.s_addr = inet_addr(ips);
  info.sin_port = htons(ports);


  int err = connect(sockfd, (struct sockaddr *)&info, sizeof(info));
  if(err==-1){
      printf("Connection error");
      return 0;
  }


  //Send a message to server
  char message[100];
  char receiveMessage[100] = {};
  // send(sockfd, message, sizeof(message), 0);
  recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);

  cout << receiveMessage;
  memset(receiveMessage, '\0', 100);
  cout << "==========================\n";
  cout << "Please Enter Your Command:\n";
  cout << "(R)egister\t(L)ogin\n";
  cout << "==========================\n";
  cout << "cmd> ";

  // state
  bool login = false;

  while (cin >> message) {
    if ((message[0] == 'R' || message[0] == 'r') && !login)
    {
      char name[100];
      cout << "Please Enter Your Name: ";
      cin >> name;
      strcpy(message, "REGISTER#");
      strcat(message, name);
    }
    else if (message[0] == 'L' || message[0] == 'l')
    {
      if (login)
      {
        strcpy(message, "List");
      }
      else
      {
        char p[7];
        cout << "Please Enter Your Account: ";
        cin >> message;
        cout << "Please Enter Your Port: ";
        cin >> p;
        strcat(message, "#");
        strcat(message, p);
      }
    }
    else if ((message[0] == 'E' || message[0] == 'e') && login)
    {
      strcpy(message, "Exit");
    }
    else
    {
      cout << "\nWrong command, please enter right command!\n\n";
      if (!login)
      {
        cout << "==========================\n";
        cout << "Please Enter Your Command:\n";
        cout << "(R)egister\t(L)ogin\n";
        cout << "==========================\n";
      }
      else
      {
        cout << "==========================\n";
        cout << "Please Enter Your Command:\n";
        cout << "(L)ist\t\t(E)xist\n";
        cout << "==========================\n";
      }
      cout << "cmd> ";
      continue;
    }
    strcat(message, "\n");

  	send(sockfd, message, sizeof(message), 0);

    recv(sockfd, receiveMessage, sizeof(receiveMessage), 0);
    if (strcmp(receiveMessage, "100 OK\n") == 0)
      cout << "\nRegister Successfully!!!\n\n";
    else if (strcmp(receiveMessage, "210 FAIL\n") == 0)
      cout << "\nRegister Fail.\n\n";
    else if (strcmp(receiveMessage, "220 AUTH_FAIL\n") == 0)
      cout << "\nLogin fail, please try again.\n\n";
    else if (strcmp(receiveMessage, "Bye\n") == 0)
    {
      cout << "\nBye~~\n\n";
      close(sockfd);
      break;
    }
    else
    {
      cout << endl << receiveMessage << endl;
      login = true;
    }

	  memset(receiveMessage, '\0', 100);
	  memset(message, '\0', 100);
    if (!login)
    {
      cout << "==========================\n";
      cout << "Please Enter Your Command:\n";
      cout << "(R)egister\t(L)ogin\n";
      cout << "==========================\n";
    }
    else
    {
      cout << "==========================\n";
      cout << "Please Enter Your Command:\n";
      cout << "(L)ist\t\t(E)xist\n";
      cout << "==========================\n";
    }
    cout << "cmd> ";
  }
	
	return 0;
}