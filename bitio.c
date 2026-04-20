#include "bitio.h"

void init_BitWriter(BitWriter* bw, FILE* out) {
    bw->out = out;
    bw->buffer = 0;
    bw->bit_count = 0;
}

void writeBit(BitWriter* bw, int bit) {
    if (bit) bw->buffer |= (1 << (7 - bw->bit_count));
    bw->bit_count++;
    if (bw->bit_count == 8) {
        fputc(bw->buffer, bw->out);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

void flush_BitWriter(BitWriter* bw) {
    if (bw->bit_count > 0) {
        fputc(bw->buffer, bw->out);
        bw->buffer = 0;
        bw->bit_count = 0;
    }
}

void init_BitReader(BitReader* br, FILE* in) {
    br->in = in;
    br->buffer = 0;
    br->bit_count = 0;
}

int readBit(BitReader* br) {
    if (br->bit_count == 0) {
        size_t bytesRead = fread(&br->buffer, 1, 1, br->in);
        if (bytesRead == 0) return -1;
        br->bit_count = 8;
    }
    int bit = (br->buffer >> (br->bit_count - 1)) & 1;
    br->bit_count--;
    return bit;
}