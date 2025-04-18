//#include<stdio.h>
//int main(){
//char ch;
//printf("Enter the  operator and others two\n");
//scanf("%c",&ch);
//switch(ch){
//case 'a': case 'e':case 'i':case 'o': case 'u':
//printf("Vowels\n");
//break;
//default:
//printf("Consonant\n");
//}
//}


////////////////////////////////////////////////////////////////////////////

#include<stdio.h>
int main(){
int a,b;
char op;
scanf("%d %c %d",&a,&op,&b);
switch(op){
case '+':
printf("%d\n",a+b);
break;
case '-':
printf("%d\n",a-b);
break;

case '*': case'x': case 'X':
printf("%d\n",a*b);
break;

case '/':
printf("%d\n",a/b);
break;
case '%':
printf("%d\n",a%b);
break;
default:
printf("Provide valid input:");
}
}

