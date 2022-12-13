#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
	pid_t pid;
	int child_status;
	int pipefd[2];
	char buff[100]="";
//	char *const parmList[] = {"/bin/ls", "-l", "/u/userid/dirname", NULL};
//	char *parmList[] = {"/bin/ls",NULL};
	char *parmList[] = {"/bin/pwd",NULL};
//	char *parmList[] = {"/bin/echo","12345",NULL};
	printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
	pipe(pipefd);
	printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
	if ((pid = fork()) == -1)
		perror("fork error");
	else if (pid == 0) {
		//In child process
		close(pipefd[0]);
		printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
		dup2(pipefd[1],1);
		printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
		dup2(pipefd[1],2);
		printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
		execv(parmList[0], parmList);
		printf("Return not expected. Must be an execv error\n");
	}else {
		//In parent process
		printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
		close(pipefd[1]);
		printf("%d:pipefd[0]=%d,pipefd[1]=%d\n",__LINE__,pipefd[0],pipefd[1]);
		wait(&child_status);
		while(read(pipefd[0],buff,sizeof(buff))!=0);
		printf("buff=%s\n",buff);
	}
	return 0;  
}
