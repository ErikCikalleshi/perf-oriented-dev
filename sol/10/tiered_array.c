#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <time.h>

#define TESTING 0
#define BENCHMARK_TIME_IN_SECONDS 5
#define DECISION_ARRAY_SIZE 1000
#if TESTING == 1
#define UNUSED(x) (void)(x)
#endif
void *init_array(int num_elems, int elem_size);
int convert_to_int(const char *str, char *variable_name);
void **init_tiered_array(int num_chunks, int elems_per_chunk, int elem_size);
void clean_up(void **tier_zero_arr, int num_chunks, int *count_arr, int *max_size);
void print_tiered_array(void **tier_zero_arr, int num_chunks, int *max_size, int elem_size);
void write(void **tier_zero_arr, int chunk, int index, void *value, int elem_size, const int *max_size);
void insert(void **tier_zero_arr, int chunk, int index, void *value, int elem_size, int *elem_counter, int num_chunks, int *max_size);
void *read(void **tier_zero_arr, int chunk, const int *max_size, int index, int elem_size);
void delete(void **tier_zero_arr, int chunk, int index, int *max_size, int *count_arr, int elem_size);
void init_decision_array(char *array, int read_op);
void shuffle(char *array);
void run_benchmark(void **tiered_array, int chunks, int *max_size, int *elem_count, int element_size, const char *decision_array);
void run_tests();

/* INPUTS
 * 1. number of chunks (tier-1 arrays)
 * 2. number of elements per chunk
 * 3. size of an element
 * 4. ins_del ratio
 * 5. read_write ratio
 * */
int main(int argc, char *argv[])
{
#if TESTING == 1
    UNUSED(argc);
    UNUSED(argv);
    run_tests();
    return EXIT_SUCCESS;
#else
    if (argc != 6)
    {
        fprintf(stderr, "USAGE: %s <number_chunks> <elems_per_chunk> <elem_size> <ins_del_ratio> <r_w_ratio>\n", argv[0]);
        exit(-1);
    }

    int number_of_chunks = convert_to_int(argv[1], "number_of_chunks");
    int elems_per_chunk = convert_to_int(argv[2], "elems_per_chunk");
    int elem_size = convert_to_int(argv[3], "elem_size");
    int ins_del_ratio = convert_to_int(argv[4], "ins_del_ratio");
    int read_write_ratio = convert_to_int(argv[5], "read_write_ratio");

    if (ins_del_ratio + read_write_ratio != 100)
    {
        fprintf(stderr, "The sum of both ratios has to be equal to 100.\n");
        exit(-1);
    }

    void **tiered_array = init_tiered_array(number_of_chunks, elems_per_chunk, elem_size);
    int *max_size = calloc(number_of_chunks, sizeof(int));
    int *elem_count = calloc(number_of_chunks, sizeof(int));
    for (int i = 0; i < number_of_chunks; i++)
    {
        max_size[i] = elems_per_chunk;
        elem_count[i] = elems_per_chunk;
    }
    char *decision_array = (char *)malloc(DECISION_ARRAY_SIZE * sizeof(char));
    init_decision_array(decision_array, read_write_ratio);
    shuffle(decision_array);

    run_benchmark(tiered_array, number_of_chunks, max_size, elem_count, elem_size, decision_array);

    free(decision_array);
    clean_up(tiered_array, number_of_chunks, elem_count, max_size);
    return EXIT_SUCCESS;
#endif
}

void run_benchmark(void **tiered_array, int chunks, int *max_size, int *elem_count, int element_size, const char *decision_array)
{
    long long int operations = 0;
    long double r_w_operations = 0;
    long double ins_del_operations = 0;
    long long int read_op = 0;
    long long int write_op = 0;
    long long int insert_op = 0;
    long long int delete_op = 0;

    time_t start = time(NULL);
    while (1)
    {
        for (int i = 0; i < chunks; i++)
        {
            for (int j = 0; j < max_size[i]; j++)
            {
                if (time(NULL) - start >= BENCHMARK_TIME_IN_SECONDS)
                {
                    // fprintf(stdout, "Operations done: %lld\nTime passed: %ld\n", operations, (time(NULL) - start));
                    // fprintf(stdout, "R/W ratio: %Lf\nIns/Del ratio: %Lf\n", r_w_operations / operations, ins_del_operations / operations);
                    // fprintf(stdout, "Read op: %lld\nWrite op: %lld\nInsert op: %lld\nDelete op: %lld\n", read_op, write_op, insert_op, delete_op);
                    fprintf(stdout, "%lld\n", operations);
                    assert(operations == (r_w_operations + ins_del_operations));
                    return;
                }
                if (decision_array[j % DECISION_ARRAY_SIZE] == 'R')
                {
                    void *read_val = read(tiered_array, i, max_size, j, element_size);
                    free(read_val);
                    read_op++;
                    r_w_operations++;
                }
                else if (decision_array[j % DECISION_ARRAY_SIZE] == 'W')
                {
                    int value = i + j;
                    write(tiered_array, i, j, &value, element_size, max_size);
                    write_op++;
                    r_w_operations++;
                }
                else if (decision_array[j % DECISION_ARRAY_SIZE] == 'I')
                {
                    int value = i * 2 + j;
                    insert(tiered_array, i, j, &value, element_size, elem_count, chunks, max_size);
                    insert_op++;
                    ins_del_operations++;
                }
                else if (decision_array[j % DECISION_ARRAY_SIZE] == 'D')
                {
                    delete (tiered_array, i, j, max_size, elem_count, element_size);
                    delete_op++;
                    ins_del_operations++;
                }
                operations++;
            }
        }
    }
}

void init_decision_array(char *array, int read_op)
{
    for (int i = 0; i < read_op * 10; i++)
    {
        array[i] = (i % 2 == 0) ? 'R' : 'W';
    }
    for (int i = read_op * 10; i < DECISION_ARRAY_SIZE; i++)
    {
        array[i] = (i % 2 == 0) ? 'I' : 'D';
    }
}

void shuffle(char *array)
{
    srand(time(NULL));
    for (int i = 0; i < DECISION_ARRAY_SIZE; i++)
    {
        int j = i + rand() / (RAND_MAX / (DECISION_ARRAY_SIZE - i) + 1);
        char t = array[j];
        array[j] = array[i];
        array[i] = t;
    }
}

void insert(void **tier_zero_arr, int chunk, int index, void *value, int elem_size, int *elem_counter, int num_chunks, int *max_size)
{
    if (chunk >= num_chunks)
    {
        fprintf(stderr, "insert: invalid chunk\n");
        exit(-1);
    }

    void *old_arr = tier_zero_arr[chunk];
    void *last_value_in_arr = read(tier_zero_arr, chunk, max_size, max_size[chunk] - 1, elem_size);
    if (index > max_size[chunk] - 1 && *(int *)last_value_in_arr == 0)
    {
        write(tier_zero_arr, chunk, max_size[chunk] - 1, value, elem_size, max_size);
        elem_counter[chunk]++;
        free(last_value_in_arr);
        return;
    }
    if (index <= max_size[chunk] - 1 && *(int *)last_value_in_arr == 0)
    {
        write(tier_zero_arr, chunk, index, value, elem_size, max_size);
        elem_counter[chunk]++;
        free(last_value_in_arr);
        return;
    }

    free(last_value_in_arr);
    tier_zero_arr[chunk] = realloc(old_arr, (max_size[chunk] + 1) * elem_size);

    void *arr_to_insert_to = tier_zero_arr[chunk];
    memmove((char *)arr_to_insert_to + (index + 1) * elem_size, (char *)arr_to_insert_to + index * elem_size, (max_size[chunk] - index) * elem_size);

    // increase size
    elem_counter[chunk]++;
    max_size[chunk]++;

    write(tier_zero_arr, chunk, index, value, elem_size, max_size);
}

void delete(void **tier_zero_arr, int chunk, int index, int *max_size, int *count_arr, int elem_size)
{
    if (index > max_size[chunk] - 1)
    {
        fprintf(stderr, "delete: chunk %d, index %d | out of bounds\n", chunk, index);
        exit(-1);
    }

    void *elem_to_delete = read(tier_zero_arr, chunk, max_size, index, elem_size);
    if (*(int *)elem_to_delete != 0)
    {
        count_arr[chunk]--;
    }
    free(elem_to_delete);

    void *arr_to_delete_from = tier_zero_arr[chunk];
    memmove((char *)arr_to_delete_from + index * elem_size, (char *)arr_to_delete_from + (index + 1) * elem_size, (max_size[chunk] - index - 1) * elem_size);

    max_size[chunk]--;
}

void write(void **tier_zero_arr, int chunk, int index, void *value, int elem_size, const int *max_size)
{
    if (index > max_size[chunk] - 1)
    {
        fprintf(stderr, "write: chunk %d, index %d | out of bounds\n", chunk, index);
        exit(-1);
    }
    void *arr = tier_zero_arr[chunk];
    memcpy((char *)arr + index * elem_size, value, elem_size);
}

// Free the variable after read()
void *read(void **tier_zero_arr, int chunk, const int *max_size, int index, int elem_size)
{
    if (index > max_size[chunk] - 1)
    {
        fprintf(stderr, "read: chunk %d, index %d | out of bounds\n", chunk, index);
        exit(-1);
    }

    void *element_to_read = (void *)malloc(elem_size);
    void *arr_to_read_from = tier_zero_arr[chunk];
    memcpy(element_to_read, (char *)arr_to_read_from + index * elem_size, elem_size);
    return element_to_read;
}

void **init_tiered_array(int num_chunks, int elems_per_chunk, int elem_size)
{
    void **tier_zero_arr = (void **)malloc(num_chunks * sizeof(void *));
    if (tier_zero_arr == NULL)
    {
        perror("malloc_tiered_array");
        exit(-1);
    }

    // init block_arrays and create pointers to them
    for (int i = 0; i < num_chunks; i++)
    {
        void *arr = init_array(elems_per_chunk, elem_size);
        tier_zero_arr[i] = arr;
    }

    return tier_zero_arr;
}

void *init_array(int num_elems, int elem_size)
{
    void *array = (void *)malloc(elem_size * num_elems);
    if (array == NULL)
    {
        perror("malloc_init_array");
        exit(-1);
    }
    for (int i = 0; i < num_elems; i++)
    {
        memset((char *)array + i * elem_size, 0, elem_size);
    }
    return array;
}

void clean_up(void **tier_zero_arr, int num_chunks, int *count_arr, int *max_size)
{
    for (int i = 0; i < num_chunks; i++)
    {
        free(tier_zero_arr[i]);
    }
    free(tier_zero_arr);
    free(count_arr);
    free(max_size);
}

int convert_to_int(const char *str, char *variable_name)
{
    char *endptr;
    long val = strtol(str, &endptr, 10);

    if (endptr == str || *endptr != '\0')
    {
        fprintf(stderr, "%s is not a number.\n", variable_name);
        exit(-1);
    }

    if (val < 0)
    {
        fprintf(stderr, "%s has to be positive.\n", variable_name);
        exit(-1);
    }

    return (int)val;
}

// Prints integer arrays
void print_tiered_array(void **tier_zero_arr, int num_chunks, int *max_size, int elem_size)
{
    for (int i = 0; i < num_chunks; i++)
    {
        fprintf(stdout, "chunk %d\n", i);
        for (int j = 0; j < max_size[i]; j++)
        {
            void *elem_to_print = read(tier_zero_arr, i, max_size, j, elem_size);
            if (j + 1 != max_size[i])
            {
                fprintf(stdout, "[%d] ", *(int *)elem_to_print);
            }
            else
            {
                fprintf(stdout, "[%d]\n", *(int *)elem_to_print);
            }
            free(elem_to_print);
        }
    }
}

// Tests
void test_initialize_tiered_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            assert(*(int *)value_in_arr == 0);
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }

    free(max_size);
    free(tiered_arr);
}

void test_write()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            int val = i + j;
            write(tiered_arr, i, j, &val, 8, max_size);
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            assert(*(int *)value_in_arr == i + j);
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
}

void test_insert_at_end_of_empty_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    int value = 100;
    insert(tiered_arr, 0, 4, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 1, 4, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 2, 4, &value, 8, elem_counter, 3, max_size);

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 5);
        assert(elem_counter[i] == 1);
        for (int j = 0; j < 5; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            if (j != 4)
            {
                assert(*(int *)value_in_arr == 0);
            }
            else
            {
                assert(*(int *)value_in_arr == 100);
            }
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_insert_at_beginning_of_empty_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    int value = 100;
    insert(tiered_arr, 0, 0, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 1, 0, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 2, 0, &value, 8, elem_counter, 3, max_size);

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 5);
        assert(elem_counter[i] == 1);
        for (int j = 0; j < 5; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            if (j != 0)
            {
                assert(*(int *)value_in_arr == 0);
            }
            else
            {
                assert(*(int *)value_in_arr == 100);
            }
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_insert_in_the_middle_of_empty_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    int value = 100;
    insert(tiered_arr, 0, 2, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 1, 2, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 2, 2, &value, 8, elem_counter, 3, max_size);

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 5);
        assert(elem_counter[i] == 1);
        for (int j = 0; j < 5; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            if (j != 2)
            {
                assert(*(int *)value_in_arr == 0);
            }
            else
            {
                assert(*(int *)value_in_arr == 100);
            }
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_insert_at_the_end_of_full_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    int value = 100;

    for (int i = 0; i < 3; i++)
    {
        elem_counter[i] = 5;
        for (int j = 0; j < 5; j++)
        {
            int val = i + j;
            write(tiered_arr, i, j, &val, 8, max_size);
        }
    }

    insert(tiered_arr, 0, 5, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 1, 5, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 2, 5, &value, 8, elem_counter, 3, max_size);

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 6);
        assert(elem_counter[i] == 6);
        for (int j = 0; j < max_size[i]; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            if (j == 5)
            {
                assert(*(int *)value_in_arr == 100);
            }
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_insert_at_the_beginning_of_full_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    int value = 100;

    for (int i = 0; i < 3; i++)
    {
        elem_counter[i] = 5;
        for (int j = 0; j < 5; j++)
        {
            int val = i + j;
            write(tiered_arr, i, j, &val, 8, max_size);
        }
    }
    void *first_value_first_chunk = read(tiered_arr, 0, max_size, 0, 8);
    void *first_value_second_chunk = read(tiered_arr, 1, max_size, 0, 8);
    void *first_value_third_chunk = read(tiered_arr, 2, max_size, 0, 8);

    insert(tiered_arr, 0, 0, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 1, 0, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 2, 0, &value, 8, elem_counter, 3, max_size);

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 6);
        assert(elem_counter[i] == 6);
        for (int j = 0; j < max_size[i]; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            if (j == 0)
            {
                assert(*(int *)value_in_arr == 100);
            }
            if (i == 0 && j == 1)
            {
                assert(*(int *)value_in_arr == *(int *)first_value_first_chunk);
            }
            if (i == 1 && j == 1)
            {
                assert(*(int *)value_in_arr == *(int *)first_value_second_chunk);
            }
            if (i == 2 && j == 1)
            {
                assert(*(int *)value_in_arr == *(int *)first_value_third_chunk);
            }
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
    free(first_value_first_chunk);
    free(first_value_second_chunk);
    free(first_value_third_chunk);
}

void test_insert_in_the_middle_of_full_array()
{
    void **tiered_arr = init_tiered_array(3, 5, 8);
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }
    int value = 100;

    for (int i = 0; i < 3; i++)
    {
        elem_counter[i] = 5;
        for (int j = 0; j < 5; j++)
        {
            int val = i + j;
            write(tiered_arr, i, j, &val, 8, max_size);
        }
    }
    void *first_chunk_index_1 = read(tiered_arr, 0, max_size, 1, 8);
    void *first_chunk_index_2 = read(tiered_arr, 0, max_size, 2, 8);
    void *second_chunk_index_1 = read(tiered_arr, 1, max_size, 1, 8);
    void *second_chunk_index_2 = read(tiered_arr, 1, max_size, 2, 8);
    void *third_chunk_index_1 = read(tiered_arr, 2, max_size, 1, 8);
    void *third_chunk_index_2 = read(tiered_arr, 2, max_size, 2, 8);

    insert(tiered_arr, 0, 2, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 1, 2, &value, 8, elem_counter, 3, max_size);
    insert(tiered_arr, 2, 2, &value, 8, elem_counter, 3, max_size);

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 6);
        assert(elem_counter[i] == 6);
        for (int j = 0; j < max_size[i]; j++)
        {
            void *value_in_arr = read(tiered_arr, i, max_size, j, 8);
            if (j == 2)
            {
                assert(*(int *)value_in_arr == value);
            }
            if (i == 0 && j == 1)
            {
                assert(*(int *)value_in_arr == *(int *)first_chunk_index_1);
            }
            if (i == 0 && j == 3)
            {
                assert(*(int *)value_in_arr == *(int *)first_chunk_index_2);
            }
            if (i == 1 && j == 1)
            {
                assert(*(int *)value_in_arr == *(int *)second_chunk_index_1);
            }
            if (i == 1 && j == 3)
            {
                assert(*(int *)value_in_arr == *(int *)second_chunk_index_2);
            }
            if (i == 2 && j == 1)
            {
                assert(*(int *)value_in_arr == *(int *)third_chunk_index_1);
            }
            if (i == 2 && j == 3)
            {
                assert(*(int *)value_in_arr == *(int *)third_chunk_index_2);
            }
            free(value_in_arr);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
    free(first_chunk_index_1);
    free(first_chunk_index_2);
    free(second_chunk_index_1);
    free(second_chunk_index_2);
    free(third_chunk_index_1);
    free(third_chunk_index_2);
}

void test_delete_at_the_beginning_of_empty_array()
{
    void **tiered_arr = init_tiered_array(3, 5, sizeof(int));
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }

    delete (tiered_arr, 0, 0, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 1, 0, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 2, 0, max_size, elem_counter, sizeof(int));

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 4);
        assert(elem_counter[i] == 0);
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_delete_in_the_middle_of_empty_array()
{
    void **tiered_arr = init_tiered_array(3, 5, sizeof(int));
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }

    delete (tiered_arr, 0, 2, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 1, 2, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 2, 2, max_size, elem_counter, sizeof(int));

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 4);
        assert(elem_counter[i] == 0);
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_delete_at_the_end_of_empty_array()
{
    void **tiered_arr = init_tiered_array(3, 5, sizeof(int));
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
    }

    delete (tiered_arr, 0, 4, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 1, 4, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 2, 4, max_size, elem_counter, sizeof(int));

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 4);
        assert(elem_counter[i] == 0);
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_delete_at_the_beginning_of_full_array()
{
    void **tiered_arr = init_tiered_array(3, 5, sizeof(int));
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
        elem_counter[i] = 5;
    }

    int value = 100;
    int first_value = 92;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (j == 0)
            {
                write(tiered_arr, i, j, &first_value, 8, max_size);
                void *value_to_read = read(tiered_arr, i, max_size, j, sizeof(int));
                assert(*(int *)value_to_read == first_value);
                free(value_to_read);
            }
            else
            {
                write(tiered_arr, i, j, &value, sizeof(int), max_size);
            }
        }
    }

    delete (tiered_arr, 0, 0, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 1, 0, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 2, 0, max_size, elem_counter, sizeof(int));

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 4);
        assert(elem_counter[i] == 4);
        for (int j = 0; j < max_size[i]; j++)
        {
            void *value_to_read = read(tiered_arr, i, max_size, j, sizeof(int));
            if (j == 0)
            {
                assert(*(int *)value_to_read != first_value);
            }
            assert(*(int *)value_to_read == value);
            free(value_to_read);
        }
    }
    for (int i = 0; i < 3; i++)
    {
        free(tiered_arr[i]);
    }
    free(max_size);
    free(tiered_arr);
    free(elem_counter);
}

void test_delete_in_the_middle_of_full_array()
{
    void **tiered_arr = init_tiered_array(3, 5, sizeof(int));
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
        elem_counter[i] = 5;
    }

    int middle_value = 92;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (j == 2)
            {
                write(tiered_arr, i, j, &middle_value, sizeof(int), max_size);
                void *value_to_read = read(tiered_arr, i, max_size, j, sizeof(int));
                assert(*(int *)value_to_read == middle_value);
                free(value_to_read);
            }
            else
            {
                int value = i + j;
                write(tiered_arr, i, j, &value, sizeof(int), max_size);
            }
        }
    }
    void *first_chunk_index_3 = read(tiered_arr, 0, max_size, 3, sizeof(int));
    void *second_chunk_index_3 = read(tiered_arr, 1, max_size, 3, sizeof(int));
    void *third_chunk_index_3 = read(tiered_arr, 2, max_size, 3, sizeof(int));

    delete (tiered_arr, 0, 2, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 1, 2, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 2, 2, max_size, elem_counter, sizeof(int));

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 4);
        assert(elem_counter[i] == 4);
        for (int j = 0; j < max_size[i]; j++)
        {
            void *value_to_read = read(tiered_arr, i, max_size, j, sizeof(int));
            if (i == 0 && j == 2)
            {
                assert(*(int *)value_to_read == *(int *)first_chunk_index_3);
            }
            if (i == 1 && j == 2)
            {
                assert(*(int *)value_to_read == *(int *)second_chunk_index_3);
            }
            if (i == 2 && j == 2)
            {
                assert(*(int *)value_to_read == *(int *)third_chunk_index_3);
            }
            free(value_to_read);
        }
    }
    free(first_chunk_index_3);
    free(second_chunk_index_3);
    free(third_chunk_index_3);
    clean_up(tiered_arr, 3, elem_counter, max_size);
}

void test_delete_at_the_end_of_full_array()
{
    void **tiered_arr = init_tiered_array(3, 5, sizeof(int));
    int *elem_counter = (int *)calloc(3, sizeof(int));
    int *max_size = (int *)calloc(3, sizeof(int));
    for (size_t i = 0; i < 3; i++)
    {
        max_size[i] = 5;
        elem_counter[i] = 5;
    }

    int value_index_3 = 92;
    for (int i = 0; i < 3; i++)
    {
        for (int j = 0; j < 5; j++)
        {
            if (j == 3)
            {
                write(tiered_arr, i, j, &value_index_3, sizeof(int), max_size);
                void *value_to_read = read(tiered_arr, i, max_size, j, sizeof(int));
                assert(*(int *)value_to_read == value_index_3);
                free(value_to_read);
            }
            else
            {
                int value = i + j;
                write(tiered_arr, i, j, &value, sizeof(int), max_size);
            }
        }
    }

    delete (tiered_arr, 0, 4, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 1, 4, max_size, elem_counter, sizeof(int));
    delete (tiered_arr, 2, 4, max_size, elem_counter, sizeof(int));

    for (int i = 0; i < 3; i++)
    {
        assert(max_size[i] == 4);
        assert(elem_counter[i] == 4);
        for (int j = 0; j < max_size[i]; j++)
        {
            if (j == 3)
            {
                void *value_to_read = read(tiered_arr, i, max_size, j, sizeof(int));
                assert(*(int *)value_to_read == value_index_3);
                free(value_to_read);
            }
        }
    }

    clean_up(tiered_arr, 3, elem_counter, max_size);
}

void run_tests()
{
    fprintf(stdout, "Running test_initialize_tiered_array()...\n");
    test_initialize_tiered_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_write()...\n");
    test_write();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_insert_at_end_of_empty_array()...\n");
    test_insert_at_end_of_empty_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_insert_at_beginning_of_empty_array()...\n");
    test_insert_at_beginning_of_empty_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_insert_in_the_middle_of_empty_array()...\n");
    test_insert_in_the_middle_of_empty_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_insert_at_the_end_of_full_array()...\n");
    test_insert_at_the_end_of_full_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_insert_at_the_beginning_of_full_array()...\n");
    test_insert_at_the_beginning_of_full_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_insert_in_the_middle_of_full_array()...\n");
    test_insert_in_the_middle_of_full_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_delete_at_the_beginning_of_empty_array()...\n");
    test_delete_at_the_beginning_of_empty_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_delete_in_the_middle_of_empty_array()...\n");
    test_delete_in_the_middle_of_empty_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_delete_at_the_end_of_empty_array()...\n");
    test_delete_at_the_end_of_empty_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_delete_at_the_beginning_of_full_array()...\n");
    test_delete_at_the_beginning_of_full_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_delete_in_the_middle_of_full_array()...\n");
    test_delete_in_the_middle_of_full_array();
    fprintf(stdout, "Test ran successfully.\n");

    fprintf(stdout, "Running test_delete_at_the_end_of_full_array()...\n");
    test_delete_at_the_end_of_full_array();
    fprintf(stdout, "Test ran successfully.\n");
}
