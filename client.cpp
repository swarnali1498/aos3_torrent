#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h> 
using namespace std;

void quit(int sockfd)
{
    string msg="3";
    char* buf=new char[msg.size()+1];
    strcpy(buf, msg.c_str());  
    int n = write(sockfd,buf,strlen(buf));
    if (n < 0) 
         cout<<"Could not write to socket"<<endl;
    close(sockfd);
}
	
int main(int argc, char *argv[])
{
    int sockfd, portno, n;
    struct sockaddr_in serv_addr;
    struct hostent *server;
    int input,i,j;
    
    char buffer[256];
    if (argc < 3) 
    {
       cout<<"Provide ip and port for client"<<endl;
       exit(0);
    }
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"Error opening socket";
    server = gethostbyname(argv[1]);
    if (server == NULL) 
    {
        cout<<"No such host"<<endl;
    }
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        cout<<"Cannot connect"<<endl;
    string command;
    while(1)
    {
    	getline(cin,command);
    	string cmd="";
    	for(i=0;i<command.size();i++)
    	{
    		if(command[i]==' ')
    		{
    			break;
    		}
    		cmd+=command[i];
    	}
    	if(cmd=="quit")
    	{
    		quit(sockfd);
    		break;
    	}
    	else if(cmd=="create_user")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter userid and password"<<endl;
    			continue;
    		}
    		command=command.substr(i+1);
    		string userid="",password;
    		for(i=0;i<command.size();i++)
    		{
    			if(command[i]==' ')
    			{
    				break;
    			}
    			userid+=command[i];
    		}	
    		if(i==command.size())
    		{
    			cout<<"Enter password"<<endl;
    			continue;
    		}
    		password=command.substr(i+1);
   	 	
   	 	string msg="1<"+userid+"><"+password+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		int n = write(sockfd,buf,strlen(buf));
    		if (n < 0) 
         		cout<<"Could not write to socket"<<endl;
		
		char buf1[256];
		int n1=read(sockfd,buf1,255);
    		if (n1<0) 
         		cout<<"Error reading from socket"<<endl;
         	else
    		{
    			for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}
  	}
    	else if(cmd=="login")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter userid and password"<<endl;
    			continue;
    		}
    		command=command.substr(i+1);
    		string userid="",password;
    		for(i=0;i<command.size();i++)
    		{
    			if(command[i]==' ')
    			{
    				break;
    			}
    			userid+=command[i];
    		}
    		if(i==command.size())
    		{
    			cout<<"Enter password"<<endl;
    			continue;
    		}
    		password=command.substr(i+1);
    		
       	string msg="2<"+userid+"><"+password+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
		int n = write(sockfd,buf,strlen(buf));
		if (n < 0) 
			cout<<"Could not write to socket"<<endl;

    		char buf1[256];
    		int n1 = read(sockfd,buf1,255);
    		if (n1 < 0) 
         		cout<<"Error reading from socket"<<endl;
         	else
         	{
         		for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}
    	}
    	else if(cmd=="create_group")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter group id"<<endl;
    			continue;
    		}
    		string gid=command.substr(i+1);
    		
    		string msg="3<"+gid+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
		int n = write(sockfd,buf,strlen(buf));
		if (n < 0) 
			cout<<"Could not write to socket"<<endl;

    		char buf1[256];
    		int n1 = read(sockfd,buf1,255);
    		if (n1 < 0) 
         		cout<<"Error reading from socket"<<endl;
         	else
         	{
         		for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}		
    	}
    	else if(cmd=="join_group")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter group id"<<endl;
    			continue;
    		}
    		string gid=command.substr(i+1);
    		
    		string msg="4<"+gid+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
		int n = write(sockfd,buf,strlen(buf));
		if (n < 0) 
			cout<<"Could not write to socket"<<endl;

    		char buf1[256];
    		int n1 = read(sockfd,buf1,255);
    		if (n1 < 0) 
         		cout<<"Error reading from socket"<<endl;
         	else
         	{
         		for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}		
    	}
    	else if(cmd=="leave_group")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter group id"<<endl;
    			continue;
    		}
    		string gid=command.substr(i+1);
    		
    		string msg="5<"+gid+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
		int n = write(sockfd,buf,strlen(buf));
		if (n < 0) 
			cout<<"Could not write to socket"<<endl;

    		char buf1[256];
    		int n1 = read(sockfd,buf1,255);
    		if (n1 < 0) 
         		cout<<"Error reading from socket"<<endl;
         	else
         	{
         		for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}		
    	}
    	else if(cmd=="list_requests")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter group id"<<endl;
    			continue;
    		}
    		string gid=command.substr(i+1);
    		
    		string msg="6<"+gid+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
		int n = write(sockfd,buf,strlen(buf));
		if (n < 0) 
			cout<<"Could not write to socket"<<endl;

    		char buf1[256];
    		int n1 = read(sockfd,buf1,255);
    		if (n1 < 0) 
         		cout<<"Error reading from socket"<<endl;
         	else
         	{
         		for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}
    	}
    	else if(cmd=="accept_request")
    	{
    		if(i==command.size())
    		{
    			cout<<"Enter group id and user id"<<endl;
    			continue;
    		}
    		command=command.substr(i+1);
    		string gid="",uid;
    		for(i=0;i<command.size();i++)
    		{
    			if(command[i]==' ')
    			{
    				break;
    			}
    			gid+=command[i];
    		}	
    		if(i==command.size())
    		{
    			cout<<"Enter user id"<<endl;
    			continue;
    		}
    		uid=command.substr(i+1);
   	 	
   	 	string msg="7<"+gid+"><"+uid+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		int n = write(sockfd,buf,strlen(buf));
    		if (n < 0) 
         		cout<<"Could not write to socket"<<endl;
		
		char buf1[256];
		int n1=read(sockfd,buf1,255);
    		if (n1<0) 
         		cout<<"Error reading from socket"<<endl;
         	else
    		{
    			for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}
    	}
 	if(cmd=="list_groups")
 	{
 		string msg="8";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		int n = write(sockfd,buf,strlen(buf));
    		if (n < 0) 
         		cout<<"Could not write to socket"<<endl;
		
		char buf1[256];
		int n1=read(sockfd,buf1,255);
    		if (n1<0) 
         		cout<<"Error reading from socket"<<endl;
         	else
    		{
    			for(int i=0;i<n1;i++)
    			cout<<buf1[i];
    			cout<<endl;
    		}
 	}   	
    }	
    close(sockfd);
    return 0;
}
