/*******************************************************************************
*                                                                              *
*  Maria Esther Carrillo                                                       *
*  10-10122                                                                    *
*                  TTTTT  H  H  RRR    EEEE   AA   DDD    SS                   *
*                    T    H  H  R  R   E     A  A  D  D  S                     *
*                    T    HHHH  RRR    EEE   AAAA  D  D   SS                   *
*                    T    H  H  R  R   E     A  A  D  D     S                  *
*                    T    H  H  R   R  EEEE  A  A  DDD    SS                   *
*                                                                              *
*******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <math.h>
#include <time.h>

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
argumentos leerArgumentos(int argc, char *argv[]);

int main(int argc, char *argv[]) {
    argumentos args;
    FILE *archivo_entrada, *archivo_salida;
    size_t leidos;
    int i;

    // leemos los argumentos
    args = leerArgumentos(argc, argv);

    // pedimos memoria para el arreglo
    arreglo = (int*) malloc(sizeof(int) * args.enteros);
    if (arreglo == NULL) {              // error
        printf("memoria insuficiente\n");
        exit(-1);
    }

    // abrimos archivo binario
    archivo_entrada = fopen(args.entrada, "rb");
    leidos = fread(arreglo, sizeof(int), args.enteros, archivo_entrada);

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

    orden_rapido(arreglo, inicio, inicio + cuantos - 1);

    return NULL;
}


void *intermedio(void* ptr) {

    time_t start, end;
    double tiempo_ejecucion;

    start = time(NULL);

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
    if (nivel == 1) {           // el siguiente nivel es hoja
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
    } else {                    // el siguiente nivel es intermedio
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
    int arreglo_temp[cuantos];
    int fin_izq, fin_der;

    fin_izq = inicio_izq + cuantos_izq;
    fin_der = inicio_der + cuantos_der;

    for (i = 0; i < cuantos; ++i) {
        if (inicio_izq >= fin_izq) {
            arreglo_temp[i] = arreglo[inicio_der];
            inicio_der++;
        } else if (inicio_der >= fin_der) {
            arreglo_temp[i] = arreglo[inicio_izq];
            inicio_izq++;
        } else if (arreglo[inicio_izq] <= arreglo[inicio_der]) {
            arreglo_temp[i] = arreglo[inicio_izq];
            inicio_izq++;
        } else {
            arreglo_temp[i] = arreglo[inicio_der];
            inicio_der++;
        }
    }

    // y lo copiamos de vuelta
    for (i = 0; i < cuantos; ++i) {
        arreglo[inicio + i] = arreglo_temp[i];
    }

    end = time(NULL);
    tiempo_ejecucion = difftime(end, start);

    printf("tiempo de ejecución de hilo con inicio = %d y cuantos = %d: %lf\n", inicio, cuantos, tiempo_ejecucion);

    return NULL;
}


/************* lecutra de argumentos *************/

argumentos leerArgumentos(int argc, char *argv[]) {
    // enteros niveles in out
    argumentos args;
    int hojas;

    if (argc < 5) {
        printf("usage: ./ordenArchivo-t NumEnteros NumNiveles ArchivoEnterosDesordenado ArchivoEnterosOrdenado\n");
        exit(-1);
    }

    args.enteros = atoi(argv[1]);
    args.niveles = atoi(argv[2]);
    args.entrada = argv[3];
    args.salida  = argv[4];

    if (args.enteros < 1) {
        printf("NumEnteros debe ser un entero positivo\n");
        exit(-1);
    }
    if (args.niveles < 1) {
        printf("NumNiveles debe ser un entero positivo\n");
        exit(-1);
    }

    hojas = pow(2, args.niveles - 1);

    if (args.enteros <= hojas) {
        printf("NumEnteros debe ser un mayor a número de hojas\n");
        exit(-1);
    }

    return args;
}


/************ quick-sort **************/

int particion(int arr[], int primero, int ultimo) {
   int izq, der, temp;
   int piv = arr[primero];
   izq = primero;
   der = ultimo;

   while (arr[izq] <= piv && izq < ultimo) izq++;
   while (arr[der] > piv  && der > primero) der--;

   while (der > izq) {
       temp = arr[izq];
       arr[izq] = arr[der];
       arr[der] = temp;
       while (arr[izq] <= piv && izq < ultimo) izq++;
       while (arr[der] > piv  && der > primero) der--;
   }

   arr[primero] = arr[der];
   arr[der] = piv;
   return der;
}

void orden_rapido(int arr[], int primero, int ultimo) {
   int indice_piv;
   if(primero < ultimo) {
       indice_piv = particion(arr, primero, ultimo);
       orden_rapido(arr, primero, (indice_piv-1));
       orden_rapido(arr, (indice_piv+1), ultimo);
   }
}

// [0,1,2,4,4,5,6,7,9]
//  0,1,2,3,4,5,6,7,8
//
// or(0,8){
//     or(0,0);
//     or(2,8){
//         or(2,6){
//             or(2,4){
//                 or(2,3){
//                     or(2,2);
//                     or(4,3);
//                 }
//                 or(5,4);
//             }
//             or(6,6);
//         }
//         or(8,8);
//     }
// }
