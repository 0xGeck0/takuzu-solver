#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include "heuristics.h"

static char opp(char c) { return (c == '0') ? '1' : '0'; }

static bool apply_double_rule(grid_t *g) {     // Heuristique 1 : La règle du double

    bool changed = false;   // Pour savoir si on change un truc ou pas dans la grille
    int n = g->size;

    // Ici on parcourt toutes les lignes
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n - 1; j++) {   // n-1 pour n'avoir aucune lecture hors-borne pour les paires consécutives 
            char a = get_cell(i, j, g);
            char b = get_cell(i, j + 1, g);

            // On va ignorer les cases vides pour cette règle
            if (a == '_' || b == '_') {
                continue;
            }

            // Si on a deux cases consécutives identiques
            if (a == b) {

                // Avant le bloc
                if (j > 0 && get_cell(i,j - 1, g) == '_') {             // Condition j > 0 pour éviter j = 0
                    set_cell(i, j - 1, g, opp(a));
                    changed = true;
                }

                //Après le bloc
                if (j + 2 < n && get_cell(i,j + 2, g) == '_') {         // Condition j + 2 < n pour éviter j = n
                    set_cell(i, j + 2, g, opp(a));
                    changed = true;
                }
            }
        }
    }

    // Ici on parcourt toutes les colonnes (même logique)
    for (int j = 0; j < n; j++) {
        for (int i = 0; i < n - 1; i++) {   // n-1 pour n'avoir aucune lecture hors-borne pour les paires consécutives 
            char a = get_cell(i, j, g);
            char b = get_cell(i + 1, j, g);

            // On va ignorer les cases vides pour cette règle
            if (a == '_' || b == '_') {
                continue;
            }

            // Si on a deux cases consécutives identiques
            if (a == b) {

                // Avant le bloc
                if (i > 0 && get_cell(i - 1,j, g) == '_') {             // Condition i > 0 pour éviter i = 0
                    set_cell(i - 1, j, g, opp(a));
                    changed = true;
                }

                //Après le bloc
                if (i + 2 < n && get_cell(i + 2,j, g) == '_') {         // Condition j + 2 < n pour éviter j = n
                    set_cell(i + 2, j, g, opp(a));
                    changed = true;
                }
            }
        }
    }

    

    return changed;
}


static bool apply_full_rule(grid_t *g) {       // Heuristique 2 : La règle du full
    bool changed = false;
    int n = g->size;
    int half = n / 2;

    // On vérifie toutes les lignes
    for (int i = 0; i < n; i++) {
        int zeros = 0, ones = 0;    //
        for (int j = 0; j < n; j++) {
            char c = get_cell(i, j, g);

            if (c == '0') {
                zeros++;
            } else {
                if (c == '1') {
                    ones++;
                }
            }
        }

        if (zeros == half) {
            for (int k = 0; k < n; k++) {
                if (get_cell(i, k, g) == '_') {
                    set_cell(i, k, g, '1');
                    changed = true;
                }
            }
        }

        if (ones == half) {
            for (int k = 0; k < n; k++) {
                if (get_cell(i, k, g) == '_') {
                    set_cell(i, k, g, '0');
                    changed = true;
                }
            }
        }
    }

    // Et on vérifie toutes les colonnes
    for (int j = 0; j < n; j++) {
        int zeros = 0, ones = 0;
        for (int i = 0; i < n; i++) {
            char c = get_cell(i, j, g);
            if (c == '0') {
                zeros++;
            } else {
                 if (c == '1') {
                    ones++;
                 }
            }
        }
        if (zeros == half) {
            for (int i = 0; i < n; i++) {
                if (get_cell(i, j, g) == '_') {
                    set_cell(i, j, g, '1');
                    changed = true;
                }
            }
        }
        if (ones == half) {
            for (int i = 0; i < n; i++) {
                if (get_cell(i, j, g) == '_') {
                    set_cell(i, j, g, '0');
                    changed = true;
                }
            }
        }
    }
    return changed;
}    


static bool apply_sandwich_rule(grid_t *g) {   // Heuristique 3 : La règle du sandwich, 1 _ 1 ou 0 _ 0.
    bool changed = false;
    int n = g->size;

    // Lignes : motif a _ a donc on veut au mileu opp(a)
    for (int i = 0; i < n; i++) {
        for (int j = 0; j + 2 < n; j++) {

            char a = get_cell(i, j, g);
            char milieu = get_cell(i, j + 1, g);
            char b = get_cell(i, j + 2, g);

            if (milieu == '_' && a != '_' && a == b) {
                set_cell(i, j + 1, g, opp(a));
                changed = true;
            }
        }
    }

    // Colonnes : motif a _ a donc on veut au mileu opp(a)
    for (int j = 0; j < n; j++) {
        for (int i = 0; i + 2 < n; i++) {

            char a      = get_cell(i, j, g);
            char milieu = get_cell(i + 1, j, g);
            char b      = get_cell(i + 2, j, g);

            if (milieu == '_' && a != '_' && a == b) {
                set_cell(i + 1, j, g, opp(a));
                changed = true;
            }
        }
    }
    return changed;
}



bool apply_all_heuristics(grid_t *g) {  // Boucle principale : Celle qui applique toutes les heuristiques 
    bool changed = false;
    do {
        changed = false;
        if (apply_double_rule(g)) {
            changed = true;
        }
        if (apply_sandwich_rule(g)) {
            changed = true;
        }
        if (apply_full_rule(g)) {
            changed = true;
        }
    } while (changed);
    return true;
}