#ifndef ARCHIVE_H
#define ARCHIVE_H

#include <stdint.h>

void createArchive(const char* archiveName, int fileCount, char** fileNames);
void extractArchive(const char* archiveName);

#endif // ARCHIVE_H