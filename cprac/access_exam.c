#include<stdio.h>
#include<unistd.h>
#include<errno.h>
#include<sys/types.h>
#include<sys/stat.h>
#include<fcntl.h>
  
extern int errno;
  
int main(int argc, const char *argv[]){
    int fd = access(argv[1], F_OK);
    if(fd == -1){
            printf("Error Number : %d\n", errno);
        perror("Error Description:");
    }
        else
                printf("No error\n");
    return 0;
}
