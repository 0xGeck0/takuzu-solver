#include <stdio.h>
#include <stdlib.h>
#include "takuzu.h"
#include "grid.h"

int main(void) {
    grid_t g;

    printf("== Test grid_allocate / grid_print ==\n");
    grid_allocate(&g, 4);
    grid_print(&g, stdout);

    printf("\n== Test set_cell / get_cell ==\n");
    set_cell(0, 0, &g, '1');
    set_cell(0, 1, &g, '0');
    set_cell(0, 2, &g, '1');
    set_cell(0, 3, &g, '0');
    printf("Row 0: ");
    for (int j = 0; j < g.size; j++)
        printf("%c ", get_cell(0, j, &g));
    printf("\n\n");

    printf("== Test règles Takuzu ==\n\n");
    printf("check_lines : %s\n", check_lines(&g) ? "OK" : "FAIL");
    printf("check_cols  : %s\n", check_cols(&g) ? "OK" : "FAIL");
    printf("check_runs_rows : %s\n", check_runs_rows(&g) ? "OK" : "FAIL");
    printf("check_nb_rows   : %s\n", check_nb_rows(&g) ? "OK" : "FAIL");
    printf("is_consistent   : %s\n", is_consistent(&g) ? "OK" : "FAIL");
    printf("is_valid        : %s\n", is_valid(&g) ? "OK" : "FAIL");

    grid_free(&g);
    return 0;
}