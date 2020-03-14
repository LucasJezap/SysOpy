#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <string.h>
#include <dirent.h>
#include <ftw.h>
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

char * unixToDate(time_t unix_time) {
    const char format[] = "%a %d-%m-%Y %H:%M:%S";
    char * res = (char *)calloc(32,sizeof(char));
    struct tm lt;
    lt = *localtime(&unix_time);
    strftime(res,32,format,&lt);
    return res;
}

void print_info(char *filename, int type, struct stat stats) {
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
    printf("------------------------------------------------------------------\n");
    printf("Absolute path: %s\n",filename);
    if (type == DT_DIR) printf("Type: Directory");
    else if (type == DT_REG) printf("Type: Regular file");
    else if (type == DT_CHR) printf("Type: Character device");
    else if (type == DT_BLK) printf("Type: Block device");
    else if (type == DT_FIFO) printf("Type: Named pipe (FIFO)");
    else if (type == DT_LNK) printf("Type: Symbolic link");
    else printf("Type: Socket");
    printf("\nNumber of links: %lu\n",stats.st_nlink);
    printf("Size (in bytes): %lu\n",stats.st_size);
    printf("Time of last access: %s\n",unixToDate(stats.st_atime));
    printf("Time of last modification: %s\n",unixToDate(stats.st_mtime));
    printf("------------------------------------------------------------------\n");
}

void findOne(char *path, int maxdepth) {
    if(maxdepth<0) return;
    DIR *dir;
    struct dirent *dp;
    dir = opendir(path);
    char actual[PATH_MAX+1];
    realpath(path,actual);
    while((dp = readdir(dir)) != NULL) {
        if (strcmp(dp->d_name,".") == 0 || strcmp(dp->d_name,"..") == 0) continue;
        char filename[PATH_MAX+1];
        strcpy(filename,actual);
        strcat(filename,"/");
        strcat(filename,dp->d_name);
        struct stat stats;
        lstat(filename,&stats);
        print_info(filename,dp->d_type,stats);
        if (dp->d_type == DT_DIR)
            findOne(filename,maxdepth-1);
    }
}

int display(const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt) {
    if(strcmp(direc,filename) == 0 || pfwt->level > depth + 1) return 0;
    int type;
    char actual[PATH_MAX+1];
    realpath(filename,actual);
    if (S_ISREG(statptr->st_mode)) type = DT_REG;
    else if (S_ISDIR(statptr->st_mode)) type = DT_DIR;
    else if (S_ISCHR(statptr->st_mode)) type = DT_CHR;
    else if (S_ISBLK(statptr->st_mode)) type = DT_BLK;
    else if (S_ISFIFO(statptr->st_mode)) type = DT_FIFO;
    else if (S_ISLNK(statptr->st_mode)) type = DT_LNK;
    else type = DT_SOCK;
    print_info(actual,type,*statptr);
    return 0;
}

void findTwo(char *path) {
    nftw(path,display,20,FTW_CHDIR | FTW_MOUNT | FTW_PHYS);
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
    printf("Results for method using opendir,makedir,stat:\n\n\n");
    findOne(argv[1],depth);
    printf("\n\n\nResults for method using nftw: \n\n\n");
    findTwo(argv[1]);
    return 0;
}