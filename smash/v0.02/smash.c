#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_CMD_LEN 4096
#define MAX_ARGS 10

/*
void run_cmd_sub()
{
	
	return;
}
*/

void run_cmd(char *ptr) 
{
	char *parmList[MAX_ARGS] = {NULL};
	char *str=NULL,*chunk=NULL;
	pid_t pid;
	int child_status,i=0;
	if(!ptr)
		return;
	printf("cmd0=%s\n",ptr);
	ptr=strsep(&ptr,"\n");
	printf("cmd1=%s\n",ptr);
	str=strdup(ptr);
	while((chunk=strsep(&str," "))!=NULL) {
		printf("chunk:%s\n",chunk);
		parmList[i]=chunk;
		printf("parmList:%s\n",parmList[i]);
		i++;
	}
//	printf("parmList[0]=%s\n",parmList[0]);
//	printf("parmList[1]=%d\n",parmList[1]);
	if ((pid = fork()) == -1)
        	perror("fork error");
     	else if (pid == 0)
        	execv(parmList[0], parmList);
	else
		wait(&child_status);
	return;
}

/*
int run_cmd(char *cmd)
{
 	char* argv[MAX_ARGS];
	pid_t child_pid;
  int child_status;

//  parsecmd(cmd,argv);
  child_pid = fork();
  if(child_pid == 0) {
    // This is done by the child process. 

    execv(argv[0], argv);
    
    // If execv returns, it must have failed.

    printf("Unknown command\n");
    exit(0);
  }
  else {
     // This is run by the parent.  Wait for the child to terminate. 

     do {
       pid_t tpid = wait(&child_status);
       if(tpid != child_pid) process_terminated(tpid);
     } while(tpid != child_pid);

     return child_status;
  }
}
*/
int main(int argc,char *argv[])
{
	char cmd[MAX_CMD_LEN]="";
	char *cmd_ptr="";
	int ret=0,line_cnt=0; 
	size_t max_cmd_len=MAX_CMD_LEN;
	FILE *fp=NULL;
	if(argc > 2) {
		printf("Entered more than 2 arguments.\nUsage is as below:\n$./smash \nor \n$./smash batch.txt\n");
		return EXIT_FAILURE;
	} else if(argc == 1) {
		if(strcmp(argv[0],"./smash")!=0) {
			printf("arg0:smash program name is not smash. Rename it to smash\n");
			return EXIT_FAILURE;
		}
		else {
			printf("arg0=%s\n",argv[0]);
			printf("entering into interactive mode\n");
		}
	} else if(argc == 2) {
                if(strcmp(argv[0],"./smash")!=0) {
                        printf("arg0:smash program name is not smash. Rename it to smash\n");
                        return EXIT_FAILURE;
                }
		if(strcmp(argv[1],"batch.txt")!=0) {
                        printf("arg1:Please pass only batch.txt as input\n");
                        return EXIT_FAILURE;
                }
		printf("arg0=%s\narg1=%s\n",argv[0],argv[1]);
        }
	
	while(1) {
		if(argc==1){
			printf("smash> ");
			cmd_ptr = (char *)malloc(MAX_CMD_LEN);
			ret = getline(&cmd_ptr, &max_cmd_len, stdin);
			if (ret == -1) {
				printf("Error in reading the input\n");
				sleep(1);
				continue;
			}
			else if(strcmp(cmd_ptr,"exit\n")==0){
				free(cmd_ptr);
				exit(0);
			}
			else {
                                printf("You entered the following string:%s,string size=%d\n",cmd_ptr,ret);
                                run_cmd(cmd_ptr);
                        }
			free(cmd_ptr);
		} else {
			break;
		}
	}
	printf("entering to batch mode\n");
	fp = fopen(argv[1], "r");
	if (!fp) {
		fprintf(stderr, "Error opening file '%s'\n", argv[1]);
		return EXIT_FAILURE;
	}
	cmd_ptr = (char *)malloc(MAX_CMD_LEN);
	ret = getline(&cmd_ptr, &max_cmd_len, fp);
	if (ret == -1) {
		printf("Error in reading the input\n");
		sleep(1);
	}
	else {
		while(ret>=0) {
			line_cnt++;
			printf("You entered the following string:%s,string size=%d\n",cmd_ptr,ret);
			run_cmd(cmd_ptr);
			ret=getline(&cmd_ptr, &max_cmd_len, fp);
		}
	}
	free(cmd_ptr);
	fclose(fp);
	return EXIT_SUCCESS;
}
