#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#define BENCHMARK_TIME_IN_SECONDS 5 

int convert_to_int(const char* str, char* variable_name);
void run_benchmark(int number_elements, int elem_size, int insertions_deletions, int reads_writes);

/*
INPUTS
1. number of elements (n + 1 for arrays)
2. element size
3. insertions & deletions in %
4. reads & writes in %
*/
int main(int argc, char* argv[]) {
    if (argc != 5) {
        fprintf(stderr, "Usage: %s <number_elements> <element_size> <insertions_deletions> <reads_writes>\n", argv[0]);
        exit(-1);
    }

    int number_elements = convert_to_int(argv[1], "number_elements");
    int element_size = convert_to_int(argv[2], "element_size");
    int insertions_deletions = convert_to_int(argv[3], "insertions_deletions");
    int reads_writes = convert_to_int(argv[4], "reads_writes");

    run_benchmark(number_elements, element_size, insertions_deletions, reads_writes);
    return EXIT_SUCCESS;
}

typedef struct Node {
    int value;
    struct Node* next;
} Node;

Node* head = NULL;
int size = 0;
// used to prevent optimizations;
volatile int read_var;
void read(int index) {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    read_var = current->value;
}

void write(int index, int value) {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    current->value = value + read_var;
}

void insert(int index, int value) {
    Node* new = malloc(sizeof(Node));
    new->value = value;
    if (index == 0) {
        new->next = head;
        head = new;
    } 
    else
    {
        Node* position = head;
        for (int i = 0; i < index - 1; i++) {
            position = position->next;
        }
        new->next = position->next;
        position->next = new;
    }
    size++;
}

void delete(int index) {
    Node* del;
    if (index == 0) {
        del = head;
        head = head->next;
        free(del);
    }
    else 
    {
        Node* pos = head;
        for (int i = 0; i < index - 1; i++) {
            pos = pos->next;
        }
        del = pos->next;
        pos->next = del->next;
    }
    free(del);
    size--;
}

void run_benchmark(int number_elements, int elem_size, int insertions_deletions, int reads_writes) {

    long long int operations = 0;
    long long int operations_inside_ifs = 0;

    for (int i = 0; i < number_elements; i++) {
        insert(i, i);
    }

    time_t start = time(NULL);
    int r_w_ratio = reads_writes != 0 ? 10 / (reads_writes / 10) : 0;
    int i_d_ratio = insertions_deletions != 0 ? 10 / (insertions_deletions / 10) : 0;

    while(1) {
        for (int i = 0; i < number_elements; i++) {
            if (time(NULL) - start >= BENCHMARK_TIME_IN_SECONDS) {
                fprintf(stdout, "Operations done: %lld\nTime passed: %ld\n", operations, (time(NULL) - start));
                assert(operations == operations_inside_ifs);
                exit(0);
            }

            if (i % r_w_ratio == 0) {
                read(i);
                write(i, i);
                operations_inside_ifs++;
            } else if (i % i_d_ratio == 0) {
                insert(i, i);
                delete(i);
                operations_inside_ifs++;
            }
            operations++;
        } 
    }
}

int convert_to_int(const char* str, char* variable_name) {
    char* endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0') {
        fprintf(stderr, "%s is not a number.\n", variable_name);
        exit(-1);
    }
    
    if (val < 0) {
        fprintf(stderr, "%s has to be positive.\n", variable_name);
        exit(-1);
    }

    return (int)val;
}