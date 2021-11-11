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
#include <bits/stdc++.h>
#include <dirent.h>
#include <time.h>
using namespace std;
	
long long int chunksize=512*1024;
string self_uid;
vector<vector<string>> downloaded_files;

void* peer_as_client(void* param)
{
  //  cout<<"client"<<endl;
    char* p = (char*)param;
    struct sockaddr_in serv_addr,cli_addr;
    string ip_port = string(p);
    string ip="",port="",filename="",fileaddr="",destpath="",chunk="";
    long long int i,j;
    for(i=0;i<ip_port.size();i++)
    {
    	if(ip_port[i]==' ')
    		break;
    	ip+=ip_port[i];
    }
  //  cout<<ip<<endl;
    for(i++;i<ip_port.size();i++)
    {
    	if(ip_port[i]==' ')
    		break;
    	port+=ip_port[i];
    }
  //  cout<<port<<endl;
    for(i++;i<ip_port.size();i++)
    {
    	if(ip_port[i]==' ')
    		break;
    	filename+=ip_port[i];
    }
    //cout<<filename<<endl;
    for(i++;i<ip_port.size();i++)
    {
    	if(ip_port[i]==' ')
    		break;
    	fileaddr+=ip_port[i];
    }
    //cout<<fileaddr<<endl;
    for(i++;i<ip_port.size();i++)
    {
    	if(ip_port[i]==' ')
    		break;
    	destpath+=ip_port[i];
    }
    //cout<<destpath<<endl;
    string user="";
    for(i++;i<ip_port.size();i++)
    {
    	if(ip_port[i]=='<')
    		break;
    	user+=ip_port[i];
    }
    //cout<<user<<endl;
    i++;
    vector<long long int> chunk_nums;
    while(i<ip_port.size())
    {
    	if(ip_port[i]=='|')
    	{
    		long long int ch1=stoll(chunk);
    		chunk_nums.push_back(ch1);
    	//	cout<<chunk<<endl;
    		chunk="";
    	}
    	else if(ip_port[i]=='>')
    		break;
    	else
    		chunk+=ip_port[i];
    	i++;
    }
    
   // cout<<"ip="<<ip<<" port="<<port<<" filename="<<filename<<" fileaddr="<<fileaddr<<" user="<<user<<endl;
   /* for(i=0;i<chunk_nums.size();i++)
    cout<<chunk_nums[i]<<endl;*/
    
    int sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        cout<<"Error opening socket";
        
    char* ip_addr=new char[ip.size()+1];
    strcpy(ip_addr, ip.c_str());  
    
    struct hostent *server;
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
    
    FILE* fp=fopen(fileaddr.c_str(),"rb");
    FILE* opfile=fopen(destpath.c_str(),"rb+");
    fseek(fp,0,SEEK_END);
    long long int filesize=ftell(fp);
    fclose(fp);
    
    int flag;
    for(auto itr:chunk_nums)
    {
    	if(itr==*chunk_nums.rbegin())
    	flag=1;
    	else
    	flag=0;
    	cout<<"Chunk "<<itr<<" downloaded from user : "<<user<<endl;		
    	//cout<<itr<<endl;
    	string msg=filename+" "+fileaddr+" "+to_string(itr)+" "+to_string(flag);
    	char* buf=new char[msg.size()+1];
    	strcpy(buf, msg.c_str());
    	
   	int n = write(sockfd,buf,strlen(buf));
    	if (n < 0) 
    		cout<<"Could not write to socket"<<endl;
    
    	
    	//cout<<"FILESIZE IS "<<filesize<<endl; 
    	long long int start_pos=itr*chunksize;
    	
    	long long int csize=chunksize;
    	if(start_pos+chunksize>=filesize)
    	{
    		csize=filesize-start_pos;
    	}
    	
    	
    	char buf1[512*1024];
    	long long int total=0;
    	fseek(opfile,start_pos, SEEK_SET);
   	//cout<<start_pos<<" "<<total<<" "<<csize<<endl;
    	while(total<csize)
    	{
    	//	cout<<total<<" "<<csize<<endl;
    		long long int n1=read(sockfd,buf1,512*1024);
   		if (n1<0) 
    			cout<<"Error reading from socket"<<endl;
 
    	//	cout<<"READ SIZE "<<n1<<endl;
    		total+=n1;
    		fwrite(buf1,1,n1,opfile);
    	}
    }
    fclose(opfile);
    pthread_exit(NULL);
}

void* peer_as_server(void* param)
{
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
     
     while(1)
     {
     	 psockfd = accept(peer_sockfd, (struct sockaddr *) &peer_cli_addr, &clilen);
      	 if (psockfd < 0) 
          	cout<<"Error on accept"<<endl;  
         while(1)
         {
		 char buffer[1024*512];
		 int n=read(psockfd,buffer,1024*512);
	     	  if (n < 0) 
	     	 	cout<<"Cannot read from socket"<<endl;
	     	  	
	     	 long long int i,j;
	     	 string s=string(buffer);
	     	 string filename="",fileaddr="",chunk="",flag;
	     	 for(i=0;i<s.size();i++)
	     	 {
	     	 	if(s[i]==' ')
	     	 	break;
	     	 	filename+=s[i];
	     	 }
	     	 for(i++;i<s.size();i++)
	     	 {
	     	 	if(s[i]==' ')
	     	 	break;
	     	 	fileaddr+=s[i];
	     	 }
	     	 for(i++;i<s.size();i++)
	     	 {
	     	 	if(s[i]==' ')
	     	 	break;
	     	 	chunk+=s[i];
	     	 }
	     	 flag=s.substr(i+1);
	     	 long long int chunknum=stoll(chunk);
	     	 //cout<<chunknum<<endl;
	     	 
	     	 char* filepath=new char[fileaddr.size()+1];
	     	 strcpy(filepath, fileaddr.c_str());
	     	 
	     	 char readbuf[512*1024];
	     	// cout<<filepath<<endl;
	     	 FILE* ipfile=fopen(filepath,"rb");
	     	 fseek(ipfile,0,SEEK_END);
	     	 long long int filesize=ftell(ipfile),csize;
	     	 long long int start_pos=chunknum*chunksize;
	     	 fseek(ipfile,start_pos,SEEK_SET);
	     	 //cout<<"FILESIZE is "<<filesize<<endl;
	     	 if (start_pos+chunksize >= filesize)
		 {
			csize = filesize-start_pos;
		 }
		 else
		 {
			csize=chunksize;
		 }
		 //cout<<"READING SIZE "<<csize<<endl;
	     	 fread(readbuf,1,csize,ipfile);
	     	// cout<<"BUFFER SIZE "<<strlen(readbuf)<<endl;
	     
	     	 int n1=write(psockfd,readbuf,csize);
	     	 if(n1<0)
	     	 	cout<<"Cannot write to socket"<<endl;
	     	 if(flag=="1")
	     	 break;
	     	}
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
    		self_uid=userid;
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
    			if(buf1[0]=='c')
    			{
    				string msg3="n<"+ip+"><"+port+"><"+userid+">";
   			 	char* buf3=new char[msg3.size()+1];
			    	strcpy(buf3, msg3.c_str());  
			    	int n = write(sockfd,buf3,strlen(buf3));
			    	if (n < 0) 
			    		cout<<"Could not write to socket"<<endl; 
			}
    			for(int i=1;i<n1;i++)
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
	     	
	     	string filename="";
		for(i=filepath.size()-1;i>=0;i--)
		{
			if(filepath[i]=='/')
			{
				break;
			}
			filename=filepath[i]+filename;
		}
	     	
	     	FILE* ipfile=fopen(filepath.c_str(),"rb");
	     	fseek(ipfile,0,SEEK_END);
	     	long long int filesize=ftell(ipfile),csize;
	     	     	
		double numc=((double)filesize)/(512*1024);
		long long int c=(long long int)numc;
		if(c!=numc)
		c++;
		//cout<<"NUMBER OF CHUNKS IS "<<c<<endl;
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
    		
    		DIR *d;
		struct dirent *dir;
		d = opendir(dest_path.c_str());
		if(d==NULL) 
		{
		    cout<<"No such path found"<<endl;
		    continue;
		}
    		
    		
    		dest_path=dest_path+"/"+filename;
    		
    		cout<<"Before opening, filepath="<<dest_path<<endl;
    		FILE* fp=fopen(dest_path.c_str(),"a+");
    		fclose(fp);
    		
		string msg="l<"+gid+"><"+filename+">";
    		char* buf=new char[msg.size()+1];
    		strcpy(buf, msg.c_str());  
    		int n = write(sockfd,buf,strlen(buf));
    		if (n < 0) 
         		cout<<"Could not write to socket"<<endl;
		
		vector<vector<string>> v;
		vector<string> temp;
		char buf1[1024*512];
		int num=0;
		int n1=read(sockfd,buf1,1024*512);
    		if (n1<0) 
         		cout<<"Error reading from socket"<<endl;
         	else
    		{
    			string chunk="",uid="";
    			int c=0,u=0;
    			for(int i=0;i<n1;i++)
    			{
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
		long long int i,j;
		
		------------------------------------------------
		
		/*for(i=0;i<v.size();i++)
		{
			cout<<i<<": ";
			for(j=0;j<v[i].size();j++)
			{
				cout<<v[i][j]<<" ";
			}
			cout<<endl;
		}*/
		
		------------------------------------------------
		
		vector<pair<long long int,pair<long long int,vector<string>>>> vect;
		for(i=0;i<v.size();i++)
		{
			long long int len_users=v[i].size();
			vect.push_back({len_users,{i,v[i]}});
		}
		sort(vect.begin(),vect.end());
		//cout<<"Vector size="<<vect.size()<<endl;
		string msg4="o<"+filename+">";
    		char* buf4=new char[msg4.size()+1];
    		strcpy(buf4, msg4.c_str());  
    		int n4 = write(sockfd,buf4,strlen(buf4));
    		if (n4 < 0) 
         		cout<<"Could not write to socket"<<endl;
		
		string fileaddr="";
    		char buf5[1024*512];
		int n5=read(sockfd,buf5,1024*512);
    		if (n5 < 0) 
         		cout<<"Error reading from socket"<<endl;
         	else
    		{
    			for(i=0;i<n5;i++)
    			{
    				fileaddr+=buf5[i];
    			}
    		}
    		downloaded_files.push_back({"D",gid,filename});
    		unordered_map<string,vector<long long int>> chunks_fetch;
    		srand(time(0));	
		for(auto itr:vect)
		{
			if(itr.first==0)
				continue;
			pair<int,vector<string>> p=itr.second;
			long long int num_users=itr.first;
			long long int index=rand()%num_users;
			string fetch_from_uid=p.second[index];
			
			chunks_fetch[fetch_from_uid].push_back(p.first);
		}
		/*for(auto itr:chunks_fetch)
		{
			cout<<itr.first<<": ";
			vector<long long int> v=itr.second;
			for(auto itr1:v)
			{
				cout<<itr1<<" ";
			}
			cout<<endl;
		}*/
		for(auto itr:chunks_fetch)
		{
			string user=itr.first;
			vector<long long int> chunk_nums=itr.second;
		//	cout<<"Chunk "<<itr.second.first<<" downloaded from user : "<<p.second[index]<<endl;
			
			string msg="m<"+user+">";
    			char* buf=new char[msg.size()+1];
    			strcpy(buf, msg.c_str());  
    			int n = write(sockfd,buf,strlen(buf));
    			if (n < 0) 
         			cout<<"Could not write to socket"<<endl;
         			
			string ip1="",port1="";
    			char buf1[1024*512];
			int num=0;
			long long int n1=read(sockfd,buf1,1024*512);
    			if (n1<0) 
         			cout<<"Error reading from socket"<<endl;
         		else
    			{
    				//cout<<buf1<<endl;
    				for(i=0;i<n1;i++)
    				{
    					if(buf1[i]==' ')
    					break;
    					ip1+=buf1[i];
    				}
    				for(i++;i<n1;i++)
    				{
    					if(buf1[i]==' ')
    					break;
    					port1+=buf1[i];
    				}
			}	
			string info_to_be_sent=ip1+" "+port1+" "+filename+" "+fileaddr+" "+dest_path+" "+user+"<";
			//cout<<info_to_be_sent<<endl;
			for(auto itr1:chunk_nums)
			{
				info_to_be_sent+=to_string(itr1)+"|";
			}
			info_to_be_sent+=">";
			char* p2=new char[info_to_be_sent.size()+1];
			strcpy(p2, info_to_be_sent.c_str());	
			
			pthread_t client_peer;
			if(pthread_create(&client_peer,NULL,&peer_as_client,(void*)p2)!=0)
				cout<<"Thread creation failed"<<endl;   
			pthread_join(client_peer,NULL);
		   }
		   for(auto itr:downloaded_files)
		   {
		   	vector<string> v={"D",gid,filename};
		   	if(v==itr)
		   	{
		   		itr[0]="C";
		   	}
		   }
 	}
 	else if(cmd=="logout")
 	{
 		string msg="p";
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
 	else if(cmd=="show_downloads")
 	{
 		for(auto itr:downloaded_files)
		{
			cout<<"["<<itr[0]<<"] ["<<itr[1]<<"] "<<itr[2]<<endl;   	
		}
	}
	else if(cmd=="stop_share")
	{
		
	}
 	else
 	{
 		cout<<"Wrong command, enter again"<<endl;
 		continue;
 	}
 	pthread_t server_peer;
	char* param=new char[port.size()+1];
	strcpy(param, port.c_str());
		
 	if(!server_running)
	{
		server_running=true;
		if(pthread_create(&server_peer,NULL,&peer_as_server,(void*)param)!=0)
			cout<<"Thread creation failed"<<endl;   
		pthread_detach(server_peer);
	}
    }	
    close(sockfd);
    return 0;
}
