void search(hash_header *table, int student_id) {
    size_t bits = (size_t)log2f(table->capacity);
    size_t index = hashing(student_id, bits);
    size_t start = index;

   
    while (table->row[index].id != 0) {
        if (table->row[index].id == student_id) {
            printf("FOUND: %d, %s, %.2f\n", table->row[index].id, table->row[index].name, table->row[index].gpa);
            return;
        }
        index = (index + 1) % table->capacity;
        if (index == start) break;
    }
    printf("StudentID %d not found.\n", student_id);
}
