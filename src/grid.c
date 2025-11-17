#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "grid.h"

void grid_allocate(grid_t *g, int size) {
    g->size = size;

    // Allocation du tableau de pointeurs :
    g->grid = (char**)malloc(size * sizeof(char*));

    // Renvoit Erreur si aucun allocation a été faite.
    if (!g->grid) {
        fprintf(stderr, "takuzu: error: Memory Allocation Failed (rows)\n");
        exit(EXIT_FAILURE);
    }

    // Allocation de chaque ligne :
    for (int i = 0; i < size; i++) {
        g->grid[i] = (char*)calloc(size, sizeof(char));

        if (!g->grid[i]) {
            printf("Error : Memory Allocation Failed\n");
            exit(EXIT_FAILURE);
        }
    // On initialise la grille avec des '_' car notre get_cell refuse '\0'.
    for (int j = 0; j < size; j++) {
        g->grid[i][j] = '_';
    }
    }
}

void grid_free(const grid_t *g) {
    for (int i = 0; i < g->size; i++) {
        free(g->grid[i]);   // Libère chaque ligne
    }
    free(g->grid);          // Libère le tableau de pointeurs
}

void grid_print(const grid_t *g, FILE *fd) {
    int size = g->size;
    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            char c = g->grid[i][j];
            if (j + 1 == size) {
                fprintf(fd, "%c", c);
            }
            else {
                fprintf(fd, "%c ", c);      // espace comme séprateur après remarque pour un meilleur rendu
            }
        }
        fputs("\n", fd);
    }
}

static bool check_char(const char c) {
    return (c == '0' || c == '1' || c == '_');
}

void set_cell(int i, int j, grid_t *g, char v) {
    if (!g || !g->grid || i < 0 || j < 0 || i >= g->size || j >= g->size) {
        fprintf(stderr, "set_cell: index (%d,%d) hors bornes (size=%d)\n",
                i, j, g ? g->size : -1);
        exit(EXIT_FAILURE);
    }
    if (!check_char(v)) {
        fprintf(stderr, "set_cell: caractère  '%c' invalide (attendus: 0,1,_)\n", v);
        exit(EXIT_FAILURE);
    }
    g->grid[i][j] = v;
}

char get_cell(int i, int j, grid_t *g) {
    if (!g || !g->grid || i < 0 || j < 0 || i >= g->size || j >= g->size) {
        fprintf(stderr, "get_cell: index (%d,%d) hors bornes (size=%d)\n", i, j, g ? g->size : -1);
        exit(EXIT_FAILURE);
    }
    char c = g->grid[i][j];
    if (!check_char(c)) {
        fprintf(stderr, "get_cell: caractère stocké  '%c' invalide à (%d,%d)\n", c, i, j);
        exit(EXIT_FAILURE);
    }
    return c;
}


    // Règle n°1.1 : Pas de lignes identiques

static bool check_lines(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }

    for (int i = 0; i < g->size -1; i++) {
        for (int j = i + 1; j < g->size; j++) {
            bool identical = true;
            bool fully_defined = true;

            for(int k = 0; k < g->size; k++) {
                char lval = get_cell(i, k, g);
                char rval = get_cell(j, k, g);

                if (lval != rval) {
                    identical = false;
                    break;
                }

                if (lval == '_' || rval == '_') {
                    fully_defined = false;
                }
            }

            if (identical && fully_defined) {
                return false;
            }
        }
    }

    return true;
}

    // Règle n°1.2 : Pas de colonnes identiques

static bool check_cols(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }

    for (int col = 0; col < g->size - 1; col++) {
        for (int other = col + 1; other < g->size; other++) {

            bool identical = true;
            bool fully_defined = true;

            for (int row = 0; row < g->size; row++) {
                char c1 = get_cell(row, col, g);
                char c2 = get_cell(row, other, g);
                
                if (c1 != c2) {
                    identical = false;
                    break;
                }
                if (c1 == '_' || c2 == '_') {
                    fully_defined = false;
                }
            }
            if (identical && fully_defined) {
                return false;
            }
        }
    }
    return true;
}

    // Règle n°2.1 : Pas plus de deux 0/1 consécutifs sur les lignes

static bool check_runs_rows(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }

    // last = dernière valeur lue ('0' ou '1') et run = longueur de la série
    for (int i = 0; i < g->size; i++) {
        char last = '\0';
        int run = 0;    
        for (int j = 0; j < g->size; j++) {
            char c = get_cell(i, j, g);

            if (c == '_') {     // '_' casse la série
                last = '\0';    // on repart à zéro
                run = 0;
                continue;
            }

            if (c == last) {    // check l'élément précédent
                run++;
                if (run >= 3) {
                    return false;  // 3 identiques consécutifs -> faux
                }
            } else {
                last = c;
                run = 1;
            }
        }    
    }
    return true;
}

    // Règle n°2.2 : Pas plus de deux 0/1 consécutifs sur les colonnes


static bool check_runs_cols(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }

// last = dernière valeur lue ('0' ou '1') et run = longueur de la série
    for (int j = 0; j < g->size; j++) {
        char last = '\0';
        int run = 0;    
        for (int i = 0; i < g->size; i++) {
            char c = get_cell(i, j, g);

            if (c == '_') {     // '_' casse la série
                last = '\0';    // on repart à zéro
                run = 0;
                continue;
            }

            if (c == last) {    // check l'élément précédent
                run++;
                if (run >= 3) {
                    return false;  // 3 identiques consécutifs -> faux
                }
            } else {
                last = c;
                run = 1;
            }
        }    
    }
    return true;
}

    // Règle n°3.1 : Au plus size/2 de zéros et uns par ligne (même incomplète)

static bool check_nb_rows(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }

    int size = g->size; int half = size / 2;

    for (int i = 0; i < size; i++) {
        int zeros = 0; int ones = 0;

        for (int j = 0; j < size; j++) {
            char c = get_cell(i, j, g);

            if (c == '0' ) {
                zeros++;
            } else { 
                if (c == '1') {
                    ones++;
                }
            }
        }
        // Pas de dépassement de la taille max.
        if (zeros > half || ones > half) {
            return false;
        }
    }
    return true;
}

    // Règle n°3.2 : Pas plus de 0 et 1 par colonne que la moitié de la taille d'une colonne.

static bool check_nb_cols(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }

    int size = g->size; int half = size / 2;

    for (int col = 0; col < size; col++) {
        int zeros = 0; int ones = 0;

        for (int row = 0; row < size; row++) {
            char c = get_cell(row, col, g);

            if (c == '0' ) {
                zeros++;
            } else {
                if (c == '1') {
                    ones++;
                }
            }
        }
        // Pas de dépassement de taille max.
        if (zeros > half || ones > half) {
            return false;  
        }
    }
    return true;
}

bool is_consistent(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return true;
    }
    
    // Règle n°1
    if (!check_lines(g) || !check_cols(g)) {
        return false;  
    }
    // Règle n°2
    if (!check_runs_cols(g) || !check_runs_rows(g)) {
        return false;
    }

    // Règle n°3
    if (!check_nb_rows(g) || !check_nb_cols(g)) {
        return false; 
    }      
    
    return true;    // Si toutes les conditions sont vérifiées, renvoie True.

}

bool is_valid(grid_t *g) {
    if (!g || !g->grid || g->size <= 0) {
        return false;
    }

    int size = g->size;

    for (int i = 0; i < size; i++) {
        for (int j = 0; j < size; j++) {
            if (get_cell(i, j, g) == '_') {
                return false;   
            }
        }
    }
    return is_consistent(g);
}   
