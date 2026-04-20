#include "archive.h"
#include "huffman.h"
#include "bitio.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void createArchive(const char* archiveName, int fileCount, char** fileNames) {
    FILE* out = fopen(archiveName, "wb");
    if (!out) {
        fprintf(stderr, "Error writing archive %s\n", archiveName);
        return;
    }

    fwrite("CARCH", 1, 5, out);
    uint32_t f_count = (uint32_t)fileCount;
    fwrite(&f_count, sizeof(uint32_t), 1, out);

    for (int i = 0; i < fileCount; i++) {
        const char* fname = fileNames[i];
        FILE* in = fopen(fname, "rb");
        if (!in) {
            fprintf(stderr, "Warning: Cannot open file %s, skipping\n", fname);
            continue;
        }

        fseek(in, 0, SEEK_END);
        uint64_t orig_size = ftell(in);
        fseek(in, 0, SEEK_SET);

        uint8_t* content = (uint8_t*)malloc(orig_size);
        if (orig_size > 0) {
            fread(content, 1, orig_size, in);
        }
        fclose(in);

        fwrite(fname, 1, strlen(fname) + 1, out);
        fwrite(&orig_size, sizeof(uint64_t), 1, out);

        uint64_t freq[256] = {0};
        for (uint64_t j = 0; j < orig_size; j++) {
            freq[content[j]]++;
        }

        fwrite(freq, sizeof(uint64_t), 256, out);

        MinHeap* pq = create_heap(256);
        for (int k = 0; k < 256; k++) {
            if (freq[k] > 0) {
                insert_heap(pq, create_leaf_node((uint8_t)k, freq[k]));
            }
        }

        while (pq->size > 1) {
            Node* child1 = extract_min(pq);
            Node* child2 = extract_min(pq);
            uint64_t parent_freq = child1->freq + child2->freq;
            Node* parent = create_internal_node(parent_freq, child1, child2);
            insert_heap(pq, parent);
        }

        Node* root = extract_min(pq);
        free_heap(pq);

        char codeTable[256][256] = {{0}};
        char currentCode[256] = {0};
        
        if (root) buildCodeTable(root, currentCode, 0, codeTable);

        uint64_t comp_size_pos = ftell(out);
        uint64_t placeholder = 0;
        fwrite(&placeholder, sizeof(uint64_t), 1, out);

        uint64_t start_pos = ftell(out);
        BitWriter bw;
        init_BitWriter(&bw, out);

        serializeTree(root, &bw);

        for (uint64_t j = 0; j < orig_size; j++) {
            uint8_t byte = content[j];
            char* code = codeTable[byte];
            for (int k = 0; code[k] != '\0'; k++) {
                writeBit(&bw, code[k] - '0');
            }
        }
        flush_BitWriter(&bw);

        uint64_t end_pos = ftell(out);
        uint64_t comp_size = end_pos - start_pos;

        fseek(out, comp_size_pos, SEEK_SET);
        fwrite(&comp_size, sizeof(uint64_t), 1, out);
        fseek(out, end_pos, SEEK_SET);

        free_tree(root);
        free(content);
    }
    
    fclose(out);
    printf("Archive created: %s\n", archiveName);
}

void extractArchive(const char* archiveName) {
    FILE* in = fopen(archiveName, "rb");
    if (!in) {
        fprintf(stderr, "Error reading archive %s\n", archiveName);
        return;
    }

    char magic[5];
    if (fread(magic, 1, 5, in) != 5) {
        fprintf(stderr, "[!] Too small to be valid\n");
        fclose(in);
        return;
    }
    if (memcmp(magic, "CARCH", 5) != 0) {
        fprintf(stderr, "[!] NOT A CARCH FILE!~\n");
        fclose(in);
        return;
    }

    uint32_t file_count;
    fread(&file_count, sizeof(uint32_t), 1, in);

    for (uint32_t i = 0; i < file_count; ++i) {
        char fname[1024];
        int fname_idx = 0;
        char c;
        while (fread(&c, 1, 1, in) && c != '\0' && fname_idx < 1023) {
            fname[fname_idx++] = c;
        }
        fname[fname_idx] = '\0';

        uint64_t orig_size;
        fread(&orig_size, sizeof(uint64_t), 1, in);

        uint64_t freq[256];
        fread(freq, sizeof(uint64_t), 256, in);

        uint64_t comp_size;
        fread(&comp_size, sizeof(uint64_t), 1, in);

        uint64_t data_start_pos = ftell(in);

        // Splice the string for "_compressed"
        char outname[1050];
        char* dot = strrchr(fname, '.');
        if (dot != NULL) {
            int base_len = dot - fname;
            strncpy(outname, fname, base_len);
            outname[base_len] = '\0';
            strcat(outname, "_compressed");
            strcat(outname, dot);
        } else {
            strcpy(outname, fname);
            strcat(outname, "_compressed");
        }

        FILE* out = fopen(outname, "wb");
        if (out && orig_size > 0) {
            BitReader br;
            init_BitReader(&br, in);

            Node* root = deserializeTree(&br);

            if (root) {
                uint64_t decoded_count = 0;
                while (decoded_count < orig_size) {
                    Node* current = root;
                    while (current->left || current->right) {
                        int bit = readBit(&br);
                        if (bit == 0) 
                            current = current->left;
                        else          
                            current = current->right;
                    }
                    fputc(current->symbol, out);
                    decoded_count++;
                }
                free_tree(root);
            }
            printf("Extracted: %s\n", outname);
        } else if (orig_size == 0) {
            printf("Extracted (empty): %s\n", outname);
        } else {
            fprintf(stderr, "Cannot open %s for extraction\n", fname);
        }

        if (out) fclose(out);
        fseek(in, data_start_pos + comp_size, SEEK_SET);
    }
    fclose(in);
}