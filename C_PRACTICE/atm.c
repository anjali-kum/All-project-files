#include<stdio.h>
unsigned long amount=1000,deposit,withdraw;
int choice, pin, k;
char transaction= 'y';
void main()
{
while(pin!=1520)
{
printf("Enter your secret pin number:");
scanf("%d",&pin);
if(pin!=1520)
printf("please enter valid password\n");
}
do
{
printf("*********welcometo ATM service *********\n");
printf("1.check balance\n");
printf("2.withdraw cash\n");
printf("3. Deposit Cash\n");
printf("4.Quit\n");
printf("******************************************?*\n\n");
printf("Enter your choice.");
scanf("%d",&choice);
switch(choice)
{
case 1:
printf("\n your balance in Rs: %lu",amount);
break;
case 2:
printf("\n enter the amount to withdraw:");
scanf("%lu",&withdraw);
if(withdraw %100 != 0)
{
printf("\n Enter the amount multiple of 100");
}
else if(withdraw >(amount-500))
{
printf("\n Insufficent balance");
}
else
{
amount = amount - withdraw;
printf("\n\n please collect cash");
printf("\n your current balance is %lu",amount);
}
break;
case 3:
printf("\n Enter the amount to deposit");
scanf("%lu",&deposit);
amount = amount + deposit;
printf("Your balance is %lu",amount);
break;
case 4:
printf("\n Thank You using ATM");
break;
default:
printf("\n Invalid Choice");
}
printf("\n\n\n Do You wish to have another transaction ?(y/n):\n");
fflush(stdin);//
scanf("%c",&transaction);
if(transaction == 'n'||transaction =='N')
k= 1;
}while(!k);

printf("\n\n Thanks for using out ATM service");
}

