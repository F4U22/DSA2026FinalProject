#define OP_IMPLEMENTATION
#include"operation.h"
#include <time.h>
#include <stdio.h>
#include <stdlib.h>


void menu(hash_header *t)
{
    char in[64] ;
    for (;;) {
        printf("\n=== %lu records ===\n", (unsigned long)t->count) ;
        printf("1.Filter  2.Filter_Sort  0.Quit\n") ;
        printf("choose: ") ;
        if (!read_line(in, sizeof(in))) return ;        // EOF -> quit
        char *s = trim(in) ;
        if (!isdigit((unsigned char)s[0])) { printf("invalid\n") ; continue ; }
        switch (atoi(s)) {
            case 1: op_filter(t) ;      break ;
            case 2: op_filter_sort(t) ; break ;
            case 0: return ;
            default: printf("invalid\n") ;
        }
    }
}

int main(void)
{
    hash_header table = {0} ;
    initTable(&table) ;
    openCSV(&table) ;
    printf("loaded %lu records\n", (unsigned long)table.count) ;

    menu(&table) ;
    clock_t start, end;
    double cpu_time;
    
    start = clock();
    search(&table, 123456); 
    
    end = clock();
    
    cpu_time = (double)(end - start) / CLOCKS_PER_SEC;
    printf("Execution Time = %.6f seconds\n", cpu_time);

    free(table.row) ;
    return 0 ;
}
