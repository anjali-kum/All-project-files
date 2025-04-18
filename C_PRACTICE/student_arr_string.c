#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
int numStudents, numSubjects;

// Input the number of subjects
printf("Enter the number of subjects: ");
scanf("%d", &numSubjects);

// Input the number of students
printf("Enter the number of students: ");
scanf("%d", &numStudents);

// Allocate memory for the subject names
char ***subjects = (char ***)malloc(numSubjects * sizeof(char **));
if (subjects == NULL) {
printf("Memory allocation error");
exit(1);
}

// Input subject names
for (int i = 0; i < numSubjects; i++) {
subjects[i] = (char **)malloc((numStudents + 1) * sizeof(char *)); // +1 for the subject name
if (subjects[i] == NULL) {
printf("Memory allocation error");
exit(1);
}

// Input subject name
printf("Enter the name of subject %d: ", i + 1);
char subjectName[100];
scanf("%s", subjectName);

subjects[i][0] = (char *)malloc(strlen(subjectName) + 1);
if (subjects[i][0] == NULL) {
printf("Memory allocation error");
exit(1);
}
strcpy(subjects[i][0], subjectName);

// Input student names for this subject
for (int j = 1; j <= numStudents; j++) {
printf("Enter the name of student %d for subject %d: ", j, i + 1);
char studentName[100];
scanf("%s", studentName);

subjects[i][j] = (char *)malloc(strlen(studentName) + 1);
if (subjects[i][j] == NULL) {
printf("Memory allocation error");
exit(1);
}
strcpy(subjects[i][j], studentName);
}
}

// Print the subject names and student names
printf("\nSubject names and student names:\n");
for (int i = 0; i < numSubjects; i++) {
printf("Subject %d: %s\n", i + 1, subjects[i][0]);
for (int j = 1; j <= numStudents; j++) {
printf("  Student %d: %s\n", j, subjects[i][j]);
}
}

// Deallocate memory
for (int i = 0; i < numSubjects; i++) {
for (int j = 0; j <= numStudents; j++) {
free(subjects[i][j]);
}
free(subjects[i]);
}
free(subjects);
return 0;
}
