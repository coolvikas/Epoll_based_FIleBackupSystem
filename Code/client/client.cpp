#include <stdio.h>
#include <string>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <time.h> 
#include <fcntl.h> /* O_WRONLY, O_CREAT */
#include <unistd.h> /* close, write, read */
#include <string.h>
#include <signal.h>
#include <iostream>
#include <cmath>
#include <stdlib.h> //for exit system call
#include <unistd.h> //for read and write functions
#include "fstream"
#include "LG1.h"
#include <signal.h>
#define BUFFER_SIZE 256

//int sessionid;

using namespace std;
void error(const char *msg){
  perror(msg);
  
}

void sig_handler(int signum)
{
 printf("Received signal %d\n", signum);
    //goto XX;
}

int loginuser(int clientsocket,char uname[1],char password[20],int no_of_uploads){
  //cout<<"inside loginuser()"<<endl;
  int sessionid;
  char *buffer=(char *)(malloc(sizeof(char)*BUFFER_SIZE));
  bzero(buffer,256);
  //printf("uname and password entered are : %s %s\n",uname,password );
  int choice=1;
  sprintf(buffer,"%d %s %s %d",choice,uname,password,no_of_uploads);
  //cout<<"login buffer is"<<buffer<<endl;
  int n = write(clientsocket,buffer,strlen(buffer));
  if(n<0){
        error("ERROR writing to socket");
  }
  bzero(buffer,256);
  n = read(clientsocket,buffer,255);
  if(n<0){
           error("ERROR reading from socket");
		cout<<"loginuser() read error"<<endl;
	       if (errno == ECONNRESET)
		{
                perror("loginuser() read error inside if");
		//close(sockfd);
		//pthread_exit(NULL);
		return 0;		
		}
  }
  int status;
  sscanf(buffer,"%d %d",&status,&sessionid);
  if(status == 1){
       //printf("Welcome!You are now logged in.\n");
  }
  else if(status ==0){
  printf("Sorry! User id or password is wrong.\n\n" );
  printf("Try to Log in again or Signup First.\n\n" );
  }
  if(status == 2)
  {
    printf("You are already logged in with session id %d.PLease do other operations.\n\n",sessionid);
  }
  free(buffer);
  //cout<<"session id recerived from server is:"<<sessionid<<endl;
  return sessionid;
  
}

void signupuser(int sockfd){
    char *buffer=(char *)(malloc(sizeof(char)*BUFFER_SIZE));
    char uname[20], password[20];
    printf("Enter username: " );
    scanf("%s",uname );
    printf("Enter password: " );
    scanf("%s",password );
    bzero(buffer,256);
  
    int choice=2;
    sprintf(buffer,"%d %s %s",choice,uname,password);
    int n = write(sockfd,buffer,strlen(buffer));
    if(n<0){
         error("ERROR writing to socket");
    }
    bzero(buffer,256);
    n = read(sockfd,buffer,255);
    if(n<0){
           error("ERROR reading from socket");
    }
    if(!strcmp(buffer,"1")){
           printf("Client is successfully signed up at server end. U may login now.\n\n");
    }
    else if(!strcmp(buffer,"0")){
            printf("Username already exists. Please choose a different username and password.\n\n" );
    }
    else if(!strcmp(buffer,"2")){
    printf("You are already logged in.\n\n" );
    }
    free(buffer);

}


int send_file(int sock, char *file_name){
  int sent_count; /* how many sending chunks, for debugging */
  int read_bytes, /* bytes read from local file */
  sent_bytes, /* bytes sent to connected socket */
  sent_file_size;
  char send_buf[BUFFER_SIZE]; /* max chunk size for sending file */
  const char * errmsg_notfound = "File not found\n";

  int f; /* file handle for reading local file*/
  sent_count = 0;
  sent_file_size = 0;
  /* attempt to open requested file for reading */
  if( (f = open(file_name, O_RDONLY)) < 0) /* can't open requested file */
  {
      error(file_name);
  }

  else /* open file successful */
  {
   // printf("Sending file: %s\n", file_name);
    //  printf("-----------------------------------------------------------");
    while( (read_bytes = read(f, send_buf, BUFFER_SIZE)) > 0 )
   
        
    {
        if( (sent_bytes = write(sock, send_buf, read_bytes))< read_bytes )
        {
            error("send error");
            return -1;
        }

        //cout<<"send_buf="<<send_buf<<endl;
        bzero(send_buf, sizeof send_buf);
        sent_count++;
        sent_file_size += sent_bytes;
        //cout<<"sent file size = "<<sent_file_size<<endl;
    }
  // cout<<"sent_file_size = "<<sent_file_size<< endl;
    close(f);
  } /* end else */
  char response[5];
  bzero(response,sizeof response);
  int n = read(sock,response,sizeof response);
  if(n<0){
  	perror("reading response after sending file");
  }
 cout<<"response from server of sendfile is"<<response<<endl;
  


  //printf("\nUploaded %d bytes to server.\n\n",sent_file_size);
}



//code to upload file to server for backup

int upload(int sockfd, char  filename[50],int sessionid3){
 
  
  if( access( filename, F_OK ) == -1 ) {  
    cout << "File is not present in your current directory.\n\n" << endl;
    return 0;
  }
  char buffer[BUFFER_SIZE];
  bzero(buffer,BUFFER_SIZE);
  int uploadfilenamecode = 3;  // 3 is the code for uploading file name to server and authenticating with session id before uplaoding.
  int n;
  FILE *fptr;
  fptr=fopen(filename,"r");
  
  if(fptr==NULL){
    error("NO such filename exists\n\n");
    return 0;
  }
  fclose(fptr);
  FILE *fptr1;
  fptr1 = fopen(filename,"r");
  fseek(fptr1,0, SEEK_END);
  unsigned long file_len =(unsigned long)ftell(fptr1);
 
  fseek(fptr1,0,SEEK_SET);
  fclose(fptr1);


  sprintf(buffer,"%d %d %s %ld",uploadfilenamecode,sessionid3,filename,file_len);
  //cout<<"upload buffer is"<<buffer<<endl;
 
  n = write(sockfd,buffer,strlen(buffer));
  if(n<0){
      error("ERROR writing to socket");
  }
  bzero(buffer,BUFFER_SIZE);
  n = read(sockfd,buffer,BUFFER_SIZE);
  if(n<0){
      error("ERROR reading from socket");
      cout<<"upload() read error"<<endl;
	if (errno == ECONNRESET)
		{
                cout<<"upload() read error inside if"<<endl;
		//close(sockfd);
		//pthread_exit(NULL);
		return 1;		
		}
  }
  
  int uploadresponse;
  sscanf(buffer,"%d",&uploadresponse);
 // cout<<"upload response = "<<uploadresponse<<endl;
  if(uploadresponse==1){
    //cout<<"Starting file upload to server.\n\n"<<endl;
    send_file(sockfd,filename);
	return 1;
    // function to upload file 
  }
  else if(uploadresponse == 2) {
    cout<<"File is already present in the file system.\n\n";

    return 0;
  }
  else if(uploadresponse == 0)
  {
    cout<<"session id did not match.please login first.\n\n";

    return 0;
  }


}  // upload() closed

//to logout the client from server and removes its session also.
void logout(int sockfd,int sessionid){
    //delete metadat file here
 // cout<<"inside logout"<<endl;
    string filename = "metadata";
    if( access( filename.c_str(), F_OK ) != -1 ){  // means metafile is present so delete it
      //  cout<<"inside access to delete metadata file at client end."<<endl;
        
        if( remove( filename.c_str() ) != 0 ){
            perror( "Error deleting file" );
        }
        else{
           // puts( "File successfully deleted.\n\n" );
        }   
        
    }  // access if closed


    char buffer1[BUFFER_SIZE];
    bzero(buffer1,BUFFER_SIZE);
    int choice = 10;
    sprintf(buffer1,"%d %d",choice,sessionid);
   // cout<<"buffer before sending to server is :"<<buffer1<<endl;
    int n = write(sockfd,buffer1,strlen(buffer1));
    if(n<0){
          error("ERROR writing to socket");
    }
    bzero(buffer1,BUFFER_SIZE);
    n = read(sockfd,buffer1,BUFFER_SIZE);
    if(n<0){
	error("ERROR reading from socket");
	if (errno == ECONNRESET)
		{
		close(sockfd);
		pthread_exit(NULL);
		}
          error("ERROR reading from socket");
    }
    int logout_response;
    sscanf(buffer1,"%d %d",&logout_response,&sessionid);
    if(logout_response==1){
      //  cout<<"User successfully logged out from system."<<endl;
    }
    else{
        cout<<"You are not logged in.\n\n"<<endl;
    }
    //cout<<"logout closed"<<endl;
    close(sockfd);
    
}  // logout() closed

int share(int sockfd,int sessionid)
{
    char filename[50];
    bzero(filename,sizeof filename);
    cout<<"Enter filename:";
    cin>>filename;
    char share_buffer[BUFFER_SIZE];

    memset(&share_buffer,0,sizeof((char *)share_buffer));

    int choice =6;
    sprintf(share_buffer,"%d %d %s",choice,sessionid,filename);
    int n;
   
    n = write(sockfd,share_buffer,strlen(share_buffer));
    if(n<0)
        cout<<"Error writing to share_buffer socket to server"<<endl;
    
    char feedback[BUFFER_SIZE];
    n = recv(sockfd,feedback,sizeof(feedback),0); 
   
    int ack;
    sscanf(feedback,"%d",&ack);
    if(ack==5)
      cout << "File has been shared successfully.\n\n";
    else if(ack == 50|| ack ==10)
      cout << "You have not uploaded this file before.Please upload it first.\n\n";
    else if(ack == 3)
      cout << "You have already shared the file.\n\n";
    else if(ack == 0){
      cout<<"You are not logged in please login first."<<endl;
    }

}

int download(int sockfd,int priv_share,int sessionid,char downloadfile[10]){
  
    //char filename[20];
    //bzero(filename,sizeof filename);
    //cout<<"Enter filename : ";
    //cin>>filename;
    //form a message request in downlaodbuffer
    char download_buffer[BUFFER_SIZE];
    bzero(download_buffer,BUFFER_SIZE);
    int choice;
    if(priv_share)
    	choice = 5;// here 4 is code for downlaod
    else
    	choice = 7;

       
    sprintf(download_buffer,"%d %d %s",choice,sessionid,downloadfile);
    cout<<"download buffer:"<<download_buffer<<endl;
    int n = write(sockfd,download_buffer,strlen(download_buffer));
    if(n<0){
        cout<<"Error writing to download_buffer socket to server"<<endl;
    }
    bzero(download_buffer,BUFFER_SIZE);
   
    n = read(sockfd,download_buffer,sizeof (download_buffer));
    if(n<0){
        cout<<"Error reading server response in download_buffer"<<endl;
    }
    int download_response;
    long download_filesize;
    sscanf(download_buffer,"%d %ld",&download_response,&download_filesize);
    if(download_response==0){
        cout<<"Sorry you are not logged in. Please login first.\n\n"<<endl;
        return 0 ;
    }
    else if (download_response==1){
  //       cout<<"Requested file is found at server with size "<<download_filesize<<" Bytes\n"<<endl;
        // file match is found on server and filesize is received so ack server that client is ready to download file.
        int n = write(sockfd,"filesize_received_ack",21);
        if(n<0){
        cout<<"Error writing filesize_received_ack to server socket."<<endl;
        }
        
        int f; 
        ssize_t rcvd_bytes, rcvd_file_size;
        int recv_count; 
        char recv_str[BUFFER_SIZE]; 
        recv_count = 0; /* number of recv() calls required to receive the file */
        rcvd_file_size = 0; /* size of received file */
 
        if ( (f = open(downloadfile, O_WRONLY|O_CREAT, 0644)) < 0 )
        {
            error("error creating file");
        }
       // cout<<"successfully opened file for writing."<<endl;

       // cout<<"Receiving data from server\n"<<endl;
        //cout<<"test after opening file in write mode"<<endl;
	// printf("-----------------------------------------------------------");
        while ((rcvd_bytes = recv(sockfd, recv_str, BUFFER_SIZE,0)) > 0){

         //   printf("-\b");

            recv_count++;
            rcvd_file_size += rcvd_bytes;
            if (write(f, recv_str, rcvd_bytes) < 0 ){
             error("error writing to file");
            }  
            if(rcvd_file_size == download_filesize)
            {
                
                int n = write(sockfd,"ack",3);
                if (n < 0) {
                    error("ERROR writing to socket");
                }
                break;
            }
        }  //while close
        cout<<endl;
        close(f); /* close file*/

    //    cout<<"\nClient Downloaded "<<rcvd_file_size<<" bytes "<<"\n"<<endl;
	return 1;

        }  // close else if
    else if(download_response == 2){
        cout<<"Requested file is not found in the File system. Please upload first.\n"<<endl;
    }



} // close download

void showuserfilesystem(char *filename)
{


  ifstream fptr;
  fptr.open(filename);
  if (fptr.is_open())
        cout << fptr.rdbuf() << endl;;
  remove(filename);
}

void get_filesystem_from_server (int sockfd,int sessionid)
{
    int choice = 4;
    char buff[BUFFER_SIZE];
    memset(&buff,BUFFER_SIZE,0);

    sprintf(buff,"%d %d",choice,sessionid); 
    //cout<<"buufer is"<<buff<<endl;

    int n = send(sockfd,buff,sizeof(buff),0);
    if (n < 0) error("get_filesystem_from_server:ERROR writing to socket");

    char download_buffer[BUFFER_SIZE];
    bzero(download_buffer,BUFFER_SIZE);
    // read response from server in download_buffer
    n = read(sockfd,download_buffer,sizeof (download_buffer));
    if(n<0){
        cout<<"Error reading server response in download_buffer"<<endl;
    }
    //cout<<"download_buffer received from server is :"<<download_buffer<<endl;
    int download_response;
    long download_filesize;
    sscanf(download_buffer,"%d %ld",&download_response,&download_filesize);
    if (download_response == 0){  // it means the client is not logged in yet.
        cout<<"Please login first."<<endl;
        return; 
    }
    else if(download_response==1){    // client is logged in and ready to receive file from server.
        //cout<<"download_filesize="<<download_filesize<<endl;
        char *filename = (char *)"metadata";
        n = write(sockfd,"filesize_received_ack",21);
        if(n<0){
                cout<<"Error writing filesize received ack to server socket."<<endl;
        }
        //open a file for writing
        int f; //file descriptor
        ssize_t rcvd_bytes, rcvd_file_size;
        int recv_count; 
        char recv_str[BUFFER_SIZE]; 
        recv_count = 0; /* number of recv() calls required to receive the file */
        rcvd_file_size = 0; /* size of received file */
 
        if ( (f = open(filename, O_WRONLY|O_CREAT, 0644)) < 0 )
        {
            error("error creating file");
        }
       
        while ((rcvd_bytes = recv(sockfd, recv_str, BUFFER_SIZE,0)) > 0){
          
      
            recv_count++;
            rcvd_file_size += rcvd_bytes;
            if (write(f, recv_str, rcvd_bytes) < 0 ){
             error("error writing to file");
            }  
            if(rcvd_file_size == download_filesize)
            {
                
                int n = write(sockfd,"ack",3);
                if (n < 0) {
                  error("ERROR writing to socket");
                }
                break;
            }
        }  //while close
        close(f); /* close file*/
        printf("File names and files sizes of your uploaded files are: \n");
        showuserfilesystem(filename);
    }
    else if(download_response == 2){
        
        cout<<"NO files are currently uploaded on server.\n"<<endl;

    }

}  // get_filesystem_from_server() closed

 /* void sigint_handler(int sig)
{
    write(0, "Ahhh! SIGINT!\n", 14);
    logout(sockfd);
    //close(sockfd);
}    */
void get_sharedfile_from_server (int sockfd,int sessionid)
{
    int choice = 8;
    char buff[BUFFER_SIZE];
    memset(&buff,BUFFER_SIZE,0);

    sprintf(buff,"%d %d",choice,sessionid); 
    //cout<<"buufer is"<<buff<<endl;

    int n = send(sockfd,buff,sizeof(buff),0);
    if (n < 0) error("get_filesystem_from_server:ERROR writing to socket");

    char download_buffer[BUFFER_SIZE];
    bzero(download_buffer,BUFFER_SIZE);
    // read response from server in download_buffer
    n = read(sockfd,download_buffer,sizeof (download_buffer));
    if(n<0){
        cout<<"Error reading server response in download_buffer"<<endl;
    }
    //cout<<"download_buffer received from server is :"<<download_buffer<<endl;
    int download_response;
    long download_filesize;
    sscanf(download_buffer,"%d %ld",&download_response,&download_filesize);
    if (download_response == 0){  // it means the client is not logged in yet.
        cout<<"Please login first."<<endl;
        return; 
    }
    else if(download_response==1){    // client is logged in and ready to receive file from server.
        //cout<<"download_filesize="<<download_filesize<<endl;
        char *filename = (char *)"share.txt";
        n = write(sockfd,"filesize_received_ack",21);
        if(n<0){
                cout<<"Error writing filesize received ack to server socket."<<endl;
        }
        //open a file for writing
        int f; //file descriptor
        ssize_t rcvd_bytes, rcvd_file_size;
        int recv_count; 
        char recv_str[BUFFER_SIZE]; 
        recv_count = 0; /* number of recv() calls required to receive the file */
        rcvd_file_size = 0; /* size of received file */
 
        if ( (f = open(filename, O_WRONLY|O_CREAT, 0644)) < 0 )
        {
            error("error creating file");
        }
       
        while ((rcvd_bytes = recv(sockfd, recv_str, BUFFER_SIZE,0)) > 0){
          
      
            recv_count++;
            rcvd_file_size += rcvd_bytes;
            if (write(f, recv_str, rcvd_bytes) < 0 ){
             error("error writing to file");
            }  
            if(rcvd_file_size == download_filesize)
            {
                
                int n = write(sockfd,"ack",3);
                if (n < 0) {
                  error("ERROR writing to socket");
                }
                break;
            }
        }  //while close
        close(f); /* close file*/
        printf("File names and names of users having shared files are: \n");
        showuserfilesystem(filename);
    }
    else if(download_response == 2){
        
        cout<<"NO files are currently uploaded on server.\n"<<endl;
        
        return;
    }


   
        

}  // get_sharedfile_from_server() closed

void deletefile(int sockfd,int sessionid){  // deletes a file from backend and other places
    char delete_buffer[BUFFER_SIZE];
    bzero(delete_buffer,sizeof((int*)delete_buffer));
    int choice = 9;
    char delete_filename[50];
    cout<<"Enter the filename to delete:"<<endl;
    cin>>delete_filename;
    sprintf(delete_buffer,"%d %d %s",choice,sessionid,delete_filename);
    int n = write(sockfd,delete_buffer,strlen(delete_buffer));
    if(n<0){
        cout<<"deletefile() error writing to buffer"<<endl;
    }
    bzero(delete_buffer,sizeof((int*)delete_buffer));
    int response;
    n = read(sockfd,delete_buffer,sizeof((int *)delete_buffer));
   // cout<<"buffer response received is = "<<delete_buffer<<endl;
    if(n<0){
        cout<<"deletefile() error reading from socket"<<endl;
    }
    sscanf(delete_buffer,"%d",&response);
    if(response == 0){
        cout<<"Sorry you are not logged in.\n"<<endl;
        return;
    }
    else if(response == 1){
        cout<<"File is successfully deleted.\n"<<endl;
        return;
    }
    else if(response == 2){
        cout<<"Sorry the requested file does not exist on server.\n"<<endl;
        return;
    }



}


int main(int argc, char *argv[]){
     signal(SIGSEGV, sig_handler);
  spawn_clients(atoi(argv[1]),atoi(argv[2]));
  return 1;
}  // main() closed
