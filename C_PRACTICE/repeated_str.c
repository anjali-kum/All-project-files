#include <stdio.h>
#define MAX 1000 // maximum length of the string
#include<string.h>
// function to find the length of a string without using string.h
int stringLength(char *s) {
int i = 0;
while (s[i] != '\0') {
i++;
}
return i;
}
// function to compare two strings without using string.h
int stringCompare(char *s1, char *s2) {
int i = 0;
while (s1[i] != '\0' && s2[i] != '\0') {
if (s1[i] != s2[i]) {
return s1[i] - s2[i];
}
i++;
}
return s1[i] - s2[i];
}
// function to find a word in a string and count its occurrences
int find_word(char *str, char *word) {
int len_str = stringLength(str); // length of the string
int len_word = stringLength(word); // length of the word
int count = 0; // number of occurrences
int i, j, k; // loop variables
// loop through the string
for (i = 0; i <= len_str - len_word; i++) {
// check if the word matches at the current position
k = i;
for (j = 0; j < len_word; j++) {
if (str[k] != word[j]) {
break;
}
k++;
}
// if the word matches, increment the count and move to the next position
if (j == len_word) {
count++;
i = k - 1;
}
}
return count;
}
int main() {
char str[MAX]; // the string to search in
char word[MAX]; // the word to search for
int count; // the number of occurrences
printf("Enter the string: ");
fgets(str, MAX, stdin); // read the string from the standard input
printf("Enter the word: ");
}
