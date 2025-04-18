// program to understand how structure members are sent to a function 

#include<stdio.h>
#include<string.h>
struct student
{
char name[20];
int rollno;
int marks;
};
display(char name[ ],int rollno,int marks);
int main(){
struct student stu1={"anjali",10,78.00};
struct student stu2;
strcpy(stu2.name,"mary");
stu2.rollno=18;
stu2.marks=90;
display(stu1.name,stu1.rollno,stu1.marks);
display(stu2.name,stu2.rollno,stu2.marks);
}
display(char name[ ],int rollno,int marks){
printf("Name-%s\t",name);
printf("Rollno- %d\n",rollno);
printf("Marks-%d\n",marks);
}
