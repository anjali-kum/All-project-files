//program to understand how an array of structure is sent to a function:
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
struct student{
	char name[20];
	int roll;
	int marks;
};
void display(struct student);
void dec_marks(struct student stuarr[ ]);
main(){
	int i;
	struct student stuarr[3]={{"marry",12,98},{"john",11,97},{"tom",13,89}};
			dec_marks(stuarr);
			for(i=0;i<3;i++)
			display(stuarr[i]);
			}
			void dec_marks(struct student stuarr[ ])
			{
			int i;
			for(i=0;i<3;i++)
			stuarr[i].marks=stuarr[i].marks-10;
			}
			void display(struct student stu){
			printf("Name-%s\t",stu.name);
			printf("Roll- %d\t",stu.roll);
			printf("Marks-%d\t",stu.marks);
			}
