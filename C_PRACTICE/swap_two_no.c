#include<stdio.h>
int main(){
int a,b,temp;
a=10;
b=20;
temp=a;// a assign to temperory variable
b=a;
b=temp;
printf("%d %d %d", a,b,temp);
return 0;
}
