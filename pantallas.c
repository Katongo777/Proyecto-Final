// pantallas.c
// Aca van todas las pantallas del programa.
// Cada funcion limpia la pantalla, dibuja la caja y retorna la opcion.
// - Ivan

#include "pantallas.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// =====================================================
//  CONSTRUCCIÓN DE CAJA
// =====================================================

// Linea de borde superior/inferior: +====...====+
void linea_borde() {
  printf("+");
  for (int i = 0; i < ANCHO_CAJA; i++) printf("=");
  printf("+\n");
}

// Linea interna vacia: |          |
void linea_vacia() {
  printf("|");
  for (int i = 0; i < ANCHO_CAJA; i++) printf(" ");
  printf("|\n");
}

// Texto alineado a la izquierda, si no cabe se corta con "..."
void linea_texto(char *texto) {
  char linea[ANCHO_CAJA + 1];
  int margen = 2;
  int disponible = ANCHO_CAJA - margen;
  int largo = strlen(texto);

  for (int i = 0; i < margen; i++) linea[i] = ' ';
  if (largo <= disponible) {
    for (int i = 0; i < largo; i++) linea[margen + i] = texto[i];
    for (int i = margen + largo; i < ANCHO_CAJA; i++) linea[i] = ' ';
  }
  else {
    // no cabe, cortamos y ponemos "..."
    int corte = disponible - 3;
    for (int i = 0; i < corte; i++) linea[margen + i] = texto[i];
    linea[margen + corte]     = '.';
    linea[margen + corte + 1] = '.';
    linea[margen + corte + 2] = '.';
  }
  linea[ANCHO_CAJA] = '\0';
  printf("|%s|\n", linea);
}

// Texto centrado dentro de la caja
void linea_centrada(char *texto) {
  char linea[ANCHO_CAJA + 1];
  int largo = strlen(texto);
  if (largo > ANCHO_CAJA) largo = ANCHO_CAJA;
  int izq = (ANCHO_CAJA - largo) / 2;
  int der = ANCHO_CAJA - largo - izq;
  int p = 0;
  for (int i = 0; i < izq; i++) linea[p++] = ' ';
  for (int i = 0; i < largo; i++) linea[p++] = texto[i];
  for (int i = 0; i < der; i++) linea[p++] = ' ';
  linea[ANCHO_CAJA] = '\0';
  printf("|%s|\n", linea);
}

// =====================================================
//  HELPERS DEL PANEL DIVIDIDO (solo se usan en este archivo)
//  Pantalla del juego: columna izquierda (info) + derecha (acciones)
//  Izquierda: 33 chars | Derecha: 24 chars | Total: 58 = ANCHO_CAJA
// =====================================================

// Borde del panel dividido: +=====...=====+=====...=====+
void linea_borde_split() {
  printf("+");
  for (int i = 0; i < 33; i++) printf("=");
  printf("+");
  for (int i = 0; i < 24; i++) printf("=");
  printf("+\n");
}

// Fila de dos columnas: | izquierda (31 chars) | derecha (22 chars) |
// Si el texto no cabe en la columna se corta automaticamente
void linea_dos_col(char *izq, char *der) {
  printf("|  %-31.31s|  %-22.22s|\n", izq, der);
}

// =====================================================
//  PANTALLAS
// =====================================================

// Pantalla 1: Menu principal
// usuario = NULL si es invitado, o el nombre si ya inicio sesion
char pantalla_menu_principal(char *usuario) {
  limpiarPantalla();
  char saludo[80];
  char opcion;

  if (usuario == NULL) sprintf(saludo, "Bienvenido, invitado.");
  else sprintf(saludo, "Bienvenido, %s.", usuario);

  linea_borde();
  linea_centrada("ANOTHER GAME?");
  linea_borde();
  linea_vacia();
  linea_texto(saludo);
  linea_vacia();
  linea_texto("[1] Iniciar sesion");
  linea_texto("[2] Buscar juego");
  linea_texto("[3] Filtrar juegos");
  linea_texto("[4] Recomendaciones");
  linea_texto("[5] Mi wishlist");
  linea_texto("[6] Deshacer ultima accion");
  linea_texto("[7] Salir");
  linea_vacia();
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 2: Pedir nombre del juego a buscar
// El resultado queda en buffer, el llamador revisa si esta vacio
char pantalla_buscar_nombre(char *buffer) {
  limpiarPantalla();

  linea_borde();
  linea_texto("BUSCAR JUEGO");
  linea_borde();
  linea_vacia();
  linea_texto("Escribe el nombre del juego que buscas:");
  linea_vacia();
  linea_texto("(deja vacio y Enter para volver)");
  linea_borde();

  printf(" > ");
  buffer[0] = '\0';
  scanf(" %149[^\n]", buffer);
  return '1';
}

// Pantalla 3: Hero screen del juego con panel dividido
// Muestra info a la izquierda y acciones a la derecha
// Abajo muestra los primeros recomendados de la lista de adyacencia
// (Dillan: ojala puedas ordenarlos por similitud antes de llamar esto xd)
char pantalla_ver_juego(Videojuego *juego) {
  limpiarPantalla();
  char temp[120];  // buffer grande para construir strings
  char opcion;

  // --- construimos el string de plataformas juntando la lista ---
  char plats[80] = "";
  char *plat = list_first(juego->plataformas_lis);
  while (plat != NULL) {
    // si cabe lo agregamos, si no cortamos con "..."
    if (strlen(plats) + strlen(plat) + 2 < 70) {
      if (strlen(plats) > 0) strcat(plats, ", ");
      strcat(plats, plat);
    }
    else {
      strcat(plats, "...");
      break;
    }
    plat = list_next(juego->plataformas_lis);
  }

  // ---- header: titulo ocupa el ancho completo ----
  linea_borde();
  linea_texto(juego->titulo);
  linea_borde_split();

  // ---- panel dividido: info a la izquierda, acciones a la derecha ----
  linea_dos_col("", "ACCIONES");
  linea_dos_col("", "");

  snprintf(temp, sizeof(temp), "Desarrollador: %s", juego->desarrollador);
  linea_dos_col(temp, "[1] Agregar wishlist");

  snprintf(temp, sizeof(temp), "Precio: $%.2f", juego->precio);
  linea_dos_col(temp, "[2] Recomendaciones");

  snprintf(temp, sizeof(temp), "Metacritic: %d", juego->metacritic);
  linea_dos_col(temp, "[3] Buscar otro");

  snprintf(temp, sizeof(temp), "Multijugador: %s", juego->es_multijugador ? "Si" : "No");
  linea_dos_col(temp, "[4] Volver");

  snprintf(temp, sizeof(temp), "Lanzamiento: %s", juego->fecha_lanzamiento);
  linea_dos_col(temp, "");

  snprintf(temp, sizeof(temp), "Plataformas: %s", plats);
  linea_dos_col(temp, "");

  // ---- etiquetas (varias filas en la columna izquierda) ----
  linea_dos_col("", "");
  linea_dos_col("Etiquetas:", "");

  // acumulamos etiquetas en filas que quepan en la columna izquierda (31 chars)
  char fila_tags[32] = "";
  int pos = 0;
  char *tag = list_first(juego->categorias_lis);
  while (tag != NULL) {
    char bloque[50];
    snprintf(bloque, sizeof(bloque), "[%s] ", tag);
    int largo = strlen(bloque);

    if (pos == 0 || pos + largo <= 28) {
      strcat(fila_tags, bloque);
      pos += largo;
    }
    else {
      // fila llena, la imprimimos y empezamos otra
      linea_dos_col(fila_tags, "");
      strncpy(fila_tags, bloque, sizeof(fila_tags) - 1);
      fila_tags[sizeof(fila_tags) - 1] = '\0';
      pos = largo;
    }
    tag = list_next(juego->categorias_lis);
  }
  // la ultima fila de tags
  if (strlen(fila_tags) > 0) linea_dos_col(fila_tags, "");

  linea_borde_split();

  // ---- recomendados (top 4 de la lista de adyacencia) ----
  linea_texto("RECOMENDADOS");
  Arista *arista = list_first(juego->juegos_similares);
  int i = 1;
  while (arista != NULL && i <= 4) {
    snprintf(temp, sizeof(temp), "  %d) %-25.25s (similitud %d)",
             i, arista->juego_vecino->titulo, arista->peso_similitud);
    linea_texto(temp);
    arista = list_next(juego->juegos_similares);
    i++;
  }
  if (i == 1) linea_texto("  No hay recomendaciones para este juego.");

  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 4: Juego no encontrado
char pantalla_juego_no_encontrado() {
  limpiarPantalla();
  char opcion;

  linea_borde();
  linea_texto("BUSCAR JUEGO");
  linea_borde();
  linea_vacia();
  linea_texto("X  No se encontro ningun juego con ese nombre.");
  linea_texto("   Revisa la escritura e intentalo de nuevo.");
  linea_vacia();
  linea_borde();
  linea_texto("[1] Buscar otra vez          [2] Volver al menu");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 5: Menu de filtros (elegir criterio)
char pantalla_menu_filtro() {
  limpiarPantalla();
  char opcion;

  linea_borde();
  linea_texto("FILTRAR JUEGOS");
  linea_borde();
  linea_vacia();
  linea_texto("Elige el criterio:");
  linea_vacia();
  linea_texto("[1] Por categoria / genero");
  linea_texto("[2] Por plataforma");
  linea_texto("[3] Por estudio (desarrollador)");
  linea_texto("[4] Filtro avanzado (combinar varios)");
  linea_texto("[5] Volver");
  linea_vacia();
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 7: Resultados de una busqueda o filtro
// lista: lista de Videojuego*, total: cantidad, desc_filtro: "Accion + PC" por ejemplo
char pantalla_resultados(List *lista, int total, char *desc_filtro) {
  limpiarPantalla();
  char header[80];
  char opcion;

  snprintf(header, sizeof(header), "RESULTADOS (%d)   Filtro: %s", total, desc_filtro);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_texto("  #  TITULO                       METACRITIC");
  linea_texto("  -  -------------------------    ----------");

  Videojuego *juego = list_first(lista);
  int i = 1;
  while (juego != NULL) {
    char fila[80];
    snprintf(fila, sizeof(fila), "  %d  %-29.29s %d", i, juego->titulo, juego->metacritic);
    linea_texto(fila);
    juego = list_next(lista);
    i++;
  }

  if (i == 1) {
    linea_vacia();
    linea_texto("No se encontraron juegos con ese criterio.");
    linea_vacia();
  }

  linea_borde();
  linea_texto("[#] Ver juego                    [V] Volver");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 8: Recomendaciones top N para un juego
// aristas: lista de Arista* ya ordenadas por peso (idealmente)
char pantalla_recomendaciones(List *aristas) {
  limpiarPantalla();
  char opcion;

  linea_borde();
  linea_texto("RECOMENDACIONES");
  linea_borde();
  linea_texto("  #  TITULO                    SIMILITUD");
  linea_texto("  -  ----------------------    ---------");

  Arista *arista = list_first(aristas);
  int i = 1;
  while (arista != NULL) {
    char fila[80];
    snprintf(fila, sizeof(fila), "  %d  %-24.24s %d",
             i, arista->juego_vecino->titulo, arista->peso_similitud);
    linea_texto(fila);
    arista = list_next(aristas);
    i++;
  }

  if (i == 1) {
    linea_vacia();
    linea_texto("No hay recomendaciones disponibles.");
    linea_vacia();
  }

  linea_borde();
  linea_texto("[#] Ver juego    [A] Agregar wishlist    [V] Volver");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 9: Aviso de que necesita sesion para la wishlist
char pantalla_necesitas_sesion() {
  limpiarPantalla();
  char opcion;

  linea_borde();
  linea_texto("MI WISHLIST");
  linea_borde();
  linea_vacia();
  linea_texto("!  Para usar la wishlist necesitas iniciar sesion.");
  linea_vacia();
  linea_texto("[1] Iniciar sesion / registrarme");
  linea_texto("[2] Volver al menu");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 10: Menu de la wishlist (ya con sesion iniciada)
char pantalla_menu_wishlist(char *usuario) {
  limpiarPantalla();
  char header[80];
  char opcion;

  snprintf(header, sizeof(header), "MI WISHLIST                    Usuario: %s", usuario);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_vacia();
  linea_texto("[1] Ver mi wishlist");
  linea_texto("[2] Agregar un juego");
  linea_texto("[3] Eliminar un juego");
  linea_texto("[4] Volver");
  linea_vacia();
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 11: Ver la wishlist del usuario
char pantalla_ver_wishlist(List *wishlist, char *usuario) {
  limpiarPantalla();
  char header[80];
  char opcion;

  // contamos los juegos de la wishlist
  int total = 0;
  Videojuego *aux = list_first(wishlist);
  while (aux != NULL) {
    total++;
    aux = list_next(wishlist);
  }

  snprintf(header, sizeof(header), "MI WISHLIST (%d)            Usuario: %s", total, usuario);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_texto("  #  TITULO                       ESTUDIO");
  linea_texto("  -  -------------------------    -------");

  Videojuego *juego = list_first(wishlist);
  int i = 1;
  while (juego != NULL) {
    char fila[80];
    snprintf(fila, sizeof(fila), "  %d  %-28.28s  %s", i, juego->titulo, juego->desarrollador);
    linea_texto(fila);
    juego = list_next(wishlist);
    i++;
  }

  linea_borde();
  linea_texto("[#] Ver juego      [E] Eliminar      [V] Volver");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 12: Wishlist vacia
char pantalla_wishlist_vacia(char *usuario) {
  limpiarPantalla();
  char header[80];
  char opcion;

  snprintf(header, sizeof(header), "MI WISHLIST (0)            Usuario: %s", usuario);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_vacia();
  linea_texto("Tu wishlist esta vacia.");
  linea_texto("Busca juegos y agregalos para verlos aqui.");
  linea_vacia();
  linea_borde();
  linea_texto("[1] Buscar juego              [2] Volver");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 13: Agregar un juego a la wishlist
// El nombre buscado queda en buffer
char pantalla_agregar_wishlist(char *usuario, char *buffer) {
  limpiarPantalla();
  char header[80];

  snprintf(header, sizeof(header), "AGREGAR A WISHLIST             Usuario: %s", usuario);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_vacia();
  linea_texto("Escribe el nombre del juego a agregar:");
  linea_vacia();
  linea_borde();

  printf(" > ");
  buffer[0] = '\0';
  scanf(" %149[^\n]", buffer);
  return '1';
}

// Pantalla 14: Eliminar un juego de la wishlist
// El numero elegido queda en num_elegido (0 = volver)
char pantalla_eliminar_wishlist(List *wishlist, char *usuario, int *num_elegido) {
  limpiarPantalla();
  char header[80];

  snprintf(header, sizeof(header), "ELIMINAR DE WISHLIST           Usuario: %s", usuario);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_texto("  Tu wishlist:");
  linea_vacia();

  Videojuego *juego = list_first(wishlist);
  int i = 1;
  while (juego != NULL) {
    char fila[80];
    snprintf(fila, sizeof(fila), "  %d) %s", i, juego->titulo);
    linea_texto(fila);
    juego = list_next(wishlist);
    i++;
  }

  linea_vacia();
  linea_texto("Numero del juego a eliminar (0 para volver):");
  linea_borde();

  printf(" > ");
  scanf("%d", num_elegido);
  return '1';
}

// Pantalla 15: Iniciar sesion (pedir nombre)
// El nombre queda en buffer, la contrasena la pide la funcion de login
char pantalla_iniciar_sesion(char *buffer) {
  limpiarPantalla();

  linea_borde();
  linea_texto("INICIAR SESION");
  linea_borde();
  linea_vacia();
  linea_texto("Ingresa tu nombre de usuario:");
  linea_vacia();
  linea_texto("(si no existe, te ofrecemos registrarte)");
  linea_borde();

  printf(" > ");
  buffer[0] = '\0';
  scanf(" %49[^\n]", buffer);
  return '1';
}

// Pantalla 16: Registro de usuario nuevo
// '1' = crear cuenta, '2' = no, volver
char pantalla_registro(char *nombre) {
  limpiarPantalla();
  char msg[80];
  char opcion;

  snprintf(msg, sizeof(msg), "El usuario \"%s\" no existe. Crear cuenta?", nombre);

  linea_borde();
  linea_texto("REGISTRO");
  linea_borde();
  linea_vacia();
  linea_texto(msg);
  linea_vacia();
  linea_texto("[1] Si, crear cuenta y entrar");
  linea_texto("[2] No, volver");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 17: Deshacer ultima accion
// desc = descripcion de lo que se va a deshacer, ej: "Agregaste Hades a tu wishlist"
// '1' = si deshacer, '2' = no, volver
char pantalla_deshacer(char *desc, char *usuario) {
  limpiarPantalla();
  char header[80];
  char msg[80];
  char opcion;

  snprintf(header, sizeof(header), "DESHACER ULTIMA ACCION    Usuario: %s", usuario);
  snprintf(msg, sizeof(msg), "Ultima accion: %s", desc);

  linea_borde();
  linea_texto(header);
  linea_borde();
  linea_vacia();
  linea_texto(msg);
  linea_vacia();
  linea_texto("Deshacer esta accion?");
  linea_texto("[1] Si, deshacer              [2] No, volver");
  linea_borde();

  printf(" > ");
  scanf(" %c", &opcion);
  return opcion;
}

// Pantalla 18: Despedida al salir
void pantalla_despedida() {
  limpiarPantalla();

  linea_borde();
  linea_vacia();
  linea_centrada("Gracias por usar ANOTHER GAME?  Vuelve!");
  linea_vacia();
  linea_borde();
}
