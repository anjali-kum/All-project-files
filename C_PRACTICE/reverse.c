// WAP reverse of a number and check whether it is apalindrome or not?
#include<stdio.h>
int reverse(int n);

main(){
int num;
printf("Enter a number:");
scanf("%d",&num);
printf("%d  %d\n",num,reverse(num));
if(num== reverse(num))
printf("Number is palindrome:");
else

printf("NUmber is not a palindrome\n");

}
reverse(int n){
int rem;
int num,rev=0;
while(n>0){
rem=n%10;
rev= rev*10+rem;
n/=10;
}
return rev;
}
