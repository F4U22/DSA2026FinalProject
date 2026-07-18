void execute_update(hash_header *table, int student_id, const char* field, float new_value) {
    size_t bits = (size_t)log2f(table->capacity);
    size_t index = hashing(student_id, bits);
    size_t start = index;

    while (table->row[index].id != 0) {
        if (table->row[index].id == student_id) {
            if (strcmp(field, "GPA") == 0) {
                table->row[index].gpa = new_value;
                printf("UPDATED: Student %d GPA updated.\n", student_id);
                return;
            }
        }
        index = (index + 1) % table->capacity;
        if (index == start) break;
    }
    printf("UPDATE FAILED.\n");
}
