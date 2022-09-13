// TODO:
// Allow searching of partial argument (probably want an option to allow substring matches)
// Will currently not search hidden directories?

/*
Current usage:
- without arguments, prints all folders/files
- with argument, will print files/directories that share the exact same name
*/

#include <stdio.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <stdbool.h>

char *search_term = NULL; // Makes 
bool search = false;

void string_compare(char *filename, char *filepath) { // Strings must be exactly same length and match
    if (strlen(filename) == strlen(search_term) && !strncmp(filename, search_term, strlen(search_term))) {
        printf("%s\n", filepath);
    }
}

void dir_search(char *parent) {

    DIR *dp = opendir(parent);

    if (dp == NULL) {
        fprintf(stderr, "Error opening %s: %s\n", parent, strerror(errno));
    }

    else {

        struct dirent *d;
        
        char fullpath[PATH_MAX]; 
        char path_ptr[PATH_MAX];

        while ((d = readdir(dp)) != NULL) {
            
            strcpy(path_ptr, parent);

            strcat(path_ptr, "/");

            strcat(path_ptr, d->d_name);

            if (path_ptr[0] == '/' && path_ptr[1] == '/') { 
            /* 
            Prevents issue where if search starts at /, paths have a double //
            realpath() was not an option as it resolved symlinks which could cause recursion issues with looping
            */
                memmove(path_ptr, path_ptr+1, strlen(path_ptr));
            }

            if (d->d_type == DT_DIR && strcmp(d->d_name, "..") && strcmp(d->d_name, ".")) { // Only show directories excluding . and .. and also excludes any directory starting with . as hidden file

                if (!search) {
                    if (strlen(parent) == 1 && !strncmp(parent, "/", 1)) { // Prevent double // at start of files in root directory
                      printf("%s%s\n", parent, d->d_name);
                    }
                    
                    else {
                        printf("%s/%s\n", parent, d->d_name);
                    }
                }

                else {
                    string_compare(d->d_name, path_ptr);
                }

                dir_search(path_ptr);

            }

            else if (d->d_type == DT_UNKNOWN && strcmp(d->d_name, "..") && strcmp(d->d_name, "."))  { // Fallback to guaranteed check from lstat if d_type unknown to clarify if directory
                
                struct stat stat_block;

                lstat(path_ptr, &stat_block);

                if (S_ISDIR(stat_block.st_mode)) {
                    if (!search) {
                        if (strlen(parent) == 1 && !strncmp(parent, "/", 1)) { // Prevent double // at start of files in root directory
                            printf("%s%s\n", parent, d->d_name);
                        }   
                    
                        else {
                            printf("%s/%s\n", parent, d->d_name);
                        }
                    }

                    else {
                        string_compare(d->d_name, path_ptr);
                    }

                    dir_search(path_ptr);
                }

                // Print out filenames
            }

            else { // for filenames 
                if (!search) {
                    if (strlen(parent) == 1 && !strncmp(parent, "/", 1)) { // Prevent double // at start of files in root directory
                        printf("%s%s\n", parent, d->d_name);
                    }
                    
                    else {
                        printf("%s/%s\n", parent, d->d_name);
                    }
                }

                else {
                    string_compare(d->d_name, path_ptr);
                }
            }
        }
    }

    closedir(dp);

}

int main(int argc, char *argv[]) {

    if (argc >= 2) {
        search_term = argv[1];
        search = true;
    }

    char fullpath[PATH_MAX];
    char *path_ptr;

    path_ptr = realpath(".", fullpath);
    
    dir_search(path_ptr);

    return 0;
}