#ifndef GRAFO_H
#define GRAFO_H

#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include "TDAs/set.h"

// STRUCT =======================================
typedef struct
{
    char id[50];
    char titulo[150];
    char desarrollador[100];
    List *categorias_lis;   // Lista de categorías para mostrar
    Set *categorias_con; // Conjunto de categorías
    float precio; // USD
    int cantidad_dlc;
    int metacritic;
    int recomendaciones;
    float puntuacion_pop;
    int es_multijugador; // 1 si es multijugador o 0 si no lo es
    List *plataformas_lis;   // Lista de plataformas para mostrar
    Set *plataformas_con; // Conjunto de plataformas
    char fecha_lanzamiento[50];
    int anio;

    // Lista de Adyacencia
    List *juegos_similares; // Guardará elementos de tipo 'Conexion'
} Videojuego;

// Estructura para las conexiones del grafo
typedef struct 
{
    Videojuego *juego_vecino; // Puntero al nodo conectado
    int peso_similitud;       // Umbral
} Conexion;
// STRUCT =======================================

// PROTOTIPOS =======================================
int is_equal_str(void *key1, void *key2);
void cargar_juegos(Map *);
void generar_conexiones(Map *);
int calcular_peso(Videojuego *, Videojuego *);
void guardar_conexiones_archivo(Map *grafo_juegos);
int cargar_conexiones_archivo(Map *grafo_juegos);
void liberar_memoria(Map *grafo_juegos);
// PROTOTIPOS =======================================

#endif 