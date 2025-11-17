#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>       // pour l'aléatoire rand()
#include "generate.h"
#include "grid.h"

void generate_grid(grid_t *g, int size, int percentage) {

    // 1. Allocation de la grille
    grid_allocate(g,size);

    // Calcule du ratio, car l'utilisateur donnera un int
    double fill_ratio = (double)percentage / 100.0;
    if (fill_ratio < 0.0) {
        fill_ratio = 0.0;
    }

    if (fill_ratio > 1.0) {
        fill_ratio = 1.0;
    }

    int total_cells = size * size;
    int cells_to_fill = (int)(total_cells * fill_ratio);

    for (int k = 0; k < cells_to_fill; k++) {
        int i, j;
        int retries = 0;

        // 2. Trouver une case vide aléatoire
        // On boucle tant que la case (i,j) n'est pas '_'

        do {
            i = rand() % size; // résultat compris entre 0 et size - 1
            j = rand() % size;

            // On crée une sécurité si on ne trouve pas de case vide
            if (retries++ > MAX_FIND_RETRIES) {
                break;
            }
        } while (get_cell(i, j, g) != '_') ;
        
        // Puisque do {} while tourne au moins une fois, on regarde si la sécurité s'est activée
        if (get_cell(i, j, g) != '_') {
            continue;
        }

        // 3. Choix d'un symbole aléatoire ('0' ou '1')
        char char1 = (rand() % 2 == 0) ? '0' : '1';
        char char2 = (char1 == '0') ? '1' : '0';  

        // 4. On essaye de placer notre premier symbole
        set_cell(i, j, g, char1);
        if (is_consistent(g)) {
            continue;
        }

        // 5. Premier symbole échoué. On essaye le second
        set_cell(i, j, g, char2);
        if (is_consistent(g)) {
            continue;
        }

        // 6. Les deux symboles ont échoué
        set_cell(i, j, g, '_');
    }
}