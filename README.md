# ANOTHER GAME? 🎮
> Team Reineta — Iván García · Renato Bermúdez · Nicolás Torres · Dillan Orellana
> Ingeniería Civil Informática — Estructura de Datos 2025

---

## Objetivo del programa:
Another Game? es una app de consola para recomendar videojuegos similares a los que has jugado y donde podrás gestionar una wishlist personal.
La idea principal es que alguien que terminó un juego que le encantó pueda buscar y encontrar juegos parecidos a ese, basándose en etiquetas, plataforma, estudio y categorías.
El sistema usa mapas para la búsqueda rápida, un grafo de similitud para las recomendaciones, listas para la wishlist y una pila como historial para deshacer acciones.

---

## Cómo compilar el programa:

Para compilar y ejecutar el programa necesitas tener GCC instalado y seguir estos pasos:

1. Clona el repositorio. Asegúrate de que el archivo `dataset_juegos.csv` esté en la misma carpeta que los `.c`. (es obligatorio tener el archivo)

2. Compilación:
```
gcc Principal.c grafo.c pantallas.c TDAs/*.c -Wno-unused-result -o Principal
```

3. Ejecución:
```
./Principal
```

> Si ya existe `conexiones_grafo.txt` en la carpeta, el programa lo carga directo y arranca rápido.
> Si no existe, genera las conexiones del grafo al iniciar (puede tardar unos segundos la primera vez).

---

## STRUCTS:

### 1. Videojuego:
> Guarda toda la información de un juego leído desde el CSV. Está definido en `grafo.h`.
> Los campos más importantes son:
> - id, titulo, desarrollador, fecha_lanzamiento
> - categorias_lis (List) y categorias_con (Set) — (son los tags del juego)
> - plataformas_lis (List) y plataformas_con (Set)
> - precio, metacritic, es_multijugador
> - juegos_similares (List de Arista*) — (son sus conexiones en el grafo)

### 2. Arista:
> Representa una conexión entre dos juegos en el grafo de similitud. Está en `grafo.h`.
> Guarda:
> - juego_vecino (puntero al Videojuego conectado)
> - peso_similitud (cuántas características o campos comparten los dos juegos)

### 3. Usuario:
> Guarda los datos de un usuario registrado. Está en `Principal.c`.
> Guarda:
> - nombre (como se muestra en pantalla)
> - nombre_llave (en minúsculas, para poder buscar en el mapa y guardarlo en el archivo)
> - clave (hash de la contraseña, no se guarda en texto plano)
> - wishlist (List de Videojuego* pendientes)
> - historial (Stack de acciones para el deshacer)

### 4. Filtro:
> Struct auxiliar para la búsqueda avanzada. Está en `Principal.c`.
> Guarda todos los criterios activos al mismo tiempo: categorias, plataformas, desarrollador, precio_max, metacritic_min, multijugador.

---

## FUNCIONES:

### 1. Cargar Juegos:
> Lee el `dataset_juegos.csv` fila por fila y guarda cada juego en el mapa principal indexado por ID.
> También arma el índice invertido de categorías, que después usa el grafo para calcular similitudes.

### 2. Generar / Cargar Conexiones:
> Genera el grafo de similitud recorriendo el índice invertido: si dos juegos comparten una etiqueta, se conectan y se suma 1 al peso.
> Si ya existe `conexiones_grafo.txt`, se carga desde ahí para no realizar calculos innecesarios.

### 3. Iniciar Sesión / Registro:
> Pide al usuario el nombre de usuario y lo busca en el mapa. Si existe, pide la contraseña (tiene hasta 3 intentos).
> Si no existe, se le ofrece registrarse y crear una nueva cuenta.
> Las contraseñas se guardan con hash (djb2), no en texto plano.

### 4. Buscar Juego:
> Le pide al usuario el nombre del juego y lo busca en el mapa. Si lo encuentra, lo muestra en la pantalla con todos sus datos y los 4 más recomendados.
> Si no existe, muestra un aviso y da la opción de ingresar el nombre de nuevo.

### 5. Filtrar Juegos:
> Permite combinar varios criterios a la vez (categoría, plataforma, desarrollador, precio máximo, metacritic mínimo, multijugador) y luego lista todos los juegos que los cumplen.

### 6. Agregar a Wishlist:
> Pide al usuario el nombre del juego, verifica que no esté ya en la lista y lo agrega. (también es necesario haber ingresado sesión)
> Guarda automáticamente en `usuarios.csv` para que persista entre ejecuciones.

### 7. Módulo de Pantallas (pantallas.c / pantallas.h):
> Contiene todas las vistas del programa (17 pantallas en total).
> Con cada uso de alguna función se limpia la pantalla primero, luego dibuja la caja de 60 caracteres y retorna un char con lo que eligió el usuario.
> De esta forma la app se ve como una sola pantalla que va cambiando, en vez de mensajes apilándose.
> Las pantallas incluyen: menú principal, buscar juego, ficha del juego, filtros, resultados, recomendaciones, wishlist (ver / agregar / eliminar / vacía), login, registro, deshacer y despedida.

---

## Ejemplo de uso:

> 1. Se ejecuta el programa y aparece el menú principal.
> 2. El usuario elige la opción 2 para buscar un juego por su nombre (ej: "Counter-Strike 2").
> 3. Si lo encuentra, muestra la ficha completa: precio, metacritic, plataformas, tags y los 4 más recomendados.
> 4. Desde ahí puede elegir pedir una recomendación o volver al menú.
> 5. Para poder usar la wishlist, primero inicia sesión con la opción 1 (o cuando el sistema te lo pida).
> 6. Entra a la opción 5 → Agregar un juego para guardarlo en tu lista.
> 7. Cierra y vuelve a abrir el programa: tu wishlist sigue guardada en `usuarios.csv`.

---

## Estado de las funciones:

1. **Cargar juegos desde CSV**: FUNCIONANDO
2. **Generar grafo de similitud**: FUNCIONANDO
3. **Guardar / Cargar grafo desde archivo**: FUNCIONANDO
4. **Iniciar sesión / Registro**: FUNCIONANDO
5. **Guardar / Cargar usuarios**: FUNCIONANDO
6. **Buscar juego por nombre**: FUNCIONANDO
7. **Mostrar ficha del juego (pantalla hero)**: FUNCIONANDO
8. **Filtros avanzados**: FUNCIONANDO
9. **Ver wishlist**: FUNCIONANDO
10. **Agregar a wishlist**: FUNCIONANDO
11. **Eliminar de wishlist**: PENDIENTE — Renato
12. **Recomendaciones (opción del menú)**: PENDIENTE — Kato / Dillan
13. **Deshacer última acción**: PENDIENTE — Iván
14. **Módulo de pantallas / TUI completo**: FUNCIONANDO

---

## Dificultades y Soluciones:

### 1. Pantallas mezcladas con la lógica en Principal.c
**Problema:** Las funciones de dibujo de cajas estaban sueltas dentro de Principal.c, junto con la lógica del menú.

> 26/06 - 14:00 > Con 17 pantallas distintas en el proyecto, tener todo junto en un solo archivo se iba a hacer inmanejable rápidamente. Había que separarlo.

> **SOLUCIONADO:** 26/06 - 15:30 > Se creó el módulo pantallas.c / pantallas.h siguiendo el mismo patrón de los TDAs del profe (con su propio #ifndef). Cada pantalla es una función independiente que limpia, dibuja y retorna la opción elegida. Principal.c quedó solo con la lógica.

### 2. Campo inexistente en el struct Videojuego
**Problema:** La función mostrar_juego usaba `juego->plataformas` como si fuera un string, pero ese campo no existía en el struct.

> 26/06 > Al revisar grafo.h nos dimos cuenta que las plataformas se guardan como lista (plataformas_lis) y como conjunto (plataformas_con), no como un string directo.

> **SOLUCIONADO:** 26/06 > Se cambió para recorrer plataformas_lis con list_first / list_next e imprimir cada plataforma separada por coma.
