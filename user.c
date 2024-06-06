#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define KSORT_DEV "/dev/sort"

int main()
{
    int fd = open(KSORT_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        goto error;
    }

    size_t n_elements = 100;
    FILE *time_f;
    time_f = fopen("time.txt", "w");
    for (size_t n = 1; n < n_elements + 1; n++) {
        size_t size = n * sizeof(int);
        int *inbuf = malloc(size);

        if (!inbuf)
            goto error;

        for (size_t i = 0; i < n; i++)
            inbuf[i] = rand() % n;

        ssize_t r_sz = read(fd, inbuf, size);

        bool pass = true;
        /* Verify the result of sorting */
        for (size_t i = 1; i < n; i++) {
            if (inbuf[i] < inbuf[i - 1]) {
                pass = false;
                break;
            }
        }


        if (time_f == NULL) {
            printf("Failed to open file \n");
            goto error;
        }

        fprintf(time_f, "%zu %ld \n", n, r_sz);

        if (!pass)
            printf("Sorting failed!\n");
    error:
        free(inbuf);
    }
    fclose(time_f);
    if (fd > 0)
        close(fd);
    return 0;
}
