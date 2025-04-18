#include<stdio.h>
int main(){
int i,j,n;
printf("Enter n: ");
scanf("%d",&n);
for(i=n;i>=1;i--)
{
for(j=n;j>=1;j--)
printf("%3d",j);
printf("\n");
}
}
