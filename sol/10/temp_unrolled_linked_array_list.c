/*
Task: Implement an unrolled linked list
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define TESTING 0
#define BENCHMARK_TIME_IN_SECONDS 5
#define DECISION_ARRAY_SIZE 1000
#if TESTING == 1
#define UNUSED(x) (void)(x)
#endif

int convert_to_int(const char *str, char *variable_name);

void run_benchmark(int elem_size, const char *decision_array, int number_chunks, int elems_per_chunk);

void init_decision_array(char *array, int read_op);

void shuffle(char *array);

void init_unrolled_linked_list(size_t number_chunks, size_t max_elem_per_chunk, size_t elem_size);

void *read(int block_index, int elem_index, size_t eleme_size);

void write(int block_index, int elem_index, void *value, size_t eleme_size);

void insert(int block_index, int elem_index, void *value, size_t eleme_size, size_t max_elem_per_chunk);

void delete(int block_index, int elem_index, size_t eleme_size);

void clean_up();

void print_block_as_linked_list(int block_index, size_t max_elem_per_chunk);

void run_example();

void print_linked_list();

typedef struct Node
{
    void *value;
    struct Node *next_node;
} Node;

typedef struct LinkedBlock
{
    struct LinkedBlock *next_block;
    Node *head;
    int count;
    int max_elem_per_chunk;
} LinkedBlock;

LinkedBlock *blockHead = NULL;

void init_block(LinkedBlock *block, size_t max_elem_per_chunk, size_t elem_size);

void print_block(LinkedBlock *block, size_t eleme_size);

/*
INPUTS
1. number of elements (n + 1 for arrays)
2. element size
3. insertions & deletions in %
4. reads & writes in %
*/
int main(int argc, char *argv[])
{
#if TESTING == 1
    UNUSED(argc);
    UNUSED(argv);
    run_example();
    return EXIT_SUCCESS;
#else
    if (argc != 6)
    {
        fprintf(stderr, "USAGE: %s <number_chunks> <elems_per_chunk> <elem_size> <ins_del_ratio> <r_w_ratio>\n", argv[0]);
        exit(-1);
    }

    int number_of_chunks = convert_to_int(argv[1], "number_of_chunks");
    int elems_per_chunk = convert_to_int(argv[2], "elems_per_chunk");
    size_t elem_size = (size_t)convert_to_int(argv[3], "elem_size");
    int ins_del_ratio = convert_to_int(argv[4], "ins_del_ratio");
    int read_write_ratio = convert_to_int(argv[5], "read_write_ratio");

    if (ins_del_ratio + read_write_ratio != 100)
    {
        fprintf(stderr, "The sum of both ratios has to be equal to 100.\n");
        exit(-1);
    }

    init_unrolled_linked_list(number_of_chunks, elems_per_chunk, elem_size);

    char *decision_array = (char *)malloc(DECISION_ARRAY_SIZE * sizeof(char));
    init_decision_array(decision_array, read_write_ratio);
    shuffle(decision_array);

    run_benchmark(elem_size, decision_array, number_of_chunks, elems_per_chunk);

    free(decision_array);
    clean_up(number_of_chunks);

//    srand(time(NULL));
//    char shuffle_array[ARRAY_SIZE];
//    size_t read_write_operations = ARRAY_SIZE * reads_writes / 100;

//    init_decision_array(shuffle_array, read_write_operations);
//    shuffle(shuffle_array);
//    run_benchmark(number_elements, element_size, shuffle_array);
#endif
    return EXIT_SUCCESS;
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

void *read(int block_index, int elem_index, size_t eleme_size)
{
    LinkedBlock *current_block = blockHead;
    for (int i = 0; i < block_index; i++)
    {
        current_block = current_block->next_block;
    }

    if (current_block == NULL)
    {
        fprintf(stderr, "Read: Block index out of bounds.\n");
        exit(-1);
    }

    Node *current = current_block->head;
    void *value = malloc(eleme_size);
    if (value == NULL)
    {
        fprintf(stderr, "Read: Failed to allocate memory for value.\n");
        exit(-1);
    }
    memcpy(value, (char *)current->value + elem_index * eleme_size, eleme_size);
    return value;
}

void write(int block_index, int elem_index, void *value, size_t eleme_size)
{
    LinkedBlock *current_block = blockHead;
    for (int i = 0; i < block_index; i++)
    {
        current_block = current_block->next_block;
    }

    if (current_block == NULL)
    {
        fprintf(stderr, "Write: Block index out of bounds.\n");
        exit(-1);
    }

    memcpy((char *)current_block->head->value + elem_index * eleme_size, value, eleme_size);
}

void insert(int block_index, int elem_index, void *value, size_t eleme_size, size_t max_elem_per_chunk)
{
    LinkedBlock *current_block = blockHead;
    for (int i = 0; i < block_index; i++)
    {
        current_block = current_block->next_block;
    }

    if (current_block == NULL)
    {
        fprintf(stderr, "insert: Block index out of bounds.\n");
        exit(-1);
    }

    if (elem_index > current_block->max_elem_per_chunk)
    {
        fprintf(stderr, "insert: Element index out of bounds for block %d and element %d.\n", block_index, elem_index);
        exit(-1);
    }

    if (current_block->count == current_block->max_elem_per_chunk)
    {
        // Create a new block and move half of the elements in the current block to the new block
        LinkedBlock *new_block = (LinkedBlock *)malloc(sizeof(LinkedBlock));
        if (new_block == NULL)
        {
            fprintf(stderr, "insert: Failed to allocate memory for new_block.\n");
            exit(-1);
        }
        new_block->head = NULL;
        new_block->count = current_block->count;
        new_block->max_elem_per_chunk = max_elem_per_chunk * 2;

        init_block(new_block, max_elem_per_chunk * 2, eleme_size);

        // copy the old values to the new block values
        for (int i = 0; i < new_block->count; i++)
        {
            memcpy((char *)new_block->head->value + i * eleme_size,
                   (char *)current_block->head->value + i * eleme_size,
                   eleme_size);
        }
        // rest of the values are initialized to 0
        for (int i = new_block->count; i < new_block->max_elem_per_chunk; i++)
        {
            int zero = 0;
            memcpy((char *)new_block->head->value + i * eleme_size, &zero, eleme_size);
        }

        if (block_index == 0)
        {
            new_block->next_block = current_block->next_block;
            free(current_block->head->value);
            current_block->head->value = NULL; // Ensure the pointer is set to NULL after freeing
            free(current_block);
            current_block = new_block;
            blockHead = new_block;
        }
        else
        {
            current_block->next_block = new_block;
            free(current_block->head->value);
            current_block->head->value = NULL; // Ensure the pointer is set to NULL after freeing
            free(current_block);
            current_block = new_block;
        }
    }
    // if array is not full, than replace the value at the index
    if (current_block->count < current_block->max_elem_per_chunk)
    {
        // Insert the new value
        memcpy((char *)current_block->head->value + elem_index * eleme_size, value, eleme_size);
        current_block->count++;
        return;
    }
    // Shift elements to make space for the new element
    memmove((char *)current_block->head->value + (elem_index + 1) * eleme_size,
            (char *)current_block->head->value + elem_index * eleme_size,
            (current_block->count - elem_index) * eleme_size);

    // Insert the new value
    memcpy((char *)current_block->head->value + elem_index * eleme_size, value, eleme_size);
    current_block->count++;
}

void delete(int block_index, int elem_index, size_t eleme_size)
{
    LinkedBlock *current_block = blockHead;
    for (int i = 0; i < block_index; i++)
    {
        current_block = current_block->next_block;
        if (current_block == NULL)
        {
            fprintf(stderr, "delete: Block index out of bounds.\n");
            exit(-1);
        }
    }

    if (current_block == NULL)
    {
        fprintf(stderr, "delete: Block index out of bounds.\n");
        exit(-1);
    }

    if (elem_index > current_block->max_elem_per_chunk || elem_index > current_block->count)
    {
        fprintf(stderr, "delete: Element index out of bounds for block %d and element %d.\n", block_index, elem_index);
        exit(-1);
    }

    memmove((char *)current_block->head->value + elem_index * eleme_size,
            (char *)current_block->head->value + (elem_index + 1) * eleme_size,
            (current_block->count - elem_index - 1) * eleme_size);
    current_block->count--;
}

void run_benchmark(int elem_size, const char *decision_array, int number_chunks, int elems_per_chunk)
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
        for (int i = 0; i < number_chunks; i++)
        {
            for (int j = 0; j < elems_per_chunk; j++)
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
                    printf("Entered read\n");
                    void *read_val = read(i, j, elem_size);
                    printf("Finished read\n");
                    free(read_val);
                    read_op++;
                    r_w_operations++;
                }
                else if (decision_array[j % DECISION_ARRAY_SIZE] == 'W')
                {
                    int value = i + j;
                    printf("Entered write\n");
                    write(i, j, &value, elem_size);
                    printf("Finished write\n");
                    write_op++;
                    r_w_operations++;
                }
                else if (decision_array[j % DECISION_ARRAY_SIZE] == 'I')
                {
                    int value = i * 2 + j;
                    printf("Entered insert\n");
                    insert(i, j, &value, elem_size, elems_per_chunk);
                    printf("Finished insert\n");
                    insert_op++;
                    ins_del_operations++;
                }
                else if (decision_array[j % DECISION_ARRAY_SIZE] == 'D')
                {
                    printf("Entered delete\n");
                    delete (i, j, elem_size);
                    printf("Finished delete\n");
                    delete_op++;
                    ins_del_operations++;
                }
                operations++;
            }
            // print actual time
            fprintf(stdout, "Time passed: %ld\n", (time(NULL) - start));
        }
    }
}

void init_block(LinkedBlock *block, size_t max_elem_per_chunk, size_t elem_size)
{
    // create an array inside the Node value
    Node *new = (Node *)malloc(sizeof(Node));
    new->value = malloc(elem_size * max_elem_per_chunk);
    if (new->value == NULL)
    {
        fprintf(stderr, "Error: Failed to allocate memory for new->value\n");
        exit(-1);
    }

    for (size_t i = 0; i < max_elem_per_chunk; i++)
    {
        memcpy((char *)new->value + i *elem_size, &i, elem_size);
    }

    new->next_node = NULL;
    if (block->head == NULL)
    {
        block->head = new;
    }
    else
    {
        Node *current = block->head;
        while (current->next_node != NULL)
        {
            current = current->next_node;
        }
        current->next_node = new;
    }
}

void init_unrolled_linked_list(size_t number_chunks, size_t max_elem_per_chunk, size_t elem_size)
{
    LinkedBlock *current_block = NULL;
    for (size_t i = 0; i < number_chunks; i++)
    {
        LinkedBlock *new_block = (LinkedBlock *)malloc(sizeof(LinkedBlock));

        if (new_block == NULL)
        {
            fprintf(stderr, "Error: Failed to allocate memory for LinkedBlock\n");
            exit(-1);
        }

        new_block->head = NULL;
        new_block->count = 0;
        new_block->next_block = NULL;
        new_block->max_elem_per_chunk = max_elem_per_chunk;

        init_block(new_block, max_elem_per_chunk, elem_size);

        if (i == 0)
        {
            current_block = new_block;
            blockHead = current_block;
        }
        else
        {
            current_block->next_block = new_block;
            current_block = new_block;
        }
    }
}

void clean_up()
{
    LinkedBlock *current_block = blockHead;
    while (current_block != NULL)
    {
        Node *current_node = current_block->head;
        while (current_node != NULL)
        {
            Node *next_node = current_node->next_node;
            free(current_node->value);
            free(current_node);
            current_node = next_node;
        }
        LinkedBlock *next_block = current_block->next_block;
        free(current_block);
        current_block = next_block;
    }
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

void print_block_as_linked_list(int block_index, size_t max_elem_per_chunk)
{
    LinkedBlock *current_block = blockHead;
    for (int i = 0; i < block_index; i++)
    {
        current_block = current_block->next_block;
    }

    if (current_block == NULL)
    {
        printf("Block index out of bounds.\n");
        return;
    }

    Node *current_node = current_block->head;
    while (current_node != NULL)
    {
        for (size_t i = 0; i < max_elem_per_chunk; i++)
        {
            printf("%d -> ", *(int *)((char *)current_node->value + i * sizeof(int)));
        }
        current_node = current_node->next_node;
    }
    printf("NULL\n");
}

void print_linked_list()
{
    LinkedBlock *current_block = blockHead;
    int block_index = 0;

    while (current_block != NULL)
    {
        printf("Block %d: ", block_index);
        print_block_as_linked_list(block_index, current_block->count);
        printf("\n");
        current_block = current_block->next_block;
        block_index++;
    }
}

void run_example()
{
    int number_of_chunks = 3;
    int elems_per_chunk = 5;
    size_t elem_size = sizeof(int);
    init_unrolled_linked_list(number_of_chunks, elems_per_chunk, elem_size);

    // Test 1: Write values to the list
    for (int i = 0; i < number_of_chunks; i++)
    {
        for (int j = 0; j < elems_per_chunk; j++)
        {
            int value = i * 10 + j;
            write(i, j, &value, elem_size);
        }
    }
    print_linked_list();

    // Test 2: Read and print values from the list
    for (int i = 0; i < number_of_chunks; i++)
    {
        for (int j = 0; j < elems_per_chunk; j++)
        {
            int *read_value = (int *)read(i, j, elem_size);
            int expected_value = i * 10 + j;
            assert(*read_value == expected_value && "Value read does not match expected value.");
            printf("Read from block %d, index %d: %d\n", i, j, *read_value);
            free(read_value);
        }
    }
    print_linked_list();
    // Test 3: Insert value into the middle of the first block
    int insert_value = 99;
    insert(0, 0, &insert_value, elem_size, elems_per_chunk);
    printf("After insertion in block 0 at index 2:\n");
    print_linked_list();

    // Test 4: Delete value from the middle of the first block
    delete (0, 0, elem_size);
    printf("After deletion in block 0 at index 2:\n");
    print_linked_list();

    // // Edge Case 1: Try to read outside the boundaries
    // printf("Attempting to read outside boundaries:\n");
    // void *read_value = read(0, 100, elem_size); // Should print an error
    // assert(read_value == NULL && "Attempted to read outside boundaries.");
    // free(read_value);
    // // Edge Case 2: Try to write outside the boundaries
    // printf("Attempting to write outside boundaries:\n");
    // int write_value = 100;
    // write(0, elems_per_chunk, &write_value, elem_size); // Should print an error

    // // Edge Case 3: Try to insert outside the boundaries
    // printf("Attempting to insert outside boundaries:\n");
    // insert(0, elems_per_chunk + 1, &insert_value, elem_size, elems_per_chunk); // Should print an error

    // // Edge Case 4: Try to delete outside the boundaries
    // printf("Attempting to delete outside boundaries:\n");
    // delete (0, elems_per_chunk, elem_size); // Should print an error
    printf("Cleaning up...\n");
    clean_up();
}