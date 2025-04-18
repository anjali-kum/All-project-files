#include<stdio.h>
int a,b,result;
int sum(int x,int y)// addition
{
int result;
result= x+y;
return result;
// return x+y;
}
int sub(int x,int y)// substraction
{
int result;
result= x-y;
return result;
return x-y;
}
int div(int x,int y)// division
{
int result;
result= x/y;
return result;
}
int sum=0;
for(int i=1;i<b; i++)
sum = add(sum,b);
return sum;
}

int main() // main function
{
int a,b,result;
a=100,b=10;

result = sum(a,b);// function invocation
printf("sum = %d\n",result);
result= sub(a,b);// function invocation
printf("sub = %d\n",result);
result= div(a,b);// function invocation
printf("div = %d\n",result);
printf("product = %d\n",mul(a,b));// function invocation
return 0;
}


