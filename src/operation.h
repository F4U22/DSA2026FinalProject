#pragma once
// all the lib needed for this work should be included here plz
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<stdint.h>
#include<math.h>

// all the constant definition will be put here

#define MAX_PATH_LEN 200
#define MAX_FILE_LEN 50
#define MAX_NAME_LEN 100
#define MAX_DEPT_LEN 5
// this is 2 to power 10
#define INIT_TABLE_SIZE 1024 
#define GOLDEN_RATIO_NUM 2654435769


//struct definition will be here 

typedef struct {
    int id ;
    char name[MAX_NAME_LEN] ;
    float gpa ;
    char dept[MAX_DEPT_LEN] ;
    int enroll_y ;
    // this is use for robin hood probing 
    unsigned char dfh ;

} table_row ;

typedef struct {
    
    table_row *row ;
    size_t count ;
    size_t capacity ;
    
    

} hash_header ;
// all the fuction(declaration) for this work should be put here plz
void openCSV(hash_header *table) ;
int initTable(hash_header *table) ;

int insert(hash_header *table,char *data) ;
uint32_t hashing(uint32_t id, int bits) ;


// this allow for all the funcs be declare and define in one .h file
#if defined(OP_IMPLEMENTATION)

void openCSV(hash_header *table)
{
    DIR *dir ;
    struct dirent *entry ;
    FILE *fs ;
    char *path = "../test/" ;
    char fullpath[MAX_PATH_LEN] ;
    char *file_list[MAX_FILE_LEN] = {0} ;
    int i = 0 ;
    char read_buff[200] ;
    int choice_file ;

    if((dir = opendir("../test")) == NULL) 
    {
        printf("No ../test will try ./test\n\n") ;
        path = "./test/" ;
        if((dir = opendir("./test")) == NULL) 
        {
            printf("NO test folder at wanted location\n") ;
            return  ;
        }
    }

 
    while((entry = readdir(dir)) != NULL) 
    {
        if(entry->d_name[0] == '.') continue ;
        else
        {
            printf("%d %s\n\n",++i,entry->d_name); 
            file_list[i-1] = malloc(strlen(entry->d_name) + 1) ;
            strcpy(file_list[i-1],entry->d_name) ;

        
        }
    }
    // close the dir when not use :-)
    closedir(dir)  ;

    // let user choose the file and concat the path
    do{
        printf("Please select the file you want to open by input the number: ") ;
        scanf("%d",&choice_file) ;

    }while(choice_file<=0 || choice_file > i) ;
    snprintf(fullpath,sizeof(fullpath),"%s%s",path,file_list[choice_file-1]) ;

    fs = fopen(fullpath,"r") ;
    if(fs == NULL) printf("no file %s",fullpath) ;

    char *head = fgets(read_buff,sizeof(read_buff),fs) ;
    if(strncmp(head,"StudentID,Name,GPA,Department,EnrollmentYear\n",strlen(head)) != 0)
    {
        printf("File potentially not in the wanted format check the table head\n") ;

    }else{
        size_t row_counter = 0 ;
        for(char *data = fgets(read_buff,sizeof(read_buff),fs);data != NULL ; 
            data = fgets(read_buff,sizeof(read_buff),fs))
        {
            row_counter++ ;
            if(insert(table,data) == -1) printf("%zu:Potential missing field\n",row_counter) ;
        }
    }



    //free both the allocated file list and close the file
    for(size_t i = 0 ; i < MAX_FILE_LEN; i++) free(file_list[i]) ;
    fclose(fs) ;
}

int initTable(hash_header *table) 
{
    if(table->capacity == 0) 
    {
        table->capacity = INIT_TABLE_SIZE ;
        table->row = calloc(table->capacity,sizeof(*table->row)) ;
    }else{
        return -1 ;
    }
    

    return 0 ;
}
int insert(hash_header *table, char *data)
{
    if (table->count >= (int)(table->capacity*0.7))
    {
        if(table->capacity == 0) table->capacity = INIT_TABLE_SIZE ;
        else table->capacity *= 2 ;
        table->row = realloc(table->row,table->capacity*sizeof(*table->row)) ;
    }
    table_row *temp ;

    int err = sscanf(data,"%d,%[a-zA-Z ],%f,%[^,],%d\n",&temp->id,temp->name,&temp->gpa,temp->dept,&temp->enroll_y) ;
    if(err <5) return -1 ;
    temp->dfh = 0 ;

    size_t index = hashing(temp->id,(int)log2f(table->capacity)) ;
    
    for(;;)
    {
        if(table->row[index].id != 0) 
        {


            if(table->row[index].dfh < temp->dfh)
            {
                table_row exchanger = *temp ;
                *temp = table->row[index] ;
                table->row[index] = exchanger ;
            }

        }else{
            table->row[index] = *temp ;
            table->count++ ;
            break ;
        }
        temp->dfh++ ;
        index = (index + 1) & (table->capacity -1) ;
    }
}

uint32_t hashing(uint32_t id,int bits) 
{
    if(bits >32 && bits < 0) return 0xFFFFFFFF ;

    return ((id * (uint32_t)GOLDEN_RATIO_NUM) >> (32 - bits)) ;
}


    

#endif 
