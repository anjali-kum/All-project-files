#include<stdio.h>
int main(){
	char a= 1, b= 0xff;
	char l = a &b;
	char k= a |b;
	char m= a^b;
	printf("%p\n %p\n %p\n", l,k,m);
	return 0;
}
