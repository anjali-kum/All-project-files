#include<stdio.h>
#include<stdlib.h>
typedef struct student{
int sid;
char sname[20];
float fees;
}std;
int main()
{
std *s;
int i=1;
char opt;
printf("Allocate the memory dynamically....\n");
do
{
s=(std*)malloc(sizeof(std));
if(s==NULL){
printf("\n memory not allocated properly");
return 1;
}
printf("Enter the details of %d Record:\n",i);
scanf("%d %s %f",&(s->sid),s->sname,&(s->fees));
printf("THe details of %d Record",i);
printf("%d %s %f\n",s->sid,(*s).sname,s->fees);
i++;
free(s);
printf("Do u create one record (Y/N):");
fflush(stdin);
opt= getchar();
}while(opt =='Y' || opt == 'y');

//link_float();

//float a,*b;
//b= &a;
//a= *b;
return 0;
}


