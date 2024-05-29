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

void read(size_t index) {
    Node* current = head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    read_value = current->value;
}

void write(size_t index, int value) {
    Node* current = head;
    for (size_t i = 0; i < index; i++) {
        current = current->next;
    }
    current->value = value + read_value;
}

void insertion(size_t index, int value) {
    Node* newNode = (Node*)malloc(sizeof(Node));
    newNode->value = value;

    if (index == 0) {
        newNode->next = head;
        head = newNode;
    } else {
        Node* current = head;
        for (size_t i = 0; i < index - 1; i++) {
            current = current->next;
        }
        newNode->next = current->next;
        current->next = newNode;
    }
}

void deletion(size_t index) {
    Node* toDelete;
    if (index == 0) {
        toDelete = head;
        head = head->next;
    } else {
        Node* current = head;
        for (size_t i = 0; i < index - 1; i++) {
            current = current->next;
        }
        toDelete = current->next;
        current->next = toDelete->next;
    }
    free(toDelete);
}

void benchmark(int ins_del_ratio, int read_write_ratio, size_t element_size, size_t num_elements) {
    long long int operations = 0;
    time_t start = time(NULL);
    while (1) {
        for (size_t i = 0; i < num_elements; i++) {
            if (time(NULL) - start >= TIME_LIMIT) {
                time_t end = time(NULL);
                printf("%lld,", operations);
                printf("%ld seconds\n", end - start);
                return;
            }
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
    time_t end = time(NULL);
    printf("%lld,", operations);
    printf("%ld seconds\n", end - start);
}

int main(int argc, char *argv[]) {
    if (argc != 5) {
        printf("Usage: %s ins_del_ratio read_write_ratio element_size num_elements\n", argv[0]);
        return 1;
    }

    int ins_del_ratio = atoi(argv[1]);
    int read_write_ratio = atoi(argv[2]);
    size_t element_size = atoi(argv[3]);
    size_t num_elements = atoi(argv[4]);

  
    for (size_t i = 0; i < element_size; i++) {
        insertion(0, i);
    }

    benchmark(ins_del_ratio, read_write_ratio, element_size, num_elements);

    return 0;
}
