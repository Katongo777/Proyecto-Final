#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include "TDAs/set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"

int is_equal_str(void *key1, void *key2)
{
    return strcmp((char *)key1, (char *)key2) == 0;
}

void cargar_juegos(Map *grafo_juegos)
{
    FILE *archivo = fopen("dataset_juegos.csv", "r");
    if (archivo == NULL)
    {
        perror("Error al abrir el archivo dataset_juegos.csv");
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

        juego->categorias_lis = split_string(campos[4], ","); // CATEGORIA (LISTA)
        juego->categorias_con = set_create(is_equal_str);  // CATEGORIA (CONJUNTO)

        /*
        Se usa un conjunto para que las búsquedas de coincidencias de categorías sean más
        rapidas
        */
        char *categ = list_first(juego->categorias_lis);
        while (categ != NULL)
        {
            set_insert(juego->categorias_con, categ);
            categ = list_next(juego->categorias_lis);
        }

        juego->precio = atof(campos[5]);                   // PRECIO
        juego->cantidad_dlc = atoi(campos[6]);             // DLC'S
        juego->metacritic = atoi(campos[7]);               // METACRITIC
        juego->recomendaciones = atoi(campos[8]);          // RECOMENDACIONES
        juego->puntuacion_pop = atof(campos[9]);   // PUNTUACION
        juego->es_multijugador = atoi(campos[12]);         // MULTIJUGADOR

        juego->plataformas_lis = split_string(campos[13], ","); // PLATAFORMAS (LISTA)
        juego->plataformas_con = set_create(is_equal_str);   // PLATAFORMAS (CONJUNTO)

        char *plat = list_first(juego->plataformas_lis);
        while (plat != NULL)
        {
            set_insert(juego->plataformas_con, plat);
            plat = list_next(juego->plataformas_lis);
        }

        strcpy(juego->fecha_lanzamiento, campos[16]); // FECHA
        int anio = atoi(juego->fecha_lanzamiento);
        juego->anio = anio;

        juego->juegos_similares = list_create(); // LISTA ADYACENCIA

        map_insert(grafo_juegos, juego->id, juego);
    }

    fclose(archivo);
    puts("=== Videojuegos cargados correctamente en el Grafo ===");
}

void generar_conexiones(Map *grafo_juegos)
{
    puts("Generando conexiones, puede demorarse un poco...");
    int umbral = 65; // Umbral para generar la conexion entre dos juegos
    int total_juegos = 9451;

    /*
    El arreglo se crea para no generar una doble conexion, o sea, no se compare
    un juego con otro que ya se comparo, además nos se ahorra la mitad de iteraciones
    */
    int indice = 0;
    Videojuego **arreglo_juegos = (Videojuego **)malloc(total_juegos * sizeof(Videojuego *));
    for (MapPair *pair = map_first(grafo_juegos) ; pair != NULL ; pair = map_next(grafo_juegos))
    {
        arreglo_juegos[indice] = (Videojuego *)pair->value;
        indice++;
    }

    for (int i = 0 ; i < total_juegos ; i++)
    {
        // Se imprime cada 100 juegos
        if (i % 100 == 0) printf("Estado de carga: %d%%\n", (i * 100) / 9451);
        for (int j = i + 1 ; j < total_juegos ; j++)
        {
            Videojuego *juego_base = arreglo_juegos[i];
            Videojuego *juego_comp = arreglo_juegos[j]; 
            int peso = calcular_peso(juego_base, juego_comp);
            
            if (peso >= umbral)
            {
                // Arista de juego base ---> juego similar
                Arista *conexion_1 = (Arista *)malloc(sizeof(Arista));
                conexion_1->juego_vecino = juego_comp;
                conexion_1->peso_similitud = peso;
                list_pushBack(juego_base->juegos_similares, conexion_1);
                
                // Arista de juego similar ---> juego base
                Arista *conexion_2 = (Arista *)malloc(sizeof(Arista));
                conexion_2->juego_vecino = juego_base;
                conexion_2->peso_similitud = peso;
                list_pushBack(juego_comp->juegos_similares, conexion_2);
            }
        }
    }

    free(arreglo_juegos);
    guardar_conexiones_archivo(grafo_juegos);
    puts("Conexiones generadas correctamente");
}

void guardar_conexiones_archivo(Map *grafo_juegos)
{
    FILE *archivo = fopen("conexiones_grafo.txt", "w");
    if (archivo == NULL)
    {
        puts("Error al crear el archivo de guardado del grafo.");
        return;
    }

    MapPair *pair = map_first(grafo_juegos);
    while (pair != NULL)
    {
        Videojuego *juego = (Videojuego *)pair->value;
        Arista *conexion = list_first(juego->juegos_similares);
        while (conexion != NULL)
        {
            /* 
            Como la conexión es doble (A->B y B->A), se usa strcmp
            para guardar la conexion una sola vez en el archivo
            */
            if (strcmp(juego->id, conexion->juego_vecino->id) < 0)
            {
                fprintf(archivo, "%s,%s,%d\n", juego->id, conexion->juego_vecino->id, conexion->peso_similitud);
            }
            conexion = list_next(juego->juegos_similares);
        }
        pair = map_next(grafo_juegos);
    }
    fclose(archivo);
    puts("Grafo serializado y guardado en 'conexiones_grafo.txt'.");
}

int cargar_conexiones_archivo(Map *grafo_juegos)    
{
    FILE *archivo = fopen("conexiones_grafo.txt", "r");
    // Si el archivo no existe, se retorna 0 para que el programa sepa que debe calcular las conexiones
    if (archivo == NULL) return 0;

    char linea[1024];
    char id_1[50], id_2[50];
    int peso;
    int total_conexiones = 572256;
    int i = 0;

    while (fgets(linea, sizeof(linea), archivo) != NULL)
    {
        if (i % 5722 == 0) printf("Estado de carga: %d%%\n", (i * 100) / 572256);
        if (sscanf(linea, "%[^,],%[^,],%d", id_1, id_2, &peso) == 3)
        {
            MapPair *pair_1 = map_search(grafo_juegos, id_1);
            MapPair *pair_2 = map_search(grafo_juegos, id_2);

            if (pair_1 != NULL && pair_2 != NULL)
            {
                Videojuego *juego_base = (Videojuego *)pair_1->value;
                Videojuego *juego_comp = (Videojuego *)pair_2->value;

                // Conexión J1 ---> J2
                Arista *conexion_1 = (Arista *)malloc(sizeof(Arista));
                conexion_1->juego_vecino = juego_comp;
                conexion_1->peso_similitud = peso;
                list_pushBack(juego_base->juegos_similares, conexion_1);

                // Conexión J2 ---> J1
                Arista *conexion_2 = (Arista *)malloc(sizeof(Arista));
                conexion_2->juego_vecino = juego_base;
                conexion_2->peso_similitud = peso;
                list_pushBack(juego_comp->juegos_similares, conexion_2);
            }
        }
        i++;
    }
    fclose(archivo);
    return 1; // Se retorna 1 indicando que la carga fue finalizada correctamente
}

int calcular_peso(Videojuego *juego_1, Videojuego *juego_2)
{
    int peso = 0;

    // DESARROLLADOR
    if (strcmp(juego_1->desarrollador, juego_2->desarrollador) == 0) peso += 5;

    // MULTIJUGADOR
    if (juego_1->es_multijugador == juego_2->es_multijugador) peso += 1;

    // CATEGORIAS
    char *categ_1 = list_first(juego_1->categorias_lis);
    while (categ_1 != NULL)
    {
        if (set_search(juego_2->categorias_con, categ_1) != NULL) peso += 4;
        categ_1 = list_next(juego_1->categorias_lis);
    }
    
    // PRECIO
    int dif_precio = abs(juego_1->precio - juego_2->precio);
    if (dif_precio <= 10) peso += 2;
    
    // METACRITIC
    int meta = juego_2->metacritic; 
    if (meta >= 70)
    {
        peso += 2;
        if (meta >= 80)
        {
            peso += 2;
            if (meta >= 90)
            {
                peso += 3;
            }
        }
    }

    // RECOMENDACIONES
    int recom = juego_2->recomendaciones;
    if (recom >= 50000)
    {
        peso += 2;
        if (recom >= 300000)
        {
            peso += 3;
            if (recom >= 500000)
            {
                peso += 5;
            }
        }
    }

    // POPULARIDAD
    float punt_pop = juego_2->puntuacion_pop; 
    if (punt_pop >= 60)
    {
        peso += 2;
        if (punt_pop >= 70)
        {
            peso += 3;
            if (punt_pop >= 90)
            {
                peso += 5;
            }
        }
    }

    // PLATAFORMAS
    char *plat_1 = list_first(juego_1->plataformas_lis);
    while (plat_1 != NULL)
    {
        if (set_search(juego_2->plataformas_con, plat_1) != NULL) peso += 3;
        plat_1 = list_next(juego_1->plataformas_lis);
    }

    // AÑO
    int dif_anio = abs(juego_1->anio - juego_2->anio);
    if (dif_anio <= 5) peso += 3;

    return peso;
}

void liberar_memoria(Map *grafo_juegos)
{
    MapPair *pair = map_first(grafo_juegos);
    while (pair != NULL)
    {
        Videojuego *juego = (Videojuego *)pair->value;

        pair = map_next(grafo_juegos);
        
        // CATEGORIAS
        list_clean(juego->categorias_lis);
        set_clean(juego->categorias_con);

        // PLATAFORMAS
        list_clean(juego->plataformas_lis);
        set_clean(juego->plataformas_con);

        Arista *arista = list_first(juego->juegos_similares);
        while(arista != NULL) 
        {
            Arista *ar_borrar = arista;
            arista = list_next(juego->juegos_similares);
            free(arista);
        }
        list_clean(juego->juegos_similares);

        free(juego);
        pair = map_next(grafo_juegos);
    }

    map_clean(grafo_juegos);
}