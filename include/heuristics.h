#ifndef HEURISTICS_H
#define HEURISTICS_H
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "grid.h"


bool apply_double_rule(grid_t *g);      // Heuristique 1
bool apply_full_rule(grid_t *g);        // Heuristique 2
bool apply_sandwich_rule(grid_t *g);    // Heuristique 3
bool apply_all_heuristics(grid_t *g);   // Boucle principale



#endif /* HEURISTICS_H */