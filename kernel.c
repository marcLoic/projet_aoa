#include <math.h> // exp
#include <stdio.h>

void baseline(unsigned n, float x[n][n],
              const float y[n][n], double c)

{
    unsigned i, j;
    for (j = 1; j < n - 1; j++)
        for (i = 1; i < n - 1; i++)
            x[i][j] = (y[i - 1][j + 0] +
                       y[i + 1][j + 0] +
                       y[i + 0][j - 1] +
                       y[i + 0][j + 1] +
                       y[i + 0][j + 0]) /
                      sqrt(c);
}
