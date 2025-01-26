#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <dispatch/dispatch.h>

// El puerto solo tiene espacio para una cantidad de barcos entonces,
// se establece 10 como el máximo número de barcos en el puerto
#define barcos_max 10

// Con este mutex puedo proteger las operaciones que se realicen en la lista enlazada del puerto
pthread_mutex_t mutex_puerto;
pthread_mutex_t mutex_consola;
pthread_mutex_t mutex_estados;
dispatch_semaphore_t espacios_puerto;

// El proyecto nos indica que tenemos un barco de carga llegará al mar territorial
// y a partir de ahí, inicia todo el proceso, por ello, iniciamos con
// La estructura para representar los barcos según los datos que va a enviar este

typedef struct {
    int barco_id;
    char tipo_carga[20];
    double peso_promedio;
    char destino_final[20];
} Barco;

// Como el sistema debe tomar una decisión y pueden existir muchos barcos presentes
// se de utilizar una lista enlazada donde se encolan los barcos listos para atracar y en su orden

typedef struct Node {
    Barco barco;
    struct Node *siguiente;
} Node;

// Inicio de la lista enlazada, 0 barcos en la fila
Node *lista_barcos = NULL;

// Estructura para almacenar el estado de los barcos
typedef struct {
    int barco_id;
    char estado[20];
} EstadoBarco;

EstadoBarco estados_barcos[barcos_max];

void inicializar_estados_barcos() {
    for (int i = 0; i < barcos_max; i++) {
        estados_barcos[i].barco_id = i + 1;
        strcpy(estados_barcos[i].estado, "Listo");
    }
}

// Ahora iniciamos la función para insertar un barco en la lista enlazada,
// tomando en cuenta las restricciones mencionadas en las instrucciones

void prioritario(Barco barco, int requiere_aforo) {
    Node *nodo = (Node *)malloc(sizeof(Node));
    if (nodo == NULL) {
        perror("Error al asignar memoria para el nodo");
        exit(EXIT_FAILURE);
    }
    nodo->barco = barco;
    nodo->siguiente = NULL;

    // Se bloquea el acceso a la lista
    pthread_mutex_lock(&mutex_puerto);

    if (requiere_aforo && strcmp(barco.destino_final, "Ecuador") != 0) {
        nodo->siguiente = lista_barcos;
        lista_barcos = nodo;
    } else {
        if (lista_barcos == NULL) {
            lista_barcos = nodo;
        } else {
            Node *nuevo = lista_barcos;
            while (nuevo->siguiente != NULL) {
                nuevo = nuevo->siguiente;
            }
            nuevo->siguiente = nodo;
        }
    }

    // Y ahora, se desbloquea la lista
    pthread_mutex_unlock(&mutex_puerto);
}

// Funciones para las entidades de control (SRI, SENAE, SUPERCIA)
void *entidad_control(void *arg) {
    char *entidad = (char *)arg;
    while (1) {
        pthread_mutex_lock(&mutex_consola);
        // Simular procesamiento de datos
        printf("[%s] Procesando datos de un barco...\n", entidad);
        pthread_mutex_unlock(&mutex_consola);
        // Simula el tiempo de procesamiento
        sleep(2);
    }
    return NULL;
}


int requiere_aforo(Barco barco) {
    if (strcmp(barco.tipo_carga, "convencional") == 0 && barco.peso_promedio > 50 && strcmp(barco.destino_final, "Ecuador") == 0) {
        return 1;
    }
    if (strcmp(barco.tipo_carga, "PANAMAX") == 0 && barco.peso_promedio >= 75 && (strcmp(barco.destino_final, "Europa") == 0 || strcmp(barco.destino_final, "USA") == 0)) {
        return 1;
    }
    int probabilidad = rand() % 100;
    if ((strcmp(barco.tipo_carga, "convencional") == 0 && probabilidad < 30) ||
        (strcmp(barco.tipo_carga, "PANAMAX") == 0 && probabilidad < 50)) {
        return 1;
    }
    return 0;
}

// Función para procesar un barco
void *procesar(void *arg) {
    Barco *barco = (Barco *)arg;

    pthread_mutex_lock(&mutex_consola);
    // Un mensaje para indicar con que barco se esta trabajando
    printf("Procesando el Barco %d...\n", barco->barco_id);
    printf("Datos del Barco:\n");
    printf("\tTipo de Carga: %s\n", barco->tipo_carga);
    printf("\tPeso Promedio: %.2f toneladas\n", barco->peso_promedio);
    printf("\tDestino Final: %s\n", barco->destino_final);
    pthread_mutex_unlock(&mutex_consola);

    int aforo = requiere_aforo(*barco);
    prioritario(*barco, aforo);

    pthread_mutex_lock(&mutex_consola);
    printf("Barco %d: Listo.\n\n", barco->barco_id);
    pthread_mutex_unlock(&mutex_consola);

    dispatch_semaphore_signal(espacios_puerto);
    return NULL;
}

// Función principal del administrador del puerto
void *administrador_puerto(void *arg) {
    while (1) {
        dispatch_semaphore_wait(espacios_puerto, DISPATCH_TIME_FOREVER);
        pthread_mutex_lock(&mutex_puerto);
        if (lista_barcos != NULL) {
            Node *b = lista_barcos;
            lista_barcos = lista_barcos->siguiente;

            pthread_mutex_lock(&mutex_consola);
            printf("Barco %d: Atracando.\n", b->barco.barco_id);
            pthread_mutex_unlock(&mutex_consola);

            // Actualizar el estado del barco
            pthread_mutex_lock(&mutex_estados);
            snprintf(estados_barcos[b->barco.barco_id - 1].estado, sizeof(estados_barcos[b->barco.barco_id - 1].estado), "Atracando");
            pthread_mutex_unlock(&mutex_estados);

            free(b);
        }
        pthread_mutex_unlock(&mutex_puerto);
        // Para simular el tiempo de atraco
        sleep(2);
    }
    return NULL;
}


int main() {
    srand(time(NULL));

    // Primero, debemos inicializar
    pthread_mutex_init(&mutex_puerto, NULL);
    pthread_mutex_init(&mutex_consola, NULL);
    pthread_mutex_init(&mutex_estados, NULL);
    espacios_puerto = dispatch_semaphore_create(barcos_max);

    inicializar_estados_barcos();
    
    pthread_t admin_thread;
    pthread_t ship_threads[barcos_max];
    pthread_t entidades[3];

    // Creamos los hilos de las entidades de control
    pthread_create(&entidades[0], NULL, entidad_control, "SRI");
    pthread_create(&entidades[1], NULL, entidad_control, "SENAE");
    pthread_create(&entidades[2], NULL, entidad_control, "SUPERCIA");

    // El hilo del administrador del puerto
    pthread_create(&admin_thread, NULL, administrador_puerto, NULL);

    // Y los hilos para barcos
    for (int i = 0; i < barcos_max; i++) {
        Barco *barco = (Barco *)malloc(sizeof(Barco));
        if (barco == NULL) {
            perror("Error al asignar memoria para el barco");
            exit(EXIT_FAILURE);
        }
        barco->barco_id = i + 1;
        snprintf(barco->tipo_carga, sizeof(barco->tipo_carga), (rand() % 2 == 0) ? "convencional" : "PANAMAX");
        barco->peso_promedio = (rand() % 100) + 1;
        snprintf(barco->destino_final, sizeof(barco->destino_final), (rand() % 2 == 0) ? "Ecuador" : "Europa");

        pthread_create(&ship_threads[i], NULL, procesar, barco);

        // Para simular el tiempo entre llegadas
        sleep(1);
    }

    // Esperamos a que terminen los hilos de los barcos
    for (int i = 0; i < barcos_max; i++) {
        pthread_join(ship_threads[i], NULL);
    }

    pthread_cancel(admin_thread);
    for (int i = 0; i < 3; i++) {
        pthread_cancel(entidades[i]);
    }

    printf("\nResumen Final de los Barcos:\n");
    pthread_mutex_lock(&mutex_estados);
    for (int i = 0; i < barcos_max; i++) {
        printf("Barco %d: %s\n", estados_barcos[i].barco_id, estados_barcos[i].estado);
    }
    pthread_mutex_unlock(&mutex_estados);

    // Se liberan los recursos
    pthread_mutex_destroy(&mutex_puerto);
    pthread_mutex_destroy(&mutex_consola);
    pthread_mutex_destroy(&mutex_estados);
    return 0;

    //Fin
}