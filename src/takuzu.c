#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include <time.h>
#include "grid.h"
#include "grid_io.h"
#include "takuzu.h"
#include "heuristics.h"
#include "generate.h"

static bool is_allowed(int n) { return (n == 4 || n == 8 || n == 16 || n == 32 || n == 64);}

static int count_blanks(const grid_t *g) {      // Cette fonction static ne sert juste qu'à compter les blanks laissé après passage du solver.
    int n = g->size, blanks = 0;

    for (int i = 0; i < n; ++i)
        for (int j = 0; j < n; ++j)
            if (get_cell(i, j, (grid_t*)g) == '_') {
                blanks++;
            }
    return blanks;
}

static void print_usage(FILE *out) {
    fprintf(out,
        "Usage:\n"
        "  ./takuzu [-a|-o FILE|-h] FILE\n"
        "  ./takuzu -g[SIZE] -p[PERCENTAGE] [-u|-o FILE|-h]\n"
        "Solve or generate takuzu grids of size: 4, 8, 16, 32, 64\n"
        "   -a, --all               search for all possible solutions\n"
        "   -g[N], --generate[=N]   generate a grid of size NxN (default: 8)\n"
        "   -p[N], --percent[=N]    set generator fill percentage (default: 50)\n"
        "   -o FILE, --output FILE  write output to FILE\n"
        "   -u, --unique            generate a grid with unique solution\n"
        "   -h, --help              display this help and exit\n");
}

int main(int argc, char *argv[]) {

    srand(time(NULL));  // Pour initialiser l'aléatoire

    // Config pour les options
    bool generator_mode = false;    // -g
    bool all = false;               // -a (uniquement le solver)
    bool unique = false;            // -u (uniquement le générateur)
    int  gsize = 8;                 // -gN (par défaut 8 si -g sans N)
    int percentage = 50;            // -p N (par défaut 50%)
    char *outfile = NULL;           // -o FILE
    char *infile  = NULL;           // argument positionnel pour le solver


    // Options longues : 
    static struct option long_opts[] = {
        {"all",         no_argument,       0, 'a'},
        {"generate",    optional_argument, 0, 'g'}, // -g, -g[N], --generate, --generate=N
        {"percent",     required_argument, 0, 'p'},
        {"output",      required_argument, 0, 'o'},
        {"unique",      no_argument,       0, 'u'},
        {"help",        no_argument,       0, 'h'},
        {0,0,0,0}
    };


    // Chaîne d’options courtes :
    //  a : sans argument
    //  g : argument optionnel
    //  p : argument optionnel
    //  o : argument requis
    //  u : sans argument
    //  h : sans argument

    int opt, idx;
    while ((opt = getopt_long(argc, argv, "ag::o:uhp:", long_opts, &idx)) != -1) {
        switch (opt) {

            case 'a':
                all = true;
                break;

            case 'g':
                generator_mode = true;
                if (optarg) {
                    char *end = NULL;
                    long n = strtol(optarg, &end, 10);
                    if (*end != '\0' || n <= 0) {
                        fprintf(stderr, "takuzu: error: invalid grid size '%s'\n", optarg);
                        return(EXIT_FAILURE);
                    }
                    gsize = (int)n;
                }
                break;

            case 'p':
                {   
                    if (!optarg) {
                        fprintf(stderr, "takuzu: error: option -p requires an argument (0-100)\n");
                        return(EXIT_FAILURE);
                    }

                    char *end = NULL;
                    long n = strtol(optarg, &end, 10);
                    if (*end != '\0' || n < 0 || n > 100) {
                        fprintf(stderr, "takuzu: error: invalide percentage '%s' (must be 0-100)\n",optarg);
                        return(EXIT_FAILURE);
                    }
                    percentage = (int)n;
                }
                break;

            case 'o':
                outfile = optarg;
                break;

            case 'u':
                unique = true;
                break;

            case 'h':
                print_usage(stdout);
                return EXIT_SUCCESS;
            default:
                print_usage(stderr);
                return EXIT_FAILURE;
        }
    }
    
    // Récupération de l’argument positionnel restant
    if (optind < argc) {
        infile = argv[optind];
        // On ignore des arguments en trop si besoin, ou on les refuse
        if (optind + 1 < argc) {
            fprintf(stderr, "takuzu: warning: extra arguments ignored after '%s'\n", infile);
        }
    }


    //------------------------------
    // Commandes pour chaque options
    //------------------------------


    // 1) Mode par défaut : solver, sauf si -g est présent
    if (generator_mode) {
        // -g : on vérifie la taille,
        if (!is_allowed(gsize)) {
            fprintf(stderr, "takuzu: error: invalid grid size %d (valid: 4,8,16,32,64)\n", gsize);
            return EXIT_FAILURE;
        }

        // Ici on check si conflit avec -a :
        if (all) {
            fprintf(stderr, "takuzu: warning: option 'all' conflicts with generator mode, exiting!\n");
            return EXIT_FAILURE;
        }

        // OK avec -u
    } else {
        // Nous sommes en mode solver donc conflit avec -u
        if (unique) {
            fprintf(stderr, "takuzu: warning: option 'unique' conflicts with solver mode, exiting!\n");
            return EXIT_FAILURE;
        }

        // Fichier d'entré nécessaire 
        if (!infile) {
            fprintf(stderr, "takuzu: error: no input grid given!");
            return EXIT_FAILURE;
        }
    }

    // 2) -o : gestion du fichier de sortie
    FILE *out = stdout;
    if (outfile) {
        out = fopen(outfile, "w");
        if (!out) {
            fprintf(stderr, "takuzu: error: cannot open output file '%s': %s\n",
                    outfile, strerror(errno));
        return EXIT_FAILURE;
        }
    }

    if (generator_mode) {
        grid_t g;
        generate_grid(&g, gsize, percentage); // On appelle notre fonction
        grid_print(&g, out);                  // On affiche la grille générée
        grid_free(&g);                        // On libère la mémoire

    } else {
        // Solver mode : on parse le fichier d’entrée pour tester l’enchaînement
        grid_t g;
        file_parser(&g, infile);           // lit et remplit la grille
        if (!is_consistent(&g)) {
            fprintf(stderr, "takuzu: error: grid is not consistent!\n");
            grid_free(&g);
            if (out != stdout) {
                fclose(out);
            }
            return EXIT_FAILURE;
        }

        // Applique les heuristiques
        apply_all_heuristics(&g);

        // Verdict avec affichage
        int blanks = count_blanks(&g);
        if (!is_consistent(&g)) {
            fprintf(stderr, "takuzu: error: grid became inconsistent after solving.\n");
            grid_free(&g);
            if (out != stdout) {
                fclose(out);
            return EXIT_FAILURE;
            }
        }   

        if (blanks == 0) {
            fprintf(out, "✅ Solved!\n");
            grid_print(&g, out);
        } else {
            fprintf(out, "🟡 Partial solve (%d blanks remain)\n", blanks);
            grid_print(&g, out);
        }        
        grid_free(&g);
    }

    if (out != stdout) {
        fclose(out);
        return EXIT_SUCCESS;
    }    
}