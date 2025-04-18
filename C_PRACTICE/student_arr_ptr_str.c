#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Function to create and initialize the subject and student names
void createSubjectsAndStudents(char ****subjects, int *numSubjects, int *numStudents) {
    printf("Enter the number of subjects: ");
    scanf("%d", numSubjects);
     printf("Enter the number of students: ");
    scanf("%d", numStudents);
// Allocate memory for the subject names
    *subjects = (char ***)malloc(*numSubjects * sizeof(char **));
    if (*subjects == NULL) {
        printf("Memory allocation error");
        return 0;
        }
 // Input subject names
    for (int i = 0; i < *numSubjects; i++) {
        (*subjects)[i] = (char **)malloc((*numStudents + 1) * sizeof(char *));
        if ((*subjects)[i] == NULL) {
            printf("Memory allocation error");
            return 0;
        }
// Input subject name
        printf("Enter the name of subject %d: ", i + 1);
        char subjectName[100];
        scanf("%s", subjectName);
 (*subjects)[i][0] = (char *)malloc(strlen(subjectName) + 1);
        if ((*subjects)[i][0] == NULL) {
            printf("Memory allocation error");
            return 0;
        }
        strcpy((*subjects)[i][0], subjectName);
// Input student names for this subject
        for (int j = 1; j <= *numStudents; j++) {
            printf("Enter the name of student %d for subject %d: ", j, i + 1);
            char studentName[100];
            scanf("%s", studentName);
(*subjects)[i][j] = (char *)malloc(strlen(studentName) + 1);
            if ((*subjects)[i][j] == NULL) {
                printf("Memory allocation error");
                return 0;
            }
            strcpy((*subjects)[i][j], studentName);
        }
    }
}
// Function to add a new subject
void addSubject(char ****subjects, int *numSubjects, int *numStudents) {
    (*numSubjects)++;
    *subjects = (char ***)realloc(*subjects, *numSubjects * sizeof(char **));
    if (*subjects == NULL) {
        printf("Memory allocation error");
        return 0;
    }
// Input new subject name
    int newIndex = *numSubjects - 1;
    printf("Enter the name of subject %d: ", newIndex + 1);
    char subjectName[100];
    scanf("%s", subjectName);

    (*subjects)[newIndex] = (char **)malloc((*numStudents + 1) * sizeof(char *));
    if ((*subjects)[newIndex] == NULL) {
        printf("Memory allocation error");
        return 0;
    }

    (*subjects)[newIndex][0] = (char *)malloc(strlen(subjectName) + 1);
    if ((*subjects)[newIndex][0] == NULL) {
        printf("Memory allocation error");
        return 0;
    }
    strcpy((*subjects)[newIndex][0], subjectName);
// Initialize student names for the new subject
    for (int j = 1; j <= *numStudents; j++) {
        (*subjects)[newIndex][j] = NULL; // Initialize with NULL
    }
}

// Function to add a new student to an existing subject
void addStudent(char ***subjects, int numSubjects, int *numStudents) {
    (*numStudents)++;
    for (int i = 0; i < numSubjects; i++) {
        subjects[i] = (char **)realloc(subjects[i], (*numStudents + 1) * sizeof(char *));
        if (subjects[i] == NULL) {
            printf("Memory allocation error");
            return 0;
        }
// Input new student name
        int newIndex = *numStudents - 1;
        printf("Enter the name of student %d for subject %d: ", newIndex + 1, i + 1);
        char studentName[100];
        scanf("%s", studentName);

        subjects[i][newIndex] = (char *)malloc(strlen(studentName) + 1);
        if (subjects[i][newIndex] == NULL) {
            printf("Memory allocation error");
            return 0;
        }
        strcpy(subjects[i][newIndex], studentName);
    }
}
// Function to display the subjects taken by each student
void displaySubjectsForStudents(char ***subjects, int numSubjects, int numStudents) {
    for (int j = 0; j < numStudents; j++) {
        printf("Subjects taken by student %d:\n", j + 1);
        for (int i = 0; i < numSubjects; i++) {
            printf("- %s\n", subjects[i][j + 1]);
        }
        printf("\n");
    }
}

// Function to display the list of students for each subject
void displayStudentsForSubjects(char ***subjects, int numSubjects, int numStudents) {
    for (int i = 0; i < numSubjects; i++) {
        printf("Students for subject %s:\n", subjects[i][0]);
        for (int j = 1; j <= numStudents; j++) {
            printf("- %s\n", subjects[i][j]);
        }
        printf("\n");
    }
}




// Function to deallocate memory
void deallocateMemory(char ****subjects, int numSubjects, int numStudents) {
    for (int i = 0; i < numSubjects; i++) {
        for (int j = 0; j <= numStudents; j++) {
            free((*subjects)[i][j]);
        }
        free((*subjects)[i]);
    }
    free(*subjects);
}
int main() {
    int numSubjects = 0, numStudents = 0;
    char ***subjects = NULL;
    int choice;
while (1) {
        printf("\nMenu:\n");
        printf("1. Create subjects and students\n");
        printf("2. Add a new subject\n");
        printf("3. Add a new student\n");
        printf("4. subjects taken by each student\n");
        printf("5. students for each subject\n");
        printf("6. Exit\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);

        switch (choice) {
            case 1:
                createSubjectsAndStudents(&subjects, &numSubjects, &numStudents);
                break;
            case 2:
                addSubject(&subjects, &numSubjects, &numStudents);
                break;
            case 3:
                addStudent(subjects, numSubjects, &numStudents);
                break;
            case 4:
// Display subjects taken by each student
        displaySubjectsForStudents(subjects, numSubjects, numStudents);
        break;
        case 5:
// Display students for each subject
        displayStudentsForSubjects(subjects, numSubjects, numStudents);
        break;
        case 6:
// Deallocate memory and exit
                deallocateMemory(&subjects, numSubjects, numStudents);
                return 0;
            default:
                printf("Invalid choice\n");
        }
    }

    return 0;
}

