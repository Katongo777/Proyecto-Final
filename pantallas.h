// pantallas.h
// Aca van todos los prototipos de las pantallas del programa.
// Es lo mismo que grafo.h pero para las pantallas.
// El #ifndef es para que si alguien incluye este archivo dos veces no explote - Ivan

#ifndef PANTALLAS_H
#define PANTALLAS_H

#include "grafo.h"
#include "TDAs/extra.h"
#include "TDAs/list.h"

// El ancho de la caja va aca, ya no en Principal.c
#define ANCHO_CAJA 58

// Helpers de caja (las funciones que dibujan las lineas)
void linea_borde();
void linea_vacia();
void linea_texto(char *texto);
void linea_centrada(char *texto);

// ===================================================
// PANTALLAS
// Cada una limpia la pantalla, dibuja la caja y
// retorna la opcion que eligio el usuario (char).
// Las que piden texto meten el resultado en un buffer.
// ===================================================

char pantalla_menu_principal(char *usuario);     // NULL si es invitado
char pantalla_buscar_nombre(char *buffer);       // guarda el nombre en buffer
char pantalla_ver_juego(Videojuego *juego);      // la pantalla hero del juego
char pantalla_juego_no_encontrado();
char pantalla_menu_filtro();
char pantalla_resultados(List *lista, int total, char *desc_filtro);
char pantalla_recomendaciones(List *aristas);
char pantalla_necesitas_sesion();
char pantalla_menu_wishlist(char *usuario);
char pantalla_ver_wishlist(List *wishlist, char *usuario);
char pantalla_wishlist_vacia(char *usuario);
char pantalla_agregar_wishlist(char *usuario, char *buffer);
char pantalla_eliminar_wishlist(List *wishlist, char *usuario, int *num_elegido);
char pantalla_iniciar_sesion(char *buffer);
char pantalla_registro(char *nombre);
char pantalla_deshacer(char *desc, char *usuario);
void pantalla_despedida();

#endif
