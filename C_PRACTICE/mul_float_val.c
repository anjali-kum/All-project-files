#include<stdio.h>
#include<stdlib.h>
float mul(float a,float b)
{
return a*b;
}
int main(){
float a,b,multiple;
printf("Enter two number:");
scanf("%f  %f",&a,&b);
multiple= mul(a,b);
printf("multple of two floating point number %f:\t",multiple);

return 0;
}


