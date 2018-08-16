#define _XOPEN_SOURCE 500

#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>

char fullpath[PATH_MAX];

int LastSeparator(const char *dirname) {
    int i = strlen(dirname);
    for(;i>0;i--){
        if (dirname[i-1] == '/') {
            return i-1;
        }
    }
    return -1;
}

int fn(const char *fpath, const struct stat *sb,
                    int typeflag, struct FTW *ftwbuf) {
    if (typeflag == FTW_SL){
        char father[PATH_MAX];
        char child[PATH_MAX];

        int lastindex = LastSeparator(fpath);
        char target[lastindex];
        strncpy(target, fpath, lastindex);
        target[lastindex] = '\0';
        realpath(fpath, father);
        realpath(target, child);

        int length = strlen(fullpath);
        char path[length];
        strncpy(path,father,length);
        path[length] = '\0';

        if(strcmp(path,fullpath) == 0 && strcmp(child,father) != 0){
            printf("Cycle found at %s\n", child);
            return 1;
        }
        return 0;
    }
    return 0;
}

int main(int argc, char *argv[]) {
    char *dirpath = malloc(PATH_MAX * sizeof(char));
    if (argc <= 2) {
        strcpy(dirpath, ".");
        realpath(dirpath,fullpath);
    } else {
        strncpy(dirpath, argv[1], PATH_MAX);
        realpath(dirpath,fullpath);
    }

    struct stat state;
    int error = stat(dirpath, &state);
    if (error == -1) {
        if (ENOENT == errno) {
            perror("Directory not found");
            exit(1);
        } else {
            perror("Stat");
            exit(1);
        }
    }
    if (nftw(dirpath, fn, 20, FTW_PHYS) == 0) {
        printf("No cycle found.\n");
    }
    free(dirpath);
    exit(0);
}
