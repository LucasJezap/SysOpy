#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <fcntl.h>
#include <dirent.h>
#define day 86400

int atime,mtime = -1;
char at,mt = 'x';
int depth;
char *direc;

int compareDates(time_t time1, time_t time2) {
    const char format1[] = "%Y";
    const char format2[] = "%m";
    const char format3[] = "%d";
    char res1a[5],res1b[5],res2a[5],res2b[5],res3a[5],res3b[5];
    struct tm lt1 = *localtime(&time1);
    struct tm lt2 = *localtime(&time2);
    strftime(res1a,5,format1,&lt1);
    strftime(res1b,5,format1,&lt2);
    if(strcmp(res1a,res1b) != 0) return (atoi(res1a)>atoi(res1b))? 1: -1;
    strftime(res2a,5,format2,&lt1);
    strftime(res2b,5,format2,&lt2);
    if(strcmp(res2a,res2b) != 0) return (atoi(res2a)>atoi(res2b))? 1: -1;
    strftime(res3a,5,format3,&lt1);
    strftime(res3b,5,format3,&lt2);
    if(strcmp(res3a,res3b) != 0) return (atoi(res3a)>atoi(res3b))? 1: -1;
    return 0;
}

void print_info(char *filename, struct stat stats) {
    time_t now;
    time(&now);
    if(atime != -1) {
        if(at == '+' && compareDates(stats.st_atime + atime*day,now) != -1) return;
        else if (at == '-' && compareDates(stats.st_atime + atime*day,now) != 1) return;
        else if (at != '+' && at != '-' && compareDates(stats.st_atime + atime*day,now) != -0) return;
    }
    if(mtime != -1) {
        if(mt == '+' && compareDates(stats.st_mtime + atime*day,now) != -1) return;
        else if (mt == '-' && compareDates(stats.st_mtime + atime*day,now) != 1) return;
        else if (mt != '+' && mt != '-' && compareDates(stats.st_mtime + atime*day,now) != -0) return;
    }
    char nname [PATH_MAX+1];
    strcpy(nname,".");
    strcat(nname,filename+strlen(direc));
    printf("------------------------------------------------------------------\n");
    printf("%s\n",nname);
    printf("PID = %d\n",(int)getpid());
    execl("/bin/ls","ls","-l",filename,NULL);
    printf("------------------------------------------------------------------\n");
}

void findOne(char *path, int maxdepth) {
    if(maxdepth<0) return;
    DIR *dir;
    struct dirent *dp;
    dir = opendir(path);
    char actual[PATH_MAX+1];
    realpath(path,actual);
    pid_t child_pid = fork();
    int w = waitpid(child_pid,NULL,0);
    if(child_pid == 0) {
        struct stat s;
        stat(actual,&s);
        print_info(actual,s);
        exit(0);
    }
    else {
        while((dp = readdir(dir)) != NULL) {
        if (dp->d_type == DT_DIR) {
            if (strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0) continue;
            char new_path[PATH_MAX+1];
            strcpy(new_path,actual);
            strcat(new_path,"/");
            strcat(new_path,dp->d_name);
            findOne(new_path,maxdepth-1);
        }
    }
    }
}

int main(int argc, char *argv[]) {
    if (argc<2 || argc % 2 == 1) {
        printf("ERROR!\n");
        return -1;
    }
    direc = argv[1];
    if (argc > 2) {
        if(strcmp(argv[2],"-atime") == 0) {
            char *tmp;
            if (argv[3][0] == '+' || argv[3][0] == '-') {
                tmp = strndup(argv[3]+1,strlen(argv[3])-1);
                atime = atoi(tmp);
                at = argv[3][0];
            }
            else atime = atoi(argv[3]);
        }
        else if (strcmp(argv[2],"-mtime") == 0) {
            char *tmp;
            if (argv[3][0] == '+' || argv[3][0] == '-') {
                tmp = strndup(argv[3]+1,strlen(argv[3])-1);
                mtime = atoi(tmp);
                mt = argv[3][0];
            }
            else mtime = atoi(argv[3]);
        }
        else {
            depth = atoi(argv[3]);
        }
    }
    if (argc > 4) {
        if(strcmp(argv[4],"-atime") == 0) {
            char *tmp;
            if (argv[5][0] == '+' || argv[5][0] == '-') {
                tmp = strndup(argv[5]+1,strlen(argv[5])-1);
                atime = atoi(tmp);
                at = argv[5][0];
            }
            else atime = atoi(argv[5]);
        }
        else if (strcmp(argv[4],"-mtime") == 0) {
            char *tmp;
            if (argv[5][0] == '+' || argv[5][0] == '-') {
                tmp = strndup(argv[5]+1,strlen(argv[5])-1);
                mtime = atoi(tmp);
                mt = argv[5][0];
            }
            else mtime = atoi(argv[5]);
        }
        else {
            depth = atoi(argv[5]);
        }
    }
    if (argc > 6) {
       if(strcmp(argv[6],"-atime") == 0) {
            char *tmp;
            if (argv[7][0] == '+' || argv[7][0] == '-') {
                tmp = strndup(argv[7]+1,strlen(argv[7])-1);
                atime = atoi(tmp);
                at = argv[7][0];
            }
            else atime = atoi(argv[7]);
        }
        else if (strcmp(argv[6],"-mtime") == 0) {
            char *tmp;
            if (argv[7][0] == '+' || argv[7][0] == '-') {
                tmp = strndup(argv[7]+1,strlen(argv[7])-1);
                mtime = atoi(tmp);
                mt = argv[7][0];
            }
            else mtime = atoi(argv[7]);
        }
        else {
            depth = atoi(argv[7]);
        }   
    }
    findOne(argv[1],depth);
    return 0;
}