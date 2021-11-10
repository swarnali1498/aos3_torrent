#include <iostream>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <pthread.h>
#include <fstream>
#include<bits/stdc++.h>
using namespace std;
	
void* peer_as_client(void* param)
{
    cout<<"client"<<endl;
    char* p = (char*)param;
    struct sockaddr_in serv_addr,cli_addr;
    string ip_port = string(p);
    string ip="",port;
    struct hostent *server;
    int i;
    for(i=0;i<ip_port.size();i++)
    {
    	if(ip_port[i]==' ')
    		break;
    	ip+=ip_port[i];
    }
    port=ip_port.substr(i+1);
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"Error opening socket";
        
    char* ip_addr=new char[ip.size()+1];
    strcpy(ip_addr, ip.c_str());  
    		
    server = gethostbyname(ip_addr);
    if (server == NULL) 
    {
        cout<<"No such host"<<endl;
    }
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr,server->h_length);
    serv_addr.sin_port = htons(stoi(port));
    
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
     	   cout<<"Cannot connect"<<endl;
    
    string msg;
    cin>>msg;
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
    return NULL;
}

void* peer_as_server(void* param)
{
    // cout<<"server"<<endl;
     char* port=(char*)param;
     int peer_sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (peer_sockfd < 0) 
         cout<<"Cannot open socket in client"<<endl;
    
     struct sockaddr_in peer_serv_addr, peer_cli_addr;
     peer_serv_addr.sin_family = AF_INET;
     peer_serv_addr.sin_addr.s_addr = INADDR_ANY;
     peer_serv_addr.sin_port = htons(atoi(port));
     if (bind(peer_sockfd, (struct sockaddr *) &peer_serv_addr,sizeof(peer_serv_addr)) < 0) 
     {         
     	cout<<"Error on binding"<<endl;
     	exit(0);
     }
     listen(peer_sockfd,5);
     int psockfd;
     socklen_t clilen;
     clilen = sizeof(peer_cli_addr);
     
     //cout<<peer_sockfd<<endl;
     while(1)
     {
     	 psockfd = accept(peer_sockfd, (struct sockaddr *) &peer_cli_addr, &clilen);
      	 if (psockfd < 0) 
          	cout<<"Error on accept"<<endl;  
         
         cout<<"HERE"<<endl;
         char buffer[256];
         int n=read(psockfd,buffer,255);
     	  if (n < 0) 
     	 	cout<<"Cannot read from socket"<<endl;
     	  	
     	  cout<<buffer<<endl;
     	  
     	  string reply;
     	  cin>>reply;
     	  char* r=new char[reply.size()+1];
     	  strcpy(r,reply.c_str());
     	  
     	  int n1=write(psockfd,r,strlen(r));
     	  if(n1<0)
     	  	cout<<"Cannot write to socket"<<endl;
     }
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
       cout<<"Provide ip,port for client and tracker info"<<endl;
       exit(0);
    }
    
    string ip_port = string(argv[1]);
    string ip,port;
    //cout<<ip_port<<endl;
    for(i=0;i<ip_port.size();i++)
    {
    	if(ip_port[i]==':')
    	{
    		break;
     	}
     	ip+=ip_port[i];
    }
    if(i==ip_port.size())
    {
     	cout<<"Please provide proper ip and port"<<endl;
     	exit(0);
    }
    port=ip_port.substr(i+1);
    
    fstream fr;
    fr.open(argv[2], ios::in);

    vector<string> info;
    if(fr.is_open())
    {
    	string line;
        while(getline(fr, line))
        {
            info.push_back(line);
        }
        fr.close();
    }
    
    char* host=new char[info[0].size()+1];
    strcpy(host, info[0].c_str());  
    		
    portno = stoi(info[1]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"Error opening socket";
    server = gethostbyname(host);
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
     
    bool server_running=false;
    
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
    		string msg="a";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		int n = write(sockfd,buf,strlen(buf));
    		if (n < 0) 
         		cout<<"Could not write to socket"<<endl;
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
   	 	
   	 	string msg="b<"+userid+"><"+password+">";
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
    		
       	string msg="c<"+userid+"><"+password+">";
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
    		
    		string msg="d<"+gid+">";
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
    		
    		string msg="e<"+gid+">";
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
    		
    		string msg="f<"+gid+">";
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
    		
    		string msg="g<"+gid+">";
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
   	 	
   	 	string msg="h<"+gid+"><"+uid+">";
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
 	else if(cmd=="list_groups")
 	{
 		string msg="i";
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
 	else if(cmd=="list_files")
 	{
 		if(i==command.size())
    		{
    			cout<<"Enter group id"<<endl;
    			continue;
    		}
    		string gid=command.substr(i+1);
    		
    		string msg="j<"+gid+">";
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
 	else if(cmd=="upload_file")
 	{
 		pthread_t server_peer;
		char* param=new char[port.size()+1];
		strcpy(param, port.c_str());
		    
		if(i==command.size())
    		{
    			cout<<"Enter file path and group id"<<endl;
    			continue;
    		}
    		command=command.substr(i+1);
    		string filepath="",gid;
    		for(i=0;i<command.size();i++)
    		{
    			if(command[i]==' ')
    			{
    				break;
    			}
    			filepath+=command[i];
    		}	
    		if(i==command.size())
    		{
    			cout<<"Enter group id"<<endl;
    			continue;
    		}
    		gid=command.substr(i+1);
   	 	
   	 	fstream fs;
	     	fs.open(filepath);
		if(fs.fail())
		{
			cout<<"No such file present"<<endl;
			continue;
		}
	     	
		ifstream fSource;
		fSource.open(filepath, ios::binary|ios::in);
		int p = 0;
		int chunksize = 512*1024;
		int lastchunksize;
		int c=1;
		fSource.seekg(0, ios::end);
		int filesize = fSource.tellg();
		
		vector<pair<string,int>> file_chunks;
		
		if (fSource.is_open())
		{
			fSource.seekg(0, ios::beg);
			while (fSource)
			{
			    fSource.seekg(p,ios::beg);
			    if (p+chunksize >= filesize-1)
			    {
			    	lastchunksize = filesize-1-p;
				char ch; 
				string chunk="";
				long long int len=lastchunksize;
				while(len--)
				{
					fSource.get(ch);
					chunk+=ch;
				}
			   	file_chunks.push_back({chunk,lastchunksize});
			    }
			    else
			    {
				char ch; 
				string chunk="";
				long long int len=chunksize;
				while(len--)
				{
					fSource.get(ch);
					chunk+=ch;
				}
				file_chunks.push_back({chunk,chunksize});
			    }
			    p+=chunksize;
			    if(p>=filesize-1)
			    	break;
			    c++;
			    //cout<<chunksize<<endl;
		       }
		       fSource.close();
		       /*for(auto itr:file_chunks)
		       {
		       	cout<<itr.first<<" "<<itr.second<<endl;
		       }*/
	        }

	
   	 	string msg="k<"+filepath+"><"+gid+"><"+to_string(c)+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		//cout<<buf<<endl;
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
    		if(!server_running)
		{
			server_running=true;
			if(pthread_create(&server_peer,NULL,&peer_as_server,(void*)param)!=0)
				cout<<"Thread creation failed"<<endl;   
			pthread_detach(server_peer);
		}
 	}
 	else if(cmd=="download_file")
 	{
 		pthread_t client_peer;
		string p1=ip+" "+port;
		    
		char* p2=new char[p1.size()+1];
		strcpy(p2, p1.c_str());  

		if(i==command.size())
    		{
    			cout<<"Enter group id, file name and destination path"<<endl;
    			continue;
    		}
    		command=command.substr(i+1);
    		string gid="",filename="",dest_path;
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
    			cout<<"Enter filename and destination path"<<endl;
    			continue;
    		}
    		for(i++;i<command.size();i++)
    		{
    			if(command[i]==' ')
    			{
    				break;
    			}
    			filename+=command[i];
    		}
    		if(i==command.size())
    		{
    			cout<<"Enter destination path"<<endl;
    			continue;
    		}
    		dest_path=command.substr(i+1);
   	 		
		string msg="l<"+gid+"><"+filename+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		int n = write(sockfd,buf,strlen(buf));
    		if (n < 0) 
         		cout<<"Could not write to socket"<<endl;
		
		vector<vector<string>> v;
		vector<string> temp;
		char buf1[256];
		int num=0;
		int n1=read(sockfd,buf1,255);
    		if (n1<0) 
         		cout<<"Error reading from socket"<<endl;
         	else
    		{
    			/*for(int i=0;i<n1;i++)
    			{
    				cout<<buf1[i]<<endl;
    			}*/
    			string chunk="",uid="";
    			int c=0,u=0;
    			for(int i=0;i<n1;i++)
    			{
    				//cout<<i<<" "<<buf1[i]<<" "<<temp.size()<<endl;
    				if(buf1[i]=='$')
    				{
    					c=1;
    					u=0;
    					if(uid!="")
    					{
    						temp.push_back(uid);
    						v.push_back(temp);
    					}
    					uid="";
    					chunk="";
    					temp.clear();
				}
    				else if(buf1[i]=='|')
    				{
    					u=1;
    					c=0;
    					if(uid!="")
    					temp.push_back(uid);
    					uid="";
    				}
    				else if(c==1)
    				{
    					chunk+=buf1[i];
    				}
    				else if(u==1)
    				{
    					uid+=buf1[i];
    				}
    			}
    			if(uid!="")
    			temp.push_back(uid);
    			if(temp.size())
    			v.push_back(temp);
    		}
		
		for(long long int i=0;i<v.size();i++)
		{
			cout<<i<<": ";
			for(long long int j=0;j<v[i].size();j++)
			{
				cout<<v[i][j]<<" ";
			}
			cout<<endl;
		}
		/*
		if(pthread_create(&client_peer,NULL,&peer_as_client,(void*)p2)!=0)
			cout<<"Thread creation failed"<<endl;   
		pthread_detach(client_peer);*/
				    
 	}
 	else
 	{
 		cout<<"Wrong command, enter again"<<endl;
 		continue;
 	}
    }	
    close(sockfd);
    return 0;
}
