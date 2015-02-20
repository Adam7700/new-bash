#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <sys/resource.h>

void proc_exit(){
	union wait wstat;
	pid_t pid;
	int status;

	waitpid(-1,&status,WNOHANG);
	printf ("Return code: %d\n", wstat.w_retcode);
}

main (){
	printf(">> ");
	while(1){
		pid_t a_pid = -1;
		pid_t thispid = getpid();
		char in_string[200];
		char *args[10];

		fgets(in_string,sizeof(in_string),stdin);
		printf("%s\n",in_string);
		signal(SIGCHLD, proc_exit);
		switch (a_pid = fork()) {
			case -1:
				perror ("main: fork");
				exit(0);
			case 0:
				printf("In child pid %d \n",a_pid);
				execl("/bin/echo","2",NULL);
		}
		if(a_pid > 0){
			int status;
			waitpid(a_pid,&status,WNOHANG);
			printf("In parent pid:\n`  child is %d \n  parent is %d\n",a_pid,getpid());
			printf(">> ");
		}
	}
}