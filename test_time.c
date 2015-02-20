#include <unistd.h>
#include <stdio.h>
#include <string.h>


int main(int argc,char **argv){
		int sleep_time = atoi(argv[1]);
		sleep(sleep_time);
		return 0;
}