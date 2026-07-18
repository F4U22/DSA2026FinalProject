#define OP_IMPLEMENTATION
#include"operation.h"

int main(void)
{
    hash_header table = {0} ;
    initTable(&table) ;
    openCSV(&table) ;

    
    for(size_t i = 0 ;i< table.capacity; i++)
    {
       /*if(table.row[i].id != 0)*/ printf("row %zu: %d:%s %hhu\n",i,table.row[i].id,table.row[i].name,table.row[i].dfh) ;
    }
    printf("%zu",table.count) ;
    
    
    return 0 ;
}
