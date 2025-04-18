#include<stdio.h>
#include<string.h>
//void maskPassword(char password[])
//{
//char ch; int i=0;
//while((ch=_getc())!=13)
//{
//printf("*");
//password[i] = ch;
//i++;
//}
//password[i] = '\0';
//}
void encrypt(char password[],int key)
{
for(int i=0;i<strlen(password);i++)
{
password[i] = password[i] - key;
}
}
void decrypt(char password[],int key)
{
for(int i=0;i<strlen(password);i++)
{
password[i] = password[i] + key;
}
}
int main(){
char password[20];
printf("Enter The password: ");
scanf("\n%s",password);
//maskPassword(password);
encrypt(password,0XAED);
printf("\n%s",password);
decrypt(password,0XAED);
printf("\n%s",password);
return 0;
}


