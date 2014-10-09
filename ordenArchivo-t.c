#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>

typedef struct {
    int niveles;
    int enteros;
    char* entrada;
    char* salida;
} argumentos;


typedef struct {
    int cuantos;
    int inicio;
    int nivel;
} thread_arg;

int* arreglo;

void *intermedio(void* ptr);
void orden_rapido(int arr[], int primero, int ultimo);

int main(int argc, char const *argv[]) {
    argumentos args;
    FILE *archivo_entrada, *archivo_salida;
    size_t leidos;
    int i;

    // cable
    args.niveles = 3;
    args.enteros = 70;
    args.entrada = "in.bin";
    args.salida  = "out.txt";

    // leemos los argumentos
    // args = leerArgumentos(argc, argv);

    // pedimos memoria para el arreglo
    arreglo = (int*) malloc(sizeof(int) * args.enteros);
    if (arreglo == NULL) {              // error
        printf("memoria insuficiente\n");
        exit(-1);
    }

    // abrimos archivo binario
    archivo_entrada = fopen(args.entrada, "rb");
    leidos = fread(arreglo, sizeof(int), args.enteros, archivo_entrada);

    for (int i = 0; i < args.enteros; ++i) {
        printf("%d\n", arreglo[i]);
    }

    if (leidos < args.enteros) {
        printf("habían menos enteros de los que me prometiste :(");
        exit(-1);
    }

    thread_arg root_arg;

    root_arg.inicio  = 0;
    root_arg.cuantos = args.enteros;
    root_arg.nivel   = args.niveles;
    intermedio((void*) &root_arg);

    archivo_salida = fopen(args.salida, "w");
    for (i = 0; i < args.enteros; ++i) {
        fprintf(archivo_salida, "%d\n", arreglo[i]);
    }

}


void *hoja(void *ptr) {

    int inicio, cuantos;

    thread_arg* arg;

    arg = (thread_arg*) ptr;

    inicio  = arg->inicio;
    cuantos = arg->cuantos;

    printf("\thoja(%d,%d)\n", inicio, cuantos);

    orden_rapido(arreglo, inicio, inicio + cuantos - 1);

    return NULL;
}


void *intermedio(void* ptr) {

    thread_arg* arg;
    int inicio, cuantos, nivel;
    int i;      // para contadores

    pthread_t izquierdo, derecho;
    thread_arg arg_izq , arg_der;
    int cuantos_izq, cuantos_der;
    int inicio_izq , inicio_der;
    int chequeo;

    arg = (thread_arg*) ptr;

    inicio  = arg->inicio;
    cuantos = arg->cuantos;
    nivel   = arg->nivel;

    printf("intermedio(%d,%d,%d)\n", inicio, cuantos, nivel);

    cuantos_izq = cuantos / 2;
    cuantos_der = (cuantos / 2) + (cuantos % 2);
    inicio_izq  = inicio;
    inicio_der  = inicio_izq + cuantos_izq;


    // preparando los argumentos
    arg_izq.inicio  = inicio_izq;
    arg_izq.cuantos = cuantos_izq;
    arg_izq.nivel   = nivel - 1;

    arg_der.inicio  = inicio_der;
    arg_der.cuantos = cuantos_der;
    arg_der.nivel   = nivel - 1;

    // hilos
    if (nivel == 1) {           // el siguiente es hoja
        chequeo = pthread_create(&izquierdo, NULL, &hoja, (void*) &arg_izq);
        if (chequeo != 0) {
            printf("error en la creación de hilos\n");
            exit(-1);
        }

        chequeo = pthread_create(&derecho  , NULL, &hoja, (void*) &arg_der);
        if (chequeo != 0) {
            printf("error en la creación de hilos\n");
            exit(-1);
        }
    } else {                    // el siguiente es intermedio
        chequeo = pthread_create(&izquierdo, NULL, &intermedio, (void*) &arg_izq);
        if (chequeo != 0) {
            printf("error en la creación de hilos\n");
            exit(-1);
        }

        chequeo = pthread_create(&derecho  , NULL, &intermedio, (void*) &arg_der);
        if (chequeo != 0) {
            printf("error en la creación de hilos\n");
            exit(-1);
        }
    }

    // espero por los hjos
    chequeo = pthread_join(izquierdo, NULL);
    if (chequeo != 0) {
        printf("error en el join de hilos\n");
        exit(-1);
    }

    chequeo = pthread_join(derecho, NULL);
    if (chequeo != 0) {
        printf("error en el join de hilos\n");
        exit(-1);
    }

    // mezcla de los arreglos
    int* arreglo_temp;
    int tope_izq, tope_der;

    arreglo_temp = (int*) malloc(sizeof(int) * cuantos);
    if (arreglo_temp == NULL) {              // error
        printf("memoria insuficiente\n");
        exit(-1);
    }


    tope_izq = inicio_izq + cuantos_izq;
    tope_der = inicio_der + cuantos_der;

    for (i = 0; i < cuantos; ++i) {
        if (inicio_izq == tope_izq) {
            arreglo_temp[i] = arreglo[inicio_der];
            inicio_der++;
        }

        if (inicio_der == tope_der) {
            arreglo_temp[i] = arreglo[inicio_izq];
            inicio_izq++;
        }

        if (arreglo[inicio_izq] < arreglo[inicio_der]) {
            arreglo_temp[i] = arreglo[inicio_izq];
            inicio_izq++;
        } else {
            arreglo_temp[i] = arreglo[inicio_der];
            inicio_der++;
        }
    }

    // y lo copiamos de vuelta
    for (i = 0; i < cuantos; ++i) {
        arreglo[inicio] = arreglo_temp[i];
        inicio++;
    }

    free(arreglo_temp);

    return NULL;
}

/************ quick-sort **************/

int particion(int arr[], int primero, int ultimo) {
   int izq, der, temp;
   int piv = arr[primero];
   izq = primero;
   der = ultimo;

   while (arr[izq] <= piv && izq < ultimo) izq++;
   while (arr[der] > piv  && der > primero ) der--;

   while (der > izq) {
       temp = arr[izq];
       arr[izq] = arr[der];
       arr[der] = temp;
       while (arr[izq] <= piv && izq < ultimo) izq++;
       while (arr[der] > piv  && der > primero ) der--;
   }
   arr[primero] = arr[der];
   arr[der] = piv;
   return der;
}

void orden_rapido(int arr[], int primero, int ultimo) {
   int indice_pivote = 0;
   if(primero < ultimo) {
       indice_pivote = particion(arr, primero, ultimo);
       orden_rapido(arr, primero, (indice_pivote-1));
       orden_rapido(arr, (indice_pivote+1), ultimo);
   }
}

