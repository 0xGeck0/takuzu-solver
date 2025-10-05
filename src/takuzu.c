#include <stdio.h>
#include <stdlib.h>
#include "takuzu.h"

int main(int argc, char *argv[]) {
    printf("Takuzu solver / generator prototype\n");
    printf("Arguments reçus : %d\n", argc - 1);

    if (argc == 1) {
        fprintf(stderr, "takuzu: error: no input grid given!\n");
        return EXIT_FAILURE;
    }

    // Pour l'instant, juste afficher le premier argument
    printf("Fichier d'entrée : %s\n", argv[1]);

    return EXIT_SUCCESS;
}