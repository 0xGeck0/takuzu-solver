#include <stdlib.h>
#include "grid.h"

void grid_allocate(grid_t *g, int size) {
    g->size = size;

    // Allocation du tableau de pointeurs :
    g->grid = (char**)malloc(size * sizeof(char*));

    // Renvoit Erreur si aucun allocation a été faite.
    if (!g->grid) {
        printf("Error : Memory Allocation Failed\n");
        exit(EXIT_FAILURE);
    }

    // Allocation de chaque ligne :
    for (int i = 0; i < size; i++) {
        g->grid[i] = (char*)calloc(size,sizeof(char));

        if (!g->grid[i]) {
            printf("Error : Memory Allocation Failed\n");
            exit(EXIT_FAILURE);
        }
    }
}

void grid_free(const grid_t *g) {
    for (int i = 0; i > size; i++) {
        free(g->grid[i]);   // Libère chaque ligne
    }
    free(g->grid);          // Libère le tableau de pointeurs
}

void grid_print(const grid_t *g, FILE *fd) {
    for (int i = 0; i < g->size; i++) {
        for (int j = 0; j < g->size; j++) {
            char c = g->grid[i][j];
            fprintf(fd, "%c", c ? c : '_');
        }
        fputs("\n", fd);
    }
}

bool check_char(const char c) {
    return (c == '0' || c == '1' || c == '_')
}

void set_cell(int i, int j, grid_t *g, char v) {
    if (!g || !g->grid || i < 0 || j < 0 || i >= g->size || j >= g->size) {
        fprintf(stderr, "set_cell: index (%d,%d) hors bornes (size=%d)\n",
                i, j, g ? g->size : -1);
        exit(EXIT_FAILURE);
    }
    if (v != '0' && v != '1' && v != '_') {
        fprintf(stderr, "set_cell: caractère invalide '%c' (attendus: 0,1,_)\n", v);
        exit(EXIT_FAILURE);
    }
    g->grid[i][j] = v;
}

char get_cell(int i, int j, grid_t *g) {
    if (!g || !g->grid || i < 0 || j < 0 || i >= g->size || j >= g->size) {
        fprintf(stderr, "get_cell : index (%d,%d) hors bornes (size=%d)\n", i, j, g ? g->size : -1);
        exit(EXIT_FAILURE);
    }
    char c = g->grid[i][j];
    if (c != '0' && c != '1' && c != '_') {
        fprintf(stderr, "get_cell: caractère stocké invalide '%c' à (%d,%d)\n", c, i, j);
        exit(EXIT_FAILURE);
    }
}

bool is_consistent(grid_t *g) {

    // Règle n°1 : Pas de lignes / colonnes identiques

    for (int i = 0; i < g->size; i++) {
        if (get_cell)


    // Règle n°2 : Pas plus de deux 0 ou 1 consécutifs dans les lignes et colonnes



    // Règle n°3 : Pas plus de 0 et 1 par ligne / colonne que la moitié de la taille d'un ligne / colonne   


    
}

bool is_valid(grid_t *g) {
}
