#ifndef JUEGO_H
#define JUEGO_H

//juego .h se usara para constantes tipos y prototipos
   
#include <stdlib.h>

//diimensiones del mapa y ventana visible
#define FILAS_MAPA    60
#define COLS_MAPA     60
#define FILAS_VISTA   20
#define COLS_VISTA    20
#define TOTAL_CELDAS  (FILAS_MAPA * COLS_MAPA)
#define NUM_NIVELES   3

//simbolos q aparecen en mapa
#define SIM_PARED    '#'
#define SIM_CAMINO   '.'
#define SIM_JUGADOR  'P'
#define SIM_MONEDA   'M'
#define SIM_LLAVE    'K'
#define SIM_PUERTA   'D'
#define SIM_SALIDA   'E'

//estructura del estado del jugador y su progreso en el nivel
typedef struct {
    int fila;
    int col;
    int tiene_llave;
    int monedas;
    int pasos;
} Jugador;

//la structura de resumen por nivel 
typedef struct {
    int monedas_recolectadas;
    int monedas_totales;
    int pasos;
} ResumenNivel;

//prototipos de funciones
void limpiar_pantalla(void);
void imprimir_hud(int nivel, const Jugador *j, int total_monedas, int celdas_libres);
void imprimir_vista(const char mapa[FILAS_MAPA][COLS_MAPA], const Jugador *j);
void mostrar_resumen_nivel(int nivel, const ResumenNivel *r);
void mostrar_resumen_final(const ResumenNivel resumen[], int puntaje);
int  procesar_input(char tecla, Jugador *j, char mapa[FILAS_MAPA][COLS_MAPA]);
int  jugar_nivel(int num_nivel, char mapa[FILAS_MAPA][COLS_MAPA], ResumenNivel *resumen);
void mostrar_pantalla_inicio(void);
void mostrar_pantalla_victoria(void);

//prototipos para el NASM
extern long long contar_caracter(const char *mapa, long long total, char caracter);
extern long long validar_movimiento(const char *mapa, long long cols, long long fila, long long col);
extern long long calcular_puntaje(long long monedas, long long pasos, long long niveles);
extern long long detectar_objeto(const char *mapa, long long cols, long long fila, long long col, char objeto);
extern long long contar_celdas_libres(const char *mapa, long long total);

#endif 