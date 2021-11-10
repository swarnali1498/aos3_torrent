#include <bits/stdc++.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#define _BSD_SOURCE
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include<fstream>
#include <pthread.h>
using namespace std;

unordered_map<string,vector<string>> client_list;
unordered_map<string,bool> logged_in;
unordered_map<string,string> client_map;
unordered_map<string,string> owners;
unordered_map<string,vector<string>> groups;
unordered_map<string,vector<string>> pending;
unordered_map<string,vector<string>> pending_clients;
unordered_map<string,unordered_map<string,vector<vector<string>>>> uploaded_files;
unordered_map<string,string> filedetails;

vector<int> sckfd;
vector<pthread_t> tid(20);
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
     		if(buf.size()>2)
     		buf=buf.substr(2);
     		//close client
     		if(ch=='a')
     		{
     			pthread_exit(NULL);
     		}
		// register client
        	if(ch=='b')
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
	     		string addr=client_ip+" "+client_port;
	     		if(client_map.find(addr)!=client_map.end())
	     		{
	     			buf="wYou have already registered. Please login to continue";
	     		}
	     		else if(client_list.find(userid)!=client_list.end())
	     		{
	     			buf="wPlease provide a different user id";	
	     		}
	     		else
	     		{
	     			if(client_list.find(userid)!=client_list.end())
	     			{
	     				buf="wUse a different user id";
	     			}
	     			else
	     			{
	     				client_list[userid]={password,client_ip,client_port};
	     				string addr=client_ip+" "+client_port;
	     				client_map[addr]=userid;
	     				logged_in[userid]=false;
	     				buf="cRegistration successful";
	     			}
	     		}
	     		char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
 		}
     	
     		//login client
     		if(ch=='c')
     		{	
     			//cout<<"2"<<endl;
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
	     		//cout<<userid<<" "<<password<<endl;
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
     			string addr=client_ip+" "+client_port;
	     		string uid=client_map[addr];
	     		string buf1;
	     		if(uid!=userid)
	     		{
	     			buf1="Wrong userid/password";
	     		}
	     		else
	     		{
     				if(logged_in[userid]==true)
	     			{
	     				buf1="Already logged in";
	     			}	
	     			else if(client_list.find(userid)==client_list.end())
	     			{
	 				buf1="Wrong userid/password";
	     			}
	     			else
	     			{
	     				vector<string> v=client_list[userid];
	     				if(v[0]!=password)
	     				{
	 					buf1="Wrong userid/password";
	 				}
	     				else
	     				{
	     					logged_in[userid]=true;
	 					buf1="Successfully logged in";
	 				}
	     			}
	     		}
     			char* msg=new char[buf1.size()+1];
	     		strcpy(msg, buf1.c_str());  
	     		int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		// create group
     		if(ch=='d')
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
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
     				string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}	
	     			else
	     			{
	     				if(groups.find(gid)!=groups.end())
	     				{
	     					buf="Please use a different group id, group "+gid+" already present";
	     				}
	     				else
	     				{
	     					owners[gid]=uid;
	     					groups[gid].push_back(uid);
	     					buf="Group successfully created";
	     				}
	     			}
	     		}
     			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		// join group
     		if(ch=='e')
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
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}	
	     			else
	     			{
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf="No such group present";
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
		     					vector<string> v=pending[gid];
		     					int f1=0;
		     					for(int i=0;i<v.size();i++)
		     					{
		     						if(v[i]==uid)
		     						{
		     							f1=1;
		     							break;
		     						}
		     					}	
		     					if(f1)
		     					{
		     						buf="Request for joining group "+gid+" already pending";
		     					}
		     					else
		     					{
		     						pending[gid].push_back(uid);
		     						buf="Joining request sent for group "+gid;
		     					}
		     				}
		     			}
	     			}
	     		}
     			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		// leave group
     		if(ch=='f')
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
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				cout<<"Please register first"<<endl;
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			cout<<uid<<endl;
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}
	     			else
	     			{
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf="No such group present";
	     				}
	     				else
	     				{
	     					vector<string>::iterator itr;
		     				for(itr=groups[gid].begin();itr!=groups[gid].end();itr++)
		     				{
		     					if(*itr==uid)
		     					{
		     						break;
		     					}
		     				}
		     				if(itr==groups[gid].end())
		     				{
		     					buf="You are not part of group "+gid;
		     				}
		     				else
		     				{
		     					if(owners[gid]==uid)
		     					{
		     						owners.erase(gid);
		     						groups.erase(gid);
		     						uploaded_files.erase(gid);
		     					}
		     					buf="You have left group "+gid;
		     				}
		     			}
	     			}
	     		}
     			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		// list pending join
     		if(ch=='g')
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
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}
	     			else
	     			{
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf="No such group present";
	     				}
	     				else
	     				{
		     				string admin=owners[gid];
						if(admin!=uid)
						{
							buf="You are not the owner of group "+gid;
						}
						else
						{
		     					if(pending[gid].size()==0)
		     						buf="No pending requests for group "+gid;
		     					else
		     					{
		     						buf="";
		     						vector<string> v=pending[gid];
			     					for(auto itr:v)
			     					{
			     						buf+=itr+" ";
			     					}
			     				}
		     				}
		     			}
	     			}
	     		}
	     		char* msg=new char[buf.size()+1];
		     	strcpy(msg, buf.c_str());  
	 		int n1 = write(newsockfd,msg,strlen(msg));
	    		if (n1 < 0) 
		 		cout<<"Could not write to socket"<<endl;
     		}	
     		// accept group joining request
     		if(ch=='h')
     		{
     			string gid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			gid+=buf[i];
	     		}
	     		i+=2;
	     		string user="";
	     		for(;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			user+=buf[i];
	     		}
	     		
	     		string buf;
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}
	     			else
	     			{
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf="No such group present";
	     				}
		     			else
		     			{	
		     				string admin=owners[gid];
						if(admin!=uid)
						{
							buf="You are not the owner of group "+gid;
						}
						else
						{
							int f=0;
							vector<string>::iterator itr;
							for(itr=pending[gid].begin();itr!=pending[gid].end();itr++)
							{
								if(*itr==user)
								{
									f=1;
									break;
								}
							}
							if(!f)
							{
								buf="User "+user+" request not pending in group "+gid;
							}
							else
							{
								pending[gid].erase(itr);
								groups[gid].push_back(user);
								buf="Successfully added user "+user+" to "+"group "+gid;
							}
						}
					}
				}
			}
			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		// list all groups in network
     		if(ch=='i')
     		{
     			string buf;
     			string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}
	     			else
	     			{	
	     				if(groups.size()==0)
	     				{
	     					buf="No groups in network";
	     				}	
	     				else
	     				{
	     					for(auto itr:groups)
	     					{
	     						if(itr!=*groups.begin())
		     						buf+="\n";
		     					buf+=itr.first+" : ";
	     						vector<string> v=itr.second;
	     						for(auto itr1:v)
		     					{
		     						buf+=itr1+" ";
		     					}
		     				}
		     			}
		     		}
			 }
			 char* msg=new char[buf.size()+1];
			 strcpy(msg, buf.c_str());  
		 	 int n1 = write(newsockfd,msg,strlen(msg));
		    	 if (n1 < 0) 
			 	cout<<"Could not write to socket"<<endl;
     		}
     		//list files
     		if(ch=='j')
     		{
     			string gid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			gid+=buf[i];
	     		}
	     		string buf1;
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf1="Please register first";
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf1="Please log in first";
	     			}
	     			else
	     			{
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf1="No such group present";
	     				}
	     				else
	     				{
	     					int f=0;
	     					for(int j=0;j<groups[gid].size();j++)
	     					{
	     						if(groups[gid][j]==uid)
	     						{
	     							f=1;
	     							break;
	     						}
	     					}
	     					if(!f)
	     					{
	     						buf1="You are not part of group "+gid;
	     					}
	     					else
		     				{
		     					if(uploaded_files[gid].size()==0)
		     						buf1="No files uploaded in group "+gid;
		     					else
		     					{
		     						unordered_map<string,vector<vector<string>>> mp=uploaded_files[gid];
		     						for(auto itr:mp)
			     					{	
			     						if(itr.first!=(*mp.begin()).first)
			     						buf1+="\n";
			     						buf1+=itr.first;
			     					}
			     					//buf1=buf1.substr(0,buf1.size()-1);
			     				}
			     			}
		     			}
	     			}
	     		}
	     		char* msg=new char[buf1.size()+1];
			strcpy(msg, buf1.c_str());
			int n1 = write(newsockfd,msg,strlen(msg));
	    		if (n1 < 0) 
		 		cout<<"Could not write to socket"<<endl;
     		}
     		//upload file
     		if(ch=='k')
     		{
     			string filepath="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			filepath+=buf[i];
	     		}
	     		i+=2;
	     		string gid="";
	     		for(;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			gid+=buf[i];
	     		}
	     		i+=2;
	     		string no_of_chunks="";
	     		for(;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			no_of_chunks+=buf[i];
	     		}
	     		long long int num=stoll(no_of_chunks);
	     		string buf;
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
     				//cout<<1<<endl;
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}
	     			else
	     			{
	     				//cout<<2<<endl;
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf="No such group present";
	     				}
		     			else
		     			{
		     				//cout<<3<<endl;
		     				vector<string> temp=groups[gid];
		     				int flag=0;
		     				for(auto itr:temp)
		     				{
		     					if(itr==uid)
		     					{
		     						flag=1;
		     						break;
		     					}	
		     				}
		     				if(!flag)
		     				{
		     					buf="You are not part of group "+gid;
		     				}
		     				else
		    				{
		    					//cout<<4<<endl;
		    					string filename="";
							for(i=filepath.size()-1;i>=0;i--)
							{
								if(filepath[i]=='/')
								{
									break;
								}
								filename=filepath[i]+filename;
							}
							if(uploaded_files.find(gid)==uploaded_files.end())
							{
								unordered_map<string,vector<vector<string>>> mp;
								vector<vector<string>> v(num);
								for(long long int j=0;j<num;j++)
								{
									v[j].push_back(uid);
								}
								mp[filename]=v;
								uploaded_files[gid]=mp;
								filedetails[filename]=filepath;
							}
							else
							{
								//cout<<5<<endl;
								if(uploaded_files[gid].find(filename)==uploaded_files[gid].end())
								{
									vector<vector<string>> v(num);
									for(long long int j=0;j<num;j++)
									{
										v[j].push_back(uid);
									}
									uploaded_files[gid][filename]=v;
									filedetails[filename]=filepath;
								}
								else
								{
									//cout<<6<<endl;
									int f=0;
									for(long long int j=0;j<uploaded_files[gid][filename].size();j++)
									{
										for(long long int k=0;k<uploaded_files[gid][filename][j].size();k++)
										{
											if(uploaded_files[gid][filename][j][k]==uid)
											{
												f=1;
												break;
											}
										}
										if(f)
										break;
									}
									if(!f)
									{
										//cout<<7<<endl;
										for(long long int j=0;j<uploaded_files[gid][filename].size();j++)
										{
											uploaded_files[gid][filename][j].push_back(uid);
										}
									}
									else
										buf="File "+filename+" has already been uploaded by "+uid;
								}
							}
							buf="Successfully uploaded file "+filename+" to group "+gid;
						}
					}
				}
			}
			char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
     		// download file
     		if(ch=='l')
     		{
     			string gid="";
	     		int i,j;
	     		for(i=0;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			gid+=buf[i];
	     		}
	     		i+=2;
	     		string filename="";
	     		for(;i<buf.size();i++)
	     		{
	     			if(buf[i]=='>')
	     			break;
	     			filename+=buf[i];
	     		}
	     		
	     		string buf;
	     		string addr=client_ip+" "+client_port;
     			if(client_map.find(addr)==client_map.end())
     			{
     				buf="Please register first";
     			}
     			else
     			{
	     			string uid=client_map[addr];
	     			if(logged_in[uid]!=true)
	     			{
	     				buf="Please log in first";
	     			}
	     			else
	     			{
	     				if(groups.find(gid)==groups.end())
	     				{
	     					buf="No such group present";
	     				}
		     			else
		     			{
		     				if(uploaded_files.find(gid)==uploaded_files.end())
		     				{
		     					buf="No files uploaded in gid "+gid;
		     				}
		     				else
		     				{
		     					if(uploaded_files[gid].find(filename)==uploaded_files[gid].end())
		     					{
		     						buf="File "+filename+" not present in gid "+gid;
		     					}
		     					else
		     					{
		     						vector<vector<string>> v=uploaded_files[gid][filename];
		     						for(int j=0;j<v.size();j++)
		     						{
		     							buf+="$"+to_string(j);
		     							for(int k=0;k<v[j].size();k++)
		     							{
		     								buf+="|"+v[j][k];
		     							}
		     						}
		     					}
		     				}
		     			}
		     		}
		     	}
		     	char* msg=new char[buf.size()+1];
	     		strcpy(msg, buf.c_str());
	     		cout<<msg<<endl;  
 			int n1 = write(newsockfd,msg,strlen(msg));
    			if (n1 < 0) 
         			cout<<"Could not write to socket"<<endl;
     		}
    	}
}

void* check_if_quit(void* param)
{
	vector<int>* sockfd=(vector<int>*)param;
	while(1)
	{
		string cmd;
		cin>>cmd;
		if(cmd=="quit")
		{
			vector<int> v=*sockfd;
			//cout<<ind<<" "<<v.size()<<endl;
			for(int i=0;i<v.size();i++)
			{	
				close(v[i]);
			}
			for(auto itr:tid)
     			{
     				pthread_cancel(itr);
     			}
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
     fstream fr;
     fr.open(argv[1], ios::in);

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
     
     
     sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        cout<<"Cannot open socket"<<endl;
     portno = stoi(info[1]);
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     sckfd.push_back(sockfd);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
     {         
     	cout<<"Error on binding"<<endl;
     	exit(0);
     }
     listen(sockfd,5);
     pthread_t check_quit;
     clilen = sizeof(cli_addr);
     
     if(pthread_create(&check_quit,NULL,check_if_quit,(void*)&sckfd)!=0)
     {
     	cout<<"Thread creation failed"<<endl;
     }
     while(1)
     {
     	newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
     	if (newsockfd < 0) 
          cout<<"Error on accept"<<endl;
        
        sckfd.push_back(newsockfd);
        
        string client_ip=string(inet_ntoa(cli_addr.sin_addr));
        string client_port=to_string(ntohs(cli_addr.sin_port));
        
        clientInfo* newinfo=new clientInfo();
        newinfo->sockfd=newsockfd;
        newinfo->ip=client_ip;
        newinfo->port=client_port;
        pthread_t t;
        if(pthread_create(&t,NULL,&tracker_functions,(void*)newinfo)!=0)
        	cout<<"Thread creation failed"<<endl;   
     	pthread_detach(t);
     }
     /*printf("Here is the message: %s\n",buffer);
     n = write(newsockfd,"I got your message",18);
     if (n < 0) 
     	cout<<"Error writing to socket"<<endl;*/
     close(newsockfd);
     close(sockfd);
     return 0; 
}
