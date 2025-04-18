#include<stdio.h>
#include<string.h>
int main()
{

char str[20],str2[20];
int i,length=0;
int flag = 0;
printf("\nEnter the string:");
scanf("%s", str);

while(str[length]!='\0')length++;
//length= strlen(str);


for(i=0;i<length;i++) 
{
	if(str[i] != str[length-i-1])
	{
		flag= 1;
		//break;
	}
	str2[i]=str[length-1-i];
}
if(flag){
	printf("%s is Not palindrome\n",str);
}
else{
	printf("%s is Palindrome\n",str);
}

}
