#include <sys/types.h>
  #include <unistd.h>
  #include <stdio.h>

int main()
  {
	char *parmList[] = {"/bin/echo","123",NULL};
        execv("/bin/echo", parmList);
	return 0;  
}
