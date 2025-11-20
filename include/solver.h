#ifndef SOLVER_H
#define SOLVER_H
#include <stdio.h>
#include "grid.h"

// Structure représentant un choix (coords + valeur)
typedef struct {
    int row;
    int column;
    char choice; // '0' ou '1'
} choice_t;

void grid_choice_apply(grid_t *grid, const choice_t choice);
void grid_choice_print(const choice_t choice, FILE *fd);
choice_t grid_choice(grid_t *grid);
grid_t *grid_solver(grid_t *grid, bool all_solutions, long *count, FILE *out);


#endif /* SOLVER_H */