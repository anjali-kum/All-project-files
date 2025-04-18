#include<stdio.h>
int menu();
int Switch(int);
int encrypt();
int convert(char string[], int integer[]);
void decrypt(int arr[100], int dec[100],int len );
void inoutDep();
int main()
{
int ch=1;
while(ch)
{
ch= menu();
ch=Switch(ch);
}
}
int menu()
{
int ch;
printf("-------------------------------------\n");
printf("\n\t\tMain Menu\n");
printf("-------------------------------------\n");
printf(" 0.exit\t  1. Encryption\t2. Decryption\n");
printf("\nchoose the operation you want to perform: ");
printf("%ls",&ch);
return ch;
}
int Switch(int ch)
{
switch(ch)
{
case 0:
printf("\nYour selected option is to exit\n");
return 0;

case 1:
printf("\nYour selected option is encryption\n");
encrypt();
break;

case 2:
printf("\nYour selected option is  decryption\n");
inoutDep();
break;

default:
printf("\nPlease select a valid option\n");
return 0;
}
return ch;
}

int encrypt()
{
char str[100],str1[100],check[100];
int i,rem,count=0,flag=0,lenen=0;
long int encri[100]={0};
printf("\n Enter the data to encrypt : ");
scanf("%s", str);
for(i=0;str[i]!='\0';i++)
{
str1[i]= str[i];
}
printf("\n Retype Your data : ");
scanf("%s",check);

for(i=0;str[i]!='\0'; i++)
{
   if(str[i]!=check[i])
{
flag = 1;
break;
}
}
if(flag)
printf("\nData is not matched");
else
{
printf("\nData is matched\n");
printf("\nThe encrypted data is");

for(i=0;str[i]!='\0';i++)
{
printf("%d",str[i]);
}
for(i=0;str1[i]!='\0';i++)
{
count=0;
while(str1[i])
{
rem=str1[i]%10;
count++;
str[i]/=10;
}
encri[i]=count;
lenen++;
}
printf("\n");printf("%d\n",lenen);
printf("\n The generated key is ");
for(i=0;i<lenen;i++)
{
printf("%ld",encri[i]);
}
}
printf("\n\n\n");
}

void inoutDep()
{
int arr[100],dec[100];
int len=0,lenarr;
char pass[100],arr1[100];
printf("\nEnter the encrypted data:\n");
scanf("%s",pass);

printf("\n Enter the key to decrypt the data:\n");
scanf("%s",arr1);

printf("\n The encrypted data:\n");
len=convert(pass,dec);

printf("\n \n The key to decrypt the data:\n");
lenarr= convert(arr1,arr);

printf("\n \n the decrypted data:\n");
decrypt(arr,dec,len);
printf("\n");
}
int convert(char string[],int integer[])
{
int length,i;
for(i=0;string[i];i++)
{
integer[i]= (string[i]-48);
printf("%d",integer[i]);
length=i;
}
return length;
}
void decrypt(int arr[100],int dec[100],int len){
int j,final=0,temp=0,i=0,k1=0;
for(j=0;i<len+1;j+=arr[k1],k1++)
{
final=0;
for(temp=0; temp<arr[k1];temp++,i++)
{
final= final*10+dec[i];
}
printf("%c",final);
}
}







