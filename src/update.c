void update(hash_header *table, int student_id, const char* field, float new_value) {
    for (size_t i = 0; i < table->count; i++) {
        if (table->row[i].id == student_id) {
            if (strcmp(field, "GPA") == 0) {
                table->row[i].gpa = new_value;
                printf("UPDATED: Student %d GPA modified.\n", student_id);
                return;
            }
        }
    }
    printf("UPDATE FAILED: Student not found.\n");
}
