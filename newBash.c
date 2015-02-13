#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

#define arrLen(a) (sizeof(a) / sizeof(*a))

int parseString(char*, char*[]);

int main(){
   
    printf(">>");

    while(1){
        char input[BUFSIZ];
        char *arr[20] = {0};
        fflush(stdin);
        fgets(input,BUFSIZ,stdin);
        int paramLen = 0;
        pid_t child = -1;

        int len;
        len = strlen(input);
        if(input[len-1] == '\n'){
            input[len-1] = 0;
        }
        paramLen = parseString(input,arr);

        if(paramLen != -1){
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
            
            if(isBuiltIn){
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
                    if(arr[1] == 0){
                        chdir(getenv("HOME"));
                    }else{
                        if(strcmp(arr[1],"~")==0){
                            chdir(getenv("HOME"));
                        }else{
                            chdir(arr[1]);
                        }
                    }
                }
            }
            else if(paramLen > 0 && arr[0] != NULL){
                if((access(arr[0],F_OK)==0) && (child = fork())==0){
                    //The file exists, we should try to
                    // run it.
                    
                    printf("Child is now, %d\n",child);

                    execv(arr[0],arr);
                }
                else if((access(arr[0],F_OK)!=0) && ((child = fork())== 0)){
                    //The file does not exist, check for it in the path
                    //
                    execv(arr[0],arr);
                }
                if(child > 0){
                    waitpid(child);
                }
            }
        }
        if(child!=0){
            printf(">>");
        }
    }
}

int parseString(char* inputString, char *arr[]){
    if(strlen(inputString) != 0){
        char *delim = " ";
        char *currentTok  = strtok (inputString , delim);
        int count=0;
        while(currentTok){
            arr[count] = currentTok;
            currentTok = strtok(NULL, delim);
            count++;
        }
        return count;
    }
    return -1;
} 