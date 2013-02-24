/* A simple server in the internet domain using TCP
   The port number is passed as an argument */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <iostream>
#include <netdb.h>
#include <new>
#include "net_variable.h"
using namespace std;
void error(const char *msg)
{
    perror(msg);
    exit(0);
}
class General_net
{
	public:
	static  char *net_read(int sock_fd,char *read_buffer)
	{
		//char read_buffer[10000];
		
		if(read(sock_fd,read_buffer,1024)>0)
		      return read_buffer;
		 //if read error
		 return NULL;
		
	}
	static   bool net_write(int sockfd,char *message)
	{
		if(write(sockfd,message,strlen(message))>0)
		     return true;
		 else false;
	}
	
	//splitting the message
	static bool split_message(char *message,char **split,char delim,int max)
	{
		int start=0,end=0,j=0,i;
		for( i=0;i<strlen(message)&&j<max;i++)
		{
			if((message[i]==delim ||i==strlen(message)-1) && i!=start)
			      {
					  if(i!=strlen(message)-1)
					  {split[j]=new char[i-start+1];
					  split[j][i-start]='\0';
					  bcopy(&message[start],split[j],(i-start)*sizeof(char));
				  }
				  else
				  {
					  split[j]=new char[i-start+2];
					  split[j][i-start+1]='\0';
					  bcopy(&message[start],split[j],(i-start+1)*sizeof(char));
				  }
					  
					  
					  //removing additional spaces
					  while(message[i]==delim)
					   i++;
					   start=i;
					   i--;
					   j++;
					  
				  }
		}
		
		if((i<strlen(message)-1) &&(j==max))
		 {
			 char *array=new char[strlen(split[j-1])+1+strlen(message)-i];
			 strcat(array,split[j-1]);
			 strcat(array,&message[i-1]);
			 split[j-1]=array;
		 }
		for(int i=0;i<max;i++)
		{
			if(split[i]==NULL)
			   return false;
		}
		return true;
	}
	
	//remove the unwanted string
	/*void remove_string(char *orginal,char *new_str,char *unwant)
	{
		int size,i=0,j=0;
		char * start=strstr(orginal,unwant);
		if(start!=-1)
		    size=strlen(unwant)*-1;
		 size+=strlen(orginal);
		new_str=new char[size];
	     while(orginal[i]!='0')
		{
			if(i==start)
			   i=start+strlen(unwant);
			
			new_str[j]=orginal[i];
			i++;j++;
			
		}
		   
	}*/
};
class Server 
{
	private:
	char *s_url;
	int s_port_number,s_client_fd,s_server_fd;
	struct sockaddr_in *s_cli_addr;
	public:
	Server()
	{
	}
	Server(char *url)
	{
		
		s_url=url;
		
	}
	Server(int server_fd,int port_number)
	{
		s_server_fd=server_fd;
		s_port_number=port_number;
	}
	//for initial set_up
	 int server_setup(int portno)
	{
		
		int sockfd;
		struct sockaddr_in serv_addr;
		sockfd = socket(AF_INET, SOCK_STREAM, 0);
     if (sockfd < 0) 
        error("ERROR opening socket");
     bzero((char *) &serv_addr, sizeof(serv_addr));
     
     
     serv_addr.sin_family = AF_INET;
     serv_addr.sin_addr.s_addr = INADDR_ANY;
     serv_addr.sin_port = htons(portno);
     if (bind(sockfd, (struct sockaddr *) &serv_addr,
              sizeof(serv_addr)) < 0) 
              error("ERROR on binding");
     listen(sockfd,5);
     s_server_fd=sockfd;
     return sockfd;
     
	}
	
	//for accepting connection
	 int server_accept(int sockfd)
	{
		int newsockfd;
		struct sockaddr_in s_cli_addr;
		socklen_t clilen = sizeof(s_cli_addr);
		bzero((char *)&s_cli_addr,clilen);
		//accepting
		newsockfd = accept(sockfd, 
                 (struct sockaddr *) &s_cli_addr, 
                 &clilen);
		if (newsockfd < 0) 
          error("ERROR on accept");
          
          s_client_fd=newsockfd;
          return newsockfd;
          
          
	}
     int server_accept()
	{
		int newsockfd=0,sockfd=s_server_fd;
		socklen_t clilen = sizeof(s_cli_addr);
		struct sockaddr_in s_cli_addr;
		bzero((char *)&s_cli_addr,clilen);
		//accepting
		newsockfd = accept(sockfd, 
                 (struct sockaddr *) &s_cli_addr, 
                 &clilen);
		if (newsockfd < 0) 
          error("ERROR on accept");
          
          s_client_fd=newsockfd;
          return newsockfd;
          
          
	}
	
	
	
};

class Client
{
	private:
	char *c_url;
	int c_port_number,c_server_fd;
	struct sockaddr_in *c_server_addr;
	public:
	Client(){};
	//connection 
	int client_setup(int portno,char *host)
	{
		cout<<"inside client"<<endl;
		int sockfd;
		struct sockaddr_in serv_addr;
		struct hostent *server; 
	sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("ERROR opening socket");
    server = gethostbyname(host);
    if (server == NULL) {
        fprintf(stderr,"ERROR, no such host\n");
        exit(0);
    }
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, 
         (char *)&serv_addr.sin_addr.s_addr,
         server->h_length);
    serv_addr.sin_port = htons(portno);
    if (connect(sockfd,(struct sockaddr *) &serv_addr,sizeof(serv_addr)) < 0) 
        error("ERROR connecting");
     // change in variable
     c_server_addr=&serv_addr;
     c_server_fd=sockfd;
     return sockfd;
     cout<<"inside client"<<endl;
	}
	
	
	
};


class ProxyServer:public Client,public Server
{
	 private:
     int s_socket_fd,s_client_fd,c_server_fd,portno;
     public:
     int proxy_server_setup(int portno)
     {
		 this->portno=portno;
		 s_socket_fd=server_setup(portno);
		 return s_socket_fd;
	 }
	 int proxy_server_accept()
	 {
		 s_client_fd=server_accept(s_socket_fd);
		 return s_client_fd;
	 }
	 bool proxy_write(int option,char *message)
	 {
		 //if option is 1 write to client
		 if(option==1)
		 {
			 return General_net::net_write(s_client_fd,message);
			 
			 
		 }
		 if(option==2)
		 {
			 return General_net::net_write(c_server_fd,message);
			 
		 }
		 return false;
		 
	 }
	 char *proxy_read(int option,char *response)
	 {
		 
		 if(option==1)
		 return General_net::net_read(s_client_fd,response);
		 if(option==2)
		 return General_net::net_read(c_server_fd,response);
	 }
	 /*void connection_check(int portno)
	 {
		 proxy_server_setup(portno);
		 proxy_server_accept();
		 char *message=proxy_read(1);
		 char *array[3]={NULL};
		cout<< General_net::split_message(message,array,' ',3);
		 for(int i=0;i<3;i++)
		   cout<<array[i]<<"\n";
		   for(int i=0;i<3;i++)
		   if(array[i]==NULL) cout<<"null\n"<<i;
		   
		 
		 
	 }*/
	 
	 //to handle after getting message
	 
	 bool handle_communication(char *http)
	 {
		 char *method[3],*url,*version,*https[3]={NULL},request[5000],*host,port_seperator[]=":",response[10000];
		 int portno=80;
		 char *port_str=NULL,temp[8];
		 
		 if(!General_net::split_message(http,method,' ',3))
			return false;
			 for(int i=0;i<3;i++)
		  cout<<method[i]<<endl;
		  //checking the method is valid
		 if(!is_valid_method(method[0]))
		    return false;
		   //checking http version
		   if(!strcmp(method[2],"http/1.1"))
		       return false;  
		  //spltting the http request
		  if(strstr(method[1],"://"))
		 General_net::split_message(method[1],https,'/',3);
		 else 
		 General_net::split_message(method[1],&https[1],'/',2);
		      
		 
		      host=https[1];
		   
		  //removing www part
		  if(strstr(https[1],"www"))
		        host=&https[1][4];
		   
		  //getting port
		  if((port_str=strstr(https[1],":"))!=NULL)
		  {
			  *port_str='\0';
			  port_str+=sizeof(char);
			  if(!port_str) return false;
			  bcopy(port_str,temp,strlen(port_str));
			  portno=atoi(temp);
			  
		  }
		  cout<<host<<endl<<"port "<<portno<<endl;
		  this->portno=portno;
		        
		
		 
		 if(!https[1] )
		     return false;
		  if(!https[2])
		   {
			   https[2]=new char[1];
			   https[2][0]='\0';
		   }
		  //client setup
		  if((c_server_fd=client_setup(portno,host))>0)
		  cout<<"okkkk\n";
		  if((sprintf(request,"%s /%s http/1.1 \r\nHost:%s\r\nConnection:close\r\n\r\n",method[0],https[2],host))>0)
		  cout<<"okkkk sp\n";
		  proxy_write(2,request);
		  cout<<request<<endl;
		  proxy_read(2,response);
		  proxy_write(1,response);
		  bzero(response,10000);
		 return true;
		 
	 }
	 
	 void connection_request(char *method,char *host)
	 {
		 char array[1024];
		 sprintf(array,"%s / http/1.1 /r/n Host:%s /r/n Conncetion:close\r\n\r\n",method,host);
		 
	 }
	 void close_server()
	 {
		 close(s_socket_fd);
	 }
	  void close_clinet()
	 {
		 close(s_client_fd);
	 }
	 void close_foriegn_server()
	 {
		 
		 
		 close(c_server_fd);
	 }
   	
};
int main(int argc, char *argv[])
{
 /*    Client cl;
     Server se;
     int option,portno=atoi(argv[1]),sock;
     cin>>option;
     if(option==1)
     {
		 char array[100];
		 sock=cl.client_setup(portno,argv[2]);
		 cin>>array;
		 General_net::net_write(sock,array);
		 
		 
	 }
	 else
	 {
		 char *array;
		 int c_sock;
		se.server_setup(portno);
		 sock= se.server_accept();
		array= General_net::net_read(sock);
		c_sock=cl.client_setup(portno,argv[2]);
		
	 }
	 */
	 ProxyServer p;
	 //int n;
	 //cin>>n;
	 //p.connection_check(n);
	 p.proxy_server_setup(atoi(argv[1]));
	 
	 while(1)
	 {
		 char request[1025]; 
		 p.proxy_server_accept();
		 p.proxy_read(1,request);
		 cout<<"request " <<request<<endl;
		 if(!p.handle_communication(request))
		 p.proxy_write(1,"error !!!!!!!!!\n");
		 p.close_foriegn_server();
		 p.close_clinet();
	 }
     
 }
