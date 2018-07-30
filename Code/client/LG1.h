#include <pthread.h>
#include <stdio.h>
#include <iostream>
#include <ctime>
#include <ratio>
#include <chrono>
#include <string>
#include <fstream>
using namespace std;
using namespace std::chrono;
int NUM_THREADS = 0;
int GLOBAL_TIME = 0;
double global_count = 0;
//double global_time=0;
//duration<double> global_max_request_time_span (0);
double global_request_time_span = 0.0;
pthread_mutex_t countlock;
pthread_mutex_t timelock;
pthread_mutex_t requestlock;

int download(int ,int ,int ,char []);
int loginuser(int,char[],char [],int);
void logout(int ,int );
int upload(int ,char [],int );

 void *new_client_thread(void *threadid)
 { 
    high_resolution_clock::time_point t1 = high_resolution_clock::now();
    double local_count = 0;
   // duration<double> thread_max_request_time_span(0);
    double thread_request_time_span = 0.0;
    long tid;
    tid = (long)threadid;
   
    int portno ,n;

    struct sockaddr_in server_addr;
    struct hostent *server;

    
    portno = atoi("6700");
    int sockfd = socket(AF_INET,SOCK_STREAM,0);
    if(sockfd<0){
      perror("Error opening socket");
    }
    else
      printf("\n");

    server = gethostbyname("10.130.5.147");
    if(server == NULL){
      fprintf(stderr, "ERROR no such host\n" );
      exit(0);
    }
    bzero((char *)&server_addr,sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    bcopy((char *)server->h_addr, (char *)&server_addr.sin_addr.s_addr,server->h_length);
    server_addr.sin_port = htons(portno);
    if(connect(sockfd,(struct sockaddr *)&server_addr,sizeof(server_addr))<0){
      perror("ERROR connecting");
    }
    char uname[6];
    bzero(uname,sizeof(uname));
    sprintf(uname,"%ld",tid);
    int sessionid;
    int no_of_uploads = GLOBAL_TIME ;
     sessionid = loginuser(sockfd,uname,uname,no_of_uploads);
    //sessionid = loginuser(sockfd,uname,uname);
   // if(sessionid)
    //{
              while(1)
              {
                       
                    	  srand(time(NULL));
                    	  int r = (rand()%100)+1;
                        char filename[6];
                        sprintf(filename,"%d",r);
                        high_resolution_clock::time_point t3 = high_resolution_clock::now();
                        	//int x = download(sockfd,1,sessionid,filename);
                        
                        int x = upload(sockfd,filename,sessionid);
                        no_of_uploads = no_of_uploads - 1;
                        
                          //  int x =1;
                        if(x==1)
                        {
                    		      high_resolution_clock::time_point t4 = high_resolution_clock::now();
                    		      duration<double> current_request_time_span = duration_cast<duration<double >> (t4 - t3);
                          /*          if(!flag)
                    		{
                    			thread_max_request_time_span=current_request_time_span;
                    			flag=1;
                    		}
                    		else{   
                    			if(current_request_time_span.count()>thread_max_request_time_span.count())
                    			{
                    				thread_max_request_time_span=current_request_time_span;
                    			}
                    		//}*/
                    		      thread_request_time_span+=current_request_time_span.count();
                    		      local_count++;
                        }
                        
                           /*else{
                                break;   
                           }  */
                           
                        high_resolution_clock::time_point t2 = high_resolution_clock::now();
                        duration<double> time_span = duration_cast<duration<double >> (t2 - t1);
                        if(no_of_uploads == 0) //10 sdeconds
                        { 
                    		      logout(sockfd,sessionid);
                    //		cout << time_span.count() << endl;
                    	//update global_count
                           		pthread_mutex_lock(&countlock);
                           		global_count += local_count;
                           		//cout << "Test " << global_count <<   endl;
                           		pthread_mutex_unlock(&countlock);
                    	// update global time
                           	/*	pthread_mutex_lock(&timelock);
                           		global_time += time_span.count();
                           		pthread_mutex_unlock(&timelock);   */
                                    
                    		      pthread_mutex_lock(&requestlock);
                    	/*	if(thread_max_request_time_span.count()>global_max_request_time_span.count())
                    			{
                    				global_max_request_time_span=thread_max_request_time_span;
                    			}*/
                    		      global_request_time_span+=thread_request_time_span;
                    		      pthread_mutex_unlock(&requestlock);
                           		break;
                         	}
                }
	
   //}
    
    
    cout << tid<< "exits" <<endl;
    pthread_exit(NULL);
    
 }

 void spawn_clients(int num_thread, int time)
 {

        NUM_THREADS  = num_thread;
        GLOBAL_TIME = time;
        high_resolution_clock::time_point t1 = high_resolution_clock::now();
        pthread_t threads[NUM_THREADS];
        pthread_mutex_init(&countlock, NULL);
        pthread_mutex_init(&timelock, NULL);
        pthread_mutex_init(&requestlock, NULL);
        int rc;
        long t;

        

        for(t = 1; t<=NUM_THREADS; t++)
        {
           //printf("In main: creating thread %ld\n", t);
               rc = pthread_create(&threads[t], NULL, new_client_thread, (void *)t);
               if (rc){
                  printf("ERROR; return code from pthread_create() is %d\n", rc);
                  exit(-1);
               }
        }
        for(t=1;t<=NUM_THREADS;t++)
        {
               if ( ! pthread_join(threads[t],NULL) ){
                    //printf("Thread detached successfully !!!\n");

               }
        
        }
        high_resolution_clock::time_point t2 = high_resolution_clock::now();
        duration<double> time_span = duration_cast<duration<double>>(t2 - t1);
        std::cout << "It took me " << time_span.count() << " seconds.";
    	
        std::ofstream ofs;
    	
    	
    	

        cout<<"total request count is: " <<global_count<<endl;
       //cout<<"total global time is: " <<global_time<<endl;
        cout<<"no of requests per second is:"<<global_count/time_span.count()<<endl;
        cout<<"Average response time is:"<< (global_request_time_span / global_count) <<endl;
        ofs.open (("results/"+to_string(NUM_THREADS)), std::ofstream::out | std::ofstream::app);
        ofs << global_count/time_span.count()<< " ";
        ofs << (global_request_time_span / global_count)<<endl;
        ofs.close();		
    	
        /* Last thing that main() should do */
        pthread_mutex_destroy(&countlock);
        pthread_mutex_destroy(&timelock);
        pthread_mutex_destroy(&requestlock);
        pthread_exit(NULL);
   
 }
