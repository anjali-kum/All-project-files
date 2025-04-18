#include<stdio.h>
#include<string.h>
int ak_strlen(const char *arr)
{
int len = 0;
int del_count= 0;
while(*arr)
{
if(*arr == ',' || *arr == '\n'){
del_count++;
}
arr++;
len++;
}
return len+del_count;
}
int main(){
char a[]= "Hello,This is Anjali Kumari\n";
int len = ak_strlen(a);
printf("delimeter: %d\n",arr);
printf("Length with delimeter: %d\n", len);
return 0;
}
