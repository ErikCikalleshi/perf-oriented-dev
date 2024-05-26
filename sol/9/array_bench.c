#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define TIME_LIMIT 4 // time limit for the benchmark in seconds

volatile int read_value;

void read(int index, int* array) {
    read_value = array[index];
}

void write(int index, int value, int* array) {
    array[index] = value + read_value;
}

void insertion(int index, int value, int* array, int size) {
    for (int i = size - 1; i > index; i--) {
        array[i] = array[i - 1];
    }
    array[index] = value;
}

void deletion(int index, int* array, int size) {
    for (int i = index; i < size - 1; i++) {
        array[i] = array[i + 1];
    }
}

void benchmark(int ins_del_ratio, int read_write_ratio, int element_size, int num_elements) {
    long long int operations = 0;
    time_t start = time(NULL);

    int* array = (int*)malloc((num_elements + 1) * sizeof(int)); // allocate array dynamically

    for (int i = 0; i < num_elements; i++) {
        array[i] = i;
    }

    while (time(NULL) - start < TIME_LIMIT) {
        for (int i = 0; i < num_elements; i++) {
            if (i % read_write_ratio == 0) {
                read(i, array);
                write(i, i, array);
            }
            if (ins_del_ratio != 0 && i % ins_del_ratio == 0) {
                insertion(i, i, array, num_elements + 1);
                deletion(i, array, num_elements + 1);
            }
            operations++;
        }
    }

    printf("%lld\n", operations);
    free(array); // free the dynamically allocated memory
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

    benchmark(ins_del_ratio, read_write_ratio, element_size, num_elements);
    return 0;
}
