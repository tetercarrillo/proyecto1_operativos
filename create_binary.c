#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    FILE *bin, *txt;
    int *array = malloc(sizeof(int) * 6000000);

    bin = fopen("in.bin", "wb");
    txt = fopen("in.txt", "w");

    int i;
    for (i = 0; i < 6000000; ++i) {
        array[i] = rand() % 1000000;
        fprintf(txt, "%d\n", array[i]);
    }

    fwrite(array, sizeof(int), 6000000, bin);
    return 0;
}
