#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>

#define MAX_CMD_LEN 4096
#define MAX_ARGS 10
//#define DEBUG0
//#define DEBUG1

void run_cmd(char *ptr) 
{
	char *parmList[MAX_ARGS] = {NULL};
	char *str=NULL,*chunk=NULL;
	char *tmp=NULL;
	pid_t pid;
	int child_status,i=0,j=0,len;
	if(!ptr)
		return;
#ifdef DEBUG0
	printf("cmd0=%s\n",ptr);
#endif
	ptr=strsep(&ptr,"\n");
#ifdef DEBUG0
	printf("cmd1=%s\n",ptr);
#endif
	//str=strdup(ptr);
	len=strlen(ptr);
	str=(char *)malloc(len+15);
	strcpy(str,ptr);
	while((chunk=strsep(&str," "))!=NULL) {
#ifdef DEBUG0
		printf("chunk:%s\n",chunk);
#endif
		parmList[i]=chunk;
#ifdef DEBUG0
		printf("parmList[%d]:%s\n",i,parmList[i]);
#endif
		i++;
	}
	if(strstr(parmList[0],"bin/")==NULL) {
		tmp=(char *)malloc(20);
		strcpy(tmp,"/usr/bin/");
#ifdef DEBUG0
		printf("tmp=%s,parmList[0]=%s,parmList[1]=%s\n",tmp,parmList[0],parmList[1]);
#endif
		strcat(tmp,parmList[0]);
#ifdef DEBUG0
		printf("tmp=%s,parmList[0]=%s,parmList[1]=%s\n",tmp,parmList[0],parmList[1]);
#endif
		if(access(tmp,F_OK)==0) {
			parmList[0]=tmp;
#ifdef DEBUG0
			printf("tmp=%s,parmList[0]=%s,parmList[1]=%s\n",tmp,parmList[0],parmList[1]);
#endif
		}else {
			strcpy(tmp,"/bin/");
#ifdef DEBUG0
			printf("tmp=%s,parmList[0]=%s,parmList[1]=%s\n",tmp,parmList[0],parmList[1]);
#endif
			strcat(tmp,parmList[0]);
#ifdef DEBUG0
			printf("tmp=%s,parmList[0]=%s,parmList[1]=%s\n",tmp,parmList[0],parmList[1]);
#endif
			if(access(tmp,F_OK)==0) {
#ifdef DEBUG0
				printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
				parmList[0]=tmp;
#ifdef DEBUG0
				printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
			} else {
				printf("Command not found\n");
				free(tmp);
				exit(0);
			}
		}
#ifdef DEBUG0
		printf("tmp=%s,parmList[0]=%s,parmList[1]=%s\n",tmp,parmList[0],parmList[1]);
#endif
	}
#ifdef DEBUG0
	printf("parmList[0]=%s\n",parmList[0]);
	printf("parmList[1]=%s\n",parmList[1]);
	printf("parmList[2]=%s\n",parmList[2]);
#endif
	if ((pid = fork()) == -1) {
        	perror("fork error");
	}
     	else if (pid == 0) {
        	execv(parmList[0], parmList);
	}	
	else {
		wait(&child_status);
	}
	free(tmp);
	return;
}

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
#ifdef DEBUG0
			printf("arg0=%s\n",argv[0]);
#endif
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
#ifdef DEBUG0
		printf("arg0=%s\narg1=%s\n",argv[0],argv[1]);
#endif
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
#ifdef DEBUG0
                                printf("You entered the following string:%s,string size=%d\n",cmd_ptr,ret);
#endif
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
#ifdef DEBUG0
			printf("You entered the following string:%s,string size=%d\n",cmd_ptr,ret);
#endif
			run_cmd(cmd_ptr);
			ret=getline(&cmd_ptr, &max_cmd_len, fp);
		}
	}
	free(cmd_ptr);
	fclose(fp);
	return EXIT_SUCCESS;
}
