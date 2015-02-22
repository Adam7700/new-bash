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

int parseString(char*, char*[],char*,char*,char*,char*[]);
char strLastChr(char *);
char strFirstChr(char *);
void nullLastChar(char *);
void reap_child();
void openStd(int, int, int, int);
void newStdin(char*);
void newStdout(char*);


int main(int argc,char **argv){
    
    int i;
    for(i=0; i<argc; i++){
        printf("argv: %s\n", argv[i]);
    }
    printf(">>Starting shsh V0.01\n");
    printf(">>");

    while(1){
        
        char input[BUFSIZ];               //Stores the input string
        char *args[20]      = {0};        //Stores the split of the input string. The "args"
        char *pipeArgs[20]  = {0};
        int paramLen        = 0;          //Keeps track of number of args
        fflush(stdin);
        fgets(input,BUFSIZ,stdin);  //Sets input from stdin
        int lastIdx         = 0;            //Last index of the args that were input
        int bg_proc         = 0;            //Keeps track of whether or not to put in bg
        int piping          = 0;
        int save_stdout     = -1;
        int save_stdin      = -1;
        pid_t child         = -1;           //Keeps track of the child proc (or parent, depneding)
        char ird[BUFSIZ]    = "";
        char ord[BUFSIZ]    = "";
        int pipeFd[2];
        
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


        char* builtIn[] = {"cd", "pwd", "version","getpid"};
        int isBuiltIn = 0;

        paramLen = parseString(input,args," \t",ird,ord,pipeArgs);   //set paramlen
        lastIdx = paramLen - 1;                             //set last index in args

        if(argc > 1){ 
            piping = 1;
        };                  


        if(paramLen != -1){                                 //Pressing enter (paramlen == 0) just skips back around
            if (strcmp(args[0],"exit") == 0){
                return 0;
            }
                
            if(strLastChr(args[lastIdx])=='&' && paramLen >= 1){
                bg_proc=1;                                  //Set flag so proc will run in bg
                if(strlen(args[lastIdx]) == 1){
                    args[lastIdx] = 0;
                    paramLen     -= 1;
                    lastIdx      -= 1;
                }else{
                    nullLastChar(args[lastIdx]);            //Null out ampersand
                }
            }
                     
            int i;
            for(i=0; i< arrLen(builtIn); i++){
                if (strcmp(args[0], builtIn[i])==0){
                    isBuiltIn = 1;
                    break;
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
                   // if(ird[0] != '\0'){newStdin(ird);}
                   // if(ord[0] != '\0'){newStdout(ord);}
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
            }else if(paramLen > 0 && args[0] != NULL){

                signal(SIGCHLD, reap_child);
                if(pipeArgs[1] != NULL){
                    pipe(pipeFd);
                }
                if(!piping && (child = fork()) == 0){
                    if(ird[0] != '\0'){newStdin(ird);}
                    if(ord[0] != '\0'){newStdout(ord);}
                    
                    int retval = execvp(args[0],args);
                    if(retval != 0){
                        printf("Apologies, command %s not found!\n",args[0]);
                    }
                    return;
                }else if(child > (pid_t) 0){                
                    if (bg_proc == 1){
                        waitpid(child, &status, WNOHANG);
                    }    
                    else{
                        waitpid(child, &status, 0);
                    }
                }
                if(pipeArgs[1] != NULL){

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

int parseString(char* inputString, char *args[],char *delim,char *ird,char *ord,char *pipeArgs[]){
    if(strlen(inputString) != 0){
        int leaveNextOut=0;
        char *currentTok  = strtok (inputString , delim);
        int count=0;
        int pipeArgCount=1;
        int foundPipe=0;
        while(currentTok){
            if((strcmp(currentTok,">"))==0){
                leaveNextOut=1;
                strcpy(ord,">");
            }else if((strcmp(currentTok,"<"))==0){
                leaveNextOut=1;
                strcpy(ird,"<");
            }else if((strcmp(currentTok,"|"))==0 || foundPipe){
                if(!(foundPipe == 0)){
                    pipeArgs[pipeArgCount] = currentTok;
                    pipeArgCount++;
                }
                foundPipe++;
            }else if(!leaveNextOut && !foundPipe){
                args[count] = currentTok;
                count++;
            }else if(leaveNextOut && strcmp(ird,"<") == 0){
                leaveNextOut = 0;
                strcpy(ird,currentTok);
            }else if(leaveNextOut && strcmp(ord,">") == 0){
                leaveNextOut = 0;
                strcpy(ord,currentTok);
            }
            currentTok = strtok(NULL, delim);
        }
        return count;
    }
    return -1;
}