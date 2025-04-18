#include<stdio.h>
int main(){
//int age=30;
//float sal= 1600.40;
//printf(" value of age = %d , Address of age = %u\n",age,&age);
//printf("value of sal = %f , Address of sal= %u\n",sal,&sal);

/*
program to dereference pointer variable */
int a=87;
float b= 4.5;
int *p1=&a;
float *p2= &b;
printf("Value of p1 = address of a= %u\n",p1);
printf("value of p2 = address of b = %u\n",p2);
printf("address of p1= %u\n",&p1);
printf("address of p2= %u\n",&p2);
printf("Value of a = %d %d %d \n",a,*p1,*(&a));
printf("value of b= %f %f %f \n ", b,*p2,*(&b));

}
