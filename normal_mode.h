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
#include<set>
using namespace std;

#define set_pos(x,y) printf("%c[%d;%dH",27,x,y)
#define set_cursor(x,y) cout << "\033[" << x+1 << ";" << y << "H";
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
		else if(ch == 58)
		{
			struct winsize w;
			ioctl(STDOUT_FILENO, TIOCGWINSZ, &w);
   			//printf("lines %d\n", w.ws_row);
   			set_cursor(w.ws_row-1,1);

		}
	}
}

void list_files(int index,int prev_pos)
{
	int n=files.size();
	int cursor=index+min(lines,n-index);
        
        //clear terminal
        cout<<"\033[H\033[2J\033[3J";
        
        int i=index;
                
	for(;i<cursor;i++)
	{	
		printfiles(i);
	}
	//if(cmd_mode)
       //	commandMode(cursor);
    	//else 
       move_cursor_normally(index,prev_pos);	
}


void copy_file(string src, string dest)
{
    int fd_src, fd_dest;
    mode_t m = get_mode(src);
    char ch;
    if((fd_src=open(src.c_str(),O_RDONLY))==-1){
        printf("Error in Opening source file");
        return;
    }
    if((fd_dest=open(dest.c_str(),O_CREAT|O_WRONLY,m))==-1){
        printf("file already exists");
        return;
    }
    while(read(fd_src,&ch,1))
        write(fd_dest,&ch,1);
        
       
    close(fd_src);
    close(fd_dest);
}


int main(int argc, char *argv[])
{
	string s = ".";
	char *path = new char[s.length() + 1];
	strcpy(path, s.c_str());
	root = path;
	open_directory(root);
	list_files(0,0);
	return 0;
}
