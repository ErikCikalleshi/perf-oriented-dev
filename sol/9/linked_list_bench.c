#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIME_LIMIT 4 // time limit for the benchmark in seconds

typedef struct Node {
    int value;
    struct Node* next;
} Node;

Node* head = NULL;
volatile int read_value;

void read(int index) {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    read_value = current->value;
}

void write(int index, int value) {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    current->value = value + read_value;
}

void insertion(int index, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;

    if (index == 0) {
        newNode->next = head;
        head = newNode;
    } else {
        Node* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void deletion(int index) {
    Node* toDelete;
    if (index == 0) {
        toDelete = head;
        head = head->next;
    } else {
        Node* current = head;
        for (int i = 0; i < index - 1; i++) {
            current = current->next;
        }
        toDelete = current->next;
        current->next = toDelete->next;
    }
    free(toDelete);
}

void benchmark(int ins_del_ratio, int read_write_ratio, int element_size, int num_elements) {
    long long int operations = 0;
    time_t start = time(NULL);
    while (time(NULL) - start < TIME_LIMIT) {
        for (int i = 0; i < num_elements; i++) {
            if (i % read_write_ratio == 0) {
                read(i % element_size);
                write(i % element_size, i);
            }
            if (ins_del_ratio != 0 && i % ins_del_ratio == 0) {
                insertion(i % element_size, i);
                deletion(i % element_size);
            }
            operations++;
        }
    }
    printf("%lld\n", operations);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s ins_del_ratio read_write_ratio element_size num_elements\n", argv[0]);
        return 1;
    }

    int ins_del_ratio = atoi(argv[1]);
    int read_write_ratio = atoi(argv[2]);
    int element_size = atoi(argv[3]);
    int num_elements = atoi(argv[4]);

    for (int i = 0; i < element_size; i++) {
        insertion(0, i);
    }

    benchmark(ins_del_ratio, read_write_ratio, element_size, num_elements);

    return 0;
}