#include<stdio.h>
int main(){
int h[] ={2,3,3,2,3,3};
char c[20];
//char encr[20] = "8310511897110105";
char encr[20] = "6511010697108105";
int m=0, x=0;
for(int i=0; h[i]!='\0'; i++)
{
int l1= h[i];
x=0;
for(int j=0;j<l1;j++)
{
int c= encr[m]-48;
x=(x*10)+c;
m=m+1;
}
c[i]=x;
}
for(int i=0; c[i]!='\0';i++)
{
printf("%c",c[i]);
}
printf("\n");
}
