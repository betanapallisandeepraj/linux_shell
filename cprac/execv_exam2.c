#include<sys/types.h>
#include<unistd.h>
#include<stdio.h>
#include<sys/types.h>
#include<sys/wait.h>

int main()
{
	pid_t pid;
	int child_status;
//	char *const parmList[] = {"/bin/ls", "-l", "/u/userid/dirname", NULL};
	char *parmList[] = {"/bin/ls",NULL};

	if ((pid = fork()) == -1)
		perror("fork error");
	else if (pid == 0) {
		execv("/bin/ls", parmList);
		printf("Return not expected. Must be an execv error.n");
	}else {
		wait(&child_status);
	}
	return 0;  
}
