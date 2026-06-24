#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include "TDAs/set.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"

// PROTOTIPOS =======================================
char * capitalizar(char *);
void mostrar_menu_principal();
void mostrar_juego(Videojuego *);
void mostrar_lista_juegos(List *);
void mostrar_menu_wishlist();
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
    if (cargar_conexiones_archivo(grafo_juegos) == 1)
    {
        puts("Archivo de conexiones cargado correctamente");
    }
    else
    {
        puts("No se encontro el archivo con el grafo de videojuegos. Iniciando conexiones...");
        generar_conexiones(grafo_juegos);
    }

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
            //recomendar_por_juego(grafo_juegos);
            //recomendar_por_filtro(grafo_juegos);
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
    printf("\nTitulo: %s | Metacritic: %d\n", juego->titulo, juego->metacritic);
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

// FUNCIONES =======================================