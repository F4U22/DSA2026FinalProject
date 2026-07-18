#pragma once
// all the lib needed for this work should be included here plz
#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include<dirent.h>
#include<ctype.h>

// all the constant definition will be put here
#define MAX_PATH_LEN 200
#define MAX_FILE_LEN 50
#define MAX_NAME_LEN 100
#define MAX_DEPT_LEN 5
#define INIT_TABLE_SIZE 1024
#define MAX_LINE_LEN 256
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
int insert(hash_header *table, char *data) ;

int field_from_name(const char *s) ;
result_set filter(hash_header *t, int field, const char *value) ;
result_set filter_sort(hash_header *t, int ffield, const char *value, int sfield, int dir) ;


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

int initTable(hash_header *table)
{
    table->capacity = INIT_TABLE_SIZE ;
    table->count = 0 ;
    table->row = malloc(table->capacity * sizeof(*table->row)) ;
    return table->row ? 0 : -1 ;
}

// append one CSV record; grow when full
int insert(hash_header *table, char *data)
{
    if (table->count >= table->capacity) {
        table->capacity *= 2 ;
        table->row = realloc(table->row, table->capacity * sizeof(*table->row)) ;
    }
    table_row *r = &table->row[table->count] ;
    // width limits stop name/dept overrunning their buffers
    int err = sscanf(data, "%d,%99[^,],%f,%4[^,],%d",
                     &r->id, r->name, &r->gpa, r->dept, &r->enroll_y) ;
    if (err < 5) return -1 ;
    table->count++ ;
    return 0 ;
}

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

    fgets(read_buff,sizeof(read_buff),fs) ;   // skip header
    for(char *data = fgets(read_buff,sizeof(read_buff),fs); data != NULL ;
        data = fgets(read_buff,sizeof(read_buff),fs))
        insert(table,data) ;

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
    for (size_t i = 0 ; i < t->count ; i++)
        if (match(&t->row[i], field, value))
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
    printf("(%lu rows)\n", (unsigned long)r->count) ;
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
    result_set r = filter(t, f, trim(value)) ;
    print_result(&r) ;
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
    result_set r = filter_sort(t, f, trim(value), s, ieq(trim(order), "DESC") ? DESC : ASC) ;
    print_result(&r) ;
}


#endif
