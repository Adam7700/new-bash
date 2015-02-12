#include <stdio.h>
#include <unistd.h>

int main(){
    char input[BUFSIZ];
//    printf("%d", BUFSIZ);
    
    printf(">>");

    while(1){
        scanf("%s", input);
        if (strcmp(input,"exit")==0){
            return 0;
        }
        char *argv[]={"/etc\0"};
        execv("/bin/ls",argv);
        printf(">>");
    }
}



//njwhite777@gmail.com 
//njwhite777
