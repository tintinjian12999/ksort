#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <time.h>
#include <unistd.h>
#define QSORT 0
#define LIBSORT 1
#define TIMSORT 2
#define PDQSORT 3
#define RAND 0
#define SAWTOOTH 1
#define STAGGER 2
#define SHUFFLE 3
#define PLATEAU 4
#define KSORT_DEV "/dev/sort"

int main(int argc, char **argv)
{
    size_t sort_method = QSORT;
    size_t data_set = RAND;
    if (argc > 1) {
        if (strcmp(argv[1], "qsort") == 0) {
            printf("sort by qsort \n");
            sort_method = QSORT;
        } else if (strcmp(argv[1], "libsort") == 0) {
            printf("sort by libsort \n");
            sort_method = LIBSORT;
        } else {
            fprintf(stderr,
                    "Invalid sort method. Use ./user [qsort|libsort]\n");
            return -1;
        }
    } else {
        printf("sort by qsort \n");
    }

    if (argc > 2) {
        if (strcmp(argv[2], "rand") == 0) {
            printf("Dataset: RAND \n");
            data_set = RAND;
        } else if (strcmp(argv[2], "sawtooth") == 0) {
            printf("Dataset: SAWTOOTH \n");
            data_set = SAWTOOTH;
        } else if (strcmp(argv[2], "stagger") == 0) {
            printf("Dataset: STAGGER \n");
        } else if (strcmp(argv[2], "shuffle") == 0) {
            printf("Dataset: SHUFFLE \n");
            data_set = SHUFFLE;
        } else if (strcmp(argv[2], "plateau") == 0) {
            printf("Dataset: PLATEAU \n");
            data_set = PLATEAU;
        } else {
            fprintf(stderr, "Invalid data set.\n");
            return -1;
        }
    } else {
        printf("Dataset: RAND \n");
    }
    int fd = open(KSORT_DEV, O_RDWR);
    if (fd < 0) {
        perror("Failed to open character device");
        goto error;
    }
    struct timespec tt1, tt2;
    size_t n_elements = 21000;
    size_t begin = 1000;

    FILE *time_f, *cmp_cnt_f, *data_f;
    time_f = fopen("time.txt", "w");
    data_f = fopen("data_dis.txt", "w");
    cmp_cnt_f = fopen("cmp_cnt.txt", "w");
    ioctl(fd, sort_method);

    for (size_t n = begin; n < n_elements + 1; n = n + 2) {
        size_t size = n * sizeof(int);

        int *inbuf = malloc(size);

        if (!inbuf) {
            printf("error while allocating inbuf \n");
            goto error;
        }
        size_t j = 0, k = 1;
        for (size_t i = 0; i < n; i++) {
            switch (data_set) {
            case RAND:
                inbuf[i] = rand() % n;
                break;
            case SAWTOOTH:
                inbuf[i] = i % 5;
                break;
            case STAGGER:
                inbuf[i] = (i * 100 + i) % n;
                break;
            case SHUFFLE:
                inbuf[i] = (rand() % 2) ? (j += 2) : (k += 2);
                break;
            case PLATEAU:
                inbuf[i] = ((i * 100 + i) % n < 500) ? 0 : 500;
                break;
            }
            if (n == begin)
                fprintf(data_f, "%zu %d \n", i, inbuf[i]);
        }

        clock_gettime(CLOCK_REALTIME, &tt1);
        ssize_t r_sz = read(fd, inbuf, size);
        clock_gettime(CLOCK_REALTIME, &tt2);

        ssize_t com_cnt = 0;
        const char *msg = "Compute comp time";
        com_cnt = write(fd, msg, strlen(msg));
        // printf("Use %ld times comparison \n", com_cnt);
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
        fprintf(cmp_cnt_f, "%zu %ld  \n", n, com_cnt);

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
