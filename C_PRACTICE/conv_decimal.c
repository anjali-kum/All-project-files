#include<stdio.h>
#include<stdlib.h>
void convert_to_base(int , int);
int main(void){
	int num, choice,base;
	while(1){
		printf("Select conversion:\n\n");
printf("Decimal to Binary \n");
printf("Decimal to octal \n");
printf("Decimal to Hexa \n");
printf("Exit \n");

printf("\nEnter your choice:");
scanf("%d",&choice);
switch(choice)
{
case 1:
	base =2;
	break;
case 2:
	base = 8;
	break;
case 3:
	base = 16;
	break;
case 4:
	printf("Exit...");
	exit(1);
default:
	printf("Invalid Choice");
	continue;
}
printf("Enter a number:");
scanf("%d",&num);
printf("Result = ");
convert_to_base(num,base);
printf("\n");
}
return 0;
}
void convert_to_base(int num,int base){
	int rem;
	if(num == 0){
		return;
	}
	else 
	{
		rem = num % base;
		convert_to_base(num/base,base);
		if(base == 16 && rem >= 10)
		{
			printf("%c", rem+55);
		}
		else
		{
			printf("%d",rem);
		}
	}
}




