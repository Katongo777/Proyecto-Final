#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include "TDAs/set.h"
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
int is_equal_str(void *, void *);
char * capitalizar(char *);
void mostrar_menu_principal();
void mostrar_juego(Videojuego *);
void mostrar_lista_juegos(List *);
void mostrar_menu_wishlist();

void recomendar_por_juego(Map *);
void recomendar_por_filtro(Map *);

void generar_conexiones(Map *);
int calcular_peso(Videojuego *, Videojuego *);
void cargar_juegos(Map *);
void liberar_memoria(Map *);
// PROTOTIPOS =======================================

// MAIN =======================================
int main()
{
    char opcion;
    int contador_carga = 0;
    char opcion_wishlist;

    // Clave: ID - Valor: Struct Videojuego
    Map *grafo_juegos = map_create(is_equal_str);
    cargar_juegos(grafo_juegos);
    generar_conexiones(grafo_juegos);

    do
    {
        mostrar_menu_principal();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        switch (opcion)
        {
        case '1': // INICIAR SESION
            //iniciar_sesion(); 
            presioneTeclaParaContinuar();
            break;
        case '2': // BUSCAR JUEGO
            //buscar_juego();
            presioneTeclaParaContinuar();
            break;
        case '3': // RECOMENDAR JUEGOS
            recomendar_por_juego(grafo_juegos);
            recomendar_por_filtro(grafo_juegos);
            presioneTeclaParaContinuar();
            break;
        case '4': // WISHLIST
            do{
                mostrar_menu_wishlist();
                printf("Ingrese su opcion: ");
                scanf(" %c", &opcion_wishlist);
                switch (opcion_wishlist)
                {
                case '1':
                    //agregar_juego_wishlist();
                    presioneTeclaParaContinuar();
                    break;
                case '2':
                    //eliminar_juego_wishlist();
                    presioneTeclaParaContinuar();
                    break;
                case '3':
                    //mostrar_wishlist();
                    presioneTeclaParaContinuar();
                    break;
                case '4':
                    break;
                default:
                    puts("=== Opcion no valida. Por favor, intente de nuevo ===");
                    presioneTeclaParaContinuar();
                    break;
                }
            } while (opcion_wishlist != '4');
            break;
        case '5': // DESHACER ULTIMA ACCION
            //deshacer_ultima_accion();
            presioneTeclaParaContinuar();
            break;
        case '6': // SALIR
            presioneTeclaParaContinuar();
            break;
        default:
            puts("=== Opcion no valida. Por favor, intente de nuevo ===");
            presioneTeclaParaContinuar();
            break;
        }
    } while (opcion != '6');

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
    puts("1) Iniciar sesión");
    puts("2) Buscar juego");
    puts("3) Recomendar juegos");
    puts("4) Wishlist");
    puts("5) Deshacer última acción");
    puts("6) Salir");
    puts("========================================");
}

void mostrar_menu_wishlist()
{
    puts("========================================");
    puts("     Mi Wishlist");
    puts("========================================");
    puts("1) Agregar Juego");
    puts("2) Eliminar Juego");
    puts("3) Mostrar Wishlist");
    puts("4) Volver");
}

void mostrar_juego(Videojuego *juego)
{
    printf("\nTitulo: %s | Metacritic: %d\n", juego->id, juego->titulo, juego->metacritic);
    printf("Desarrollador: %s | Precio: $%.2f | Multijugador: %s\n", juego->desarrollador, juego->precio, juego->es_multijugador ? "Si" : "No");

    printf("Etiquetas: ");
    char *tag = list_first(juego->categorias_lis);
    while (tag != NULL)
    {
        printf("[%s] ", tag);
        tag = list_next(juego->categorias_lis);
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
    FILE *archivo = fopen("dataset_juegos.csv", "r");
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

        juego->categorias_lis = split_string(campos[4], ","); // CATEGORIA (LISTA)
        juego->categorias_con = set_create(is_equal_str);  // CATEGORIA (CONJUNTO)

        /*
        Se usa un conjunto para que las búsquedas de coincidencias de categorías sean más
        eficientes
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

void liberar_memoria(Map *grafo_juegos)
{
    MapPair *pair = map_first(grafo_juegos);
    while (pair != NULL)
    {
        Videojuego *juego = (Videojuego *)pair->value;

        // CATEGORIAS
        list_clean(juego->categorias_lis);
        set_clean(juego->categorias_con);

        // PLATAFORMAS
        list_clean(juego->plataformas_lis);
        set_clean(juego->plataformas_con);

        Conexion *arista = list_first(juego->juegos_similares);
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

void generar_conexiones(Map *grafo_juegos)
{
    puts("Generando conexiones, puede demorarse un poco...");
    puts("Mientras ve pensando que juegos vas a querer comparar");
    int umbral = 40; // Umbral para generar la conexion entre dos juegos
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
                // Conexion de juego base ---> juego similar
                Conexion *conexion_1 = (Conexion *)malloc(sizeof(Conexion));
                conexion_1->juego_vecino = juego_comp;
                conexion_1->peso_similitud = peso;
                list_pushBack(juego_base->juegos_similares, conexion_1);
                
                // Conexion de juego similar ---> juego base
                Conexion *conexion_2 = (Conexion *)malloc(sizeof(Conexion));
                conexion_2->juego_vecino = juego_base;
                conexion_2->peso_similitud = peso;
                list_pushBack(juego_comp->juegos_similares, conexion_2);
            }
        }
    }

    free(arreglo_juegos);
    puts("Conexiones generadas correctamente");
}

int calcular_peso(Videojuego *juego_1, Videojuego *juego_2)
{
    int peso = 0;

    // DESARROLLADOR
    if (strcmp(juego_1->desarrollador, juego_2->desarrollador) == 0) peso += 5;

    // MULTIJUGADOR
    if (juego_1->es_multijugador == juego_2->es_multijugador) peso += 2;

    // CATEGORIAS
    char *categ_1 = list_first(juego_1->categorias_lis);
    while (categ_1 != NULL)
    {
        if (set_search(juego_2->categorias_con, categ_1) != NULL) peso += 3;
        categ_1 = list_next(juego_1->categorias_lis);
    }
    
    // PRECIO
    int dif_precio = abs(juego_1->precio - juego_2->precio);
    if (dif_precio <= 10) peso += 3;
    
    // METACRITIC
    int meta = juego_2->metacritic; 
    if (meta >= 70)
    {
        peso += 2;
        if (meta >= 80)
        {
            peso += 3;
            if (meta >= 90)
            {
                peso += 5;
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
        if (set_search(juego_2->plataformas_con, plat_1) != NULL) peso += 2;
        plat_1 = list_next(juego_1->plataformas_lis);
    }

    // AÑO
    int dif_anio = abs(juego_1->anio - juego_2->anio);
    if (dif_anio <= 5) peso += 3;


    return peso;
}

void recomendar_por_juego(Map *grafo_juegos)
{

}

void recomendar_por_filtro(Map *grafo_juegos)
{

}

// FUNCIONES =======================================