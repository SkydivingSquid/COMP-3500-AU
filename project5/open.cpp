#include <cstdio>
#include <stdlib.h>
/*
 * Open file. To open a file using name as an input, file_descriptor is an output.
 */
FILE* open(char *filename){
    FILE *file;
    if (! (file = fopen(filename, "r"))) {
        printf("File %s can't be opened...\n", filename);
        return file;
    }
    printf("Open file: %s\n", filename);
    return file;
}