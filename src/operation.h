#pragma once
// all the lib needed for this work should be included here plz
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<stdint.h>
#include<ctype.h>
#include<math.h>
#include<time.h>

// all the constant definition will be put here
#define MAX_PATH_LEN 200
#define MAX_FILE_LEN 50
#define MAX_NAME_LEN 100
#define MAX_DEPT_LEN 5
#define INIT_TABLE_SIZE 1024
#define MAX_LINE_LEN 256
#define GOLDEN_RATIO_NUM 2654435769

//define for macro if needed one
#define NELEM(a) ((int)(sizeof(a) / sizeof(*(a))))

// fields and sort direction
enum { F_ID, F_NAME, F_GPA, F_DEPT, F_YEAR, F_NONE = -1 };
enum { ASC = 0, DESC = 1 };

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

// matching rows, copied out
typedef struct {
    table_row *items ;
    size_t count ;
} result_set ;

// all the fuction(declaration) for this work should be put here plz
void openCSV(hash_header *table) ;
int initTable(hash_header *table) ;
int insert(hash_header *table,char *data) ;
int reinsert(hash_header *table,table_row *row) ;
uint32_t hashing(uint32_t id, int bits) ;

//for filter and filter_sort command
int field_from_name(const char *s) ;
result_set filter(hash_header *t, int field, const char *value) ;
result_set filter_sort(hash_header *t, int ffield, const char *value, int sfield, int dir) ;

//for insertion and deletion and related func 
void op_insert(hash_header *table) ;
void op_delete(hash_header *table) ;
void shiftBack(hash_header *table,int index, int next_index) ;
int convertInt(char *int_char,char end_char) ;

//for update and search and related func
int search(hash_header *table, int id) ;
int update(hash_header *table, int index, char *data,int field) ;
void op_search(hash_header *table) ;
void op_update(hash_header *table) ;


// varible for compute time
clock_t start,end ;
double cpu_time ;

// this allow for all the funcs be declare and define in one .h file
#if defined(OP_IMPLEMENTATION)

// read one line from stdin, strip the newline. NULL on EOF.
static char *read_line(char *buf, size_t n)
{
    if (!fgets(buf, (int)n, stdin)) return NULL ;
    buf[strcspn(buf, "\r\n")] = 0 ;
    return buf ;
}

static char *trim(char *s)
{
    while (*s && isspace((unsigned char)*s)) s++ ;
    char *e = s + strlen(s) ;
    while (e > s && isspace((unsigned char)e[-1])) *--e = 0 ;
    return s ;
}

static int ieq(const char *a, const char *b)
{
    for (; *a && *b; a++, b++)
        if (tolower((unsigned char)*a) != tolower((unsigned char)*b)) return 0 ;
    return *a == *b ;
}

int field_from_name(const char *s)
{
    if (ieq(s, "StudentID") || ieq(s, "ID"))              return F_ID ;
    if (ieq(s, "Name"))                                   return F_NAME ;
    if (ieq(s, "GPA"))                                    return F_GPA ;
    if (ieq(s, "Department") || ieq(s, "Dept"))           return F_DEPT ;
    if (ieq(s, "EnrollmentYear") || ieq(s, "Enrollment")) return F_YEAR ;
    return F_NONE ;
}

// ---- storage ---------------------------------------------------------------

// init the table 
int initTable(hash_header *table)
{
    if(table->capacity == 0) 
    {
        table->capacity = INIT_TABLE_SIZE ;
        table->row = calloc(table->capacity,sizeof(*table->row)) ;
    }else{
        return -1 ;
    }
}

//hashing algorithm for fibo hashing
uint32_t hashing(uint32_t id,int bits) 
{
    if(bits >32 && bits < 0) return 0xFFFFFFFF ;

    return ((id * (uint32_t)GOLDEN_RATIO_NUM) >> (32 - bits)) ;
}
//for reinsert stuff
int reinsert(hash_header *table,table_row *row)
{
    row->dfh = 0 ;
    size_t index = hashing(row->id,(int)log2f(table->capacity)) ;
    for(;;)
    {
        if(table->row[index].id != 0) 
        {

            if(table->row[index].dfh < row->dfh)
            {

                table_row exchanger = *row ;
                *row = table->row[index] ;
                table->row[index] = exchanger ;
            }

        }else{


            table->row[index] = *row ;
            table->count++ ;
            break ;
        }
        row->dfh++ ;
        index = (index + 1) & (table->capacity -1) ;
    }
}

// append one CSV record; grow when full
int insert(hash_header *table, char *data)
{
    if (table->count >= (int)(table->capacity*0.7))
    {
        int old_capacity = table->capacity ;
        table_row *old_row = table->row ;

        table->capacity = 2 *old_capacity ;
        table->row = calloc(table->capacity, sizeof(*table->row));

        table->count = 0 ;
        for(int i = 0 ; i< old_capacity ;i++)
        {
            if(old_row[i].id != 0) reinsert(table,&old_row[i]) ;
        }

        free(old_row) ;
    }
    table_row *temp = malloc(sizeof(table_row)) ;
    int err = sscanf(data,"%d,%99[^,],%f,%4[^,],%d\n",&temp->id,temp->name,&temp->gpa,temp->dept,&temp->enroll_y) ;
    if(err <5) { free(temp); return -1 ; }
    temp->dfh = 0 ;

    size_t index = hashing(temp->id,(int)log2f(table->capacity)) ;
    int insert_index = -1 ;
    for(;;)
    {
        if(table->row[index].id != 0) 
        {

            if(table->row[index].dfh < temp->dfh)
            {
                //for tracking when it got inserted
                if(insert_index == -1) insert_index = (int)index ;

                table_row exchanger = *temp ;
                *temp = table->row[index] ;
                table->row[index] = exchanger ;
            }

        }else{

            //for tracking when it got inserted
            if(insert_index == -1) insert_index = (int)index ;

            table->row[index] = *temp ;
            table->count++ ;
            break ;
        }
        temp->dfh++ ;
        index = (index + 1) & (table->capacity -1) ;
    }
    free(temp) ;
    return insert_index ;
}

//store stuff
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
    int choice_file = 0 ;

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

    printf("Choose a dataset:\n") ;
    while((entry = readdir(dir)) != NULL)
    {
        if(entry->d_name[0] == '.') continue ;
        printf("  %d. %s\n",++i,entry->d_name);
        file_list[i-1] = malloc(strlen(entry->d_name) + 1) ;
        strcpy(file_list[i-1],entry->d_name) ;
    }
    closedir(dir) ;

    do{
        printf("dataset #: ") ;
        if (!read_line(read_buff, sizeof(read_buff))) return ;
        choice_file = atoi(read_buff) ;
    }while(choice_file<=0 || choice_file > i) ;

    snprintf(fullpath,sizeof(fullpath),"%s%s",path,file_list[choice_file-1]) ;

    fs = fopen(fullpath,"r") ;
    if(fs == NULL) { printf("no file %s",fullpath) ; return ; }


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


    for(size_t k = 0 ; k < MAX_FILE_LEN; k++) free(file_list[k]) ;
    fclose(fs) ;
}

// ---- filter: linear scan ---------------------------------------------------

static int match(const table_row *r, int field, const char *value)
{
    switch (field) {
        case F_DEPT: return strcmp(r->dept, value) == 0 ;
        case F_NAME: return strcmp(r->name, value) == 0 ;
        case F_YEAR: return r->enroll_y == atoi(value) ;
        case F_ID:   return r->id == atoi(value) ;
        case F_GPA:  return r->gpa == (float)atof(value) ;
        default:     return 0 ;
    }
}

result_set filter(hash_header *t, int field, const char *value)
{
    result_set r = { malloc(t->count * sizeof(table_row)), 0 } ;
    for (size_t i = 0 ; i < t->capacity ; i++)

        //add first condition to check for empty row should make it a little faster
    
        if (t->row[i].id != 0 && match(&t->row[i], field, value))
            r.items[r.count++] = t->row[i] ;
    return r ;
}

// ---- merge sort ----------------------------------

// -1 / 0 / +1 comparison of a and b on one field
static int row_cmp(const table_row *a, const table_row *b, int field)
{
    switch (field) {
        case F_GPA:  return (a->gpa > b->gpa) - (a->gpa < b->gpa) ;
        case F_ID:   return (a->id > b->id) - (a->id < b->id) ;
        case F_YEAR: return (a->enroll_y > b->enroll_y) - (a->enroll_y < b->enroll_y) ;
        case F_NAME: return strcmp(a->name, b->name) ;
        case F_DEPT: return strcmp(a->dept, b->dept) ;
        default:     return 0 ;
    }
}

// stable, O(k log k) — no worst case on the already-sorted CSV rows
static void msort(table_row *a, table_row *tmp, size_t n, int field, int desc)
{
    if (n < 2) return ;
    size_t mid = n / 2 ;
    msort(a, tmp, mid, field, desc) ;
    msort(a + mid, tmp + mid, n - mid, field, desc) ;
    size_t i = 0, j = mid, k = 0 ;
    while (i < mid && j < n) {
        int c = row_cmp(&a[j], &a[i], field) ;
        if (desc) c = -c ;
        tmp[k++] = (c < 0) ? a[j++] : a[i++] ;   // tie -> take left -> stable
    }
    while (i < mid) tmp[k++] = a[i++] ;
    while (j < n)   tmp[k++] = a[j++] ;
    memcpy(a, tmp, n * sizeof(table_row)) ;
}

result_set filter_sort(hash_header *t, int ffield, const char *value, int sfield, int dir)
{
    result_set r = filter(t, ffield, value) ;
    if (r.count > 1) {
        table_row *tmp = malloc(r.count * sizeof(table_row)) ;
        msort(r.items, tmp, r.count, sfield, dir) ;
        free(tmp) ;
    }
    return r ;
}

// ---- interactive UI --------------------------------------------------------

static void print_row(const table_row *r)
{
    printf("%d, %s, %.2f, %s, %d\n", r->id, r->name, r->gpa, r->dept, r->enroll_y) ;
}

static void print_result(result_set *r)
{
    for (size_t i = 0 ; i < r->count ; i++) print_row(&r->items[i]) ;
    printf("(%zu rows)\n", r->count) ;
    free(r->items) ;
}

// canonical name of a field id, for building prompts
static const char *field_name(int f)
{
    switch (f) {
        case F_ID:   return "StudentID" ;
        case F_NAME: return "Name" ;
        case F_GPA:  return "GPA" ;
        case F_DEPT: return "Department" ;
        case F_YEAR: return "EnrollmentYear" ;
        default:     return "?" ;
    }
}

// prompt for a field restricted to `allowed`, keep asking until it's valid
static int ask_field(const char *label, const int *allowed, int n)
{
    char in[64] ;
    for (;;) {
        printf("%s (", label) ;
        for (int k = 0 ; k < n ; k++) printf("%s%s", field_name(allowed[k]), k + 1 < n ? "/" : "") ;
        printf("): ") ;
        if (!read_line(in, sizeof(in))) return F_NONE ;
        int f = field_from_name(trim(in)) ;
        for (int k = 0 ; k < n ; k++) if (f == allowed[k]) return f ;
        printf("  invalid field, try again\n") ;
    }
}

// which fields Filter and Sort accept
static const int FILTER_FIELDS[] = { F_GPA, F_DEPT, F_YEAR } ;
static const int SORT_FIELDS[]   = { F_ID, F_GPA, F_YEAR } ;

static void op_filter(hash_header *t)
{
    char value[MAX_LINE_LEN] ;
    int f = ask_field("Field", FILTER_FIELDS, NELEM(FILTER_FIELDS)) ;
    if (f == F_NONE) return ;
    printf("Value: ") ;
    if (!read_line(value, sizeof(value))) return ;

    start = clock() ;
    
    result_set r = filter(t, f, trim(value)) ;
    print_result(&r) ;

    end = clock() ;
    cpu_time = (double)(end-start) / CLOCKS_PER_SEC ;
    printf("Execution Time = %.6f seconds\n",cpu_time) ;

}

static void op_filter_sort(hash_header *t)
{
    char value[MAX_LINE_LEN], order[16] ;
    int f = ask_field("Filter field", FILTER_FIELDS, NELEM(FILTER_FIELDS)) ;
    if (f == F_NONE) return ;
    printf("Filter value: ") ;
    if (!read_line(value, sizeof(value))) return ;
    int s = ask_field("Sort field", SORT_FIELDS, NELEM(SORT_FIELDS)) ;
    if (s == F_NONE) return ;
    printf("Order (ASC/DESC): ") ;
    if (!read_line(order, sizeof(order))) return ;

    start = clock() ;

    result_set r = filter_sort(t, f, trim(value), s, ieq(trim(order), "DESC") ? DESC : ASC) ;
    print_result(&r) ;

    end = clock() ;
    cpu_time = (double)(end - start) / CLOCKS_PER_SEC ;
    printf("Execution Time = %.6f seconds\n",cpu_time) ;

}

//--insert operation and delete along with related func---------------------------------------------

void shiftBack(hash_header *table,int index, int next_index) 
{
    // move data from row in front to the back row
    // look a bit messy but should be fine
    table->row[index].id = table->row[next_index].id ;
    strcpy(table->row[index].name,table->row[next_index].name);
    table->row[index].gpa = table->row[next_index].gpa ;
    strcpy(table->row[index].dept,table->row[next_index].dept) ;
    table->row[index].enroll_y = table->row[next_index].enroll_y ;
    table->row[index].dfh = table->row[next_index].dfh - 1 ;
}
// this convertion function convert str to int return -1 as error work
// work in this case since all possible int are positive 
// end_char for character that will be at the end when strtol read all the num
int convertInt(char *int_char,char end_char) {
    char *endptr ;
    int val ;

    val = (int)strtol(int_char,&endptr,10) ;
    if(int_char == endptr || *endptr != end_char) return -1 ;
    else  return val ;

}

void op_insert(hash_header *table)
{
    char buffer_read[MAX_LINE_LEN] ;
    char *data ;
    int res_index ;

    printf("Please insert row with the following format:id,name,gpa,dept,enroll year\nInput row:") ;

    data = fgets(buffer_read,sizeof(buffer_read),stdin) ;
    if(data == NULL) printf("ERROR something went wrong while try to read input\n") ;
    

    // if it return -1 error occur other than that should be fine
    start = clock() ;
    if((res_index = insert(table,data)) == -1) {end = clock() ; printf("Wrong input format\n") ;} 
    else{
        printf("Insertion successful insert the following row at %d\n",res_index) ;
        print_row(&table->row[res_index]) ;

    }
    end = clock() ;
    cpu_time = (double)(end - start) / CLOCKS_PER_SEC ;
    printf("Execution Time = %.6f seconds\n",cpu_time) ;

}

void op_delete(hash_header *table)
{
    // this number should be fine
    char input_buff[100] ;
    int id,index,id_dfh = 0  ;
    for (;;)
    {
        //for the strtol function
        char *endptr ;

        printf("Insert ID for deletion:") ;
        char *id_char = fgets(input_buff,sizeof(input_buff),stdin) ;
        if(id_char == NULL) printf("Some erros occur\n") ;
        
        id = convertInt(id_char,'\n');
        if(id == -1) printf("Invalid input\n") ;
        else break ;
    }
    start = clock() ;
    index = hashing(id,log2f(table->capacity)) ;
    // this loop is use to find the index to delete
    for(;;)
    {
        if(table->row[index].id == 0) 
        {
            printf("key not found\n") ;

            end = clock() ;
            cpu_time = (double)(end - start) / CLOCKS_PER_SEC ;
            printf("Execution Time = %.6f seconds\n",cpu_time) ;

            return ;
        }

        // found it yea
        if(table->row[index].id == id) 
        {
            printf("row found at %d\n",index) ;
            print_row(&table->row[index]) ;
            break ;
        }

        // robin hood early exit condition
        if(table->row[index].dfh < id_dfh){
            printf("key not found\n") ;
            
            end = clock() ;
            cpu_time = (double)(end - start) / CLOCKS_PER_SEC ;
            printf("Execution Time = %.6f seconds\n",cpu_time) ;

            return ;
        } 
        index = (index+1) &(table->capacity -1) ;
        id_dfh++ ;
    }
    // this loop is use to delete and shift row
    for(;;)
    {
        int next_index ;
        // delete the row (this should be enough)
        table->row[index].id = 0 ;

        next_index = (index+1) &(table->capacity -1) ;

        if(table->row[next_index].dfh == 0 || table->row[next_index].id == 0) break;

        shiftBack(table,index,next_index) ;

        index = next_index ;
        
    }

    table->count-- ;

    end = clock() ;
    cpu_time = (double)(end - start) / CLOCKS_PER_SEC ;
    printf("Execution Time = %.6f seconds\n",cpu_time) ;

}
//--section for search and update function and related func ----------------
static const int SEARCH_FIELD[] = { F_ID, F_GPA, F_DEPT, F_YEAR } ;
static const int UP_FIELD[] = {F_NAME, F_GPA, F_DEPT, F_YEAR } ;

int search(hash_header *table, int id) 
{
    int index, dfh = 0 ;
    index = hashing(id,(int)log2f(table->capacity)) ;
    for(;;)
    {
        if(table->row[index].id == 0) return -1 ;

        // found it yea
        if(table->row[index].id == id) return index ;
        

        // robin hood probing exit condition
        if(table->row[index].dfh < dfh) return -1 ;

        index = (index + 1) & (table->capacity -1) ;
        dfh++ ;
    }
}
int update(hash_header *table, int index, char *data, int field) 
{
    int val ;
    if(isdigit(*data)) 
    {
        if((val = convertInt(data,'\0')) == -1) { printf("Wrong num format\n");return -1 ; }
    }
    switch(field) 
    {
        case F_NAME: 
            if(strlen(data) > (MAX_NAME_LEN -1)) {printf("Name too long\n"); return -1 ;} 
            strcpy(table->row[index].name,data) ;

        break ;
        case F_GPA:
            table->row[index].gpa = val ;

        break ;
        case F_DEPT:
            if(strlen(data) > MAX_DEPT_LEN -1) {printf("dept name too long\n") ; return -1 ; }
            strcpy(table->row[index].dept,data) ;

        break ;
        case F_YEAR:
            table->row[index].enroll_y = val ;

        break ;
        default: 
            printf("Unknown field\n") ;
            return -1 ;

        break ;
    }
    return 0 ;
}

//similar to filter func
void op_search(hash_header *table)
{
    char value[MAX_LINE_LEN] ;
    int id, index ;

    int field = ask_field("Field", SEARCH_FIELD, NELEM(SEARCH_FIELD)) ;
    if (field == F_NONE) return ;

    printf("Search value: ") ;
    if (!read_line(value, sizeof(value))) return ;

    start = clock() ;
    if(field == F_ID)
    {

        id = convertInt(value,'\0') ;

        if(id == -1) {printf("Invalid input\n") ; return ;}
        else index = search(table,id) ;

        if(index >=0) 
        {
            printf("Found the record\n") ;
            print_row(&table->row[index]) ;
        }
        else printf("StudentId not found\n") ;

        end = clock() ;

    }else{
        result_set r = filter(table, field, trim(value)) ;
        print_result(&r) ;

        end = clock() ;
    }

    cpu_time = (double)(end - start) / CLOCKS_PER_SEC ;
    printf("Execution Time = %.6f seconds\n",cpu_time) ;
}

void op_update(hash_header *table) 
{
    char value[MAX_LINE_LEN], input_buff[100];
    int id, index ;
    clock_t start_s, end_s ;
    double cpu_time_s ;

    for (;;)
    {

        printf("Insert ID for update:") ;
        char *id_char = fgets(input_buff,sizeof(input_buff),stdin) ;
        if(id_char == NULL) printf("Some erros occur\n") ;
        
        id = convertInt(id_char,'\n') ;

        if(id == -1) printf("Invalid input\n") ;
        else break ;
    }
    start_s = clock() ;
    if((index = search(table,id)) == -1) {printf("NO id:%d\n",id) ; return ; }
    else print_row(&table->row[index]) ;
    end_s = clock() ;
    cpu_time_s = (double) (end - start) / CLOCKS_PER_SEC ;

    int field = ask_field("Field", UP_FIELD, NELEM(UP_FIELD)) ;
    if (field == F_NONE) return ;

    printf("Change to: ") ;
    if (!read_line(value, sizeof(value))) return ;
    
    start = clock() ;
    if(update(table,index,value,field) == -1) printf("Update fail\n") ;
    else {printf("Update successful\n") ; print_row(&table->row[index]) ;}
    end = clock() ;
    cpu_time = (double)(end -start) / CLOCKS_PER_SEC ;
    printf("Execution Time %.6f seconds\n",cpu_time + cpu_time_s) ;

}


#endif
