//juego.c se usara para la parte lógica del juego, se complementara con ciertas funciones
//en nasm para optimizar ciertas tareas como conteo de monedas, validacion de movimientos
#include "juego.h"
#include <conio.h>   // _getch() para Windows 


//Limpia la pantalla usando comando del sistema
void limpiar_pantalla(void) {
    system("cls");
}

//copia un mapa de src a dest
void copiar_mapa(char dest[FILAS_MAPA][COLS_MAPA],
                 const char src[FILAS_MAPA][COLS_MAPA]) {
    for (int r = 0; r < FILAS_MAPA; r++)
        for (int c = 0; c < COLS_MAPA; c++)
            dest[r][c] = src[r][c];
}

//busca la posicion inicial 'P' del jugador en el mapa
void buscar_jugador(char mapa[FILAS_MAPA][COLS_MAPA], Jugador *j) {
    for (int r = 0; r < FILAS_MAPA; r++) {
        for (int c = 0; c < COLS_MAPA; c++) {
            if (mapa[r][c] == JUGADOR) {
                j->fila = r;
                j->col  = c;
                return;
            }
        }
    }
    /*fallback esquina superior izquierda */
    j->fila = 1;
    j->col  = 1;
}

//imprime la ventana visible de 20x20 centrada en el jugador, si el jugador esta cerca de los bordes la ventana se ajustra
void imprimir_ventana(char mapa[FILAS_MAPA][COLS_MAPA], Jugador *j) {
    //calcular esquina superior izq de la ventana 
    int fila_ini = j->fila - FILAS_VENTANA / 2;
    int col_ini  = j->col  - COLS_VENTANA  / 2;

    //ajusramos limites del mapa para no salirnos
    if (fila_ini < 0)                       fila_ini = 0;
    if (col_ini  < 0)                       col_ini  = 0;
    if (fila_ini + FILAS_VENTANA > FILAS_MAPA) fila_ini = FILAS_MAPA - FILAS_VENTANA;
    if (col_ini  + COLS_VENTANA  > COLS_MAPA)  col_ini  = COLS_MAPA  - COLS_VENTANA;

    //borde superior de ventana
    printf("+");
    for (int c = 0; c < COLS_VENTANA; c++) printf("-");
    printf("+\n");

    //imprimir las filas visibles
    for (int r = fila_ini; r < fila_ini + FILAS_VENTANA; r++) {
        printf("|");
        for (int c = col_ini; c < col_ini + COLS_VENTANA; c++) {
            char celda = mapa[r][c];
            //mostramos la posicion del jugador real
            if (r == j->fila && c == j->col)
                printf("P");
            else
                printf("%c", celda);
        }
        printf("|\n");
    }

    //borde inf
    printf("+");
    for (int c = 0; c < COLS_VENTANA; c++) printf("-");
    printf("+\n");
}

//imprime info del jugador y nivel en la parte inferior de la pantalla
void imprimir_hud(Jugador *j, EstadoNivel *en) {
    printf("  Nivel: %d  |  Monedas: %d/%d  |  Llave: %s  |  Pasos: %d\n",
           en->nivel_num,
           j->monedas_recogidas,
           en->total_monedas,
           j->tiene_llave ? "SI" : "No",
           j->pasos);
    printf("  Celdas libres en mapa: %d\n", en->celdas_libres);
    printf("  [W/A/S/D] Mover   [Q] Salir\n");
}

//la logica del movimiento del jugador

//intenta mover al jugador en la direccion indicada, usa las funciones NASM para validar y detectar objetos.

void mover_jugador(char mapa[FILAS_MAPA][COLS_MAPA],
                   Jugador *j, EstadoNivel *en, char dir) {
    int nueva_fila = j->fila;
    int nueva_col  = j->col;

    switch (dir) {
        case 'W': case 'w': nueva_fila--; break;
        case 'S': case 's': nueva_fila++; break;
        case 'A': case 'a': nueva_col--;  break;
        case 'D': case 'd': nueva_col++;  break;
        default: return;
    }

    //verificar si hay una puerta en la nueva posicion antes de validar el movimiento
    //llamamos a funcion de nasm
    long long hay_puerta = detectar_objeto(
        &mapa[0][0], COLS_MAPA, nueva_fila, nueva_col, PUERTA);

    if (hay_puerta) {
        if (!j->tiene_llave) {
            //no se puede pasar
            return;
        } else {
            //abrimos puerta y se convierte en camino
            mapa[nueva_fila][nueva_col] = CAMINO;
        }
    }

    //validar mov con nasm
    long long valido = validar_movimiento(
        &mapa[0][0], COLS_MAPA, nueva_fila, nueva_col);

    if (!valido) return;

    //retirar al jugador de su posicion actual
    mapa[j->fila][j->col] = CAMINO;

    //detectar moneda en la nueva posicion con nasm 4
    long long hay_moneda = detectar_objeto(
        &mapa[0][0], COLS_MAPA, nueva_fila, nueva_col, MONEDA);
    if (hay_moneda) {
        j->monedas_recogidas++;
        mapa[nueva_fila][nueva_col] = CAMINO;
    }

    //detectamos llave nasm 4
    long long hay_llave = detectar_objeto(
        &mapa[0][0], COLS_MAPA, nueva_fila, nueva_col, LLAVE);
    if (hay_llave) {
        j->tiene_llave = 1;
        mapa[nueva_fila][nueva_col] = CAMINO;
    }

    //movemos al jugador
    j->fila = nueva_fila;
    j->col  = nueva_col;
    j->pasos++;

    //colocar P en la nueva posicion 
    mapa[j->fila][j->col] = JUGADOR;
}

//todos los resumenes

void mostrar_resumen_nivel(Jugador *j, EstadoNivel *en) {
    printf("\n=================================\n");
    printf("  Nivel %d completado\n", en->nivel_num);
    printf("  Monedas recolectadas: %d / %d\n",
           j->monedas_recogidas, en->total_monedas);
    printf("  Pasos realizados: %d\n", j->pasos);
    printf("=================================\n");
    printf("\n  Presiona cualquier tecla para continuar...\n");
    _getch();
}

void mostrar_resumen_final(ResumenFinal *rf) {
    printf("\n=================================\n");
    printf("  Juego completado!\n");
    printf("  Monedas totales: %d / %d\n",
           rf->monedas_total, rf->monedas_posibles);
    printf("  Pasos totales:   %d\n", rf->pasos_total);
    printf("  Niveles completados: %d\n", rf->niveles_completados);
    printf("  Puntaje final:   %d\n", rf->puntaje_final);
    printf("=================================\n");
}

void mostrar_menu(void) {
    limpiar_pantalla();
    printf("=============================================\n");
    printf("   BITQUEST: Explorador de Matrices\n");
    printf("         C + NASM 64 bits\n");
    printf("=============================================\n");
    printf("\n");
    printf("  Controles:\n");
    printf("    W / A / S / D  -> Mover jugador\n");
    printf("    Q              -> Salir del juego\n");
    printf("\n");
    printf("  Objetivo:\n");
    printf("    Recolecta monedas, encuentra la llave,\n");
    printf("    abre la puerta y llega a la salida [E].\n");
    printf("\n");
    printf("  Simbolos del mapa:\n");
    printf("    #  Pared       .  Camino libre\n");
    printf("    P  Jugador     M  Moneda\n");
    printf("    K  Llave       D  Puerta\n");
    printf("    E  Salida\n");
    printf("\n");
    printf("  Presiona cualquier tecla para comenzar...\n");
    _getch();
}

//bucle principal de un nivel 

//ejecuta un nivel completo.
//retorna 1 si el jugador completo el nivel, 0 si salio
int jugar_nivel(char mapa[FILAS_MAPA][COLS_MAPA],
                int num_nivel, ResumenFinal *rf) {
    Jugador j;
    EstadoNivel en;

    //inicializamos al jugador
    j.fila             = 0;
    j.col              = 0;
    j.tiene_llave      = 0;
    j.monedas_recogidas = 0;
    j.pasos            = 0;

    //buscamos la posicion inicial del jugador en el mapa
    buscar_jugador(mapa, &j);

    //inicializar estado del nivel
    en.nivel_num = num_nivel;

    //FUNCION NASM 1: contar_caracter-total de monedas
    en.total_monedas = (int)contar_caracter(
        &mapa[0][0], TOTAL_CELDAS, MONEDA);

    //FUNCION NASM 5: contar_libres-celdas libres
    en.celdas_libres = (int)contar_libres(
        &mapa[0][0], TOTAL_CELDAS);

    en.completado = 0;

    //Bucle del juego
    while (1) {
        limpiar_pantalla();
        imprimir_ventana(mapa, &j);
        imprimir_hud(&j, &en);

        char tecla = (char)_getch();

        if (tecla == 'Q' || tecla == 'q') {
            return 0;  /* Jugador salio */
        }

        //verificamos si la nueva posicion es la salida ANTES de mover
        int nf = j.fila, nc = j.col;
        if      (tecla=='W'||tecla=='w') nf--;
        else if (tecla=='S'||tecla=='s') nf++;
        else if (tecla=='A'||tecla=='a') nc--;
        else if (tecla=='D'||tecla=='d') nc++;

        //FUNCION NASM 4: detectamos salida
        long long es_salida = detectar_objeto(
            &mapa[0][0], COLS_MAPA, nf, nc, SALIDA);

        if (es_salida) {
            //movemos al jugador a la salida
            mapa[j.fila][j.col] = CAMINO;
            j.fila = nf;
            j.col  = nc;
            j.pasos++;
            en.completado = 1;

            //mostramos resumen del nivel
            limpiar_pantalla();
            imprimir_ventana(mapa, &j);
            imprimir_hud(&j, &en);

            mostrar_resumen_nivel(&j, &en);

            //actualizamos resumen final
            rf->monedas_total    += j.monedas_recogidas;
            rf->monedas_posibles += en.total_monedas;
            rf->pasos_total      += j.pasos;
            rf->niveles_completados++;
            return 1;
        }
        mover_jugador(mapa, &j, &en, tecla);
    }
}