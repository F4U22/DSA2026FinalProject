#define OP_IMPLEMENTATION
#include"operation.h"


void menu(hash_header *t)
{
    char in[64] ;
    for (;;) {
        printf("\n=== %zu records ===\n", t->count) ;
        printf("1.Filter  2.Filter_Sort 3.Insert 4.Delete 5.Update 6.Search 0.Quit\n") ;
        printf("choose: ") ;
        if (!read_line(in, sizeof(in))) return ;        // EOF -> quit
        char *s = trim(in) ;
        if (!isdigit((unsigned char)s[0])) { printf("invalid\n") ; continue ; }
        switch (atoi(s)) {
            case 1: op_filter(t) ;      break ;
            case 2: op_filter_sort(t) ; break ;
            case 3: op_insert(t);       break ;
            case 4: op_delete(t) ;      break ;
            case 5: op_update(t) ;      break ;
            case 6: op_search(t) ;      break ;
            case 0: return ;
            default: printf("invalid\n") ;
        }
    }
}
//print some sample rows 
void printSample(hash_header *table)
{
    int printed = 0;
    size_t i = 0 ;
    int print_val = ((int)(table->count*0.05) > printed) ? table->count*0.05:table->count ;
    printf("***Some sample %d records***\n",print_val) ;
    while(printed < print_val)
    {
        if(table->row[i].id != 0){ print_row(&table->row[i]) ; printed++ ; }
        i++ ;
    }
}

int main(void)
{
    hash_header table = {0} ;
    initTable(&table) ;
    openCSV(&table) ;
    printf("loaded %zu records\n", table.count) ;
    printSample(&table) ;

    menu(&table) ;

    free(table.row) ;
    return 0 ;
}
