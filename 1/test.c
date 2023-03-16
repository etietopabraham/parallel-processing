#include <stdio.h>
#include <omp.h>

int main() {
    int i, sum = 0;
    int array_size = 100;
    int array[array_size];

    // Initialize the array with values
    for (i = 0; i < array_size; i++) {
        array[i] = i + 1;
    }

    // Calculate the sum using OpenMP
    #pragma omp parallel for reduction(+:sum)
    for (i = 0; i < array_size; i++) {
        sum += array[i];
    }

    printf("Sum of the array elements: %d\n", sum);
    return 0;
}
