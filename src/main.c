#define OP_IMPLEMENTATION
#include"operation.h"


void menu(hash_header *t)
{
    char in[64] ;
    for (;;) {
        printf("\n=== %lu records ===\n", (unsigned long)t->count) ;
        printf("1.Filter  2.Filter_Sort 3.Insert 4.Delete  0.Quit\n") ;
        printf("choose: ") ;
        if (!read_line(in, sizeof(in))) return ;        // EOF -> quit
        char *s = trim(in) ;
        if (!isdigit((unsigned char)s[0])) { printf("invalid\n") ; continue ; }
        switch (atoi(s)) {
            case 1: op_filter(t) ;      break ;
            case 2: op_filter_sort(t) ; break ;
            case 3: op_insert(t);       break ;
            case 4: op_delete(t) ;      break ;
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
    printf("loaded %zu records\n", table.count) ;

    menu(&table) ;

    free(table.row) ;
    return 0 ;
}
