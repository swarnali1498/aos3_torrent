#include<bits/stdc++.h>
#include<stdio.h>
#include<dirent.h>
#include<pwd.h>
#include <unistd.h>
#include<stdlib.h>
#include<sys/stat.h> 
#include<sys/ioctl.h>
#include<termios.h>
#include<grp.h>
#include<fcntl.h>
#include<sys/types.h>
#include<cstdio>
#include<set>
using namespace std;

#define set_pos(x,y) printf("%c[%d;%dH",27,x,y)
#define set_cursor(x,y) cout << "\033[" << x+1 << ";" << y << "H";
#define resize_terminal() printf("\033[8;30;120t");
#define clear_terminal() cout<<"\033[H\033[2J\033[3J";
#define clear_line() printf("%c[2K", 27);

void list_files(int,int);
vector<string> files;
char* root;
int cmd_mode=0;
int st=0,e;
stack<string> fstack,bstack;
char currdir[256];
int c1=0;
int lines;

int getch(void)
{
    struct termios oldattr, newattr;
    int ch;
    tcgetattr( STDIN_FILENO, &oldattr );
    newattr = oldattr;
    newattr.c_lflag &= ~( ICANON | ECHO );
    tcsetattr( STDIN_FILENO, TCSANOW, &newattr );
    ch = getchar();
    tcsetattr( STDIN_FILENO, TCSANOW, &oldattr );
    return ch;
}

void copy_file(string src, string dest)
{
    int s,d;
    char ch;
   // cout<<src<<" "<<dest<<endl;
    if((s=open(src.c_str(),O_RDONLY))==-1)
    {
        printf("\nCannot open source file\n");
        return;
    }
    if((d=open(dest.c_str(),O_WRONLY|O_CREAT, S_IRUSR|S_IWUSR))==-1)
    {
        printf("\nCannot open destination folder\n");
        return;
    }
    while(read(s,&ch,1)>0)
        write(d,&ch,1);
        
    close(s);
    close(d);
    
    struct stat srcstat,deststat;
    char* source=new char[src.length() + 1];
    strcpy(source, src.c_str());
    char* des=new char[dest.length() + 1];
    strcpy(des, dest.c_str());
    
    stat(source,&srcstat);
    stat(des,&deststat);
    
    int status1=chown(des,srcstat.st_uid, srcstat.st_gid);
	if(status1!=0)
		cout<<endl<<"Error in setting ownership of file using chown"<<endl;

    int status2=chmod(des,srcstat.st_mode);
	if(status2!=0)
		cout<<endl<<"Error in setting permission of file using chmod"<<endl;
}

void copy_directory(string src,string dest)
{	  	
	char* dname=new char[dest.size()+1];
	strcpy(dname, dest.c_str());	
	int status= mkdir(dname ,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        //cout<<"dname "<<dname<<endl;
        if(status==-1)
	{
	 	cout<<endl<<"Error in creating directory"<<endl;
	}  	
	char* from=new char[src.size()+1];
	strcpy(from, src.c_str());

	char* to=new char[dest.size()+1];
	strcpy(to, dest.c_str());	

	DIR *d;
	struct dirent *dir;
	d = opendir(from);
	if(d==NULL) 
	{
	    cout<<"No such directory found"<<endl;
	}
	else
	{
		while (dir = readdir(d)) 
	        {	 
	    		string filename=dir->d_name;
	    	       // cout<<filename<<endl;
		        if(filename!="." && filename!="..")
		        {	
		      	  	string path_from=src+"/"+filename;	
		  	  	string path_to=dest+"/"+filename;	
		  	  	
		  	  	char* f=new char[path_from.size()+1];
				strcpy(f, path_from.c_str());
				
		  	  	char* t=new char[path_to.size()+1];
				strcpy(t, path_to.c_str());
				
				struct stat st;
		  	  	if (stat(f,&st) == -1) 
				{
			        	cout<<"Cannot read source file/folder"<<endl;
				}
		  		else if(S_ISDIR(st.st_mode))
				{
				    	copy_directory(string(f),string(t));
				}
				else
				{
					copy_file(string(f),string(t));
			        }
		        }
	        }
	        closedir(d);
	}
}

void print_permissions(string s)
{
	struct stat fileInfo;
	char* temp=new char[s.length() + 1];
	strcpy(temp, s.c_str());
	if(stat(temp, & fileInfo)==-1)
	cout<<"Error"<<endl;
	printf((S_ISDIR(fileInfo.st_mode)) ? "d" : "-");
	printf((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
	printf((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
	printf((fileInfo.st_mode & S_IXUSR) ? "x" : "-");
	printf((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
	printf((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
	printf((fileInfo.st_mode & S_IXGRP) ? "x" : "-");
	printf((fileInfo.st_mode & S_IROTH) ? "r" : "-");
	printf((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
	printf((fileInfo.st_mode & S_IXOTH) ? "x" : "-");
	cout<<endl;
}

bool search(string name,char* path)
{
	DIR *d;
	struct dirent *dir;
	//cout<<path<<endl;
	d = opendir(path);
	bool flag=false;
	if(d==NULL) 
	{
	    cout<<"No such directory found"<<endl;
	}
	else
	{
		while (dir = readdir(d)) 
	        {	 
	    		string filename=dir->d_name;
	    	       // cout<<filename<<endl;
		        if(filename==name)
		        {
		        	flag=true;
		        	break;
		        }
		        if(filename!="." && filename!="..")
		        {	
		        	string nextpath=string(path)+"/"+filename;	
		  	  	
		  	  	char* np=new char[nextpath.size()+1];
				strcpy(np, nextpath.c_str());
				
				struct stat st;
		  	  	if (stat(np,&st) == -1) 
				{
			        	//cout<<"Cannot read file/folder "<<np<<endl;
				}
		  		else if(S_ISDIR(st.st_mode))
				{
				    	flag = flag || search(name,np);
				}
		        }
	        }
	        closedir(d);
	}
	return flag;
}

void delete_file(char* file_path)
{
	remove(file_path);
}

void delete_dir(char* dir_path)
{
	//cout<<dir_path<<endl;
	DIR *d;
	struct dirent *dir;
	d = opendir(dir_path);
	if(d==NULL) 
	{
	    cout<<"No such directory found"<<endl;
	}
	else
	{
		while (dir = readdir(d)) 
	        {	 
	    		string filename=dir->d_name;
	    	      //  cout<<filename<<endl;
		        if(filename!="." && filename!="..")
		        {	
		      	  	string newpath=string(dir_path)+"/"+filename;	
		  	  	
		  	  	char* np=new char[newpath.size()+1];
				strcpy(np, newpath.c_str());
				
				struct stat st;
		  	  	if (stat(np,&st) == -1) 
				{
			        	cout<<"Cannot read file/folder"<<endl;
				}
		  		else if(S_ISDIR(st.st_mode))
				{
				    	delete_dir(np);
				}
				else
				{
					delete_file(np);
			        }
		        }
		        else
		        {
		        	string newpath=string(dir_path)+"/"+filename;	
		  	  	
		        	char* np=new char[newpath.size()+1];
				strcpy(np, newpath.c_str());
				
				remove(np);
		        }
	        }
	        closedir(d);
	        remove(dir_path);
	}
}

char* command_mode(char* path)
{
	//cout<<path<<endl;
	while(1)
	{
		//clear_terminal();
		int input;
		string temp="";
		vector<string> cmds;
		stack<char> stck;
		while(1)
   		{
   			input=getch();
   			if(input==10)
   			{
   				break;
   			}
   			if(input==27)
   				return path;
   			if(input=='q')
   				exit(0);
   			if(input==127)
   			{
   				clear_line();
   				cout << "\r";
   				string sentence="";
   				if(stck.empty())
   					continue;
   				char del_char=stck.top();
   				stck.pop();
   				stack<char> cpy=stck;
   				while(!cpy.empty())
   				{
   					char t=cpy.top();
   					cpy.pop();
   					sentence=t+sentence;
   				}
   				cout<<sentence;
   				if(temp!="")
   				{
   					temp=temp.substr(0,temp.size()-1);
   				}
   				else if(temp=="" && stck.top()!=' ')
   				{
   					temp=*cmds.rbegin();
   					cmds.pop_back();
   				}
   				continue;
   			}
   			cout<<(char)input;
   			char ele=(char)input;
   			stck.push(ele);
   			if(ele==' ')
   			{
   				if(temp!="" && temp!=" ")
   				cmds.push_back(temp);
   				temp="";
   				continue;
   			}
   			temp+=input;
   		}
   		if(temp!="" && temp!=" ")
   		{
   			cmds.push_back(temp);
   		}
   		
   		/*cout<<cmds.size()<<endl;
   		for(auto itr:cmds)
   		cout<<itr<<" ";
   		cout<<endl;*/
   		
   		if(cmds.size()==1)
   		{
   			if(cmds[0]=="q")
   			{
   				exit(0);
   			}
   		}
   		else if(cmds.size()==2)
   		{
   			string op=cmds[0];
   			if(op=="search")
   			{
   				//cout<<"path= "<<path<<endl;
   				bool b=search(cmds[1],path);
   				if(b)
   				cout<<"True"<<endl;
   				else
   				cout<<"False"<<endl;
   			}
   			else if(op=="goto")
   			{
   				string finalpath=string(path)+"/"+cmds[1];
   				
   				char* fp=new char[finalpath.size()+1];
   				strcpy(fp,finalpath.c_str());
   				
   				path=fp;
   			}
   			else if(op=="delete_file")
   			{
   				string finalpath=string(path)+"/"+cmds[1];
   				
   				char* fp=new char[finalpath.size()+1];
   				strcpy(fp,finalpath.c_str());
   				
   				delete_file(fp);
   			}
   			else if(op=="delete_dir")
   			{
   				string finalpath=string(path)+"/"+cmds[1];
   				
   				char* fp=new char[finalpath.size()+1];
   				strcpy(fp,finalpath.c_str());
   				
   				delete_dir(fp);
   			}
   		}
   		else if(cmds.size()>=3)
   		{
   			string op=cmds[0];
   			if(op == "copy")
   			{	
   				for(int i=1;i<cmds.size()-1;i++)
   				{
   					string src,dest;
   					if(cmds[cmds.size()-1][0]=='~')
   					{
   						if(cmds[i][0]=='~')
   						dest=string(currdir)+"/"+path+"/"+cmds[cmds.size()-1].substr(1)+"/"+cmds[i].substr(1);
   						else
   						dest=string(currdir)+"/"+path+"/"+cmds[cmds.size()-1].substr(1)+"/"+cmds[i];
   					}
   					else
   					{
   						if(cmds[i][0]=='~')
   						dest=string(currdir)+"/"+path+"/"+cmds[cmds.size()-1]+"/"+cmds[i].substr(1);
   						else
   						dest=string(currdir)+"/"+path+"/"+"/"+cmds[cmds.size()-1]+"/"+cmds[i];
   					}
   					if(cmds[i][0]=='~')
   					{
   						src=string(currdir)+"/"+path+"/"+"/"+cmds[i].substr(1);	
   					}
   					else
   					{
   						src=string(currdir)+"/"+path+"/"+"/"+cmds[i];
   					}
   					struct stat st;
    					char* s=new char[src.length() + 1];
    					//cout<<s<<endl;
    					strcpy(s, src.c_str());
    					stat(s,&st);
    					if(S_ISDIR(st.st_mode))
    					{
    						copy_directory(src,dest);
    					}
    					else
   					{
   						copy_file(src,dest);
   					}
   				}
   			}
   			else if(op=="move")
   			{
   				for(int i=1;i<cmds.size()-1;i++)
   				{
   					string src,dest;
   					if(cmds[cmds.size()-1][0]=='~')
   					{
   						if(cmds[i][0]=='~')
   						dest=string(currdir)+"/"+path+"/"+cmds[cmds.size()-1].substr(1)+"/"+cmds[i].substr(1);
   						else
   						dest=string(currdir)+"/"+path+"/"+cmds[cmds.size()-1].substr(1)+"/"+cmds[i];
   					}
   					else
   					{
   						if(cmds[i][0]=='~')
   						dest=string(currdir)+"/"+path+"/"+cmds[cmds.size()-1]+"/"+cmds[i].substr(1);
   						else
   						dest=string(currdir)+"/"+path+"/"+"/"+cmds[cmds.size()-1]+"/"+cmds[i];
   					}
   					if(cmds[i][0]=='~')
   					{
   						src=string(currdir)+"/"+path+"/"+"/"+cmds[i].substr(1);	
   					}
   					else
   					{
   						src=string(currdir)+"/"+path+"/"+"/"+cmds[i];
   					}
   					struct stat st;
    					char* s=new char[src.length() + 1];
    					//cout<<s<<endl;
    					strcpy(s, src.c_str());
    					stat(s,&st);
    					if(S_ISDIR(st.st_mode))
    					{
    						copy_directory(src,dest);
    						delete_dir(s);
    					}
    					else
   					{
   						copy_file(src,dest);
   						delete_file(s);
   					}
   				}
   			}
   			else if(op=="rename")
   			{
   				if(cmds.size()>3)
   				cout<<"Invalid input"<<endl;
   				else
   				{
   					string old_name=string(currdir)+"/"+path+"/"+cmds[1];
   					string new_name=string(currdir)+"/"+path+"/"+cmds[2];
   					
   					char* on=new char[old_name.size()+1];
   					strcpy(on,old_name.c_str());
   					char* nn=new char[new_name.size()+1];
   					strcpy(nn,new_name.c_str());
					
					int r=rename(on,nn);
					if(r)
					cout<<"Renaming is unsuccesful"<<endl;   
					else
					cout<<"Successful renaming"<<endl;					
   				}
   			}
   			else if(op=="create_file")
   			{
   				string name=cmds[1];
   				string filepath=string(currdir)+"/"+cmds[2]+"/"+name;
   				
   				char* path=new char[filepath.size()+1];
   				strcpy(path,filepath.c_str());
   				
   				//cout<<path<<endl;
   				
   				int status=open(path,O_RDONLY | O_CREAT,S_IRUSR | S_IWUSR | S_IRGRP | S_IROTH ); 	
				if (status == -1)
	   			{
			 		cout<<"Error in creating new file"<<endl;	       
	    			}
   			}
   			else if(op=="create_dir")
   			{
   				string name=cmds[1];
   				string filepath=string(currdir)+"/"+cmds[2]+"/"+name;
   				
   				char* path=new char[filepath.size()+1];
   				strcpy(path,filepath.c_str());
   				
   				//cout<<path<<endl;
   				
   				int status=mkdir(path ,S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH); 	
				if (status == -1)
	   			{
			 		cout<<"Error in creating new directory"<<endl;	       
	    			}
   			}
   		}
   	}
   	return path;
}

void open_directory(char* path)
{
	struct winsize w;
	ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
   	//lines=w.ws_row;
	lines=20;
	getcwd(currdir, 256);
	root=path;
	files.clear();
	DIR *dir;
	struct dirent *diread;
	dir = opendir(path);
	if(dir)
	{
		while ((diread = readdir(dir)) != nullptr) 
		{
			string str(diread->d_name);
            		files.push_back(str);
        	}
        	closedir(dir);
        }
       
        sort(files.begin(),files.end());
}

void printfiles(int index)
{
	struct stat fileInfo;  
        register uid_t uid;
        register gid_t gid;
        register struct group * g;
        register struct passwd * pw;
        
        string s;
        if(string(root)!=".")
        s=string(root)+"/"+files[index];
        else
        s=files[index];
        
       // cout<<s<<endl;
	char* temp=new char[s.length() + 1];
	strcpy(temp, s.c_str());
	if(stat(temp, & fileInfo)==-1)
	cout<<"Error"<<endl;
	
	
	// display's file or directory name 
	string str=files[index];
	
        if(str.size()>20)   
        cout << str.substr(0,17)+"...";
        else
        {
                cout << str;
                for(int i=str.size();i<=20;i++)
               	cout<<" ";
        }

		 
	// size of a file
   	/*double size = (float)fileInfo.st_size / 1024;
        printf("\t%10.4f KB", size);     */
        
        float size=(float)fileInfo.st_size;
        int c=0;
        while(size/1024.0>1)
        {
        	size/=1024.0;     
		c++;
	}
	printf("\t%10.4f ", size);
	if(c==0)
	printf("B");
	else if(c==1)
	printf("KB");
	else if(c==2)
	printf("MB");
	else if(c==3)
	printf("GB");
	

	cout << "\t";

        // permission of a file (r-w-x of user,group and other)
        printf((S_ISDIR(fileInfo.st_mode)) ? "d" : "-");
        printf((fileInfo.st_mode & S_IRUSR) ? "r" : "-");
        printf((fileInfo.st_mode & S_IWUSR) ? "w" : "-");
        printf((fileInfo.st_mode & S_IXUSR) ? "x" : "-");
        printf((fileInfo.st_mode & S_IRGRP) ? "r" : "-");
        printf((fileInfo.st_mode & S_IWGRP) ? "w" : "-");
        printf((fileInfo.st_mode & S_IXGRP) ? "x" : "-");
        printf((fileInfo.st_mode & S_IROTH) ? "r" : "-");
        printf((fileInfo.st_mode & S_IWOTH) ? "w" : "-");
        printf((fileInfo.st_mode & S_IXOTH) ? "x" : "-");

                
        uid = geteuid();
        pw = getpwuid(uid);
        if (pw) 
        {
               printf("\t%s", pw -> pw_name);
               printf("\t%s", pw -> pw_name);
        }

        string dt = ctime( & fileInfo.st_mtime);

        printf("\t");

        // displays last modified date of a file
        for (int i = 0; i < dt.size() - 1; i++)
                printf("%c", dt[i]);
         cout << "\n";
}

void move_cursor_normally(int index,int prev_pos)
{
	e=files.size();
	int n=min(lines,e-index),pos=prev_pos;
	set_pos(0,0);	
	set_cursor(prev_pos,1);
	while(1)
	{
		int ch = getch();
		if (ch == 27)
		{
			ch = getch();
			ch = getch();
			
			switch(ch)
			{
				//If up-arrow Key is pressed
				case 65 : if(pos>0)
				      	   {
                    				printf("\033[1A");
                    				pos--;
                    			   }
                    			   break;
        			
        			//If down-arrow Key is pressed
        			case 66:  if(pos<n-1)
        				   {
       			             printf("\033[1B") ;
                    			     pos++;
                    			   }
				           break;
				
				//If right-arrow key is pressed
				case 67: if(!fstack.empty())
					 {
					 	string oldpath(root),newpath;
					 	bstack.push(oldpath);
					 	string dest=fstack.top();
					 	fstack.pop();
					 	char* path=new char[dest.length() + 1];
					 	strcpy(path, dest.c_str());
					 	open_directory(path);
						list_files(0,0);
					 }
					 break;
					
				//If left-arrow key is pressed 
				case 68: if(!bstack.empty())
					 {
					 	string oldpath(root),newpath;
					 	fstack.push(oldpath);
					 	string dest=bstack.top();
					 	bstack.pop();
					 	char* path=new char[dest.length() + 1];
					 	strcpy(path, dest.c_str());
					 	open_directory(path);
						list_files(0,0);
					 }
					 break;
                	}
		}
		//If k is pressed 
		else if(ch == 107)
		{
			if(index-1>=st)
			{
				list_files(index-1,pos);
                    	}
		}
		//If l is pressed
		else if(ch == 108)
		{
			if(index+n<=e-1)
			{
				list_files(index+1,pos);
				//cout<<index+1<<" "<<pos<<endl;
			}
		}
		//If backspace is pressed
		else if(ch == 127)
		{
			c1--;
			string str(root);
			if(c1<=0)
			{
				str=str+"/..";
			}
			else
			{
				int i=str.size()-1;
				for(;i>=0;i--)
				{
					if(i>=0 && str[i]=='/')
					break;
				}
				str=str.substr(0,i);
			}
			while(!fstack.empty())
                	{
                		fstack.pop();
                	}
              		bstack.push(string(root));
			char* path=new char[str.length() + 1];
			strcpy(path, str.c_str());
			open_directory(path);
			list_files(0,0);
		}
		//If h is pressed
		else if (ch == 104 || ch == 72)
		{
			c1=0;
			if(string(root)!=".")
			{
				while(!fstack.empty())
                		{
                			fstack.pop();
                		}
              			bstack.push(string(root));
              		}
              		string str("/home/dell");
              		char* path=new char[str.length() + 1];
			strcpy(path, str.c_str());
			open_directory(path);
			list_files(0,0);
		}	
		//If enter is pressed
		else if(ch == 10)
		{
			c1++;
			string dir=files[index+pos];
			string currpath(root);
			string newpath;
			if(currpath==".")
			{
				newpath=dir;
			}
			else
			{
				newpath=currpath+"/"+dir;
			}
			char *path = new char[newpath.length() + 1];
			strcpy(path, newpath.c_str());
			
			struct stat sb;
			stat(path, &sb);
			
			//If file type is Directory
			if ((sb.st_mode & S_IFMT) == S_IFDIR)
			{
				if (dir != string("."))
				{
					if (root != NULL)
					{
						bstack.push(string(root));
						while(!fstack.empty())
						{
							fstack.pop();
						}
					}
					root = path;
				}
				open_directory(path);
				list_files(0,0);
			}
			//If file type is Regular File
			else if ((sb.st_mode & S_IFMT) == S_IFREG)
			{
				int fileOpen=open("/dev/null",O_WRONLY);
				dup2(fileOpen,2);
				close(fileOpen);
				pid_t processID = fork();
				if(processID == 0)
				{
					execlp("xdg-open","xdg-open",path,NULL);
					exit(0);
				} 
			}
			else
			{
				cout<<"ERROR"<<endl;
			}
		}
		//If colon is pressed
		else if(ch == 58)
		{
			struct winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
   			//printf("lines %d\n", w.ws_row);
   			//set_cursor(w.ws_row-1,1);
   			clear_terminal();
   			root=command_mode(root);
   			open_directory(root);
   			list_files(0,0);
   		}
   		//If q is pressed
   		else if(ch == 113)
   		{
   			//clear_terminal();
   			exit(0);	
   		}	
   			

	}
}

void list_files(int index,int prev_pos)
{
	int n=files.size();
	int cursor=index+min(lines,n-index);
        
        int i=index;
           
        clear_terminal();
             
	for(;i<cursor;i++)
	{	
		printfiles(i);
	}
	//if(cmd_mode)
       //	commandMode(cursor);
    	//else 
       move_cursor_normally(index,prev_pos);	
}


int main(int argc, char *argv[])
{
	getcwd(currdir,256);
	resize_terminal();
	string s = ".";
	char *path = new char[s.length() + 1];
	strcpy(path, s.c_str());
	root = path;
	open_directory(root);
	list_files(0,0);
	return 0;
}
