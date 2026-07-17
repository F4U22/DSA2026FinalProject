#define OP_IMPLEMENTATION
#include"operation.h"

int main(void)
{
    hash_header table = {0} ;
    initTable(&table) ;
    openCSV(&table) ;

    
    for(size_t i = 0 ;i< 50; i++)
    {
        printf("row %zu: %d:%s\n",i,table.row[i].id,table.row[i].name) ;
    }
        
    return 0 ;
}
