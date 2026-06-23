#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include "TDAs/stack.h"
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

// ESTRUCTURA PARA USUARIOS (Inicio de Sesión)
typedef struct {
    char nombre[50];        // Nombre identico al de ingreso
    char nombre_llave[50];    // Nombre con formato reestablecido a minusculas (para buscar y guardar)
    unsigned long clave;    // Hash de la contraseña
    List *wishlist;         // Guardar juegos deseados
    Stack *historial;       // Deshacer acciones.
} Usuario;

// ESTRUCTURA PARA FILTROS
typedef struct {
    List *categorias; // Lista de strings con los tags
    char desarrollador[100];
    float precio_max;
    int metacritic_min;
    int multijugador; // 1 = Si, 0 = No, -1 = No filtrar
    char plataforma[100];
} Filtro;
// STRUCT =======================================

// PROTOTIPOS =======================================
int is_equal_str(void *, void *);
char * capitalizar(char *);
void a_minusculas(char *, char *);
unsigned long hash_clave(const char *);
Usuario *registrar_usuario(Map *, char *, char*);
Usuario *iniciar_sesion(Map *);
void mostrar_menu_principal();
void mostrar_juego(Videojuego *);
void mostrar_lista_juegos(List *);
void filtrar_juegos(Map *grafo_juegos);
int cumple_filtros(Videojuego *juego, Filtro *filtro);
int cumple_categorias(Videojuego *juego, List *categorias);
void mostrar_menu_filtro();
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

    do
    {
        mostrar_menu_principal();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        switch (opcion)
        {
        case '1': // INICIAR SESION
            iniciar_sesion(); 
            presioneTeclaParaContinuar();
            break;
        case '2': // BUSCAR JUEGO
            buscar_juego();
            presioneTeclaParaContinuar();
            break;
        case '3': // RECOMENDAR JUEGOS
            recomendaciones();
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
                    agregar_juego_wishlist();
                    presioneTeclaParaContinuar();
                    break;
                case '2':
                    eliminar_juego_wishlist();
                    presioneTeclaParaContinuar();
                    break;
                case '3':
                    mostrar_wishlist();
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
            deshacer_ultima_accion();
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

// Esta funcion convierte una cadena a minusculas enviando la cadena convertida a otro string sin modificar la original.
void a_minusculas(char *textoIngresado, char *variableDestino) {
    int i = 0;
    while (textoIngresado[i] != '\0') {
        variableDestino[i] = tolower((unsigned char)textoIngresado[i]);
        i++;
    }
    variableDestino[i] = '\0';
    return;
}

// "Encriptar" la clave con una función hash. (En el login solo se compara el resultado de colocar la clave en el hash).
unsigned long hash_clave(const char *clave) {
    unsigned long hash = 5381;
    int c;
    while ((c = *clave++) != '\0') {
        hash = hash * 33 + c;
    }
    return hash;
}

Usuario *registrar_usuario(Map *usuarios, char *nombre, char*nombre_llave) {
    char clave[50];

    printf("Crea una contraseña: ");
    scanf(" %49[^\n]", clave);

    Usuario *nuevo = (Usuario*) malloc(sizeof(Usuario));
    if (nuevo == NULL) exit(EXIT_FAILURE);

    strcpy(nuevo->nombre, nombre);                  // Para mostrarlo
    strcpy(nuevo->nombre_llave, nombre_llave);      // minusculas para busqueda
    nuevo->clave = hash_clave(clave);               // se guarda la clave encriptada
    nuevo->wishlist = list_create();
    nuevo->historial = stack_create(NULL);

    map_insert(usuarios, nuevo->nombre_llave, nuevo);  // la clave del nodo es el nombre formateado

    printf("\nCuenta creada con exito. Bienvenido, %s!\n", nuevo->nombre); 
    return nuevo;
}

Usuario *iniciar_sesion(Map *usuarios) {
    char nombre[50];
    char nombre_llave[50];
    char clave[50];

    printf("Ingresa tu nombre de usuario: ");
    scanf(" %49[^\n]", nombre);
    a_minusculas(nombre, nombre_llave);

    MapPair *pair = map_search(usuarios, nombre_llave);

    // CASI 1: el usuario ya existe => se pide la clave y se comparan los resultados de la función hash
    if (pair != NULL) {
        Usuario *usuario = (Usuario*)pair->value;

        for (int intentos = 3; intentos > 0; intentos--) {
            printf("Contraseña: ");
            scanf(" %49[^\n]", clave);

            if (hash_clave(clave) == usuario->clave) {
                printf("\nBienvenido de vuelta, %s!\n", usuario->nombre);
                return usuario;
            }
            if (intentos > 1) {
                printf("Contraseña incorrecta. Te quedan %d intento(s).\n", intentos - 1);
            }
        } 
        printf("Has excedido el número de intentos.\n");
        return NULL;
    }

    // CASO 2: no existe => registrar usuario
    char opcion;
    printf("\nEl usuario '%s' no existe. Crear cuenta? (1 = Si): ", nombre);
    scanf(" %c", &opcion);
    if (opcion != '1') return NULL;

    return registrar_usuario(usuarios, nombre, nombre_llave);
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
void mostrar_menu_filtro(){
    limpiarPantalla();
    puts("\n=== Filtrar Juegos ===");
    puts("1) Agregar categoría");
    puts("2) Establecer desarrollador");
    puts("3) Establecer precio máximo");
    puts("4) Establecer metacritic mínimo");
    puts("5) Establecer multijugador (1 = Sí, 0 = No)");
    puts("6) Establecer plataforma");
    puts("7) Filtrar y mostrar resultados");
    puts("8) Limpiar filtros");
    puts("9) Volver");
}


int cumple_categorias(Videojuego *juego, List *categorias) {
    char *categoria_filtro = list_first(categorias);
    while (categoria_filtro != NULL) {
        int encontrado = 0;
        char *categoria_juego = list_first(juego->categorias);
        while (categoria_juego != NULL) {
            if (strcmp(categoria_filtro, categoria_juego) == 0) {
                encontrado = 1;
                break;
            }
            categoria_juego = list_next(juego->categorias);
        }
        if (!encontrado) return 0; // Si alguna categoría no coincide, no cumple
        categoria_filtro = list_next(categorias);
    }
    return 1; // Todas las categorías coinciden
}

int cumple_filtros(Videojuego *juego, Filtro *filtro){
    if(filtro->categorias != NULL && list_size(filtro->categorias) > 0){
        if(!cumple_categorias(juego, filtro->categorias)) return 0;
    }
    if(strlen(filtro->desarrollador) > 0){
        if(strcmp(juego->desarrollador, filtro->desarrollador) != 0) return 0;
    }
    if(filtro->precio_max != -1){
        if(juego->precio > filtro->precio_max) return 0;
    }
    if(filtro->metacritic_min != -1){
        if(juego->metacritic < filtro->metacritic_min) return 0;
    }
    if(filtro->multijugador != -1){
        if(juego->es_multijugador != filtro->multijugador) return 0;
    }
    if(strlen(filtro->plataforma) > 0){
        if(strstr(juego->plataformas, filtro->plataforma) == NULL) return 0;
    }
    return 1;

}

void filtrar_juegos(Map *grafo_juegos)
{
    Filtro filtro;
    filtro.categorias = list_create();
    strcpy(filtro.desarrollador, ""); // No filtrar por desarrollador por defecto
    strcpy(filtro.plataforma, ""); // No filtrar por plataforma por defecto
    filtro.precio_max = -1; // No filtrar por precio por defecto
    filtro.metacritic_min = -1; // No filtrar por metacritic por defecto
    filtro.multijugador = -1; // No filtrar por multijugador por defecto

    char opcion;
    do {
        mostrar_menu_filtro();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        switch (opcion) {
            case '1':
            {
                char categoria[100];
                printf("Ingrese la categoría a agregar: ");
                scanf(" %99[^\n]", categoria);
                char *nueva = malloc(strlen(categoria) + 1);
                strcpy(nueva, categoria);
                list_pushBack(filtro.categorias, nueva);
                break;
            }
            case '2':
                printf("Ingrese el desarrollador: ");
                scanf(" %99[^\n]", filtro.desarrollador);
                break;  
            case '3':
                printf("Ingrese el precio máximo: ");
                scanf("%f", &filtro.precio_max);
                break;
            case '4':
                printf("Ingrese el metacritic mínimo: ");
                scanf("%d", &filtro.metacritic_min);
                break;
            case '5':
                printf("Ingrese 1 para Sí, 0 para No: ");
                scanf("%d", &filtro.multijugador);
                break;
            case '6':
                printf("Ingrese la plataforma: ");
                scanf(" %99[^\n]", filtro.plataforma);
                break;
            case '7':
                //Aqui se aplican filtros
                int encontrados = 0;

                MapPair *pair = map_first(grafo_juegos);

                while (pair != NULL) {
                    Videojuego *juego = (Videojuego *)pair->value;
                    if (cumple_filtros(juego, &filtro)) {
                        mostrar_juego(juego);
                        encontrados++;
                    }
                    pair = map_next(grafo_juegos);
                }
                printf("Se encontraron %d juegos que cumplen con los filtros.\n", encontrados);
                presioneTeclaParaContinuar();
                break;
            case '8':
                char *cat = list_first(filtro.categorias);

                while (cat != NULL) {
                    free(cat);
                    cat = list_next(filtro.categorias);
                } 
                list_clean(filtro.categorias);
                strcpy(filtro.desarrollador, "");
                strcpy(filtro.plataforma, "");
                filtro.precio_max = -1;
                filtro.metacritic_min = -1;
                filtro.multijugador = -1;
                break;
            case '9':
                char *cat = list_first(filtro.categorias);

                while(cat != NULL)
                {
                    free(cat);
                    cat = list_next(filtro.categorias); 
                }

                list_clean(filtro.categorias);
                break;
        }
    } while (opcion != '9');
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
    printf("Plataformas: %s | Lanzamiento: %s\n", juego->plataformas, juego->fecha_lanzamiento);

    // Mostrar Etiquetas (Recorriendo la lista de etiquetas/tags)
    printf("Etiquetas: ");
    char *tag = list_first(juego->categorias);
    while (tag != NULL) {
        printf("[%s] ", tag);
        tag = list_next(juego->categorias);
    }
    printf("\n----------------------------------------\n");
    return;
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