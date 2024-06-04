#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define BENCHMARK_TIME_IN_SECONDS 5
#define ARRAY_SIZE 100

int convert_to_int(const char* str, char* variable_name);
void run_benchmark(int number_elements, int elem_size, const char *decision_array);
void init_decision_array(char *array, size_t read_op);
void shuffle(char *array);
void init_linked_list(size_t number_elements, size_t elem_size);
void* read(int index);
void write(int index, void* value);

typedef struct Node {
    void* value;
    struct Node* next;
} Node;

Node* head = NULL;
int size = 0;
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

    if (insertions_deletions + reads_writes != 100) {
        fprintf(stderr, "The ratios should have a total sum of 100.\nCurrent sum: %d\n", insertions_deletions + reads_writes);
        exit(-1);
    }

    init_linked_list(number_elements, element_size);
    int write_value = 5;
    write(5, &write_value);
    for (int i = 0; i < number_elements; i++) {
        if (i + 1 != number_elements) {
            fprintf(stdout, "%d -> ", *(int*)read(i));
        } else {
            fprintf(stdout, "%d\n", *(int*)read(i));
        }
    }

//    srand(time(NULL));
//    char shuffle_array[ARRAY_SIZE];
//    size_t read_write_operations = ARRAY_SIZE * reads_writes / 100;

//    init_decision_array(shuffle_array, read_write_operations);
//    shuffle(shuffle_array);
//    run_benchmark(number_elements, element_size, shuffle_array);
    return EXIT_SUCCESS;
}

void init_decision_array(char *array, size_t read_op) {
    for (size_t i = 0; i < read_op; i++) {
        array[i] = 'R';
    }
    for (size_t i = read_op; i < ARRAY_SIZE; i++) {
        array[i] = 'I';
    }
}

void shuffle(char *array) {
    for (size_t i = 0; i < ARRAY_SIZE; i++) {
        size_t j = i + rand() / (RAND_MAX / (ARRAY_SIZE - i) + 1);
        char t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void* read(int index) {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }

    // Prevent optimizations the compiler would've done
    volatile void* read_var;
    read_var = current->value;
    return current->value;
}

void write(int index, void* value) {
    Node* current = head;
    for (int i = 0; i < index; i++) {
        current = current->next;
    }
    current->value = value;
}

void insert(int index, void* value, size_t eleme_size) {
    Node* new = (Node*)malloc(sizeof(Node));
    new->value = malloc(eleme_size);
    memcpy(new->value, value, eleme_size);
    // Insert at the beginning
    if (index == 0) {
        new->next = head;
        head = new;
    }
    // Insert in between
    else if (index < size)
    {
        Node* position = head;
        for (int i = 0; i < index - 1; i++) {
            position = position->next;
        }
        new->next = position->next;
        position->next = new;
    }
    // Insert at the end of the list
    else {
        Node* pos = head;
        for (int i = 0; i < size; i++) {
            pos = pos->next;
        }
        pos->next = new;
    }
    size++;
}

void delete(int index) {
    Node* del = NULL;
    // Delete at the beginning of the list
    if (index == 0) {
        del = head;
        head = head->next;
        del->next = NULL;
    }
    // Delete in between
    else if (index <= size - 1) {
        Node* prev_node = head;
        for (int i = 0; i < index - 1; i++) {
            prev_node = prev_node->next;
        }
        del = prev_node->next;
        prev_node->next = del->next;
        del->next = NULL;
    }
    // Delete at the end of the list
    else {
        Node* prev_node = head;
        del = head->next;
        while (del->next != NULL) {
            prev_node = prev_node->next;
            del = del->next;
        }
        prev_node->next = NULL;
    }
    free(del->value);
    free(del);
    size--;
}

void run_benchmark(int number_elements, int elem_size, const char *decision_array) {
    for (int i = 0; i < number_elements; i++) {
        insert(i, &i, elem_size);
    }

    long long int operations = 0;
    long double read_write_operations_done = 0;
    long double insert_delete_operations_done = 0;

    time_t start = time(NULL);

    while(1) {
        for (int i = 0; i < number_elements; i++) {
            if (time(NULL) - start >= BENCHMARK_TIME_IN_SECONDS) {
                fprintf(stdout, "Operations done: %lld\nTime passed: %ld\n", operations, (time(NULL) - start));
                assert((read_write_operations_done + insert_delete_operations_done) == operations);
                fprintf(stdout, "Ratio computed for R/W: %Lf\nRatio computed for I/D: %Lf\n", read_write_operations_done / operations, insert_delete_operations_done / operations);
                exit(0);
            }

            if (decision_array[i % ARRAY_SIZE] == 'R') {
                read(i);
                write(i, &i);
                read_write_operations_done++;
            } else {
                insert(i, &i, elem_size);
                delete(i);
                insert_delete_operations_done++;
            }
            operations++;
        }
    }
}

void init_linked_list(size_t number_elements, size_t elem_size) {
    Node* start = (Node*)malloc(sizeof(Node));
    start->value = malloc(elem_size);
    head = start;
    memset(start->value, 0, elem_size);
    for (size_t i = 0; i < number_elements; i++) {
        // Assign value to start_node; create new_node to point start_node->new_node; start_node becomes the new_node(last_node) at the end of the iteration
        Node* new_node = (Node*)malloc(sizeof(Node));
        new_node->value = malloc(elem_size);
        memset(new_node->value, 0, elem_size);
        new_node->next = NULL;

        start->next = new_node;
        start = new_node;
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
