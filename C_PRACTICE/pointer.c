#include<stdio.h>
int main(){
char day;              
day = getchar();
switch(day){
case '1':
printf("Sunday");
break;

case '2':
printf("Monday");
break;

case '3':
printf("Tuesday");
break;

case '4':
printf("wednesday");
break;

case '5':
printf("Thursday");
break;

case '6':
printf("friday");
break;

case '7':
printf("saturday");
break;
default:
printf("Invalid ");
}
}
