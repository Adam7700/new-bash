#include <stdio.h>
#include <unistd.h>
#include <linux/limits.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <signal.h>
#include <sys/resource.h>
#include <fcntl.h>

#define arrLen(a) (sizeof(a) / sizeof(*a))

int parseString(char*, char*[],char*);
char strLastChr(char *);
char strFirstChr(char *);
void nullLastChar(char *);
void reap_child();
void openStd(int, int, int, int);
void newStdin(char*);
void newStdout(char*);


int main(){
   
    printf(">> Starting shsh V0.01\n");
    printf(">>");

    while(1){
        
        char input[BUFSIZ];         //Stores the input string
        char *args[20] = {0};       //Stores the split of the input string. The "args"
        int paramLen = 0;           //Keeps track of number of args
        fflush(stdin);
        fgets(input,BUFSIZ,stdin);  //Sets input from stdin
        int lastIdx = 0;            //Last index of the args that were input
        int bg_proc = 0;            //Keeps track of whether or not to put in bg
        pid_t child = -1;           //Keeps track of the child proc (or parent, depneding)
        
        int status;

        /*
        //
        // Setting up facts about input obtained this iteration
        //
        */
        int len = strlen(input);    //lenght of string typed at prompt
        if(input[len-1] == '\n'){   //Strip newline
            input[len-1] = 0;
        }

        paramLen = parseString(input,args," \t");   //set paramlen
        lastIdx = paramLen - 1;                     //set last index in args

        if(paramLen != -1){                         //Pressing enter (paramlen == 0) just skips back around
            if (strcmp(args[0],"exit") == 0){
                return 0;
            }
                
            if(strLastChr(args[lastIdx])=='&' && paramLen > 0){
                bg_proc=1;
                if(strlen(args[lastIdx])==1){
                    args[lastIdx] = 0;
                    paramLen -= 1;
                    lastIdx -=1;
                }else{
                    nullLastChar(args[lastIdx]);
                }
            }
                     
            if(isBuiltIn){
                if(strcmp(args[0], "version")==0){
                    printf("%s\n","Version: 0.01");
                }
                else if(strcmp(args[0], "pwd")==0){
                   char cwd[PATH_MAX];
                   char* curPath = getcwd(cwd, sizeof(cwd));
                   printf("%s\n", curPath);
                }
                else if(strcmp(args[0], "cd")==0){
                    int o;
                    if(args[1] == 0){
                        chdir(getenv("HOME"));
                    }else{
                        if(strcmp(args[1],"~")==0){
                            chdir(getenv("HOME"));
                        }else{
                            chdir(args[1]);
                        }
                    }
                }else if(strcmp(args[0],"getpid")==0){
                    printf("%d\n",getpid());

                }
               // openStd(outputRedirectPos, inputRedirectPos, stdoutCpy, stdinCpy); 
               if(inputRedirectPos){
                   newStdin(stdinFile); 
               } 
               if(outputRedirectPos){
                   newStdout(stdoutFile);
               }

            }

            else if(paramLen > 0 && args[0] != NULL){
                int abs_path = 0;
                if(strFirstChr(args[0])=='.' || strFirstChr(args[0])=='/'){
                    abs_path=1;
                }
                signal (SIGCHLD, reap_child);

                if((access(args[0],F_OK)==0) && (child = fork())==0){
                    //The file exists in whatever path was provided, 
                    //  we should try to run it.
                 if(inputRedirectPos){
                     newStdin(stdinFile); 
                 } 
                 if(outputRedirectPos){
                     newStdout(stdoutFile);
                 }
   
                    execv(args[0],args);
              //      openStd(outputRedirectPos, inputRedirectPos, stdoutCpy, stdinCpy); 

                }
                else if((access(args[0],F_OK)!=0) && !abs_path && ((child = fork())== 0) ){
                    //The file does not exist, check for it in the path
                    //
                    char *path_arr[20] = {0};
                    int path_count = parseString(getenv("PATH"),path_arr,":");
                    int m;
                    for(m=0;m<path_count;m++){
                        char full_path[256];
                        strcpy(full_path,path_arr[m]);
                        if(strLastChr(path_arr[m]) != '/'){
                            strcat(full_path,"/");
                        }
                        strcat(full_path,args[0]);
                        if(access(full_path,F_OK)==0){
                            args[0] = full_path;

                            if(inputRedirectPos){
                               newStdin(stdinFile); 
                            } 
                            if(outputRedirectPos){
                                newStdout(stdoutFile);
                            }
                            execv(full_path,args);
                         //   openStd(outputRedirectPos, inputRedirectPos, stdoutCpy, stdinCpy);
                            break;
                        }
                    }
                }
                
                if(child > 0){                
                    if (bg_proc == 1){
                        waitpid(child, &status, WNOHANG);
                    }    
                    else{
                        waitpid(child, &status, 0);
                    }
                }
            }
        }
        if(child != 0){
            printf(">>");
        }
    }
}

void newStdin(char* file){
    close(0);
    open(file, O_RDONLY);
}

void newStdout(char* file){

     close(1);       
     open(file, O_WRONLY | O_CREAT, S_IRWXU | S_IRGRP | S_IWGRP| S_IXOTH); 
}

void openStd(int orp, int irp, int oldOut, int oldIn){
    if(orp){
        printf("at openStd");
        close(1);
        dup2(oldOut, 1);
    }
    if(irp){
        close(0);
        dup2(oldIn, 0);
    }
}

void reap_child(){
    pid_t pid;
    int status;

    while((pid = waitpid (-1, &status, WNOWAIT)) > 0){
        printf("Reaping child with pid: %d\n",pid);
    };
    return;
}

char strFirstChr(char *string){
    return string[0];
}

char strLastChr(char  *string){
    return string[strlen(string)-1];
}

void nullLastChar(char *string){
    string[strlen(string)-1] = 0;
    return;
}

int parseString(char* inputString, char *args[],char *delim){

    if(strlen(inputString) != 0){
        char *currentTok  = strtok (inputString , delim);
        int count=0;
        while(currentTok){
            if((strcmp(currentTok,">"))==0 || (strcmp(currentTok,"<"))==0){
                if((strcmp(currentTok,">"))==0){
                    orf = 1;
                }else{

                }
            }else{
                args[count] = currentTok;
                count++;
            }
            currentTok = strtok(NULL, delim);
        }
        return count;
    }
    return -1;
}



   int inputRedirectPos;
            int redirectCheck;
            int outputRedirectPos;
            for(redirectCheck=0; redirectCheck<paramLen; redirectCheck++){
                if (strcmp(args[redirectCheck], "<")==0){
                    inputRedirectPos = redirectCheck;
                }
                if (strcmp(args[redirectCheck],">")==0){ 
                    outputRedirectPos = redirectCheck;
                }
            }


            char* builtIn[] = {"cd", "pwd", "version","getpid"};
            int isBuiltIn = 0;

            int i;
            for(i=0; i< arrLen(builtIn); i++){
                if (strcmp(args[0], builtIn[i])==0){
                    isBuiltIn = 1;
                    break;
                }
            }
            char* stdoutFile;
            char* stdinFile;
            if(inputRedirectPos || outputRedirectPos){
                if(inputRedirectPos){
              //      stdinCpy = dup(0);
                    stdinFile = args[inputRedirectPos+1];
                    if(outputRedirectPos && outputRedirectPos > inputRedirectPos){
                        outputRedirectPos -=2;
                    }
                    int newPos;
                    for(newPos=inputRedirectPos; newPos<paramLen-2; newPos++){
                        args[newPos]=args[newPos+2];
                    }
                    args[newPos+1]=0;
                    args[newPos+2]=0;
                    paramLen -= 2;
                } 
                if (outputRedirectPos){
                    stdoutFile = args[outputRedirectPos+1];
           //         stdoutCpy = dup(1); 
                    
                    int newPosOut;
                    for(newPosOut=outputRedirectPos; newPosOut<paramLen-2; newPosOut++){
                        args[newPosOut]=args[newPosOut+2];
                    }
                    if(paramLen == 3){
                        args[newPosOut] = 0;
                    }
                    args[newPosOut+1]=0;
                    args[newPosOut+2]=0;
                    paramLen -=2;
                    
                }
            }