void search(hash_header *table, int student_id) {
    for (size_t i = 0; i < table->count; i++) {
        if (table->row[i].id == student_id) {
            printf("FOUND: %d, %s, %.2f, %s, %d\n", 
                   table->row[i].id, table->row[i].name, 
                   table->row[i].gpa, table->row[i].dept, table->row[i].enroll_y);
            return;
        }
    }
    printf("StudentID %d not found.\n", student_id);
}
