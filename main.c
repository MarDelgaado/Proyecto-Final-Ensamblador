//PROYECTO HIBRIDO LENGUAJE ENSAMBLADOR
//PARTE EN LENGUAJE C

#include <stdio.h>
#include <string.h>
#include "juego.h"
#include "mapas.h"

int main(void) {
    ResumenNivel resumen[NUM_NIVELES];
    long long    puntaje_final;
    int          total_monedas_rec = 0;
    int          total_pasos       = 0;

    char nivel1[FILAS_MAPA][COLS_MAPA];
    char nivel2[FILAS_MAPA][COLS_MAPA];
    char nivel3[FILAS_MAPA][COLS_MAPA];

    memcpy(nivel1, mapa_nivel1, sizeof(mapa_nivel1));
    memcpy(nivel2, mapa_nivel2, sizeof(mapa_nivel2));
    memcpy(nivel3, mapa_nivel3, sizeof(mapa_nivel3));

    mostrar_pantalla_inicio();

    //level 1
    limpiar_pantalla();
    printf("\n  === NIVEL 1: El Primer Laberinto ===\n");
    printf("  Familiarizate con los controles.\n");
    printf("  Presiona ENTER para comenzar...\n");
    getchar();

    if (!jugar_nivel(1, nivel1, &resumen[0])) {
        printf("\n  Hasta la proxima!\n");
        return 0;
    }
    mostrar_resumen_nivel(1, &resumen[0]);

    //level 2
    limpiar_pantalla();
    printf("\n  === NIVEL 2: El Laberinto Espiral ===\n");
    printf("  El camino se tuerce. Encuentra la llave.\n");
    printf("  Presiona ENTER para comenzar...\n");
    getchar();

    if (!jugar_nivel(2, nivel2, &resumen[1])) {
        printf("\n  Hasta la proxima\n");
        return 0;
    }
    mostrar_resumen_nivel(2, &resumen[1]);

    //level 3
    limpiar_pantalla();
    printf("\n  === NIVEL 3: Las Habitaciones ===\n");
    printf("  Un laberinto de salas. La salida esta lejos.\n");
    printf("  Presiona ENTER para comenzar...\n");
    getchar();

    if (!jugar_nivel(3, nivel3, &resumen[2])) {
        printf("\n  Hasta la proxima!\n");
        return 0;
    }
    mostrar_resumen_nivel(3, &resumen[2]);

    //contar todos
    for (int i = 0; i < NUM_NIVELES; i++) {
        total_monedas_rec += resumen[i].monedas_recolectadas;
        total_pasos       += resumen[i].pasos;
    }

    //calcular puntaje pero con NASM
    puntaje_final = calcular_puntaje(
        (long long)total_monedas_rec,
        (long long)total_pasos,
        (long long)NUM_NIVELES
    );

    //pantalla win y resumen
    mostrar_pantalla_victoria();
    mostrar_resumen_final(resumen, (int)puntaje_final);

    printf("\n  Gracias por jugar BitQuest!\n\n");
    return 0;
}
