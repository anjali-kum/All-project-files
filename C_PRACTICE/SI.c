#include<stdio.h>
int main(){
	int t,p,r,int_amt;
	printf("Input principle, Rate of interest & time to find simple interest: \n");
	scanf("%d%d%d", &p, &r,&t);
	int_amt = (p*r*t)/100;
	printf("simple interest = %d",int_amt);
	return 0;
}
