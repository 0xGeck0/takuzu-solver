#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <getopt.h>
#include <errno.h>
#include "grid.h"
#include "grid_io.h"
#include "takuzu.h"

static bool is_allowed(int n) { return (n == 4 || n == 8 || n == 16 || n == 32 || n == 64);}

static void print_usage(FILE *out) {
    fprintf(out,
        "Usage:\n"
        "  takuzu [-a|-o FILE|-h] FILE\n"
        "  takuzu -g[SIZE] [-u|-o FILE|-h]\n"
        "Solve or generate takuzu grids of size: 4, 8, 16, 32, 64\n"
        "  -a, --all               search for all possible solutions\n"
        "  -g[N], --generate[=N]   generate a grid of size NxN (default: 8)\n"
        "  -o FILE, --output FILE  write output to FILE\n"
        "  -u, --unique            generate a grid with unique solution\n"
        "  -h, --help              display this help and exit\n");
}

int main(int argc, char *argv[]) {

    // Config pour les options
    bool generator_mode = false;   // -g
    bool all = false;              // -a (uniquement le solver)
    bool unique = false;           // -u (uniquement le générateur)
    int  gsize = 8;                // -gN (par défaut 8 si -g sans N)
    char *outfile = NULL;          // -o FILE
    char *infile  = NULL;          // argument positionnel pour le solver




    // Options longues : 
    static struct option long_opts[] = {
        {"all",      no_argument,       0, 'a'},
        {"generate", optional_argument, 0, 'g'}, // -g, -g[N], --generate, --generate=N
        {"output",   required_argument, 0, 'o'},
        {"unique",   no_argument,       0, 'u'},
        {"help",     no_argument,       0, 'h'},
        {0,0,0,0}
    };


    // Chaîne d’options courtes :
    //  a : sans argument
    //  g : argument optionnel
    //  o :  argument requis
    //  u : sans argument
    //  h : sans argument

    int opt, idx;
    while ((opt = getopt_long(argc, argv, "ag::o:uh", long_opts, &idx)) != -1) {
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
                        fprintf(stderr, "takuzu: error: invalide grid size '%s'\n", optarg);
                        return(EXIT_FAILURE);
                    }
                    gsize = (int)n;
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
        fprintf(out, "Generator mode selected. Size=%d, unique=%s\n",
                gsize, unique ? "true" : "false");
    } else {
        // Solver mode : on parse le fichier d’entrée pour tester l’enchaînement
        grid_t g;
        file_parser(&g, infile);           // lit et remplit la grille
        grid_print(&g, out);               // simple affichage pour vérif
        grid_free(&g);
    }

    if (out && out != stdout) fclose(out);
    return EXIT_SUCCESS;
}


































// int main(void) {
//     grid_t g;

//     printf("== Test grid_allocate / grid_print ==\n");
//     grid_allocate(&g, 8);
//     grid_print(&g, stdout);

//     printf("\n== Test set_cell / get_cell ==\n");
//     set_cell(0, 0, &g, '1');
//     set_cell(0, 1, &g, '0');
//     set_cell(0, 2, &g, '1');
//     set_cell(0, 3, &g, '0');
//     printf("Row 0: ");
//     for (int j = 0; j < g.size; j++)
//         printf("%c ", get_cell(0, j, &g));
//     printf("\n\n");

//     printf("== Test règles Takuzu ==\n\n");
//     printf("check_lines : %s\n", check_lines(&g) ? "OK" : "FAIL");
//     printf("check_cols  : %s\n", check_cols(&g) ? "OK" : "FAIL");
//     printf("check_runs_rows : %s\n", check_runs_rows(&g) ? "OK" : "FAIL");
//     printf("check_nb_rows   : %s\n", check_nb_rows(&g) ? "OK" : "FAIL");
//     printf("is_consistent   : %s\n", is_consistent(&g) ? "OK" : "FAIL");
//     printf("is_valid        : %s\n", is_valid(&g) ? "OK" : "FAIL");

//     grid_free(&g);
//     return 0;
// }