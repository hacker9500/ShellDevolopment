#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/types.h>
#include <string.h>
#include <sys/utsname.h>
#include <curses.h>

struct utsname sysDetail;

int main(int argc, char const *argv[])
{
    uname(&sysDetail);
    char *shellPath = getcwd(0,0);
    char *path = (char*)calloc(150,sizeof(char));
    strcpy(path,shellPath);
    strcat(path,"/shell.sh");
    FILE *fp = fopen(path,"w");
    fprintf(fp,"#!/bin/bash\ncd %s\ngcc 2014100_A1_2.c -o shell1\n./shell1",shellPath);
    fclose(fp);
    printf("%s-\n",sysDetail.sysname );
    if(strcmp(sysDetail.sysname,"Darwin")==0){
        printf("aya\n");
        pid_t pid;
        pid = fork();
        if (pid == 0) {
        // Child process
            if (execlp("chmod","chmod","+x",path,NULL) == -1) {
                printf("Error\n");
            }
            exit(1);
        } else if (pid < 0) {
        // Error forking
            printf("Error...!!!\n");
        } else {
        // Parent process
         wait(NULL);
        }
        if(execlp("open","open","-a","Terminal.app",path,NULL)==-1){
            printf("error\n");
        }
        //system("open -a Terminal.app /Users/ShubhamGoswami/Documents/Sem\\ Stuff/Shell");
    }
    else{
        pid_t pid;
        pid = fork();
        if (pid == 0) {
        // Child process
            if (execlp("chmod","chmod","+x",path,NULL) == -1) {
                printf("Error\n");
            }
            exit(1);
        } else if (pid < 0) {
        // Error forking
            printf("Error...!!!\n");
        } else {
        // Parent process
         wait(NULL);
        }
        if(execlp("gnome-terminal","gnome-terminal","-e",path,NULL)==-1){
            printf("error\n");
        }
    }

    return 0;
}