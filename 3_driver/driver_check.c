#include <math.h> // exp
#include "stdio.h"
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include "kernel.c"

#define NB_METAS 31

extern uint64_t rdtsc();

// Initialisation of the two dimensional array
void init_array(int n, float a[n][n])
{
    int i, j;

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            a[i][j] = (float)rand() / RAND_MAX;
}

// Writing data into a file
// static void print_array(const char *output_file_name, float a[n][n], int n)
// {
//     int i, j;

//     FILE *fp = fopen(output_file_name, "w");
//     if (fp == NULL)
//     {
//         fprintf(stderr, "Cannot write to %s\n", output_file_name);
//         return;
//     }

//     for (i = 0; i < n; i++)
//         for (j = 0; j < n; j++)
//             fprintf(fp, "%f\n", a[i][j]);

//     fclose(fp);
// }

static int cmp_uint64(const void *a, const void *b)
{
    const uint64_t va = *((uint64_t *)a);
    const uint64_t vb = *((uint64_t *)b);

    if (va < vb)
        return -1;
    if (va > vb)
        return 1;

    return 0;
}

int main(int argc, char *argv[])
{
    if (argc != 5)
    {
        fprintf(stderr, "Usage: %s <size> <nb warmup repets> <nb measure repets>\n", argv[0]);
        abort();
    }

    unsigned i, m;
    struct timespec ts, te;
    uint64_t tdiff[NB_METAS];

    int size = atoi(argv[1]); /* matrix size */
    int repw = atoi(argv[2]); /* repetition number warmup */
    int repm = atoi(argv[3]); /* repetition number */
    int square_root = atoi(argv[4]); /* square root number */

    for (m = 0; m < NB_METAS; m++)
    {
        printf("Metarepetition %u/%d: running %u warmup instances and %u measure instance\n", m + 1, NB_METAS, m == 0 ? repw : 1, repm);

        /* allocate arrays */
        float(*x)[size] = malloc(size * size * sizeof x[0][0]);
        float(*y)[size] = malloc(size * size * sizeof y[0][0]);

        /* init arrays */
        srand(0);
        init_array(size, x);
        init_array(size, y);

        /* warmup (repw repetitions in first meta, 1 repet in next metas) */
        if (m == 0)
        {
            for (i = 0; i < repw; i++)
            {
                baseline(size, x, y, square_root);
            }
        }
        else
        {
            baseline(size, x, y, square_root);
        }

        /* measure repm repetitions */
        clock_gettime(CLOCK_MONOTONIC_RAW, &ts);

        for (i = 0; i < repm; i++)
        {
            baseline(size, x, y, square_root);
        }

        clock_gettime(CLOCK_MONOTONIC_RAW, &te);

        tdiff[m] = ((te.tv_sec*1000000000UL) + te.tv_nsec) - (((ts.tv_sec*1000000000UL) + ts.tv_nsec));

        /* free arrays */
        free(x);
        free(y);
    }

    printf("\n");

    for (i = 0; i < NB_METAS; i++)
    {
        printf("%llu  ", tdiff[i]);
    }

    printf("\n");

    const unsigned nb_inner_iters = size * repm;

    qsort(tdiff, NB_METAS, sizeof tdiff[0], cmp_uint64);

    printf("MIN %llu nsec (%.2f per inner-iter)\n", tdiff[0], (float)tdiff[0] / nb_inner_iters);
    printf("MED %llu nsec (%.2f per inner-iter)\n", tdiff[NB_METAS / 2], (float)tdiff[NB_METAS / 2] / nb_inner_iters);

    const float stab = (tdiff[m / 2] - tdiff[0]) * 100.0f / tdiff[0];

    if (stab >= 10)
    {
        printf("BAD STABILITY : %.2f %%\n", stab);
    }
    else if (stab >= 5)
    {
        printf("AVERAGE STABILITY : %.2f %%\n", stab);
    }
    else
    {
        printf("GOOD STABILITY : %.2f %%\n", stab);
    }

    return EXIT_SUCCESS;
}