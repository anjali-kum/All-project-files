#include <stdio.h>
#include <string.h>

void searchByName(int no, char names[][20], int sub[][6], int nos) {
char searchName[20];
int found = 0;

printf("Enter the name to find the marks of a particular student: ");
scanf("%s", searchName);

for (int i = 0; i < no; i++) {
if (strcmp(names[i], searchName) == 0) {
printf("Printing the marks for student %s, roll no --> %d\n", names[i], sub[i][0]);
for (int j = 1; j <= nos; j++) {
printf("Marks for subject %d is %d\n", j, sub[i][j]);
}
found = 1;
break;
}
}
if (!found) {
printf("Name is not valid.\n");
}
}

void searchByRegNo(int no, char names[][20], int sub[][6], int nos) {
int searchRegNo;
int found = 0;

printf("Enter the regno to find the marks of a particular student: ");
scanf("%d", &searchRegNo);

for (int i = 0; i < no; i++) {
if (sub[i][0] == searchRegNo) {
printf("Printing the marks for student %s, roll no --> %d\n", names[i], sub[i][0]);
for (int j = 1; j <= nos; j++) {
printf("Marks for subject %d is %d\n", j, sub[i][j]);
}
found = 1;
break;
}
}
if (!found) {
printf("Reg no is not valid.\n");
}
}

int main() {
int no, nos, choice;

printf("Enter the number of students: ");
scanf("%d", &no);

char names[no][20];
int sub[no][6];

printf("Enter student names\n");
for (int i = 0; i < no; i++) {
printf("Enter the student %d name: ", i + 1);
scanf("%s", names[i]);

printf("Enter the regno for student %d: ", i + 1);
scanf("%d", &sub[i][0]);
}

printf("Enter the number of subjects: ");
scanf("%d", &nos);

for (int i = 0; i < no; i++) {
printf("Enter the marks for student %s, rollno --> %d\n", names[i], sub[i][0]);
for (int j = 1; j <= nos; j++) {
printf("Enter marks for subject %d: ", j);
scanf("%d", &sub[i][j]);
}
}

do {
printf("0. Exit\n1. Search with name\n2. Search with reg no\n");
scanf("%d", &choice);

switch (choice) {
case 0:
printf("Exiting...\n");
break;
case 1:
searchByName(no, names, sub, nos);
break;
case 2:
searchByRegNo(no, names, sub, nos);
break;
default:
printf("Invalid choice.\n");
}
} while (choice != 0);
return 0;
}
