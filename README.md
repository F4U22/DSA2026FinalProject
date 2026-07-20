2026 DSA Final Project 
note:
the program will ask which file you want to open just type the number I forgot to put the question mb


18/7 update:
- Records now load into a plain growable array (fixed the old hash insert that
  crashed and dropped rows on collision). Dataset now loads all rows.

- Menu trimmed to: 1.Filter  2.Filter_Sort  0.Quit (Search/Insert/Update/Delete
  dropped - not part of the assignment).

- Filter: linear scan. Filter_Sort: filter then stable merge sort, ASC/DESC.

- Filter field limited to GPA / Department / EnrollmentYear.
  Sort field limited to StudentID / GPA / EnrollmentYear.

- Dataset picker now prints a prompt; %zu -> %lu so it builds clean on MinGW.

- Usage: run, pick a dataset number, then choose an operation and answer the
  Field / Value / (Sort field / Order) prompts.

- Menu added in main.c. Please add cases and operations.

20/7 update:
- Records now load into hash table with fib hash and  robin hood probing

- Menu is now 1.Filter  2.Filter_Sort 3.Insert 4.Delete 5.Update 6.Search 0.Quit
- Filter: linear scan. Filter_Sort: filter then stable merge sort, ASC/DESC.

- Filter field limited to GPA / Department / EnrollmentYear.
  Sort field limited to StudentID / GPA / EnrollmentYear.

- Usage: run, pick a dataset number, then choose an operation and answer the
  Field / Value / (Sort field / Order) prompts.

- Search operation for gpa/dept/enrollyear is identical to filter of the same field,
since it kinda overlapping but IDK.

