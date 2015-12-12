#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
//#include <sys/types.h>
#include <string.h>
#include <sys/utsname.h>
#include <curses.h>
#include <signal.h>

static volatile int keepRunning = 1;

#define die(e) do { fprintf(stderr, "%s\n", e); exit(EXIT_FAILURE); } while (0);


char *shellPath=NULL;

struct utsname sysDetail;
int running = 1,maxlen=0,sz=0,prev=0;

void launch();
void DisplayThar();

void sigintHandler(int sig_num)
{
    //signal(SIGINT, sigintHandler);
    fprintf(stderr, "\n");
    char* sysName = (char*)calloc(30,sizeof(char));
    char* displayString = (char*)calloc(60,sizeof(char));
    strcpy(sysName,sysDetail.nodename);
    int i=0;
    while(sysName[i]){
        if(sysName[i] == '.'){
            sysName[i] = '\0';
            break;
        }
        i++;
    }
    strcpy(displayString,sysName);
    fprintf(stderr,"%s: %s$ ",displayString,getenv("USER"));
}

void zhandler(int sig_num)
{
    fprintf(stderr, "\n");
    char* sysName = (char*)calloc(30,sizeof(char));
    char* displayString = (char*)calloc(60,sizeof(char));
    strcpy(sysName,sysDetail.nodename);
    int i=0;
    while(sysName[i]){
        if(sysName[i] == '.'){
            sysName[i] = '\0';
            break;
        }
        i++;
    }
    strcpy(displayString,sysName);
    fprintf(stderr,"%s: %s$ ",displayString,getenv("USER"));
}

char** initialiseArgs(char *line){
    int i=0,j=0,k=0;
    while(line[i]){
        if(line[i] != ' ' && line[i] != '\n'){
            maxlen++;
        }
        else if(line[i] == ' '){
            sz++;
            if(maxlen>prev){
                prev = maxlen;
            }
            maxlen = 0;
        }
        else if(line[i] == '\n'){
            sz++;
            if(maxlen>prev){
                prev = maxlen;
            }
            maxlen = 0;
            break;
        }
        i++;
    }
    char **args = (char**)calloc(sz+1,sizeof(char*));
    i=0;
    for(i=0;i<sz;i++){
        args[i] = (char*)calloc(maxlen+3,sizeof(char));
    }
    args[sz] = NULL;
    i=0;

    while(line[i]){
        if(line[i] != ' ' && line[i] != '\n'){
            if(line[i] == '"'){
                args[j][k] = '\"';
                k++;
            }
            else{
            args[j][k] = line[i];
            k++;
        }
        }
        else if(line[i] == ' '){
            k=0;
            j++;
        }
        else if(line[i] == '\n'){
            k=0;j++;
        }
        i++;
    }
    return args;
}

char** Scan(){
    size_t len = 0;
    char *ln = NULL;
    getline(&ln, &len, stdin);
    return initialiseArgs(ln);
}

void DisplayThar(int ini){
    char* sysName = (char*)calloc(30,sizeof(char));
    char* displayString = (char*)calloc(60,sizeof(char));
    strcpy(sysName,sysDetail.nodename);
    int i=0;
    while(sysName[i]){
        if(sysName[i] == '.'){
            sysName[i] = '\0';
            break;
        }
        i++;
    }
    strcpy(displayString,sysName);
    if(!ini)
        printf("%s: %s$ ",displayString,getenv("USER"));
    else
        printf("\n%s: %s$ ",displayString,getenv("USER"));
}

void history_write(char **args,char *shellPath){
    char *path = (char*)calloc(1024,sizeof(char));
    strcpy(path,shellPath);
    strcat(path,"/history.txt");
    FILE *f = fopen(path, "a+");
    int i=0;
    while(args[i]){
        fprintf(f, "%s ", args[i]);
        i++;
    }
    fprintf(f, "\n");
    fclose(f);
}

void history_read(){
    char *path = (char*)calloc(1024,sizeof(char));
    strcpy(path,shellPath);
    strcat(path,"/history.txt");
    FILE *f = fopen(path, "a+");
    int i=0;
    //fprintf(f, "history\n" );
    fclose(f);
    f = fopen(path, "r");
    char *buffer = (char*)calloc(1000,sizeof(char));
    i=0;
    while(fgets(buffer,1000,f) != NULL){printf("%d. %s", i+1,buffer);i++;}
    fclose(f);
}

void oye(char **args,char *path,char *cmd1) {
if(cmd1 == NULL){
  int link[2];
  pid_t pid;
  char foo[4096];
  int i=0;
  // while(args[i]){
  //   printf("%s-",args[i]);
  //   i++;
  // }
  // printf("\n");
  if (pipe(link)==-1)
    die("pipe");

  if ((pid = fork()) == -1)
    die("fork");

  if(pid == 0) {
    dup2 (link[1], STDOUT_FILENO);
    close(link[0]);
    close(link[1]);
    execvp(args[0],args);
    die("execvp");

  } else {

    close(link[1]);
    int nbytes = read(link[0], foo, sizeof(foo));
    FILE *f = fopen(path,"w");
    fprintf(f,"%.*s", nbytes, foo);
    fclose(f);
    wait(NULL);

  }
}
else{
    pid_t pd = fork();
    if(pd == 0){
    int fds[2];
    FILE *f = fopen(cmd1,"r");
    char *buffer1 = (char*)calloc(100000,sizeof(char));
    char *buffer = (char*)calloc(1000,sizeof(char));
    while(fgets(buffer,1000,f) != NULL){strcat(buffer1,buffer);}
    strcat(buffer1,"\n");
  pipe(fds);
  close(STDIN_FILENO);
  dup2(fds[0], STDIN_FILENO);
  write(fds[1], buffer1, strlen(buffer1));
  fclose(f);
  execvp(args[0], args);
}else{
    wait(NULL);
}
}
}

void execute(char** args,char *path, char *cmd){
    //printf("%s\n",args[0]);
    // if(strcmp(args[0],"clear")==0){
    //     system("clear");
    // }
    if(strcmp(args[0],"cd")==0){
        chdir(args[1]);
    }
    else if(strcmp(args[0],"help")==0){
        printf("Basic Shell devoloped by..\n");
        printf("Shubham Goswami... 2014100 IIITD\n");
        printf("for OS Assignment\n");
        FILE *f = fopen(path,"w");
        fprintf(f, "Basic Shell devoloped by..\n" );
        fprintf(f, "Shubham Goswami... 2014100 IIITD\n" );
        fprintf(f,"for OS Assignment\n" );
        fclose(f);

    }
    else if(strcmp(args[0],"history")==0){
        //FILE *fr = fopen()
        history_read();
        FILE *h,*f;
        f = fopen(path,"w");
        char *path1 = (char*)calloc(1024,sizeof(char));
        strcpy(path1,shellPath);
        strcat(path1,"/history.txt");
        h = fopen(path1,"r");
        char *buffer = (char*)calloc(1000,sizeof(char));
        while(fgets(buffer,1000,h) != NULL){fprintf(f, "%s",buffer);}
        fclose(h);
        fclose(f);

    }
    else if(strcmp(args[0],"exit")==0){
        kill(getppid(), SIGKILL);
    }
    else{
        oye(args,path,cmd);
    }
}

void oclear(){
    pid_t pid;
        pid = fork();
        if (pid == 0) {
        // Child process
            if (execlp("clear","clear",NULL) == -1) {
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
}

char **extrct(char **args,int index,int fibi,char* path){
        if(!fibi){
                char **nt = (char**)calloc(index+1,sizeof(char*));
        nt[index] = NULL;
        int i=0;
        for(i=0;i<index;i++){
            nt[i] = args[i];
        }
        return nt;
        }

        char **nt = (char**)calloc(index+2,sizeof(char*));
        nt[index] = (char*)calloc(100,sizeof(char));
        strcpy(nt[index],path);
        nt[index+1] = NULL;
        int i=0;
        for(i=0;i<index;i++){
            nt[i] = args[i];
        }
        return nt;
}

void ex(char **args){
    char **argv = (char**)calloc(sz+1,sizeof(char*));
    int i=0,j=0;
    while(args[i]){
            argv[j] = args[i];
            i++;j++;
    }
    char *path = (char*)calloc(500,sizeof(char));
    strcpy(path,shellPath);
    strcat(path,"/temp.txt");
    j=0;
    int cntPipe=0,cntGt=0,cntLt=0;
    int fibi=0;
    while(argv[j]){
        if(argv[j][0] == '>')
            cntGt++;
        else if(argv[j][0] == '|')
            cntPipe++;
        else if(argv[j][0] == '<')
            cntLt++;
        j++;
    }
    int toto = cntPipe + cntGt + cntLt;
    if(toto>0){
        i=0;
        while(argv[i] != NULL){
            if(argv[i][0] == '|'){
                //printf("coming %d\n",i);
                execute(extrct(argv,i,fibi,path),path,NULL);
                argv = argv + i+1;
                fibi=1;
                i=0;
                //printf("%s %s\n",argv[0],argv[1]);
            }
            else if(argv[i][0] == '>'){
                execute(extrct(argv,i,fibi,path),path,NULL);
                FILE *f = fopen(argv[i+1], "w");
                FILE *fm = fopen(path,"r");
                //printf("%s\n",path);
                char *buffer = (char*)calloc(1000,sizeof(char));
                while(fgets(buffer,1000,fm) != NULL){fprintf(f, "%s",buffer);}
                fclose(f);
                fclose(fm);
                fibi = 0;
                break;
            }
            else if(argv[i][0] == '<'){
                execute(extrct(argv,i,fibi,path),path,argv[i+1]);
                fibi = 0;
                break;
            }
            i++;
        }
        if(fibi){
                //printf("coming %d\n",i);
                execute(extrct(argv,i,fibi,path),path,NULL);
                //printf("achha\n");
                FILE *f = fopen(path,"r");
                 char *buffer = (char*)calloc(1000,sizeof(char));
                while(fgets(buffer,1000,f) != NULL){printf( "%s",buffer);}
                }
    }
    else{
        //printf("coming\n");
        int i=0;
        // for(i=0;argv[i] != NULL;i++){
        //     printf("%s\n",argv[i]);
        // }
        execute(argv,path,NULL);
        FILE *f = fopen(path,"r");
        char *buffer = (char*)calloc(1000,sizeof(char));
        while(fgets(buffer,1000,f) != NULL){printf( "%s",buffer);}
    }
}


void launch(int dummy){
    signal(SIGINT, sigintHandler);
    signal(SIGTSTP,zhandler);
    while(running){
        DisplayThar(0);
        char **args = Scan();
        if(args[0][0] == '\0'){
             continue;
         }
         history_write(args,shellPath);
         int l = 2;
            ex(args);
            free(args);
            sz=0;
    }
}

int main(){
    oclear();
    uname(&sysDetail);
    shellPath = getcwd(0,0);
    launch(0);
    //printf("%s\n", shellPath);

}
