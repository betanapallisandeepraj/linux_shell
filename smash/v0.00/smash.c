#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<unistd.h>

#define MAX_CMD_LEN 4096
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
			ret = getline (&cmd_ptr, &max_cmd_len, stdin);
			if (ret == -1) {
				printf("Error in reading the input\n");
				sleep(1);
				continue;
			}
			else {
				printf("You entered the following string:%s,string size=%d\n",cmd_ptr,ret);
			}
			if(strcmp(cmd_ptr,"exit\n")==0){
				free(cmd_ptr);
				exit(0);
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
			ret=getline(&cmd_ptr, &max_cmd_len, fp);
		}
	}
	free(cmd_ptr);
	fclose(fp);
	return EXIT_SUCCESS;
}
