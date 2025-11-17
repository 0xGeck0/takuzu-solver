#ifndef GENERATE_H
#define GENERATE_H

#include "grid.h"

#define MAX_FIND_RETRIES 20     // Sécurité pour ne pas boucler à l'infini

void generate_grid(grid_t *g, int size, int percentage);

#endif /* GENERATE_H */