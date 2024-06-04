#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>
#define BENCHMARK_TIME_IN_SECONDS 5 
#define ARRAY_SIZE 100

int convert_to_int(const char* str, char* variable_name);
void run_benchmark(int number_elements, int elem_size, char *decision_array);
void init_decision_array(char *array, size_t read_op);
void shuffle(char *array);

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
    // unused variable
    insertions_deletions = insertions_deletions;
    int reads_writes = convert_to_int(argv[4], "reads_writes");
  


    srand(time(NULL));
    char shuffle_array[ARRAY_SIZE];
    size_t read_write_operations = ARRAY_SIZE * reads_writes / 100;

    init_decision_array(shuffle_array, read_write_operations);
    shuffle(shuffle_array);
    run_benchmark(number_elements, element_size, shuffle_array);
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

void read(int index, char* array, int arr_len, int elem_size) {
    if (index > arr_len - 1) {
        fprintf(stderr, "Index %d is higher than arr_len %d (read-op).\n", index, arr_len);
        exit(-1);
    }

    void* element = malloc(elem_size);
    if (element == NULL) {
        perror("malloc_read");
        exit(-1);
    }
    // copy elem_size bytes into 'element' address, starting from array_addr + index * elem_size
    memcpy(element, array + index * elem_size, elem_size);
    // prevent optimizations
    volatile int elem = *((int*)element);
    elem = elem;
    free(element);
}

void write(int index, char* array, int arr_len, int value, int elem_size) {
    if (index > arr_len - 1) {
        fprintf(stderr, "Index %d is higher than arr_len %d (write-op).\n", index, arr_len);
        exit(-1);
    }
    memcpy(array + index * elem_size, &value, elem_size);
}

void insert(int index, char* array, int value, int* arr_len, int elem_size) {
    if (index > (*arr_len) - 1) {
        fprintf(stderr, "Index %d is higher than arr_len %d (insert-op).\n", index, *arr_len);
        exit(-1);
    }

    // shift all elements starting at index to the right and insert new value at index
    memmove(array + (index + 1) * elem_size, array + index * elem_size, (*arr_len - index - 1) * elem_size);
    write(index, array, *arr_len, value, elem_size);
    (*arr_len)++;
}

void delete(int index, char* array, int* arr_len, int elem_size) {
    memmove(array + index * elem_size, array + (index + 1) * elem_size, (*arr_len - index - 1) * elem_size);
    (*arr_len)--;
}

void run_benchmark(int number_elements, int elem_size, char *decision_array) {
    char* arr = malloc((number_elements + 1) * elem_size);
    if (arr == NULL) {
        perror("array_init_malloc");
        exit(-1);
    }

    for (int i = 0; i < number_elements;  i++) {
        arr[i] = i;
    }

    int arr_len = number_elements;
    long long int operations = 0;
    long double read_write_operations_done = 0;
    long double insert_delete_operations_done = 0;

    time_t start = time(NULL);

    while(1) {
        for (int i = 0; i < number_elements; i++) {
            if (time(NULL) - start >= BENCHMARK_TIME_IN_SECONDS) {
                //fprintf(stdout, "Operations done: %lld\nTime passed: %ld\n", operations, (time(NULL) - start));
                free(arr);
                //assert((read_write_operations_done + insert_delete_operations_done) == operations);
                //fprintf(stdout, "Ratio computed for R/W: %Lf\nRatio computed for I/D: %Lf\n", read_write_operations_done / operations, insert_delete_operations_done / operations);
                fprintf(stdout, "%lld\n", operations);
                exit(0);
            }

            if (decision_array[i % ARRAY_SIZE] == 'R') {
                read(i, arr, arr_len, elem_size);
                write(i, arr, arr_len, i, elem_size);
                read_write_operations_done++;
            } else {
                insert(i, arr, i, &arr_len, elem_size);
                delete(i, arr, &arr_len, elem_size);
                insert_delete_operations_done++;
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