#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#define f(x) ((x)*(x))

int main(int argc, char *argv[]){
    double a = 0.0;     // Lower limit of integration
    double b = 1.0;     // Upper limit of integration
    int n = 100000;    // Number of trapezoids
    double h = (b - a) / n; // Width of each trapezoid

    // to time
    clock_t startwtime, endwtime;
    startwtime = clock();

    // Calculate the integral
    double integral = (f(a) + f(b)) / 2.0;
    for (int i = 1; i < n; i++) {
        double x = a + i * h;
        integral += f(x);
    }
    integral *= h;

    // End timing
    endwtime = clock();

    // Print the result and timing
    printf("The integral from %.2f to %.2f is approximately %.16f\n", a, b, integral);
    printf("Wall clock time = %f seconds\n", ((double)(endwtime - startwtime)) / CLOCKS_PER_SEC);

    return 0;
}