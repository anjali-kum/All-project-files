// ***********************without using Recursion *****************************//
// using add function multiplication of two variables
#include<stdio.h>
int sum(int *a,int *b);// function declaration
int mul(int *a,int *b);
int main() 
{
int *a,*b,result;
a=10,b=10;
result = *sum(*a,*b);// function invocation
printf("sum = %d\n",result);
printf("product = %d\n",*mul(*a,*b));// function invocation
return 0;
}
int sum(int *x,int *y)// addition
{
int result;
result= (*x)+(*y);
return result;
}
int mul(int *x, int *y)// multiplication
{
int *add;
add=0;
for(int i=1;i<=y; i++)
add = sum(add,*x);
return *add;
}
