#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <inttypes.h>

typedef struct {
    int64_t repeats;
    int64_t iterations;
    int64_t lower, upper;
} thread_args;

typedef struct {
    void* arena; // head
    size_t arena_size;
    void* next; 
} bump_allocator;

void* bump_alloc(bump_allocator* allocator, size_t size) {
	// Check if there is enough space left in the arena (in bytes)
    if ((char*)allocator->next + size > (char*)allocator->arena + allocator->arena_size) {
        return NULL;  // Out of memory
    }
    
    void* allocated = allocator->next;
    allocator->next = (char*)allocator->next + size;
    return allocated;
}

void init_bump_allocator(bump_allocator* allocator, size_t size) {
    allocator->arena = malloc(size);
    if (allocator->arena == NULL) {
        fprintf(stderr, "Failed to allocate memory for arena\n");
        exit(EXIT_FAILURE);
    }
    allocator->arena_size = size;
    allocator->next = allocator->arena; // Start with the whole arena free
}

void* benchmark_thread(void *args) {
    thread_args *t_args = (thread_args*)args;

    // Initialize bump allocator with enough space for the largest possible size (2 loops)
    bump_allocator allocator;
    init_bump_allocator(&allocator, t_args->iterations * t_args->upper);

    for (int64_t r = 0; r < t_args->repeats; ++r) {
        unsigned seed = 0; 

        // Allocate an array to hold the pointers to each allocation
        void **allocations = (void**)bump_alloc(&allocator, t_args->iterations * sizeof(void*));

        for (int64_t i = 0; i < t_args->iterations; ++i) {
            int64_t to_alloc = rand_r(&seed) % (t_args->upper - t_args->lower) + t_args->lower;
            allocations[i] = bump_alloc(&allocator, to_alloc);
            if (allocations[i] == NULL) {
                fprintf(stderr, "Allocation failed\n");
                exit(EXIT_FAILURE);
            }
        }
        allocator.next = allocator.arena;
    }

    free(allocator.arena);
    return NULL;
}

int main(int argc, char** argv) {
	int64_t num_threads = 100;
    if (argc != 6) {
        printf("USAGE: ./malloctest_custom [num_threads] [num_repeats] [num_iterations] [lower] [upper]\n");
        return -1;
    }

    num_threads = atol(argv[1]);
    thread_args t_args;
    t_args.repeats = atol(argv[2]);
    t_args.iterations = atol(argv[3]);
    t_args.lower = atol(argv[4]);
    t_args.upper = atol(argv[5]);

    pthread_t* threads = (pthread_t*)calloc(num_threads, sizeof(pthread_t));
  
    for (int64_t i = 0; i < num_threads; ++i) {
        pthread_create(&threads[i], NULL, benchmark_thread, &t_args);
    }

    for (int64_t i = 0; i < num_threads; ++i) {
        pthread_join(threads[i], NULL);
    }

    free(threads);

    return 0;
}
