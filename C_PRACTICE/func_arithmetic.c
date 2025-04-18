#include<stdio.h>
int sum(int a,int b);// function declaration
int sub(int a,int b);
int mul(int a,int b);
int div(int a,int b);

int main() // main function
{
int a,b,result;
a=10,b=10;
//printf("Enter two integers:\n");
//scanf("%d%d", &a, &b);

result = sum(a,b);// function invocation
printf("sum = %d\n",result);
result= sub(a,b);// function invocation
printf("sub = %d\n",result);
result= div(a,b);// function invocation
printf("div = %d\n",result);
printf("product = %d\n",mul(a,b));// function invocation
return 0;
}

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
//return x/y;

}
int mul(int x, int y)// multiplication
//{
//if(y == 0)
//return 0;
//if(y >0)
//return (x + mul(x,y-1));
//if(y<0)
//return -mul(x,-y);
{
int add=0;
for(int i=1;i<=y; i++)
add = sum(add,x);
return add;
}


