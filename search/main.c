// TODO:
// Handle errors when opening permission denied directories
// Tidy memory usage and improve readability

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>

void dir_search(char *parent) {

    //char full_name[256]; // Used to give full path for opening
    //printf("%s and %s\n", parent, name);
    //strcpy(full_name, parent);
    //strcat(full_name, name);
    printf("Parent: %s\n", parent);

    DIR *dp = opendir(parent);
    struct dirent *d;
    
    char fullpath[PATH_MAX]; // Used to pass to next call
    char tmp[PATH_MAX];

    char *path_ptr;

    while ((d = readdir(dp)) != NULL) {
        printf("%s\n", d->d_name);
        
        path_ptr = realpath(d->d_name, fullpath); // Works for first level of recursion

        if (path_ptr == NULL) { // Used for 2+ levels of recursion
            strcpy(tmp, parent);
            strcat(tmp, "/");
            strcat(tmp, d->d_name);
            printf("%s\n", tmp);
            path_ptr = realpath(tmp, fullpath);
        }
        
        if (d->d_type == DT_DIR && strcmp(d->d_name, "..") && strcmp(d->d_name, ".")) {
            dir_search(path_ptr);
        }
    }

    closedir(dp);
}

int main() {

    char fullpath[PATH_MAX];
    char *path_ptr;

    path_ptr = realpath(".", fullpath);
    
    dir_search(path_ptr);

    return 0;
}