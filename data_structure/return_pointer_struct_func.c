// program to understand how a pointer to structure is returned from a function
#include<stdio.h>
#include<string.h>
#include<stdlib.h>

struct student{
	char name[20];
	int roll;
	int marks;
};
void display(struct student *);
struct student *func();
main(){
	struct student *stuptr;
	stuptr=func();
	display(stuptr);
	}
struct student *func(){
	struct student *ptr;
	ptr=(struct student *)malloc(sizeof(struct student));
	strcpy(ptr->name,"anjali");
	ptr->roll=15;
	ptr->marks=98;
	return ptr;
}
void display(struct student *stuptr){
	printf("Name- %s\t",stuptr->name);
	printf("Roll-%d\t",stuptr->roll);
	printf("marks=%d\t",stuptr->marks);
}
