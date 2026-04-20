#ifndef BITIO_H
#define BITIO_H

#include <stdio.h>

typedef struct {
    FILE* out;
    unsigned char buffer;
    int bit_count;
} BitWriter;

typedef struct {
    FILE* in;
    unsigned char buffer;
    int bit_count;
} BitReader;

void init_BitWriter(BitWriter* bw, FILE* out);
void writeBit(BitWriter* bw, int bit);
void flush_BitWriter(BitWriter* bw);

void init_BitReader(BitReader* br, FILE* in);
int readBit(BitReader* br);

#endif // BITIO_H