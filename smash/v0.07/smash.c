#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/wait.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<ctype.h>

#define MAX_CMD_LEN 4096
#define MAX_ARGS 10
#define PATH_VAR_LENGTH 128
#define MAX_OUTPUT_LEN 4096

#define DEBUG0
//#define DEBUG1

char path_var[PATH_VAR_LENGTH];

char *trimwhitespace(char *str)
{
	char *end;
	// Trim leading space
	while(isspace((unsigned char)*str)) str++;
	if(*str == 0)  // All spaces?
		return str;
	// Trim trailing space
	end = str + strlen(str) - 1;
	while(end > str && isspace((unsigned char)*end)) end--;
	// Write new null terminator character
	end[1] = '\0';
	return str;
}

void run_cmd(char *ptr) 
{
	char *parmList[MAX_ARGS] = {NULL};
	char *str=NULL,*chunk=NULL;
	char *tmp=NULL,*tmp1=NULL;
	pid_t pid;
	int child_status,i=0,j=0,len;
	int pipefd[2],redirection_flag=0;
	char output_log[MAX_OUTPUT_LEN]="";
	char output_file[100]="";
	int fd;
	
	if(!ptr)
		return;
#ifdef DEBUG0
	printf("cmd0=%s\n",ptr);
#endif
	ptr=strsep(&ptr,"\n");
#ifdef DEBUG0
	printf("cmd1=%s\n",ptr);
#endif
	if(strstr(ptr,">")!=NULL) {
		redirection_flag=1;
		pipe(pipefd);
	}
	len=strlen(ptr);
	str=(char *)malloc(len+15);
	strcpy(str,ptr);
	while((chunk=strsep(&str," "))!=NULL) {
#ifdef DEBUG1
		printf("%d:str:%s\n",__LINE__,str);
#endif
		if(str)
			str=trimwhitespace(str);
#ifdef DEBUG0
		printf("%d:chunk:%s\n",__LINE__,chunk);
#ifdef DEBUG1
		printf("%d:str:%s\n",__LINE__,str);
#endif
#endif
		parmList[i]=chunk;
#ifdef DEBUG0
		printf("%d:parmList[%d]:%s\n",__LINE__,i,parmList[i]);
#endif
		i++;
	}
//change directory command 'cd' implementation
	if(strcmp(parmList[0],"cd")==0) {
		if((parmList[1]==NULL) || (parmList[2]!=NULL)) {
			printf("cd command needs a path to change\n");
			return;
		}
#ifdef DEBUG0
		printf("%d:parmList[0]=%s,parmList[1]=%s\n",__LINE__,parmList[0],parmList[1]);
#endif
		chdir(parmList[1]);
		return;
	}
//path command with add, remove, clear, show sub-commands.
	if(strcmp(parmList[0],"path")==0) {
		if(strcmp(parmList[1],"show")==0) {
			printf("path=%s\n",path_var);
			return;
		}else if(strcmp(parmList[1],"clear")==0) {
			strcpy(path_var,"");
			printf("path=%s\n",path_var);
			return;
		}else if(strcmp(parmList[1],"add")==0) {
			if(strcmp(path_var,"")==0)
				strcat(path_var,parmList[2]);
			else {
				strcat(path_var,":");
				strcat(path_var,parmList[2]);
			}
			printf("path=%s\n",path_var);
			return;
		}else if(strcmp(parmList[1],"remove")==0) {
#ifdef DEBUG0
			printf("%d:path_var=%s\ntmp=%s\nparmList[2]=%s\n",__LINE__,path_var,tmp,parmList[2]);
#endif
			if(strcmp(path_var,"")==0) {
				printf("path variable is already empty\n");
				return;
			}
			tmp=strstr(path_var,parmList[2]);
#ifdef DEBUG0
			printf("%d:path_var=%s\ntmp=%s\nparmList[2]=%s\n",__LINE__,path_var,tmp,parmList[2]);
#endif
			if(strstr(path_var,":")==NULL){
				if(strcmp(path_var,tmp)==0) {
					strcpy(path_var,"");
					printf("path=%s\n",path_var);
					return;
				}
			}
#ifdef DEBUG0
			printf("%d:path_var=%s\ntmp=%s\nparmList[2]=%s\n",__LINE__,path_var,tmp,parmList[2]);
#endif
			len=strlen(parmList[2]);
#ifdef DEBUG0
			printf("len=%d\n",len);
#endif
			tmp1=tmp+len;
#ifdef DEBUG0
			printf("tmp1=%s\n",tmp1);
#endif
			if(*tmp1==':')
				tmp1++;
#ifdef DEBUG0
			printf("tmp1=%s\n",tmp1);
#endif
			if(*tmp1=='\0')
				*(tmp-1)='\0';
			else
				strcpy(tmp,tmp1);
			printf("path_var=%s\n",path_var);
			return;
		}else {
			printf("Invalid command for path\n");
			return;
		}
	}

	if(strstr(parmList[0],"bin/")==NULL) {
		tmp=(char *)malloc(20);
		strcpy(tmp,"/usr/bin/");
#ifdef DEBUG0
		printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
		strcat(tmp,parmList[0]);
#ifdef DEBUG0
		printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
		if(access(tmp,F_OK)==0) {
			parmList[0]=tmp;
#ifdef DEBUG0
			printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
		}else {
			strcpy(tmp,"/bin/");
#ifdef DEBUG0
			printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
			strcat(tmp,parmList[0]);
#ifdef DEBUG0
			printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
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
		printf("%d:tmp=%s,parmList[0]=%s,parmList[1]=%s\n",__LINE__,tmp,parmList[0],parmList[1]);
#endif
	}
#ifdef DEBUG0
	printf("parmList[0]=%s\n",parmList[0]);
	printf("parmList[1]=%s\n",parmList[1]);
	printf("parmList[2]=%s\n",parmList[2]);
#endif
	for(i=0;((i<MAX_ARGS) && (parmList[i]!='\0'));i++) {
#ifdef DEBUG1
		printf("parmList[%d]=%s\n",i,parmList[i]);
#endif
		parmList[i]=trimwhitespace(parmList[i]);
#ifdef DEBUG1
		printf("parmList[%d]=%s\n",i,parmList[i]);
#endif
	}

	if(redirection_flag==1) {
		for(i=1;((i<MAX_ARGS) && (parmList[i]!='\0'));i++) {
#ifdef DEBUG0
			printf("parmList[%d]=%s\n",i,parmList[i]);
#endif
			if(strcmp(parmList[i],">")==0) {
				if(parmList[i+1]!='\0') {
					parmList[i]='\0';
					strcpy(output_file,parmList[i+1]);
					parmList[i+1]='\0';
				} else {
					printf("output file is expected after redirection(>)\n");
					return;
				}
			}
		}
	}	
	if ((pid = fork()) == -1) {
		perror("fork error");
	}
     	else if (pid == 0) {
		// child process
		if(redirection_flag==1) {
			close(pipefd[0]);
			dup2(pipefd[1],1);
			dup2(pipefd[1],2);
		}
		execv(parmList[0], parmList);
	}
	else {
		//parent process
		wait(&child_status);
		if(redirection_flag==1) {
			close(pipefd[1]);
			while(read(pipefd[0],output_log,sizeof(output_log))!=0);
#ifdef DEBUG0
                	printf("output_log=%s\n",output_log);
                	printf("output_file=%s\n",output_file);
#endif
			if(output_file!=NULL) {
				fd=open(output_file,O_RDWR | O_CREAT | O_TRUNC, 0644);
				if (fd < 0) {
					printf("can't open file %s\n",output_file);
					return;
				}
				write(fd,output_log,strlen(output_log));
				close(fd);
			}
		}
	}
	free(tmp);
	return;
}

void parse_run_cmd(char *ptr)
{
	if(!ptr)
		return;
#ifdef DEBUG0
	printf("parse:%s\n",ptr);
#endif
	ptr=trimwhitespace(ptr);
	run_cmd(ptr);
	return;
}

void parse_multiple_cmd(char *ptr)
{
	char *str=NULL,*chunk=NULL;
	int len;
	if(!ptr)
		return;
#ifdef DEBUG0
	printf("multi:%s\n",ptr);
#endif
        ptr=trimwhitespace(ptr);
#ifdef DEBUG0
	printf("multi:%s\n",ptr);
#endif
	len=strlen(ptr);
	str=(char *)malloc(len+1);
	strcpy(str,ptr);
	while((chunk=strsep(&str,";"))!=NULL) {
		if(*chunk=='\0')
			break;
#ifdef DEBUG1
                printf("%d:str:%s\n",__LINE__,str);
#endif
                if(str)
                        str=trimwhitespace(str);
#ifdef DEBUG0
                printf("%d:chunk:%s\n",__LINE__,chunk);
#endif
#ifdef DEBUG1
                printf("%d:chunk:%d\n",__LINE__,*chunk);
#endif
		parse_run_cmd(chunk);
#ifdef DEBUG1
                printf("%d:str:%s\n",__LINE__,str);
#endif
        }
	free(str);
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
				if(strstr(cmd_ptr,";"))
					parse_multiple_cmd(cmd_ptr);
				else
					parse_run_cmd(cmd_ptr);
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
