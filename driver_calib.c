#include <math.h> // exp
#include <stdint.h>
#include "stdio.h"
#include <stdlib.h>
#include <time.h>
// #include "kernel.c"
#include <unistd.h>
#include <inttypes.h>

#define NB_METAS 31

extern uint64_t rdtsc();

extern void baseline(unsigned n, float x[n][n],
                     const float y[n][n], double c);

// Initialisation of the two dimensional array
void init_array(int n, float a[n][n])
{
    int i, j;

    for (i = 0; i < n; i++)
        for (j = 0; j < n; j++)
            a[i][j] = (float)rand() / RAND_MAX;
}

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
    if (argc != 4)
    {
        fprintf(stderr, "Usage: %s <size>  <nb measure repets> \n", argv[0]);
        abort();
    }

    unsigned i, m;

    int size = atoi(argv[1]);        /* matrix size */
    int repm = atoi(argv[2]);        /* repetition number */
    int square_root = atoi(argv[3]); /* square root number */

    uint64_t tdiff[repm][NB_METAS];

    for (m = 0; m < NB_METAS; m++)
    {
        // printf("Metarepetition %u/%d: running %u warmup instances and %u measure instance\n", m + 1, NB_METAS, m == 0 ? repw : 1, repm);

        /* allocate arrays */
        float(*x)[size] = malloc(size * size * sizeof x[0][0]);
        float(*y)[size] = malloc(size * size * sizeof y[0][0]);

        /* init arrays */
        srand(0);
        init_array(size, x);
        init_array(size, y);

        /* measure repm repetitions */
        uint64_t t1, t2;

        /* kernel call */
        for (i = 0; i < repm; i++)
        {
            /* measure repm repetitions */
            t1 = rdtsc();

            baseline(size, x, y, square_root);

            t2 = rdtsc();

            tdiff[i][m] = t2 - t1;
        }
        /* free arrays */
        free(x);
        free(y);
        sleep(3);
    }

    FILE *fp = fopen("repm_en_fonction_med.csv", "w");
    if (fp == NULL)
    {
        fprintf(stderr, "Cannot open\n");
    }

    fprintf(fp, "repm,mediane\n");

    for (int i = 0; i < repm; i++) {

        qsort(tdiff[i], NB_METAS, sizeof(tdiff[i][0]), cmp_uint64);
        
        printf("MIN %lu RDTSC-cycles\n", tdiff[i][0]);
        printf("MED %lu RDTSC-cycles\n", tdiff[i][NB_METAS / 2]);

        const float stab = (tdiff[i][NB_METAS / 2] - tdiff[i][0]) * 100.0f / tdiff[i][0];

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

        fprintf(fp, "%d,%lu\n", i, tdiff[i][NB_METAS / 2]);
    }

    fclose(fp);

    return EXIT_SUCCESS;
}