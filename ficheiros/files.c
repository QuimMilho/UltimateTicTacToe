
#include <stdio.h>
#include <string.h>

#include "files.h"

int fExists(char* path, char* readType) {
    FILE *f;
    if ((f = fopen(path, readType))) {
        fclose(f);
        return 1;
    }
    return 0;
}
