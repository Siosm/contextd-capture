#include <stdio.h>
#include <malloc.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>


int keep_going = 1;

void signal_manager(int signal)
{
	printf("^C detected...\n");
	keep_going = 0;
}

int main(int argc, char* argv[])
{
	struct sigaction action;
	int filedes = 0;

	memset(&action, 0, sizeof(struct sigaction));
  	action.sa_handler = signal_manager;
  	sigaction(SIGINT, &action, NULL);
	
	while(keep_going){
		printf("%s: trying to open file test\t", *argv);
		filedes = open("test", O_RDWR);
		close(filedes);
		printf("%s: file test opened & closed\n", *argv);
	}

	return 0;
}

