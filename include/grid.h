#ifndef GRID_H
#define GRID_H
#include <string.h>
#include <stdbool.h>
#include <stdio.h>


typedef struct {
    int size;   // Nombre d'éléments dans un ligne
    char **grid; // Tableau de pointeurs vers lignes
} grid_t;

void grid_allocate(grid_t *g, int size);
void grid_free(const grid_t *g);
void grid_print(const grid_t *g, FILE *fd);
void set_cell(int i, int j, grid_t *g, char v);
char get_cell(int i, int j, grid_t *g);
bool is_consistent(grid_t *g);
bool is_valid(grid_t *g);

#endif /* GRID_H */