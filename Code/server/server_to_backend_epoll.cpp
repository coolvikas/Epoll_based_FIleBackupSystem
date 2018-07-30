#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <iterator>
#include <errno.h>
#include <fcntl.h> /* O_WRONLY, O_CREAT */
#include <unistd.h> /* close, write, read */
#include <string.h>
#include <pthread.h>
#include <unistd.h> //for read and write functions
#include <arpa/inet.h> //for inet_ntop() function
#include <pthread.h>
#include <unistd.h> //for read and write functions
#include <arpa/inet.h> //for inet_ntop() function
#include <sys/types.h>
#include <sys/socket.h>   
#include <netdb.h>
#include <fstream>
#include <sys/epoll.h>
#include <vector>
#include <algorithm>
#include <math.h>
#include <sstream>
#include <signal.h>
// #define pt(a) (cout << "Test case:" << a << endl)
#define BUFF_SIZE 256 
#define MAXEVENTS 10000
using namespace std;

int efd;

int curr_sockfd;
int backend_sockfd;

struct epoll_event event;
struct epoll_event *events;

//login state 
vector< pair<int,vector<int> > > cli_login_state;
vector< pair<int,vector<int> > >::iterator login_it;

//login arguments
vector<pair <int,string > > login_args;
vector<pair <int,string > >::iterator login_args_it;

// this map will store the number of upload to be done from client passed as fourth arg of login buffer
vector <pair <int , int> > cli_no_of_uploads;
vector <pair <int , int> >:: iterator cli_no_of_uploads_it;

// upload state
vector< pair<int,vector<int> > > cli_upload_state;
vector< pair<int,vector<int> > >::iterator upload_it;

//upload arguments
vector<pair <int,string> > saved_args1;
vector<pair <int,string> >::iterator it1;

vector<pair <int,vector<string> > > saved_args2;
vector<pair <int,vector<string> > >::iterator it2;

//logout state 
vector< pair<int,vector<int> > > cli_logout_state;
vector< pair<int,vector<int> > >::iterator logout_it;

//logout arguments
vector<pair <int,string > > logout_args;
vector<pair <int,string > >::iterator logout_args_it;

//server to backend upload state
vector <pair <int,vector<int> > > server_backend_upload_state;
vector <pair <int,vector<int> > > :: iterator server_backend_upload_itr;

//server to backend upload arguments

vector<pair <int,vector<string> > > server_backend_upload_args;
vector<pair <int,vector<string> > >::iterator server_backend_upload_args_it;

// this contains backend_sockfd and client socket map  
vector <pair <int , int> > cli_backend_map;
vector <pair <int , int> >:: iterator cli_backend_map_it;

/*
pthread_mutex_t sharedfilelock;
pthread_mutex_t authfilelock;
pthread_mutex_t maplock;   */
const char *shared_file = "share.txt";
const char *auth_file = "new.txt";
//map <char*, int> ip_map_sessionid;
map <int,string> sessionid_map_uname;

//map <char*, string>ip_map_uname;
char *backend_ip;
char *backend_port;
struct clientArgs {
    int socket;
    char buffer[BUFF_SIZE];
};
struct backendArgs {
    char filename[64];
    char userid[64];
    long filesize;
};
void error(const char *msg){
  perror(msg);
  exit(1);
}


void StrCopy(char* str_1, char* str_2)
{
      while(*str_1 != '\0')
      {
      *str_2 = *str_1++;
      ++str_2;
      }
}

bool isEqual(const pair<int, vector<int> >& element)
{
    //cout << curr_sockfd << endl;
    return element.first ==  curr_sockfd;
}
bool isEqual1(const pair<int, string>& element)
{
    return element.first ==  curr_sockfd;
}
bool isEqual2(const pair<int,vector<string > >& element)
{
    return element.first ==  curr_sockfd;
}
bool isEqual3(const pair<int, int>& element)
{
  return element.first ==  curr_sockfd;
}
// backend_sockfd finding functions for verctors
bool isEqualBackend(const pair<int, vector<int> >& element)
{
    //cout << curr_sockfd << endl;
    return element.first ==  backend_sockfd;
}
bool isEqualbackend1(const pair<int, string>& element)
{
    return element.first ==  backend_sockfd;
}
bool isEqualbackend2(const pair<int,vector<string > >& element)
{
    return element.first ==  backend_sockfd;
}

bool isEqualbackend3(const pair<int, int>& element)
{
  return element.first ==  backend_sockfd;
}
void sig_handler(int signum)
{
     // printf("Received signal %d\n", signum);
    //goto XX;
}
void test(string s)
{
  //cout << "TEST " << s << endl;
}
static int make_socket_non_blocking (int sfd)
{
  int flags, s;

  flags = fcntl (sfd, F_GETFL, 0);
  if (flags == -1)
    {
      perror ("fcntl");
      return -1;
    }

  flags |= O_NONBLOCK;
  s = fcntl (sfd, F_SETFL, flags);
  if (s == -1)
    {
      perror ("fcntl");
      return -1;
    }

  return 0;
}


int generatesessionid(char uname[64]){
  
  string username(uname);
  username = uname;
  
  
 	int sessionid = rand(); // generate a random session ID
	
  
  sessionid_map_uname.insert(pair <int,string> (sessionid,username));
  
 
  	return sessionid;
}  // generatesessionid() closed


 
int checksessionactive(int sessionid){
  
	int sessionactiveflag=0;
   
	if (sessionid_map_uname.count(sessionid)>0){
		  sessionactiveflag=1;
  	}
  
	return sessionactiveflag;
}

// to login the user into server 
void verifyuserlogin(int newsockfd,char buffer[BUFF_SIZE])
{

    curr_sockfd = newsockfd;
   
    login_args_it = find_if(login_args.begin(),login_args.end(),isEqual1);

    
    char uname[64],passwd[64];
    bzero(uname, sizeof uname);
    bzero(passwd, sizeof passwd);
  	int initial;
    
    int no_of_uploads;
  	sscanf(buffer,"%d %s %s %d",&initial,uname,passwd,&no_of_uploads);
    
    cli_no_of_uploads.push_back(make_pair(curr_sockfd,no_of_uploads));
  	char funame[64],fpasswd[64];
    bzero(funame, sizeof funame);
    bzero(fpasswd, sizeof fpasswd);
  	char line[64];
    bzero(line, sizeof line);
  	int flag =0;
  	int n;
   
    FILE *fptr;
  	fptr = fopen(auth_file,"r");

  	while((fgets(line,sizeof(line),fptr)!= NULL) && (flag != 1) ){
    	sscanf(line,"%s %s",funame,fpasswd);
    
    	if(!strcmp(uname,funame)){
      	if(!strcmp(passwd,fpasswd)){
      	
      		flag =1;
        
      		break;
      		}
    	}
	   } //while ends
  	fclose(fptr);
    

    
  	if( flag == 1 ){
		//cout<<"Generating a session id"<<endl;
  		int sessionid =generatesessionid(uname);
  		
        bzero(buffer,BUFF_SIZE);
        sprintf(buffer,"%d %d",flag,sessionid);
       
        n = write(newsockfd,buffer,strlen(buffer));
        if (n == -1)
        {
                                                               
                      if (errno != EAGAIN)
                      {
                               //   perror ("read");
                               //done = 1;
                      }
                      return;
        }
        
  	}

  	else if(flag == 0){
  		
        bzero(buffer,BUFF_SIZE);
        int sessionid = 0;
        sprintf(buffer,"%d %d",flag,sessionid);
              //  cout << "hello11\n";
        n = write(newsockfd,buffer,strlen(buffer));
        if (n == -1)
        {
                           if (errno != EAGAIN)
                           {
                                     //   perror ("read");
                                     //done = 1;
                           }
                           return;
        }
        
  	}
    
  (*login_it).second[1] = 1;
  
}  // verifyuserlogin() ends


//function to check if user is previously registered or not 

int checkcredentials(char uname[64],char passwd[64])
{
	
  	char funame[64],fpasswd[64];
  	char line[64];
  	int flag =0;
  	int n;
    
    FILE *fptr;
  	fptr = fopen(auth_file,"r");
  	while(fgets(line,sizeof(line),fptr)!= NULL){
    	sscanf(line,"%s%s",funame,fpasswd);
    	if(!strcmp(uname,funame)){
      		if(!strcmp(passwd,fpasswd)){
      			flag =1;
      			break;
      		}
  		}
  	}

	fclose(fptr);
  	
	return flag;
}


// to connecto to backend server for file transfer to backend
void connect_to_backend()
{
	  
	  struct addrinfo hints, // Used to provide hints to getaddrinfo()
                    *res,  // Used to return the list of addrinfo's
                    *p;    // Used to iterate over this list
    /* Host and port */
  	const char *host=backend_ip;
  	const char *port=backend_port;
	
  	memset(&hints, 0, sizeof(hints));
  	hints.ai_family = AF_UNSPEC;
	  hints.ai_socktype = SOCK_STREAM;
  	if (getaddrinfo(host, port, &hints, &res) != 0)
    {
    	perror("getaddrinfo() failed");
        exit(-1);
    }
   	for(p = res;p != NULL; p = p->ai_next) 
    {
        
        if ((backend_sockfd = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) 
        {
            perror("Could not open socket");
            continue;
        }

        
        if (connect(backend_sockfd, p->ai_addr, p->ai_addrlen) == -1) 
        {
            close(backend_sockfd);
            perror("Could not connect to socket");
            continue;
        }
		break;
    }
    
    /* We don't need the linked list anymore. Free it. */
    freeaddrinfo(res);
    
} //connect_to_backend() ends




// this function sends the file to backend server 
void send_file_to_backend(int choice, char file_name[50],long filesize, char username[64])
{

      server_backend_upload_itr= find_if(server_backend_upload_state.begin(),server_backend_upload_state.end(),isEqualBackend);
      char toSend[BUFF_SIZE];
      bzero(toSend,BUFF_SIZE);

      if((*server_backend_upload_itr).second[0]==0)
      {

            sprintf(toSend,"%d %ld %s %s",choice,filesize,username,file_name);
            int len = write(backend_sockfd,toSend,strlen(toSend));
            if (len == -1)
            {
                              /* If errno == EAGAIN, that means we have read all
                                 data. So go back to the main loop. */
                              perror ("write ");
                              if (errno != EAGAIN)
                                {
                                  //cout<<"inside if errno != EAGAIN"<<endl;
                                  perror ("write");
                                }
                              return; 
            }
            (*server_backend_upload_itr).second[0]=1;
      }

    

      if((*server_backend_upload_itr).second[1]==0)
      {
        char feedback_from_backend[BUFF_SIZE]; 
        memset(&feedback_from_backend,0,sizeof(feedback_from_backend));

        int len = read(backend_sockfd,feedback_from_backend,sizeof feedback_from_backend);
        if (len == -1)
            {
                              /* If errno == EAGAIN, that means we have read all
                                 data. So go back to the main loop. */
                             // perror ("read ");
                              if (errno != EAGAIN)
                                {
                                  //cout<<"inside if errno != EAGAIN"<<endl;
                                 // perror ("read");
                                }
                              return; 
            }
        //cout << "feedback from backend:" << feedback_from_backend << endl;
        (*server_backend_upload_itr).second[1]=1;
             
      }
           
    int n = ceil(double(double(filesize)/BUFF_SIZE));
   
    if((*server_backend_upload_itr).second[2]==0)
    {
         
          (*server_backend_upload_itr).second[2] = (n + 1) ;

    }

    int sentData=0;
    //----buffer chunk to create the file in chunk.
  	char chunk[BUFF_SIZE];
  	int toggle = 0;
    int len;

    FILE *sendFile = NULL;
   
   while ((*server_backend_upload_itr).second[2] != 1)
    {   

            bzero(chunk,sizeof(chunk));
            int val = (*server_backend_upload_itr).second[2];
               
            sendFile = fopen(file_name,"r");
            if(!sendFile)
                 fprintf(stderr, "Error fopen ----> %s", strerror(errno));
               
            long offset = (n-(val-1));
            
            fseek(sendFile, (BUFF_SIZE)*offset, SEEK_SET);
            size_t read_file_size = fread(chunk,sizeof chunk,1, sendFile);
            if(  read_file_size<0){
                perror("cant read from file");
            }
            
            len=write(backend_sockfd,chunk, BUFF_SIZE);
            if (len == -1)
            {
                      /* If errno == EAGAIN, that means we have read all
                         data. So go back to the main loop. */
                      perror ("write ");
                      if (errno != EAGAIN)
                        {
                         // cout<<"inside if errno != EAGAIN"<<endl;
                          perror ("write");
                          //done = 1;
                        }
                      toggle=1;
                      break; 
            }
            sentData+=len;
            (*server_backend_upload_itr).second[2] = (val - 1);
            fclose(sendFile);
              
    } // while closed
   
    if(toggle){
                fclose(sendFile);
                return;
    }


    if((*server_backend_upload_itr).second[2] == 1 && (*server_backend_upload_itr).second[3] == 0)
    {

            	char response[64];
            	bzero(response,64);
              len = read(backend_sockfd,response,64);
              if (len == -1)
              {
                                /* If errno == EAGAIN, that means we have read all
                                   data. So go back to the main loop. */
                              //  perror ("read response after sending file");
                                if (errno != EAGAIN)
                                  {
                                   // cout<<"inside if errno != EAGAIN"<<endl;
                                    perror ("read response after sending file");
                                    //done = 1;
                                  }
                                
                                return;
              }

              
            	if(!strcmp(response,"ack")){  // means backend successfullly received file 
               // cout<<"backend recived full file from server"<<endl;
            	} 
            	
            
              //cout<<"file_name="<<file_name<<endl;
            	if( access( file_name, F_OK ) != -1 )
              {  // means metafile is present so delete it
                    		if( remove( file_name) != 0 ){
                        		perror( "Error deleting file" );
                    		}
                      		else{
                        		//puts( "Intermediate File successfully deleted from server after sending to backend." );
                    		}	
          		
          	}  // access if closed
            (*server_backend_upload_itr).second[3] = 1; 
    }
    

  	close(backend_sockfd);
   
} // send_file_to_backend() ends


 

int receive_file(int socket,const char file_name[64],const char userId[64],long filesize)
{

    string fileLocation = string(file_name);
    FILE *receivedFile = NULL;
    int receivedData=0;
   
    char chunk[BUFF_SIZE];
    int n = ceil(double(double(filesize)/BUFF_SIZE));
    curr_sockfd = socket;
    upload_it = find_if(cli_upload_state.begin(),cli_upload_state.end(),isEqual);
    
    if((*upload_it).second[2] == 0)
    {     
          (*upload_it).second[2] = (n + 1) ;
    }
    
    int file_recv = 0;
    memset(&chunk,0,sizeof(chunk));
    int len;
    int toggle=0;
     
    while ((*upload_it).second[2] != 1)
    {   

           receivedFile = fopen(fileLocation.c_str(),"a");
           if(!receivedFile)
           fprintf(stderr, "Error fopen ----> %s", strerror(errno)); 
           int val = (*upload_it).second[2];
           bzero(chunk,sizeof(chunk));
           int len = read(socket, chunk, sizeof(chunk));
           if (len == -1)
           {
                      //perror ("read ");
                      if (errno != EAGAIN)
                        {
                         // cout<<"inside if errno != EAGAIN"<<endl;
                        //  perror ("read");
                          //done = 1;
                        }
                      toggle=1;
                      break; 
           }
           if(int n = fwrite(chunk, 1,len, receivedFile)<0){
                  error("cant write to file");
           }
           (*upload_it).second[2] = (val - 1);
           receivedData+=len;
           fclose(receivedFile);
              
    } // while closed
     
    if(toggle){
                fclose(receivedFile);
                return 0;
    }
    
    if((*upload_it).second[2] == 1 && (*upload_it).second[3] == 0){
               // upload file to backend
                          connect_to_backend();   // this will give backend_sockfd

                          cli_backend_map.push_back(make_pair(backend_sockfd,curr_sockfd));

                          stringstream ss;
                          ss << filesize;

                          vector<string> v ;
                          v.push_back(string(file_name));
                          v.push_back(string(userId));
                          v.push_back(ss.str());
                          server_backend_upload_args.push_back(make_pair(backend_sockfd,v));





                          int s = make_socket_non_blocking (backend_sockfd);
                          if (s == -1)
                            abort ();
                         

                          event.data.fd = backend_sockfd;
                          event.events = EPOLLIN | EPOLLET;
                          s = epoll_ctl (efd, EPOLL_CTL_ADD, backend_sockfd, &event);
                          if (s == -1)
                            {
                              perror ("epoll_ctl");
                              abort ();
                            }
                         
                           vector <int> server_backend_upload_tmp(4,0);
                           server_backend_upload_state.push_back(make_pair(backend_sockfd,server_backend_upload_tmp));
                           
                           send_file_to_backend(1,(char*)file_name,filesize,(char*)userId);
                       
                            if((*server_backend_upload_itr).second[3]==1)
                            {

                                    cli_backend_map_it = find_if(cli_backend_map.begin(),cli_backend_map.end(),isEqualbackend3);
                                    int client_socket = (*cli_backend_map_it).second;
                                    curr_sockfd = client_socket;
                                    char response[64];
                                    bzero(response,sizeof response);
                                    string reply = "ack";
                                    sprintf(response,reply.c_str(),sizeof reply);
                                    
                                    int n = write(client_socket,response,strlen(response));

                                    if (n< 0) 
                                        perror("ERROR writing to socket");
                                     cli_no_of_uploads_it = find_if(cli_no_of_uploads.begin(),cli_no_of_uploads.end(),isEqual3);
                                     (*cli_no_of_uploads_it).second = (*cli_no_of_uploads_it).second - 1 ;
                                   
                                  
                                    (*upload_it).second[3] = 1;

                                    // clear backend_sockfd maps
                                    server_backend_upload_itr =  find_if(server_backend_upload_state.begin(),server_backend_upload_state.end(),isEqualBackend);
                                    server_backend_upload_state.erase(server_backend_upload_itr);

                                    server_backend_upload_args_it =  find_if(server_backend_upload_args.begin(),server_backend_upload_args.end(),isEqualbackend2);
                                    server_backend_upload_args.erase(server_backend_upload_args_it);

                                    cli_backend_map_it =  find_if(cli_backend_map.begin(),cli_backend_map.end(),isEqualbackend3);
                                    cli_backend_map.erase(cli_backend_map_it);



                            }
                             
                
                test("601");
     }
   
  return (*upload_it).second[2];
     
}  



void receive_from_client(int newsockfd, char buffer[BUFF_SIZE])
{
    it1 = find_if(saved_args1.begin(),saved_args1.end(),isEqual1);
    StrCopy(buffer,(char*)((*it1).second).c_str());
    char filename[10];
    bzero(filename, sizeof filename);
	  int n;
	  int initial;
	  int sid;
	  long int filesize;
    sscanf(buffer,"%d %d %s %ld",&initial,&sid,filename,&filesize);
   
  	int sessionactiveflag=checksessionactive(sid);
   
    
  	bzero(buffer,sizeof buffer);
    if(sessionactiveflag==1 )
    {
  		sprintf(buffer,"%d",sessionactiveflag);
  		n = write(newsockfd,buffer,strlen(buffer));
		  if(n<0)
        	error("Error writing to socket");
  	   (*upload_it).second[1] = 1;
    	char username[64];
      sprintf(username,"%s",sessionid_map_uname[sid].c_str());
     

    	string fileName = string(username) + "_" + string(filename);
      char updated_filename[64];
      sprintf(updated_filename,"%s",fileName.c_str());
      stringstream ss;
      ss << filesize;

      vector<string> v ;
      v.push_back(fileName);
      v.push_back(string(username));
      v.push_back(ss.str());
      saved_args2.push_back(make_pair(newsockfd,v));
      curr_sockfd = newsockfd;
      upload_it = find_if(cli_upload_state.begin(),cli_upload_state.end(),isEqual);
      int status;
      if((*upload_it).second[2] == 0)
      {
        status = receive_file(newsockfd,(char *)fileName.c_str(),username,filesize);
      }
    	     
  
     
   		if(status == 1){
   	 		// update filename in username metadata file.  
			  // handle_backend(updated_filename,username,filesize,1);
    	} //

    	else
;      		//printf("could not receive file from client\n");
	 }

  	// else notify client to login first
  	else if(!sessionactiveflag)
  	{
  		printf("session match not found at server\n");
  		sprintf(buffer,"%d",sessionactiveflag);
  		n = write(newsockfd,buffer,strlen(buffer));
  		if(n<0)
  			error("Error writing to socket");
  		
	   	}  // else if closed
  	


} // receive_from_client() closed



// clear_session 
void clear_session(int newsockfd, char buff[BUFF_SIZE])
{
         test("107");
        int session_cleared=0;

      	int client_sessionid,choice;
      	sscanf(buff,"%d %d",&choice,&client_sessionid);
      	if( access( sessionid_map_uname[client_sessionid].c_str(), F_OK ) != -1 ){  // means metafile is present so delete it
      		if( remove( sessionid_map_uname[client_sessionid].c_str() ) != 0 ){
          		perror( "Error deleting file" );
      		}
        		else{
          		//puts( "User Metadata File successfully deleted" );
      		}	
      		
      	}  // access if closed
      	if (sessionid_map_uname.count(client_sessionid)>0){
      	     sessionid_map_uname.erase (client_sessionid);
            session_cleared=1;
      	 		//cout<<"Map cleared for user."<<endl;
      		}
      	
      	char buffer[BUFF_SIZE];
      	bzero(buffer,BUFF_SIZE);
      	sprintf(buffer,"%d",session_cleared);
      	int n = write(newsockfd,buffer,strlen(buffer));
          if(n<0){
        		error("Error writing to socket");
          }

          (*logout_it).second[1] = 1;
          
          // clear all the maps
          curr_sockfd = newsockfd;
          login_it =  find_if(cli_login_state.begin(),cli_login_state.end(),isEqual);
          cli_login_state.erase(login_it);

          login_args_it =  find_if(login_args.begin(),login_args.end(),isEqual1);
          login_args.erase(login_args_it);

          cli_no_of_uploads_it = find_if(cli_no_of_uploads.begin(),cli_no_of_uploads.end(),isEqual3);
          cli_no_of_uploads.erase(cli_no_of_uploads_it);

          upload_it =  find_if(cli_upload_state.begin(),cli_upload_state.end(),isEqual);
          cli_upload_state.erase(upload_it);

          it1 =  find_if(saved_args1.begin(),saved_args1.end(),isEqual1);
          saved_args1.erase(it1);

          it2 =  find_if(saved_args2.begin(),saved_args2.end(),isEqual2);
          saved_args2.erase(it2);

          logout_it =  find_if(cli_logout_state.begin(),cli_logout_state.end(),isEqual);
          cli_logout_state.erase(logout_it);

          logout_args_it =  find_if(logout_args.begin(),logout_args.end(),isEqual1);
          logout_args.erase(logout_args_it);   
          close(newsockfd);
    
}  // logout closed






int main(int argc, char *argv[])
{
	

  backend_ip = argv[2];
  backend_port = argv[3];
 

  if(argc<3){
    fprintf(stderr, "ERROR, Usage own port, backend ip, backend port\n" );
    exit(1);
  }

  int sfd, portno, x;
  socklen_t clilen;
  
  struct sockaddr_in server_addr, cli_addr;
  int n,s;

  signal(SIGSEGV, sig_handler);
  
  
  sfd = socket(AF_INET, SOCK_STREAM,0);
  if(sfd<0){
    error("ERROR opening socket");
  }
  else{
    printf("Port opened successfully %s \n",(argv[1]) );
  }
  bzero((char *)&server_addr, sizeof(server_addr));
  portno = atoi(argv[1]);
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(portno);
  if(bind(sfd,(struct sockaddr *) &server_addr,sizeof(server_addr))<0){
    error("ERROR on binding");
  }
  s = make_socket_non_blocking (sfd);
  if (s == -1)
    abort ();
  
  printf("Listening for incoming connections\n");
  listen(sfd,1000);
	clilen = sizeof(cli_addr);
  if (s == -1)
    {
      perror ("listen");
      abort ();
    }
    
   
  efd = epoll_create1 (0);
  if (efd == -1)
    {
      perror ("epoll_create");
      abort ();
    }

  event.data.fd = sfd;
  event.events = EPOLLIN | EPOLLOUT | EPOLLET;

  s = epoll_ctl (efd, EPOLL_CTL_ADD, sfd, &event);
  if (s == -1)
    {
      perror ("epoll_ctl");
      abort ();
    }

  /* Buffer where events are returned */
  events = (epoll_event*)calloc (MAXEVENTS, sizeof event);


  while (1)
    {
      int n, i;

      n = epoll_wait (efd, events, MAXEVENTS, 0);

     
      for (i = 0; i < n; i++)
      {
             if ((events[i].events & EPOLLERR) ||
              (events[i].events & EPOLLHUP) ||
              (!(events[i].events & EPOLLIN)))
              {
                      /* An error has occured on this fd, or the socket is not
                         ready for reading (why were we notified then?) */
                fprintf (stderr, "epoll error\n");
                close (events[i].data.fd);
                continue;
              }

             else if (sfd == events[i].data.fd)
              {
                      /* We have a notification on the listening socket, which
                         means one or more incoming connections. */
                      while (1)
                        {
                          struct sockaddr in_addr;
                          socklen_t in_len;
                          int infd;
                          char hbuf[NI_MAXHOST], sbuf[NI_MAXSERV];

                          in_len = sizeof in_addr;
                          infd = accept (sfd,(struct sockaddr *) &cli_addr, &clilen);
                          if (infd == -1)
                            {
                              if ((errno == EAGAIN) ||
                                  (errno == EWOULDBLOCK))
                                {
                                  /* We have processed all incoming
                                     connections. */
                                  break;
                                }
                              else
                                {
                                  perror ("accept");
                                  break;
                                }
                            }

                          s = getnameinfo ((struct sockaddr *)&cli_addr, clilen,
                                           hbuf, sizeof hbuf,
                                           sbuf, sizeof sbuf,
                                           NI_NUMERICHOST | NI_NUMERICSERV);
                          if (s == 0)
                            {
                              printf("Accepted connection on descriptor %d "
                                     "(host=%s, port=%s)\n", infd, hbuf, sbuf);
                            
                            }

                          /* Make the incoming socket non-blocking and add it to the
                             list of fds to monitor. */

                         
                          s = make_socket_non_blocking (infd);
                          if (s == -1)
                            abort ();
                         

                          event.data.fd = infd;
                          event.events = EPOLLIN | EPOLLET;
                          s = epoll_ctl (efd, EPOLL_CTL_ADD, infd, &event);
                          if (s == -1)
                            {
                              perror ("epoll_ctl");
                              abort ();
                            }
                           // initialize the vectors to 0
                           vector <int> login_tmp(2,0);
                           cli_login_state.push_back(make_pair(infd,login_tmp));
                           vector <int> upload_tmp(4,0);
                          cli_upload_state.push_back(make_pair(infd,upload_tmp));
                           vector <int> logout_tmp(2,0);
                          cli_logout_state.push_back(make_pair(infd,logout_tmp));
                         
                        }
                        
                      continue;
                }
          else 
            {
              /* We have data on the fd waiting to be read. Read and
                 call the corresponding function. We must read whatever data is available
                 completely, as we are running in edge-triggered mode
                 and won't get a notification again for the same
                 data. */
              int upload_read = 0,login_read = 0;
              int skip_switch = 0;
              int newsockfd = events[i].data.fd;
            
              char buffer[BUFF_SIZE];
              bzero(buffer, sizeof buffer);
              curr_sockfd = newsockfd;
              backend_sockfd = newsockfd;
              upload_it = find_if(cli_upload_state.begin(),cli_upload_state.end(),isEqual);
              server_backend_upload_itr = find_if(server_backend_upload_state.begin(),server_backend_upload_state.end(),isEqualBackend);
               int count = 0;

              // if the incoming socket activity is on client socket then enter this if
              if(upload_it != cli_upload_state.end())
              {
                          login_it = find_if(cli_login_state.begin(),cli_login_state.end(),isEqual);
                          upload_it = find_if(cli_upload_state.begin(),cli_upload_state.end(),isEqual);
                          logout_it = find_if(cli_logout_state.begin(),cli_logout_state.end(),isEqual);
                         
                          if((*login_it).second[0] == 0)     //login part
                          {
                                    login_read = 1;
                                   
                                    count = read(newsockfd,buffer,sizeof buffer);
                                    if (count == -1)
                                        {
                                                                /* If errno == EAGAIN, that means we have read all
                                                                   data. So go back to the main loop. */
                                                                if (errno != EAGAIN)
                                                                  {
                                                                 //   perror ("read");
                                                                    //done = 1;
                                                                  }
                                                                break;
                                        }

                                    login_args.push_back(make_pair(curr_sockfd,string(buffer)));

                                  
                                    (*login_it).second[0] = 1;
                                    test("101");
                         }
                         else if((*login_it).second[1] == 0)
                         {
                                    login_read = 1;
                                    test("102");
                                    verifyuserlogin(newsockfd,(char*)(*login_args_it).second.c_str());
                                    skip_switch = 1;

                         }
                       
                         else    // upload part
                         {
                                    
                                    test("100");
                                    cli_no_of_uploads_it = find_if(cli_no_of_uploads.begin(),cli_no_of_uploads.end(),isEqual3);
                                   
                                    if(((*upload_it).second[0] == 0) && ((*cli_no_of_uploads_it).second!=0))
                                    {
                                       upload_read = 1 ;
                                       
                                      count = read(newsockfd,buffer,256);
                                        if (count == -1)
                                        {
                                                                /* If errno == EAGAIN, that means we have read all
                                                                   data. So go back to the main loop. */
                                                                if (errno != EAGAIN)
                                                                  {
                                                                 //   perror ("read");
                                                                    //done = 1;
                                                                  }
                                                                break;
                                        }
                                      saved_args1.push_back(make_pair(curr_sockfd,string(buffer)));
                                      (*upload_it).second[0] = 1;
                                    }
                                    else
                                    {
                                       test("151");
                                      if(((*upload_it).second[1] == 0)&&((*cli_no_of_uploads_it).second!=0))
                                        {
                                          upload_read = 1 ;
                                           test("152");
                                            skip_switch =1;
                                           receive_from_client(newsockfd,(char*)(*it1).second.c_str());
                                       
                                          
                                        
                                          
                                        }
                                      else  if(((*cli_no_of_uploads_it).second!=0))
                                      {
                                         test("153");
                                         
                                        it2 = find_if(saved_args2.begin(),saved_args2.end(),isEqual2);
                                        int sockettmp = (*it2).first;
                                        string file_name = (*it2).second[0];
                                        string userId  = (*it2).second[1];
                                        long filesize  = atol((*it2).second[2].c_str());
                                        if((*upload_it).second[2] != 1){
                                              test("900");
                                              upload_read = 1 ;
                                              receive_file(sockettmp,file_name.c_str(),(char*)userId.c_str(),filesize);
                                              skip_switch =1;
                                        }
                                     
                                      }
                                    }

                          }
                          // logout part
                          if((upload_read==0) && (login_read == 0))
                          {  
                                        test("1000");
                                        if(((*logout_it).second[0] == 0))
                                        {
                                                  test("103");
                                                 
                                                  count = read(newsockfd,buffer,sizeof buffer);
                                                   if (count == -1)
                                                      {
                                                                              /* If errno == EAGAIN, that means we have read all
                                                                                 data. So go back to the main loop. */
                                                                              if (errno != EAGAIN)
                                                                                {
                                                                               //   perror ("read");
                                                                                  //done = 1;
                                                                                }
                                                                              break;
                                                      }
                                                 
                                                  logout_args.push_back(make_pair(curr_sockfd,string(buffer)));
                                                  (*logout_it).second[0] = 1;  
                                        }
                                       
                                        else if((*logout_it).second[1] == 0)
                                        {         
                                                  test("105");
                                                  cout<<"hello6\n";
                                                  clear_session(newsockfd,(char*)(*logout_args_it).second.c_str());
                                                  skip_switch = 1;
                                        }
                        }
                          test("701");
                          

             }
             // if the incoming socket activity is on backend socket then enter this else if and call send file to backend as per the previous state
            else if(server_backend_upload_itr != server_backend_upload_state.end())
            {

           
              skip_switch =1;  // since we are going to directly call the function here so we will skip passing through switch case
              server_backend_upload_args_it = find_if(server_backend_upload_args.begin(),server_backend_upload_args.end(),isEqualbackend2);

              int sockettmp = (*server_backend_upload_args_it).first;
              backend_sockfd = sockettmp;
              string file_name = (*server_backend_upload_args_it).second[0];
              string userId  = (*server_backend_upload_args_it).second[1];
              long filesize  = atol((*server_backend_upload_args_it).second[2].c_str());
          
              send_file_to_backend(1,(char*)file_name.c_str(),filesize,(char*)userId.c_str());
            
              if((*server_backend_upload_itr).second[3]==1)
              {

                      test("1600");
                      cli_backend_map_it = find_if(cli_backend_map.begin(),cli_backend_map.end(),isEqualbackend3);
                      curr_sockfd = (*cli_backend_map_it).second;
                      upload_it = find_if(cli_upload_state.begin(),cli_upload_state.end(),isEqual);
                    
                      char response[64];
                      bzero(response,sizeof response);
                      string reply = "ack";
                      sprintf(response,reply.c_str(),sizeof reply);
                   
                      count = write(curr_sockfd,response,3);
                      if (count == -1)
                                        {
                                                                /* If errno == EAGAIN, that means we have read all
                                                                   data. So go back to the main loop. */
                                                                if (errno != EAGAIN)
                                                                  {
                                                                 //   perror ("read");
                                                                    //done = 1;
                                                                  }
                                                                break;
                                        }
                       cli_no_of_uploads_it = find_if(cli_no_of_uploads.begin(),cli_no_of_uploads.end(),isEqual3);
                      (*cli_no_of_uploads_it).second = (*cli_no_of_uploads_it).second - 1 ;
                    
                      test("1610");
                      (*upload_it).second[3] = 1;
                      test("1601");

                      // clear backend_sockfd maps
                      server_backend_upload_itr =  find_if(server_backend_upload_state.begin(),server_backend_upload_state.end(),isEqualBackend);
                      server_backend_upload_state.erase(server_backend_upload_itr);
                      test("1602");
                      server_backend_upload_args_it =  find_if(server_backend_upload_args.begin(),server_backend_upload_args.end(),isEqualbackend2);
                      server_backend_upload_args.erase(server_backend_upload_args_it);
                      test("1603");
                      cli_backend_map_it =  find_if(cli_backend_map.begin(),cli_backend_map.end(),isEqualbackend3);
                      cli_backend_map.erase(cli_backend_map_it);
                      for (int i = 0; i < 4; ++i)
                                    {
                                      (*upload_it).second[i]=0;
                                    }
                      

                        test("1604");
              }
              test("1605");
            }
          

            
             // if skip_switch is zero it means we have to call the corressponding function from switch case as no other function is callled directly in the previous part

              if(skip_switch == 0){   
                    char *garbage=NULL; 
                    int choice;
                   // cout<<"buffer received at server is:"<<buffer<<endl;
                    sscanf(buffer,"%d %s",&choice,garbage);
                  //  cout<<"choice="<<choice<<endl;
                    switch(choice){
                                case 1:
                                    {
                                        verifyuserlogin(newsockfd,buffer);
                                        break;
                                    }
                                case 2:
                                    {
                                       
                                        //signupuser(newsockfd,buffer,ipstr);
                                        break;
                                    }
                                 case 3:
                                  {
                                    test("3");
                                    receive_from_client(newsockfd,buffer); 
                                   
                                    
                                    break;
                                  }
                                  case 4:
                                  {
                                    //show_filesystem_to_client(newsockfd,buffer,ipstr);
                                    break;
                                  }
                                  case 5:
                                  {
                                    //send_to_client(newsockfd,buffer,ipstr,1);
                                    break;
                                  }
                                  case 6:
                                  {
                                    //communicate_with_backend_to_send(choice,file_name,filesize,username.c_str());
                                    //share_filename_with_backend(newsockfd,buffer,ipstr);
                                    break;
                                  }
                                    case 7:
                                  {
                                    //send_to_client(newsockfd,buffer,ipstr,0);
                                    break;
                                  }
                                    case 8:
                                  {
                                    //show_sharedfile_to_client(newsockfd,buffer,ipstr);
                                    break;
                                  }
                                  case 9:
                                  {
                                    //deletefile(newsockfd,buffer,ipstr);
                                    break;
                                  }
                                

                                  case 10:
                                  {
                                    clear_session(newsockfd,buffer);
                                    break;
                                  }
                      }  // switch close
                }
              test("8");
        

            
           
           continue;
            }
        }
    }

  free (events);  // clear the events structure

  close (sfd);   // closing sfd will remove it from epoll efd file descriptor also
	return 0;
}
