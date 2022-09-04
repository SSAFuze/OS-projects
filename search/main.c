// TODO:
// Subfolders under /media/<user>/<OS version>, whilst directories, do not seem to be recognised as such
// Allow searching of argument

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
            // && strcmp(d->d_name, ".") used to identify the . directory, currently falls under check if dname starts with ., maybe be needed if later allowing argument to also list hidden dirs
            if (d->d_type == DT_DIR && strcmp(d->d_name, "..") && strcmp(d->d_name, ".")) { // Only show directories excluding . and .. and also excludes any directory starting with . as hidden file

                if (!strcmp(parent, "/")) { // Prevent double // at start of files in root directory
                    printf("%s%s\n", parent, d->d_name);
                }
                
                else {
                    printf("%s/%s\n", parent, d->d_name);
                } 
            
                strcpy(tmp, parent);
                strcat(tmp, "/");
                strcat(tmp, d->d_name);
                path_ptr = realpath(tmp, fullpath); // Resolves duplicate /'s (ie //) at beginning of filepath to single /

                dir_search(path_ptr);

            }
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