#include<stdio.h>
#include<string.h>
float fah_to_cel(float f)
{
return ((f-32.0)*5.0/9.0);
}
int main(){
float f=40;
printf("Temperature in degree celcius: %0.2f",fah_to_cel(f));

return 0;
}
