#include <stdio.h>
#include <stdlib.h>

enum sys_call {
	ECHO, 
	LS,
	PS,
	SHUTDOWN
};

int main (int argc, char ** argv){
	enum sys_call call;
	printf("Quel est la commande:\n");
	scanf("%d", &call);
	
	char cmd[64];
	//pid_t pid = getpid();
	//sprintf(cmd, "cat /proc/%u/maps", pid);
	//system(cmd);

	switch(call){
		case ECHO:
			printf("Echo\n");

			break;
		case LS:
			printf("ls\n");
			sprintf(cmd, "ls");
			system(cmd);
			break;
		case PS:
			printf("ps\n");
			sprintf(cmd, "ps");
			system(cmd);
			break;
		case SHUTDOWN:
			printf("shutdown");
			break;
	}


}
