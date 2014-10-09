#include <stdio.h>
#include <stdlib.h>

int main(int argc, char const *argv[])
{
    FILE *bin, *txt;
    int *array = malloc(sizeof(int) * 101);

    bin = fopen("in.bin", "wb");
    txt = fopen("in.txt", "w");

    int i;
    for (i = 0; i < 101; ++i) {
        array[i] = rand() % 200;
        fprintf(txt, "%d\n", array[i]);
    }

    fwrite(array, sizeof(int), 101, bin);
    return 0;
}
