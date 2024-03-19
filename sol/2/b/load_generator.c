#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>

#define FILE_SIZE 1024 // Size of each file in bytes
#define MAX_FILES 100 // Maximum number of files to create

void generate_load(int num_files, int num_iterations) {
    printf("Generating files");
    char filename[20];
    char buffer[FILE_SIZE];
    int i, j;
    int fd;

    // Seed the random number generator
    srand(time(NULL));

    for (i = 0; i < FILE_SIZE; i++) {
        // Fill the buffer with random data
        buffer[i] = rand() % 256; // Generate a random byte (0-255)
    }

    for (i = 0; i < num_iterations; i++) {
        for (j = 0; j < num_files; j++) {
            // Generate a random filename
            sprintf(filename, "file_%d_%d.txt", i, j);

            // Open the file for writing
            fd = open(filename, O_CREAT | O_WRONLY | O_TRUNC, 0644);
            if (fd == -1) {
                perror("Error opening file");
                exit(EXIT_FAILURE);
            }

            // Write random data from the buffer to the file
            write(fd, buffer, FILE_SIZE);

            // Close the file
            close(fd);

            // Delete the file
            unlink(filename);
        }
    }
}

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "Usage: %s <num_files> <num_iterations>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int num_files = atoi(argv[1]);
    int num_iterations = atoi(argv[2]);

    generate_load(num_files, num_iterations);

    return 0;
}
