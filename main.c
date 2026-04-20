#include <stdio.h>
#include <string.h>
#include "archive.h"

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage:\n");
        fprintf(stderr, "  %s c <archive.carch> <file1> <file2> ...\n", argv[0]);
        fprintf(stderr, "  %s x <archive.carch>\n", argv[0]);
        return 1;
    }

    char* cmd = argv[1];
    char* archiveName = argv[2];

    if (strcmp(cmd, "c") == 0) {
        int fileCount = argc - 3;
        char** fileNames = &argv[3];
        createArchive(archiveName, fileCount, fileNames);
    } else if (strcmp(cmd, "x") == 0) {
        extractArchive(archiveName);
    } else {
        fprintf(stderr, "Unknown command: %s\n", cmd);
        return 1;
    }

    return 0;
}