#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
using namespace std;

unordered_map<string,vector<string>> client_list;
unordered_map<string,bool> logged_in;
unordered_map<string,string> client_map;
unordered_map<string,string> owners;
unordered_map<string,vector<string>> groups;
unordered_map<string,vector<string>> pending_clients;
pthread_t clients[30];
int ind=0;

class clientInfo
{
	public:
	int sockfd;
	string ip;
	string port;
};

void* tracker_functions(void* info)
{
	clientInfo* client_info=(clientInfo*)info;
	int newsockfd=client_info->sockfd;
	string client_ip=client_info->ip;
     	string client_port=client_info->port;
     	//cout<<client_ip<<" "<<client_port<<endl;
     	char buffer[256];
  
	while(1)
	{
		int n=read(newsockfd,buffer,255);
     		if (n < 0) 
     			cout<<"Cannot read from socket"<<endl;
     		string buf=string(buffer);
     		char ch=buf[0];
     		buf=buf.substr(2);
		// register client
        	if(ch=='1')
        	{
        		string userid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     				break;
	     			userid+=buf[i];
	     		}
	     		i+=2;
	     		string password="";
	     		for(;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			password+=buf[i];
	     		}
	     		
	     		string buf;
	     		if(client_list.find(userid)!=client_list.end())
	     		{
	     			buf="Please provide a different user id";	
	     		}
	     		else
	     		{
	     			client_list[userid]={password,client_ip,client_port};
	     			string addr=client_ip+" "+client_port;
	     			client_map[addr]=userid;
	     			logged_in[userid]=false;
	     			buf="Registration successful";
	     		}
	     		char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
 		}
     	
     		//login client
     		if(ch=='2')
     		{	
     			string userid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			userid+=buf[i];
	     		}
	     		i+=2;
	     		string password="";
	     		for(;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			password+=buf[i];
	     		}
	     		/*cout<<"Showing clients"<<endl;
     			for(auto itr:client_list)
     			{
     				cout<<itr.first<<" ";
     				vector<string> v=itr.second;
     				for(j=0;j<v.size();j++)
     				{
     					cout<<v[j]<<" ";
     				}
     				cout<<endl;
     			}*/
     			
     			string buf;
     			if(logged_in[userid]==true)
     			{
     				buf="Already logged in";
     			}	
     			else if(client_list.find(userid)==client_list.end())
     			{
 				buf="Wrong userid/password";
     			}
     			else
     			{
     				vector<string> v=client_list[userid];
     				if(v[0]!=password)
     				{
 					buf="Wrong userid/password";
 				}
     				else
     				{
     					logged_in[userid]=true;
 					buf="Successfully logged in";
 				}
     			}
     			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		if(ch=='3')
     		{
     			string gid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			gid+=buf[i];
	     		}
	     		string buf;
	     		string addr=client ip+" "+client_port;
     			if(client_map[addr]==client_map.end())
     			{
     				cout<<"Please register first"<<endl;
     				continue;
     			}
     			string uid=client_map[addr];
     			if(logged_in[uid]!=true)
     			{
     				buf="Please log in first";
     			}	
     			else
     			{
     				owners[gid]=uid;
     				groups[gid].push_back(uid);
     				buf="Group successfully created";
     			}
     			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		if(ch=='4')
     		{
     			string gid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			gid+=buf[i];
	     		}
	     		string buf;
	     		string addr=client ip+" "+client_port;
     			if(client_map[addr]==client_map.end())
     			{
     				cout<<"Please register first"<<endl;
     				continue;
     			}
     			string uid=client_map[addr];
     			if(logged_in[uid]!=true)
     			{
     				buf="Please log in first";
     			}	
     			else
     			{
     				vector<string> v=groups[gid];
     				int f=0;
     				for(int i=0;i<v.size();i++)
     				{
     					if(v[i]==uid)
     					{
     						buf="Already inside group "+gid;
     						f=1;
     						break;
     					}
     				}
     				if(!f)
     				{
     					groups[gid].push_back(uid);
     					buf="Successfully joined group "+gid;
     				}
     			}
     			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}	
    	}
}

void* check_if_quit(void* param)
{
	int* sockfd=(int*)param;
	while(1)
	{
		string cmd;
		cin>>cmd;
		if(cmd=="quit")
		{
			//cout<<*sockfd<<endl;
			close(*sockfd);
			exit(0);
		}
	}
}

int main(int argc, char *argv[])
{
     int sockfd, newsockfd, portno;
     socklen_t clilen;
     struct sockaddr_in serv_addr, cli_addr;
     int n;
     if (argc < 2) 
     {
     	cout<<"No port provided"<<endl;
     }
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        cout<<"Cannot open socket"<<endl;
     portno = atoi(argv[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     //cout<<sockfd<<endl;
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
     {         
     	cout<<"Error on binding"<<endl;
     	exit(0);
     }
     listen(sockfd,5);
     
     pthread_t check_quit;
     clilen = sizeof(cli_addr);
     
     if(pthread_create(&check_quit,NULL,check_if_quit,(void*)&sockfd)!=0)
     {
     	cout<<"Thread creation failed"<<endl;
     }
     while(1)
     {
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	cout<<newsockfd<<endl;
     	if (newsockfd < 0) 
          cout<<"Error on accept"<<endl;
        
        string client_ip=string(inet_ntoa(cli_addr.sin_addr));
        string client_port=to_string(ntohs(cli_addr.sin_port));
        
        clientInfo* newinfo=new clientInfo();
        newinfo->sockfd=newsockfd;
        newinfo->ip=client_ip;
        newinfo->port=client_port;
        
        pthread_t tid;	
        if(pthread_create(&tid,NULL,&tracker_functions,(void*)newinfo)!=0)
        	cout<<"Thread creation failed"<<endl;
        
        pthread_detach(tid);
        
     }
     /*printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) 
     	cout<<"Error writing to socket"<<endl;*/
     close(newsockfd);
     close(sockfd);
     return 0; 
}
