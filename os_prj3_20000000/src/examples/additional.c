#include <stdio.h>
#include <stdlib.h>
#include <syscall.h>

int main(int argc, char **argv) {
    if (argc != 5) {
        printf("Usage : additional [num 1] [num 2] [num 3] [num 4]\n");
        return EXIT_FAILURE;
    }
    int fibo_result = fibonacci(atoi(argv[1]));
    int max_result = max_of_four_int(atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]));

    printf("%d %d\n", fibo_result, max_result);

    return EXIT_SUCCESS;
}
