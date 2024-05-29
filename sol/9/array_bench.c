#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/mman.h>
#include <errno.h>


#define TIME_LIMIT 4 // time limit for the benchmark in seconds

volatile int read_value;

/*void read(int index, int* array) {
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
}*/


void read(int index, void* array, size_t element_size) {
    void* element = (char*)array + index * element_size;
    // use element to prevent it from being optimized away
    read_value = *(int*)element;
}

void write(int index, int value, void* array, size_t element_size) {
    void* element = (char*)array + index * element_size;
    memcpy(element, &value, element_size);
}

void insertion(int index, int value, void* array, size_t size, size_t element_size) {
    void* element = (char*)array + index * element_size;
    memmove((char*)element + element_size, element, (size - index - 1) * element_size);
    memcpy(element, &value, element_size);
}

void deletion(int index, void* array, size_t size, size_t element_size) {
    void* element = (char*)array + index * element_size;
    memmove(element, (char*)element + element_size, (size - index - 1) * element_size);
}

void benchmark(int ins_del_ratio, int read_write_ratio, size_t element_size, size_t num_elements) {
    long long int operations = 0;
    time_t start = time(NULL);

    size_t total_size = (num_elements + 1) * element_size;
    void* array = malloc(total_size);

    if (array == NULL) {
        fprintf(stderr, "Memory allocation failed: %s\n", strerror(errno));
        return;
    }

    while (1) {
        for (size_t i = 0; i < num_elements; i++) {
            if (time(NULL) - start >= TIME_LIMIT) {
                time_t end = time(NULL);
                printf("%lld,", operations);
                printf("%ld\n", end - start);
                munmap(array, total_size);
                return;
            }
            if (i % read_write_ratio == 0) {
                read(i, array, element_size);
                write(i, i, array, element_size);
            }
            if (ins_del_ratio != 0 && i % ins_del_ratio == 0) {
                insertion(i, i, array, num_elements + 1, element_size);
                deletion(i, array, num_elements + 1, element_size);
            }
            operations++;
        }
    }

    time_t end = time(NULL);
    printf("%lld,", operations);
    printf("%ld\n", end - start);
    munmap(array, total_size);
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

    benchmark(ins_del_ratio, read_write_ratio, element_size, num_elements);
    return 0;
}