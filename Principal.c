#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

// STRUCT =======================================
typedef struct
{
    char id[50];
    char titulo[150];
    char desarrollador[100];
    List *categorias; // Lista de strings con los tags
    float precio;
    int cantidad_dlc;
    int metacritic;
    int recomendaciones;
    float puntuacion_popularidad;
    char nivel_calificacion[50];
    int es_multijugador;
    char plataformas[200];
    char fecha_lanzamiento[50];

    // Lista de Adyacencia
    List *juegos_similares; // Guardará elementos de tipo 'Arista'
} Videojuego;

// Estructura para las conexiones del grafo
typedef struct 
{
    Videojuego *juego_vecino; // Puntero al nodo conectado
    int peso_similitud;       // Umbral
} Arista;
// STRUCT =======================================

// PROTOTIPOS =======================================
int is_equal_str(void *, void *);
char * capitalizar(char *);
void mostrar_menu_principal();
void mostrar_juego(Videojuego *);
void mostrar_lista_juegos(List *);
void cargar_juegos(Map *);
void liberar_memoria(Map *);
// PROTOTIPOS =======================================

// MAIN =======================================
int main()
{
    char opcion;
    int contador_carga = 0;

    // Clave: ID - Valor: Struct Videojuego
    Map *grafo_juegos = map_create(is_equal_str);
    cargar_juegos(grafo_juegos);

    do
    {
        mostrar_menu_principal();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        switch (opcion)
        {
        case '1':
            presioneTeclaParaContinuar();
            break;
        case '2': // SALIR
            presioneTeclaParaContinuar();
            break;
        default:
            puts("=== Opcion no valida. Por favor, intente de nuevo ===");
            presioneTeclaParaContinuar();
            break;
        }
    } while (opcion != '2');

    liberar_memoria(grafo_juegos);
    puts("=== CERRANDO PROGRAMA ===");
    return 0;
}
// MAIN =======================================

// FUNCIONES =======================================
int is_equal_str(void *key1, void *key2)
{
    return strcmp((char *)key1, (char *)key2) == 0;
}

char * capitalizar(char *palabra)
{
    if (palabra[0] == '\0') return NULL;

    palabra[0] = toupper(palabra[0]); 
    for (int i = 1 ; palabra[i] != '\0' ; i++)
    {
        if (palabra[i - 1] == ' ' || palabra[i - 1] == '-') palabra[i] = toupper(palabra[i]);
        else palabra[i] = tolower(palabra[i]);
    } 
    return palabra;
}

void mostrar_menu_principal()
{
    limpiarPantalla();
    puts("========================================");
    puts("           Another Game?");
    puts("========================================");
    puts("1)");
    puts("2) Salir");
    puts("========================================");
}

void mostrar_juego(Videojuego *juego)
{
    printf("\nTitulo: %s | Metacritic: %d\n", juego->id, juego->titulo, juego->metacritic);
    printf("Desarrollador: %s | Precio: $%.2f | Multijugador: %s\n", juego->desarrollador, juego->precio, juego->es_multijugador ? "Si" : "No");

    printf("Etiquetas: ");
    char *tag = list_first(juego->categorias);
    while (tag != NULL)
    {
        printf("[%s] ", tag);
        tag = list_next(juego->categorias);
    }
    printf("\n----------------------------------------\n");
}

void mostrar_lista_juegos(List *lista)
{
    Videojuego *juego = list_first(lista);
    while (juego != NULL)
    {
        mostrar_juego(juego);
        juego = list_next(lista);
    }
}

void cargar_juegos(Map *grafo_juegos)
{
    FILE *archivo = fopen("dataset_maestro_final.csv", "r");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo dataset_maestro_final.csv");
        return;
    }

    // Lee los encabezados del CSV
    char **campos = leer_linea_csv(archivo, ',');

    // Lee cada línea del archivo CSV hasta el final
    while ((campos = leer_linea_csv(archivo, ',')) != NULL)
    {
        Videojuego *juego = (Videojuego *)malloc(sizeof(Videojuego));
        if (juego == NULL) exit(EXIT_FAILURE);

        strcpy(juego->id, campos[0]);                      // ID
        strcpy(juego->titulo, campos[1]);                  // TITULO
        strcpy(juego->desarrollador, campos[2]);           // ESTUDIO

        juego->categorias = split_string(campos[4], ",");  // CATEGORIA

        juego->precio = atof(campos[5]);                   // PRECIO
        juego->cantidad_dlc = atoi(campos[6]);             // DLC'S
        juego->metacritic = atoi(campos[7]);               // METACRITIC
        juego->recomendaciones = atoi(campos[8]);          // RECOMENDACIONES
        juego->puntuacion_popularidad = atof(campos[9]);   // PUNTUACION

        strcpy(juego->nivel_calificacion, campos[10]);     // CALIFICACION (STRING)

        juego->es_multijugador = atoi(campos[12]);         // MULTIJUGADOR
        strcpy(juego->plataformas, campos[13]);            // PLATAFORMAS

        strcpy(juego->fecha_lanzamiento, campos[16]);      // FECHA

        juego->juegos_similares = list_create();           // LISTA ADYACENCIA

        map_insert(grafo_juegos, juego->id, juego);
    }

    fclose(archivo);
    puts("=== Videojuegos cargados correctamente en el Grafo ===");
}

void liberar_memoria(Map *grafo_juegos)
{
    MapPair *pair = map_first(grafo_juegos);
    while (pair != NULL)
    {
        Videojuego *juego = (Videojuego *)pair->value;

        list_clean(juego->categorias);

        Arista *arista = list_first(juego->juegos_similares);
        while(arista != NULL) 
        {
            free(arista);
            arista = list_next(juego->juegos_similares);
        }
        list_clean(juego->juegos_similares);

        free(juego);
        pair = map_next(grafo_juegos);
    }

    map_clean(grafo_juegos);
}
// FUNCIONES =======================================