// TODO:
// Tidy memory usage/efficiency and improve readability

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>


void dir_search(char *parent) {

    DIR *dp = opendir(parent);

    if (dp == NULL) {
        fprintf(stderr, "Error opening %s: %s\n", parent, strerror(errno));
    }

    else {

        struct dirent *d;
        
        char fullpath[PATH_MAX]; 
        char tmp[PATH_MAX];

        char *path_ptr;

        while ((d = readdir(dp)) != NULL) {
            if (d->d_type == DT_DIR && strcmp(d->d_name, "..") && strcmp(d->d_name, ".")) { // Only show directories excluding . and ..
                
                if (*parent == '/') { // Prevent double // at start of files in root directory
                    printf("%s%s\n", parent, d->d_name);
                }
                
                else {
                    printf("%s/%s\n", parent, d->d_name);
                } 
            
            
                path_ptr = realpath(d->d_name, fullpath); // Works for first level of recursion

                if (path_ptr == NULL) { // Used for 2+ levels of recursion
                    strcpy(tmp, parent);
                    strcat(tmp, "/");
                    strcat(tmp, d->d_name);
                    path_ptr = realpath(tmp, fullpath);
                }
                
                dir_search(path_ptr);

            }
        }

        closedir(dp);

    }
}

int main() {

    char fullpath[PATH_MAX];
    char *path_ptr;

    path_ptr = realpath(".", fullpath);
    
    dir_search(path_ptr);

    return 0;
}