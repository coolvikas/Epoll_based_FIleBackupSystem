#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include <fcntl.h>    /* For O_RDWR */
#include <unistd.h> 
#include <string.h>
#include <sys/stat.h>
using namespace std;

#define no_of_duplicate 100

int main(int argc, char const *argv[])
{
	
	
    int src_fd, dst_fd, n, err;
    char buffer[1024];
    char src_path[20];
    char dst_path[20];
    //cout<<"before for loop"<<endl;
    sprintf(src_path,"%s","file.txt");
    cout<<"src_path="<<src_path<<endl;
    for (int i = 1; i <= no_of_duplicate; i++)
    {
    	/* code */
        char newfile[20];
        bzero(newfile,sizeof newfile);
        sprintf(newfile,"%d",i);
       /* const int dir_err = mkdir(string(newfile).c_str(), S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
        if (-1 == dir_err)
        {
            printf("Error creating directory!n");
            exit(1);
        }

        for (int j = 1;j <=50; j++)
        {
                    char file[20];
                    bzero(file,sizeof newfile);
                    sprintf(file,"%d",j);

                    string dst_path = string(newfile)+"/"+string(newfile)+"_"+string(file);
                    
                	//cout<<"in"<<endl;
                	//sprintf(dst_path.c_str(),"%d",i);
                	//cout<<"dst_path="<<dst_path<<endl;
                	//dst_path = i;
                	//cout<<i<<endl;   */
                	FILE *src_fd=NULL;
                	FILE *dst_fd=NULL;
                	src_fd = fopen(src_path,"rb");
                	dst_fd = fopen(newfile,"wb");
                	//cout<<"before while"<<endl;
                	//cout<<sizeof(buffer);
                	while ( fgets(buffer,sizeof(buffer),src_fd)!= NULL) {
                	
                    //cout<<"strlen(buffer)"<<strlen(buffer)<<endl;
                    int count = fwrite(buffer,strlen(buffer),1,dst_fd);;
                    if (count == -1) {
                        printf("Error writing to file.\n");
                        exit(1);

                    }
                	}
                	//cout<<"after while"<<endl;

                	fclose(src_fd);
    	            fclose(dst_fd);
       // }
    }
    
	return 0;
}
