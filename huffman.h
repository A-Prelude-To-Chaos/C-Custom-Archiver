#ifndef HUFFMAN_H
#define HUFFMAN_H

#include <stdint.h>
#include "bitio.h"

typedef struct Node {
    uint8_t symbol;
    uint64_t freq;
    struct Node* left;
    struct Node* right;
} Node;

// Node constructors and destructors
Node* create_leaf_node(uint8_t symbol, uint64_t freq);
Node* create_internal_node(uint64_t freq, Node* left, Node* right);
void free_tree(Node* root);

// Huffman operations
void buildCodeTable(Node* root, char* currentCode, int depth, char codeTable[256][256]);
void serializeTree(Node* root, BitWriter* bw);
Node* deserializeTree(BitReader* br);

// Min-Heap structures for C priority queue implementation
typedef struct {
    Node** array;
    int size;
    int capacity;
} MinHeap;

MinHeap* create_heap(int capacity);
void insert_heap(MinHeap* heap, Node* node);
Node* extract_min(MinHeap* heap);
void free_heap(MinHeap* heap);

#endif // HUFFMAN_H