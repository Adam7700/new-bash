#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define arrLen(a) (sizeof(a) / sizeof(*a))

void parseString(char*, char*[]);

int main(){
    char *arr[20] = {0};
   
    printf(">>");

    while(1){
        char input[BUFSIZ];
        fflush(stdin);
        fgets(input,BUFSIZ,stdin);

        int len;
        len = strlen(input);
        if(input[len-1] == '\n'){
            input[len-1] = 0;
        }
        parseString(input,arr);

        if (strcmp(arr[0],"exit")==0){
            return 0;
        }
            
        char* builtIn[] = {"cd", "pwd", "version"};
        int isBuiltIn = 0;

        int i;
        for(i=0; i<arrLen(builtIn); i++){
            if (strcmp(arr[0], builtIn[i])==0){
                isBuiltIn = 1;
                break;
            }
        }
        pid_t child;
        
        if(isBuiltIn){
            printf("BUILTIN");
            if(strcmp(arr[0], "version")==0){
                printf("%s\n","Version: 0.01");
            }
            else if(strcmp(arr[0], "pwd")==0){
               char cwd[PATH_MAX];
               char* curPath = getcwd(cwd, sizeof(cwd));
               printf("%s\n", curPath);
            }
            else if(strcmp(arr[0], "cd")==0){
                int o;
                printf("CDING\n");
                if(arr[1] == 0){
                    chdir(getenv("HOME"));
                }else{
                    printf("CDING\n");
                    if(strcmp(arr[1],"~")==0){
                        chdir(getenv("HOME"));
                    }else{
                        chdir(arr[1]);
                    }
                }
             
            }
        }
        else{
            if((child = fork())==0){
                // char *argv[]={arr};
                // execv(input,argv);
                return 0;
            }
        }
        if(getpid() != 0){
            waitpid(child);
            printf(">>");
        }
    }
}

void parseString(char* inputString, char *arr[]){
    char *delim = " ";
    char *currentTok  = strtok (inputString , delim);
    int count=0;
    while(currentTok){
        arr[count] = currentTok;
        currentTok = strtok(NULL, delim);
        count++;
    }
    return;
} 