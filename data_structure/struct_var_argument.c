// program to understand how a structure variable is sent to a function
#include<stdio.h>
struct student
{
char name[20];
int roll;
int marks;
};
display (struct student);
main(){
struct student stu1={"anjali",12,87};
struct student stu2={"mary",18,90};
display(stu1);
display(stu2);
}
display(struct student stu){
printf("Name- %s\t",stu.name);
printf("ROllno-%d\t",stu.roll);
printf("Marks-%d\n",stu.marks);
}

