#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <string.h>

#define arrLen(a) (sizeof(a) / sizeof(*a))

void parseString(char*, char*[]);

int main(){
    char input[BUFSIZ];
//    printf("%d", BUFSIZ);
    
    printf(">>");

    while(1){
        scanf("%s", input);
        if (strcmp(input,"exit")==0){
            return 0;
        }
        pid_t child;
            
        if((child = fork()) == 0){
            char* builtIn[] = {"cd", "pwd", "version"};
            int isBuiltIn = 0;

            int i;
            for(i=0; i<arrLen(builtIn); i++){
                if (strcmp(input, builtIn[i])==0){
                    isBuiltIn = 1;
                }
            }
            

            
            if(isBuiltIn){
                if(strcmp(input, "version")==0){
                    printf("%s\n","Version: 0.01");
                }
                else if(strcmp(input, "pwd")==0){
                   char cwd[PATH_MAX];
                   char* curPath = getcwd(cwd, sizeof(cwd));
                   printf("%s\n", curPath);
                }
                else if(strcmp(input, "cd")==0){
                    char inputString[1024] = "cd /there";
                    char* arr[10];
                    parseString(inputString,arr );
                }
            }
            else{
                char *argv[]={input};
                execv(input,argv);


            }
            return 0;
        }
        else{
            waitpid(child);
            printf(">>");
        }

    }
}

void parseString(char* inputString, char*arr[]){
        printf("%s\n", inputString);
        char *delim = " ";
    char *currentTok  = strtok (inputString , delim);


    while(currentTok!= NULL){
        printf("%s\n", currentTok);
        currentTok = strtok(NULL, delim);
    }

    
    return;
} 

