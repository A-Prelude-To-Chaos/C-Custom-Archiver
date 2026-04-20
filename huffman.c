#include "huffman.h"
#include <stdlib.h>
#include <string.h>

Node* create_leaf_node(uint8_t symbol, uint64_t freq) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->symbol = symbol;
    n->freq = freq;
    n->left = NULL;
    n->right = NULL;
    return n;
}

Node* create_internal_node(uint64_t freq, Node* left, Node* right) {
    Node* n = (Node*)malloc(sizeof(Node));
    n->symbol = 0;
    n->freq = freq;
    n->left = left;
    n->right = right;
    return n;
}

void free_tree(Node* root) {
    if (!root) return;
    free_tree(root->left);
    free_tree(root->right);
    free(root);
}

void buildCodeTable(Node* root, char* currentCode, int depth, char codeTable[256][256]) {
    if (!root) return;
    
    if (!root->left && !root->right) {
        if (depth == 0) {
            strcpy(codeTable[root->symbol], "1");
        } else {
            currentCode[depth] = '\0';
            strcpy(codeTable[root->symbol], currentCode);
        }
    } else {
        currentCode[depth] = '0';
        buildCodeTable(root->left, currentCode, depth + 1, codeTable);
        
        currentCode[depth] = '1';
        buildCodeTable(root->right, currentCode, depth + 1, codeTable);
    }
}

void serializeTree(Node* root, BitWriter* bw) {
    if (root == NULL) return;
    
    if (root->left == NULL && root->right == NULL) {
        writeBit(bw, 1);
        char c = root->symbol;
        for (int i = 7; i >= 0; i--) {
            int bit = (c >> i) & 1;
            writeBit(bw, bit);
        }
        return;
    }
    writeBit(bw, 0);
    serializeTree(root->left, bw);
    serializeTree(root->right, bw);
}

Node* deserializeTree(BitReader* br) {
    int bit = readBit(br);
    if (bit == -1) return NULL; // EOF or Error
    
    if (bit == 0) {
        Node* left = deserializeTree(br);
        Node* right = deserializeTree(br);
        return create_internal_node(0, left, right);
    } else {
        char ch = 0;
        for (int i = 7; i >= 0; i--) {
            int b = readBit(br);
            if (b != -1) ch |= (b << i);
        }
        return create_leaf_node((uint8_t)ch, 0);
    }
}

// Custom MinHeap logic since we are in bare-metal C
MinHeap* create_heap(int capacity) {
    MinHeap* heap = (MinHeap*)malloc(sizeof(MinHeap));
    heap->size = 0;
    heap->capacity = capacity;
    heap->array = (Node**)malloc(capacity * sizeof(Node*));
    return heap;
}

void swap_node(Node** a, Node** b) {
    Node* t = *a;
    *a = *b;
    *b = t;
}

void heapify(MinHeap* heap, int idx) {
    int smallest = idx;
    int left = 2 * idx + 1;
    int right = 2 * idx + 2;

    if (left < heap->size && heap->array[left]->freq < heap->array[smallest]->freq)
        smallest = left;

    if (right < heap->size && heap->array[right]->freq < heap->array[smallest]->freq)
        smallest = right;

    if (smallest != idx) {
        swap_node(&heap->array[smallest], &heap->array[idx]);
        heapify(heap, smallest);
    }
}

void insert_heap(MinHeap* heap, Node* node) {
    if (heap->size == heap->capacity) return;
    
    int i = heap->size;
    heap->size++;
    heap->array[i] = node;

    while (i != 0 && heap->array[(i - 1) / 2]->freq > heap->array[i]->freq) {
        swap_node(&heap->array[i], &heap->array[(i - 1) / 2]);
        i = (i - 1) / 2;
    }
}

Node* extract_min(MinHeap* heap) {
    if (heap->size <= 0) return NULL;
    if (heap->size == 1) {
        heap->size--;
        return heap->array[0];
    }

    Node* root = heap->array[0];
    heap->array[0] = heap->array[heap->size - 1];
    heap->size--;
    heapify(heap, 0);

    return root;
}

void free_heap(MinHeap* heap) {
    free(heap->array);
    free(heap);
}