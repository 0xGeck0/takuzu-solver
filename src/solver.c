#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "grid.h"
#include "solver.h"
#include "heuristics.h"

// Applique le choix unique sur la grille
void grid_choice_apply(grid_t *grid, const choice_t choice) {
    set_cell(choice.row, choice.column, grid, choice.choice);
}

// Afficher le choix fait sur un flux donné
void grid_choice_print(const choice_t choice, FILE *fd) {
    fprintf(fd, "Choice (%d,%d) -> '%c'\n", choice.row, choice.column, choice.choice);
}

// Sélectionne un choix à faire 
// Stratégie simple pour l'instant : trouver la première case vide et on propose '0'
choice_t grid_choice(grid_t *grid) {
    choice_t c;
    c.row = -1;     // Valeur par défaut si aucune case vide trouvée
    c.column = -1;
    c.choice = '_';

    if (!grid || !grid->grid) {
        return c;
    }

    for (int i = 0; i < grid->size; i++) {
        for (int j = 0; j < grid->size; j++) {
            if (get_cell(i, j, grid) == '_') {
                c.row = i;
                c.column = j;
                c.choice = (rand() % 2) ? '0' : '1';
                return c;
            }
        }
    }
    return c;
}

grid_t *grid_solver(grid_t *grid, bool all_solutions, long *count, FILE *out) {
    // 1. On travaille sur une COPIE pour ne pas modifier l'original lors des tests
    grid_t *work = (grid_t *)malloc(sizeof(grid_t));
    work->grid = NULL;
    work->size = 0;
    grid_copy(grid, work);

    // 2. On applique les heuristiques
    apply_all_heuristics(work);

    // 3. Vérification de l'état de notre grid de travail
    if (!is_consistent(work)) {
        // Impasse : la grille est fausse
        grid_free(work);
        free(work);
        return NULL;
    }

    if (is_valid(work)) {
        if (all_solutions) {
            // Mode ALL : On affiche, on compte et on backtrack (return NULL) pour chercher encore
            *count += 1;
            fprintf(out, "Solution %ld\n", *count);
            grid_print(work, out);
            fprintf(out, "Grid for solution %ld\n", *count);

            grid_free(work);
            free(work);
            return NULL;
        } else {
            // Mode FIRST : On a gagné, on renvoie la solution
            return work;
        }
    }   

    // 4. Backtracking

    choice_t c = grid_choice(work);

    // Sécurité : si aucune case n'est trouvée alors que 'is_valid' a échoué
    if (c.row == -1) {
        grid_free(work);
        free(work);
        return NULL;
    }

    // --- Branche 1 : On essaye la valeur proposée (ex: '0') ---
    grid_choice_apply(work, c);

    // Appel récursif
    grid_t *solved = grid_solver(work, all_solutions, count, out);

    if (solved != NULL && !all_solutions) {
        // Une solution a été trouvée dans cette branche !
        grid_free(work); // On n'a plus besoin de notre copie de travail
        free(work);
        return solved;   // On fait remonter la solution
    }

    // --- Branche 2 : Si branche 1 a échoué, on essaye l'OPPOSÉ (ex: '1') ---
    // On annule le choix précédent en écrasant la valeur
    char other_choice = (c.choice == '0') ? '1' : '0';
    set_cell(c.row, c.column, work, other_choice);

    // Appel récursif sur l'autre branche
    solved = grid_solver(work, all_solutions, count, out);

    if (solved != NULL && !all_solutions) {
        grid_free(work);
        free(work);
        return solved;
    }

    // --- Échec total : Aucune branche ne marche ---
    grid_free(work);
    free(work);
    return NULL;
}