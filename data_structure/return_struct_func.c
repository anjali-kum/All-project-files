// program to understand how a structure variable is returned from a function.
#include<stdio.h>
#include<stdlib.h>
struct student{
	char name[20];
	int roll;
	int marks;
};
void display(struct student);
struct student change(struct student stu);
main(){
	struct student stu1={"anjali",12,87};
	struct student stu2= {"marry",18,90};
	stu1=change(stu1);
	stu2=change(stu2);
	display(stu1);
	display(stu2);
}
struct student change(struct student stu){
	stu.marks= stu.marks+5;
	stu.roll=stu.roll-10;
	return stu;
}
void display(struct student stu){
	printf("Name-%s\t",stu.name);
	printf("Roll-%d\t",stu.roll);
	printf("%d\t",stu.marks);
}
