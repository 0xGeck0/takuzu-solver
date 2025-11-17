#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <ctype.h>
#include "grid_io.h"
#include "grid.h"

// Retourne vrai si la taille (nb de colonnes) est autorisée
static bool is_allowed_size(int n) { return (n == 4 || n == 8 || n == 16 || n == 32 || n == 64);}

// Un séparateur valide entre caractères significatifs : espace ou tabulation.
static int is_sep(int c) { return (c == ' ' || c == '\t'); }

// Caractères significatifs acceptés dans la grille.
static int is_sig(int c) { return (c == '0' || c == '1' || c == '_'); }


// Affiche un message d’erreur standardisé, nettoie et quitte le programme.
static void errors_exit(FILE *f, grid_t *g, char *filename, int line, char *msg) {
    if (line > 0) {
        fprintf(stderr, "takuzu: error: '%s': %s at line %d!\n", filename, msg, line);
    } else {
        fprintf(stderr, "takuzu: error: '%s': %s\n", filename, msg);
    }

    // Ferme le fichier si ouvert
    if (f) {
        fclose(f);
    }       
    
    // Libère la grille si allouée
    if (g && g->grid) {
        grid_free(g);
    }    

    exit(EXIT_FAILURE); // Termine avec error
}


/* Parse un fichier de grille Takuzu et remplir notre grille *g.
   Règles gérées :
   - On ignore les commentaires '#' jusqu’à fin de ligne ;
   - On autorise espaces/tabs entre caractères significatifs ;
   - ON impose ≥1 séparateur entre deux significatifs ;
   - Les caractères valides : '0','1','_' ;
   - La taille détectée sur 1re ligne significative (doit être 4/8/16/32/64) ;
   - La grille doit être carrée. */
void file_parser(grid_t *g, char *filename) {
    FILE *f = fopen(filename, "r");
    if (!f) {
        fprintf(stderr, "takuzu: error: cannot open '%s'\n", filename);
        exit(EXIT_FAILURE);
    }

    int size    = -1;   // Nb de colonnes, déterminer à la 1re ligne significative
    int row     = 0;    // Nb de lignes significatives déjà stockées
    int line_no = 0;    // Compteur de lignes lues dans le fichier

    char row_vals[MAX_GRID_SIZE];   // Tampon pour la ligne courante (pour les caractères significatifs)
    char buffer[1024];              // Tampon pour la lecture (fgets    )


    // Lecture du fichier ligne par ligne :
    while (fgets(buffer, sizeof buffer, f)) {
        line_no++;

        // Coupe la ligne au 1er '#' : tout ce qui suit est du commentaire
        for (char *p = buffer; *p; ++p) {
            if (*p == '#') {
                *p = '\0'; break;
            }
        }

        int  sig_count = 0;                 // Nb de caractères significatifs trouvés sur la ligne lue.
        bool need_sep_before_sig = false;   // Impose au moins un séparateur en chaque caractères significatifs.


        // Parcourt la ligne caractères par caractères :
        for (char *p = buffer; *p; ++p) {
            unsigned char c = (unsigned char)*p;

            // fin de la ligne
            if (c == '\n') {
                break;
            }  

            if (is_sep(c)) {
                // On vient de voir un séparateur alors on autorise un nouveau caractère significatif
                need_sep_before_sig = false;
                continue;
            }

            if (is_sig(c)) {
                // Deux caractères significatifs consécutifs sans séparateurs entraîne une erreur
                if (need_sep_before_sig) {
                    errors_exit(f, g, filename, line_no,
                                "missing separator between significant characters");
                }
                // Sécurité : cela évite de dépasser notre tampon de caractères
                if (sig_count >= MAX_GRID_SIZE) {
                    errors_exit(f, g, filename, line_no,
                                "too many characters on this line");
                }
                row_vals[sig_count++] = (char)c;    // On empile ce caractère
                need_sep_before_sig = true;         // On exige un sérateur avec le prochain caractère lu
                continue;
            }

            // Check pour un caractère non valide.
            char msg[64];
            if (isprint(c)) {
                snprintf(msg, sizeof msg, "wrong character '%c'", c);
            } else {
                snprintf(msg, sizeof msg, "wrong character 0x%02X", c);
            }
            errors_exit(f, g, filename, line_no, msg);
        }

        // Pour une ligne vide, on passe.
        if (sig_count == 0) {
            continue;
        }


        // Première ligne significative : on fixe la taille et on alloue la grille.
        if (size < 0) {
            size = sig_count;
            if (!is_allowed_size(size)) {
                errors_exit(f, g, filename, line_no,
                            "line is malformed (wrong number of columns)");
            }
            grid_allocate(g, size);
        } else {

            // Pour les lignes suivantes : elles doivent avoir exactement 'size' colonnes.
            if (sig_count != size) {
                errors_exit(f, g, filename, line_no,
                            "line is malformed (wrong number of columns)");
            }
        }

        // Grille carré obligatoire
        if (row >= size) {
            errors_exit(f, g, filename, line_no, "grid has too many lines");
        }

        // On a juste à recopier notre tampon local vers la grille.
        for (int j = 0; j < size; ++j) {
            set_cell(row, j, g, row_vals[j]);
        }

        row++;  // Ligne validée
    }

    fclose(f);

    // Si aucune ligne significative n'a été lue    
    if (size < 0) {
        fprintf(stderr, "takuzu: error: '%s': no grid data found\n", filename);
        exit(EXIT_FAILURE);
    }

    // Check du carré : nb de ligne lues = nb de colonnes.
    if (row != size) {
        int missing = size - row;   // supérieur à 0 si lignes manquantes.
        if (missing > 0) {
            char msg[64];
            snprintf(msg, sizeof msg, "grid has %d missing lines!", missing);
            errors_exit(NULL, g, filename, 0, msg);
        } else {
            errors_exit(NULL, g, filename, 0, "grid has too many lines");
        }
    }
}
