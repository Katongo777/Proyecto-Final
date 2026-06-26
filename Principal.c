#include "TDAs/extra.h"
#include "TDAs/list.h"
#include "TDAs/map.h"
#include "TDAs/set.h"
#include "TDAs/stack.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "grafo.h"
#define ANCHO_CAJA 58

// STRUCT =======================================
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
    List *plataformas; // Lista de strings con las plataformas
    char desarrollador[100];
    float precio_max;
    int metacritic_min;
    int multijugador; // 1 = Si, 0 = No, -1 = No filtrar
} Filtro;
// STRUCT =======================================

// PROTOTIPOS =======================================
void linea_borde();
void linea_vacia();
void linea_texto(char *);
void linea_centrada(char *);
void guardar_usuarios(Map *);
void cargar_usuarios(Map *, Map *);
char * capitalizar(char *);
void a_minusculas(char *, char *);
unsigned long hash_clave(const char *);
Usuario *registrar_usuario(Map *, char *, char*);
Usuario *iniciar_sesion(Map *);
void mostrar_menu_principal();
void mostrar_menu_wishlist();
void mostrar_juego(Videojuego *);
void mostrar_lista_juegos(List *);
void mostrar_menu_wishlist();
void filtrar_juegos(Map *grafo_juegos);
int cumple_filtros(Videojuego *juego, Filtro *filtro);
int cumple_categorias(Videojuego *juego, List *categorias);
void vaciar_lista_filtro(List *lista);
void mostrar_menu_filtro();
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
    Map *usuarios = map_create(is_equal_str);
    cargar_usuarios(usuarios, grafo_juegos);
    Usuario *usuario_activo = NULL;

    do
    {
        mostrar_menu_principal();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        switch (opcion)
        {
        case '1': // INICIAR SESION
            usuario_activo = iniciar_sesion(usuarios); 
            presioneTeclaParaContinuar();
            break;
        case '2': // BUSCAR JUEGO
            buscar_juego(grafo_juegos);
            presioneTeclaParaContinuar();
            break;
        case '3': // FILTRAR JUEGOS
            filtrar_juegos(grafo_juegos);
            presioneTeclaParaContinuar();
            break;
        case '4': // RECOMENDAR JUEGOS
            // recomendaciones();
            presioneTeclaParaContinuar();
            break;
        case '5': // WISHLIST
            do{
                mostrar_menu_wishlist();
                printf("Ingrese su opcion: ");
                scanf(" %c", &opcion_wishlist);
                switch (opcion_wishlist)
                {
                case '1':
                    agregar_juego_wishlist(grafo_juegos, usuarios, usuario_activo);
                    presioneTeclaParaContinuar();
                    break;
                case '2':
                    // eliminar_juego_wishlist();
                    presioneTeclaParaContinuar();
                    break;
                case '3':
                    // mostrar_wishlist();
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
        case '6': // DESHACER ULTIMA ACCION
            // deshacer_ultima_accion();
            presioneTeclaParaContinuar();
            break;
        case '7': // SALIR
            presioneTeclaParaContinuar();
            break;
        default:
            puts("=== Opcion no valida. Por favor, intente de nuevo ===");
            presioneTeclaParaContinuar();
            break;
        }
    } while (opcion != '7');

    liberar_memoria(grafo_juegos);
    puts("=== CERRANDO PROGRAMA ===");
    return 0;
}
// MAIN =======================================

// FUNCIONES =======================================

// --- PRINTEO DE CAJAS ---

// Parte de arriba
void linea_borde() {
    printf("+");
    for (int i = 0; i < ANCHO_CAJA; i++) printf("=");
    printf("+\n");
}

// Linea interna
void linea_vacia() {
    printf("|");
    for (int i = 0; i < ANCHO_CAJA; i++) printf(" ");
    printf("|\n");
}

// Texto alineado a la izquierda. Si no cabe se corta y termina en "..."
void linea_texto(char *texto) {
    char buffer[ANCHO_CAJA + 1];
    int margen = 2;
    int disponible = ANCHO_CAJA - margen;
    int largo = strlen(texto);

    for(int i = 0; i < margen; i++) buffer[i] = ' ';
    if (largo <= disponible) {
        for (int i = 0; i < largo; i++) buffer[margen + i] = texto[i];
        for (int i = margen + largo; i < ANCHO_CAJA; i++) buffer[i] = ' ';
    }
    else {
        int corte = disponible -3;
        for(int i = 0; i < corte; i++) buffer[margen + i] = texto[i];
        buffer[margen + corte] = '.';
        buffer[margen + corte + 1] = '.';
        buffer[margen + corte + 2] = '.';
    }

    buffer[ANCHO_CAJA] = '\0';
    printf("|%s|\n", buffer);
}

// Centrado

void linea_centrada(char *texto) {
    char buffer[ANCHO_CAJA + 1];
    int largo = strlen(texto);
    if (largo > ANCHO_CAJA) largo = ANCHO_CAJA;
    int izquierda = (ANCHO_CAJA - largo) / 2;
    int derecha = ANCHO_CAJA - largo - izquierda;
    int p = 0;
    for (int i = 0; i < izquierda; i++) buffer[p++] = ' ';
    for (int i = 0; i < largo; i++) buffer[p++] = texto[i];
    for (int i = 0; i < derecha; i++) buffer[p++] = ' ';

    buffer[ANCHO_CAJA] = '\0';
    printf("|%s|\n", buffer);
}

// --- PRINTEO DE CAJAS ---

// Guarda todos los usuarios en un csv: "usuarios.csv".
void guardar_usuarios(Map *usuarios) {
    FILE *archivo = fopen("usuarios.csv", "w");
    if (archivo == NULL) {
        printf("No se pudo abrir usuarios.csv para guardar.\n");
        return;
    }

    // Columnas de los datos
    fprintf(archivo, "nombre;clave;wishlist\n");

    // Se recorre el mapa de usuarios guardando los datos de cada uno
    MapPair *pair = map_first(usuarios);
    while (pair != NULL) {
        Usuario *usuario = (Usuario *)pair->value;
        fprintf(archivo, "%s;%lu", usuario->nombre, usuario->clave);
        
        Videojuego *juego = list_first(usuario->wishlist);
        while (juego != NULL) {
            fprintf(archivo, ";%s", juego->titulo);
            juego = list_next(usuario->wishlist);
        }

        fprintf(archivo, "\n");
        pair = map_next(usuarios);
    }
    fclose(archivo);
}

void cargar_usuarios(Map *usuarios, Map *grafo_juegos) {
    FILE *archivo = fopen("usuarios.csv", "r");
    if (archivo == NULL) return; 

    char **campos = leer_linea_csv(archivo, ';');
    while ((campos = leer_linea_csv(archivo, ';')) != NULL) {
        Usuario *nuevo = (Usuario *)malloc(sizeof(Usuario));
        if (nuevo == NULL) exit(EXIT_FAILURE);

        strcpy(nuevo->nombre, campos[0]);
        a_minusculas(campos[0], nuevo->nombre_llave);
        nuevo->clave = strtoul(campos[1], NULL, 10);
        nuevo->wishlist = list_create();
        nuevo->historial = stack_create(NULL);

        // Los juegos de la wishlist son del 2 en adelate
        for (int i = 2; campos[i] != NULL; i++) {
            MapPair *pair = map_search(grafo_juegos, campos[i]);
            if (pair != NULL) list_pushBack(nuevo->wishlist, pair->value);
        }
        map_insert(usuarios, nuevo->nombre_llave, nuevo);
    }
    fclose(archivo);
}

// Esta función limpia TODO: pasa a minúsculas y elimina espacios y guiones
void simplificar_texto(char *textoIngresado, char *variableDestino) {
    int i = 0;
    int j = 0;
    while (textoIngresado[i] != '\0') {
        // Si es un espacio o un guion, lo saltamos
        if (textoIngresado[i] == ' ' || textoIngresado[i] == '-') {
            i++;
            continue;
        }
        // Lo demás lo pasamos a minúsculas y lo guardamos
        variableDestino[j] = tolower((unsigned char)textoIngresado[i]);
        i++;
        j++;
    }
    variableDestino[j] = '\0'; // Cerramos el string
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
    puts("1) Iniciar sesion");
    puts("2) Buscar juego");
    puts("3) Filtrar juegos");
    puts("4) Recomendar juegos");
    puts("5) Wishlist");
    puts("6) Deshacer ultima accion");
    puts("7) Salir");
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

void vaciar_lista_filtro(List *lista) {
    char *str = list_first(lista);
    while (str != NULL) {
        free(str);
        str = list_next(lista);
    }
    list_clean(lista);
}

int cumple_categorias(Videojuego *juego, List *categoriasFiltro)
{
    char *categoria = list_first(categoriasFiltro);

    while (categoria != NULL)
    {
        if (set_search(juego->categorias_con, categoria) == NULL)
            return 0;

        categoria = list_next(categoriasFiltro);
    }

    return 1;
}

int cumple_plataformas(Videojuego *juego, List *plataformasFiltro)
{
    char *plataforma = list_first(plataformasFiltro);

    while (plataforma != NULL)
    {
        if (set_search(juego->plataformas_con, plataforma) == NULL)
            return 0;

        plataforma = list_next(plataformasFiltro);
    }

    return 1;
}

int cumple_filtros(Videojuego *juego, Filtro *filtro)
{
    // Desarrollador
    if (strlen(filtro->desarrollador) > 0)
    {
        char des_juego_limpio[100];
        simplificar_texto(juego->desarrollador, des_juego_limpio);
        if (strcmp(des_juego_limpio, filtro->desarrollador) != 0)
            return 0;
    }

    // Precio máximo
    if (filtro->precio_max != -1)
    {
        if (juego->precio > filtro->precio_max)
            return 0;
    }

    // Metacritic mínimo
    if (filtro->metacritic_min != -1)
    {
        if (juego->metacritic < filtro->metacritic_min)
            return 0;
    }

    // Multijugador
    if (filtro->multijugador != -1)
    {
        if (juego->es_multijugador != filtro->multijugador)
            return 0;
    }

    // Categorías
    if (list_first(filtro->categorias) != NULL)
    {
        if (!cumple_categorias(juego, filtro->categorias))
            return 0;
    }

    // Plataformas
    if (list_first(filtro->plataformas) != NULL)
    {
        if (!cumple_plataformas(juego, filtro->plataformas))
            return 0;
    }

    return 1;
}

void filtrar_juegos(Map *grafo_juegos)
{
    Filtro filtro;

    filtro.categorias = list_create();
    filtro.plataformas = list_create();

    strcpy(filtro.desarrollador, "");

    filtro.precio_max = -1;
    filtro.metacritic_min = -1;
    filtro.multijugador = -1;

    char opcion;
    do {
        mostrar_menu_filtro();
        printf("Ingrese su opcion: ");
        scanf(" %c", &opcion);
        switch (opcion) {
            case '1':
            {
                char categoria[100];
                printf("Ingrese la categoría: ");
                scanf(" %99[^\n]", categoria);

                char categoria_limpia[100];
                simplificar_texto(categoria, categoria_limpia); 

                char *nueva = (char *)malloc(strlen(categoria_limpia) + 1);
                strcpy(nueva, categoria_limpia);
                list_pushBack(filtro.categorias, nueva);
                break;
            }
            case '2':
            {
                char des[100];
                printf("Ingrese el desarrollador: ");
                scanf(" %99[^\n]", des);
                
                simplificar_texto(des, filtro.desarrollador); // <-- Nuevo
                break; 
            }  
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
            {
                char plataforma[100];
                char plataforma_limpia[100];

                printf("Ingrese la plataforma: ");
                scanf(" %99[^\n]", plataforma);

                simplificar_texto(plataforma, plataforma_limpia); // <-- Nuevo

                char *nueva = (char *)malloc(strlen(plataforma_limpia) + 1);
                strcpy(nueva, plataforma_limpia);

                list_pushBack(filtro.plataformas, nueva);

                break;
            }
            case '7':
            {
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
            }
            case '8':
            {
                vaciar_lista_filtro(filtro.categorias);
                vaciar_lista_filtro(filtro.plataformas);
                strcpy(filtro.desarrollador, "");
                filtro.precio_max = -1;
                filtro.metacritic_min = -1;
                filtro.multijugador = -1;
                puts("Filtros limpiados correctamente.");
                presioneTeclaParaContinuar();
                break;
            }
            case '9':
            {
                vaciar_lista_filtro(filtro.categorias);
                vaciar_lista_filtro(filtro.plataformas);
                break;
            }
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
    char *tag = list_first(juego->categorias_lis);
    while (tag != NULL) {
        printf("[%s] ", tag);
        tag = list_next(juego->categorias_lis);
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

void buscar_juego(Map *grafo_juegos)
{
    char titulo[150];
    limpiarPantalla();
    puts("========================================");
    puts("           Buscar Juego");
    puts("========================================");
    printf("Ingrese el título del juego: ");
    scanf(" %149[^\n]", titulo);
    
    MapPair *pair = map_search(grafo_juegos, titulo);

    if(pair != NULL)
    {
        printf("\nNo se encontró ningún juego con el título \"%s\".\n", titulo);
        presioneTeclaParaContinuar();
        return;
    }

    Videojuego *juego = (Videojuego *)pair->value;
    puts("\n=== Juego Encontrado ===");
    mostrar_juego(juego);
    presioneTeclaParaContinuar();
}

void agregar_juego_wishlist(Map *grafo_juegos, Map *usuarios, Usuario *usuario_activo)
{
    if(usuario_activo == NULL)
    {
        puts("Debes iniciar sesión para agregar juegos a tu wishlist.");
        presioneTeclaParaContinuar();
        return;
    }

    char titulo[150];
    limpiarPantalla();
    puts("========================================");
    puts("       Agregar a Wishlist");
    puts("========================================");
    printf("Ingrese el título del juego: ");
    scanf(" %149[^\n]", titulo);

    MapPair *pair = map_search(grafo_juegos, titulo);
    if(pair == NULL)
    {
        printf("\nNo se encontró ningún juego con el título \"%s\".\n", titulo);
        presioneTeclaParaContinuar();
        return;
    }

    Videojuego *juego = (Videojuego *)pair->value;
    Videojuego *juego_aux = list_first(usuario_activo->wishlist);
    while(juego_aux != NULL)
    {
        if(strcmp(juego_aux->titulo, juego->titulo) == 0)
        {
            printf("\n\"%s\" ya está en tu wishlist.\n", juego->titulo);
            presioneTeclaParaContinuar();
            return;
        }
        juego_aux = list_next(usuario_activo->wishlist);
    }

    list_pushBack(usuario_activo->wishlist, juego);
    guardar_usuarios(usuarios);
    printf("\n\"%s\" agregado a tu wishlist con éxito.\n", juego->titulo);
    presioneTeclaParaContinuar();
}

// FUNCIONES =======================================