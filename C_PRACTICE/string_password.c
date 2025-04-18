#include<stdio.h>
#include<string.h>
int main(){
char str[100];
int i;
 printf("Enter a string:");
 gets(str);
 printf("ASCII values of characters in the string are:");
 for(i=0; i<strlen(str);i++)
 {
 printf("%d\t",str[i]);
 }
 return 0;
 }
 
 
