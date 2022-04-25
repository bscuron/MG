#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <limits.h>
#include <dirent.h>
#include <sys/stat.h>

void parseArguments(int, char**);
int isFile(char*);
int isDir(char*);
void traverse(char*);
void findQuery(char*);

char* query;
char** paths;
int numPaths = 0;

int main(int argc, char** argv){
    parseArguments(argc, argv);
    for(int i = 0; i < numPaths; i++){
        traverse(paths[i]);
    }
}

void parseArguments(int argc, char** argv){
    if(argc < 3){
        fprintf(stderr, "ERROR: invalid number of arguments.\n");
        exit(1);
    }

    query = argv[1];

    for(int i = 2; i < argc; i++){
        char resolvedPath[PATH_MAX];
        if(realpath(argv[i], resolvedPath) == NULL){
            fprintf(stderr, "ERROR: invalid path argument.\n");
            exit(1);
        }
        numPaths++;
        if(!paths){
            paths = malloc(sizeof(char*));
        } else{
            paths = realloc(paths, sizeof(char*) * numPaths);
        }
        char* resolvedPathPtr = strdup(resolvedPath);
        if(!resolvedPathPtr){
            fprintf(stderr, "ERROR: failed to save copy of string.\n");
            exit(1);
        }
        paths[numPaths - 1] = resolvedPathPtr;
    }
}

void traverse(char* path){
    if(isFile(path)){
        findQuery(path);
        return;
    } else if(!isDir(path)){
        // not a directory or normal file
        return;
    }

    DIR* dir = opendir(path);
    if(!dir){
        fprintf(stderr, "ERROR: failed to open directory: %s.\n", path);
        return;
    }

    struct dirent* entry;
    /* printf("%s\n", path); */
    while((entry = readdir(dir)) != NULL){
        char* entryName = entry->d_name;

        if(strcmp(entryName, ".") == 0 || strcmp(entryName, "..") == 0)
            continue;

        char entryPath[PATH_MAX];
        strcpy(entryPath, path);
        strcat(entryPath, "/");
        strcat(entryPath, entry->d_name);
        /* printf("\t%s\n", entryPath); */
        traverse(entryPath);
    }
    closedir(dir);
}

int isFile(char* path){
    struct stat s;
    stat(path, &s);
    return S_ISREG(s.st_mode);
}

int isDir(char* path){
    struct stat s;
    stat(path, &s);
    return S_ISDIR(s.st_mode);
}

void findQuery(char* path){

    FILE* fp = fopen(path, "r");
    if(!fp){
        fprintf(stderr, "ERROR: failed to open file: %s\n", path);
        return;
    }

    char* line = NULL;
    size_t size = 0;
    int row = 1;
    /* printf("%s\n", path); */
    while(getline(&line, &size, fp) != -1){
        char* startPtr = strstr(line, query);
        if(startPtr != NULL){
            printf("%s:%d:%lu:%s", path, row, startPtr - line + 1, line);
        }
        row++;
    }
    fclose(fp);
}
