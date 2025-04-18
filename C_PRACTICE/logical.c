#include<stdio.h>
int main(){
int a,b,c,result;
//x,y,z,r,re,res,resu,
printf("Enter the Input a b c ");
scanf("%d %d %d ",&a,&b,&c);
printf("%d %d %d",a,b,c);
result= (a==b) && (c>b);
printf("(a==b) && (c>b) is %d \n", result);
//printf("(%d==%d) && (%d > %d) is %d \n", a,b,c,result);

result= (a==b) && (c<b);
printf("(a==b) && (c<b) is %d \n", result);
//printf("(%d==%d) && (%d < %d) is %d \n", a,b,c,result);

result= (a==b) || (c<b);
printf("(a==b) || (c<b) is %d \n", result);
//printf("(%d==%d) || (%d < %d) is %d \n", a,b,c,result);

result= (a!=b) || (c<b);
printf("(a!=b) || (c<b) is %d \n", result);
//printf("(%d!=%d) || (%d < %d) is %d \n", a,b,c,result);

result = !(a != b);
printf("!(a!=b) is %d \n",result);
//printf("!(%d!=%d) is %d \n",a,b,c,result);

result = !(a==b);
printf("!(a==b) is %d \n",result);
//printf("!(%d==%d) is %d \n",a,b,c,result);
return 0;
}
