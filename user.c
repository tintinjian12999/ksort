#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#define KSORT_DEV "/dev/sort"

int main()
{
    int fd = open(KSORT_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        goto error;
    }
    struct timespec tt1, tt2;
    size_t n_elements = 20000;
    FILE *time_f;
    time_f = fopen("time.txt", "w");
    for (size_t n = 1000; n < n_elements + 1; n = n + 1000) {
        size_t size = n * sizeof(int);
        int *inbuf = malloc(size);

        if (!inbuf) {
            printf("error while allocating inbuf");
            goto error;
        }

        for (size_t i = 0; i < n; i++)
            inbuf[i] = rand() % n;

        clock_gettime(CLOCK_REALTIME, &tt1);
        ssize_t r_sz = read(fd, inbuf, size);
        clock_gettime(CLOCK_REALTIME, &tt2);

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

        ssize_t tt1_ns = tt1.tv_sec * 1000000000LL + tt1.tv_nsec;
        ssize_t tt2_ns = tt2.tv_sec * 1000000000LL + tt2.tv_nsec;
        fprintf(time_f, "%zu %ld %ld \n", n, r_sz, tt2_ns - tt1_ns);

        if (pass)
            printf("Sorting pass!\n");
    error:
        free(inbuf);
    }
    fclose(time_f);
    if (fd > 0)
        close(fd);
    return 0;
}
