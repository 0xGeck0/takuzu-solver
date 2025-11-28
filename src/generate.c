#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <time.h>       // pour l'aléatoire rand()
#include "generate.h"
#include "grid.h"
#include "solver.h"

// Alogrithme pour mélanger un tableau
static void shuffle(int *coords, int n) {
    for (int i = n - 1; i > 0; i--) {
        int j = rand() % (i + 1);
        int temporaire = coords[i];
        coords[i] = coords[j];
        coords[j] = temporaire;
    }
}

void generate_grid(grid_t *g, int size, int percentage, bool unique) {

    // 1. Allocation de la grille
    grid_allocate(g,size);

    // --- OPTIMISATION VITESSE (16, 32, 64) ---
    // Pour les grandes tailles, on aide le solveur en pré-remplissant la ligne 0
    // avec un motif valide (0011...) décalé aléatoirement.
    // Cela rend la génération instantanée au lieu de "réfléchir" indéfiniment.
    if (size > 8) {
        int offset = rand() % size; 
        for (int j = 0; j < size; j++) {
            // Motif périodique 0011 décalé
            int val_idx = (j + offset) % 4;
            char val = (val_idx < 2) ? '0' : '1';
            set_cell(0, j, g, val);
        }
    }
    
    // 2. Utilisation du solveur pour trouver une grille pleine et valide.
    // Remarque : On ouvre /dev/null pour ne pas afficher notre solveur
    FILE *null_out = fopen("/dev/null", "w");
    if (!null_out) {
        null_out = stdout;
    }

    long count = 0;
    // On demande une seule solution (all=false)
    grid_t *full_solution = grid_solver(g, false, &count, null_out);

    if (!full_solution) {
        fprintf(stderr, "takuzu: error: could not generate base grid.\n");
        if (null_out != stdout) {
            fclose(null_out);
        }
        return;
    }

    // On copie notre solution pleine dans notre grille 'g'.
    grid_copy(full_solution, g);
    grid_free(full_solution);
    free(full_solution);

    // 3. Et maintenant on creuse la grille

    int total_cells = size * size;
    double fill_ratio = (double)percentage / 100.0;
    int target_filled = (int)(total_cells * fill_ratio);
    int current_filled = total_cells;

    // Création d'une liste de toutes les coordonnées de notre grille
    int *coords = malloc(total_cells * sizeof(int));
    if (!coords) {
        fprintf(stderr, "Memory allocation failed\n");
        exit(EXIT_FAILURE);
    }

    // Elle remplit notre tableau coords avec tous les numéros de cases possibles, dans l'ordre.
    for (int i = 0; i < total_cells; i++) {
        coords[i] = i;
    }

    // Pour ainsi mélanger ces coords avec notre algorithme 
    shuffle(coords, total_cells);

    // On parcourt toutes les cases dans l'ordre mélangé
    for (int k = 0; k < total_cells; k++) {
        // Si on a atteint l'objectif de remplissage, on arrête
        if (current_filled <= target_filled) {
            break;
        }

        int index = coords[k];
        int r = index / size;
        int c = index % size;

        char old_value = get_cell(r, c, g);
        set_cell(r, c, g, '_');

        if (unique) {
            // Mode Unique : On doit vérifier si la grille a toujours exactement 1 solution
            long solution_count = 0;

            // On lance notre solveur en mode 'ALL SOLUTION' sur notre grille 'g' actuelle
            grid_solver(g, true, &solution_count, null_out);

            if (solution_count != 1) {
                // Création d'une ambiguïté : On remet la valeur 
                set_cell(r, c, g, old_value);
            } else {
                current_filled--;
            }
        } else {
            // Mode Normal : on supprime sans vérification
            current_filled--;
        }
    }

    free(coords);
    if (null_out != stdout) {
        fclose(null_out);
    }
}