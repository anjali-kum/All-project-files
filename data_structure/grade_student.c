// program to find out and grade of students
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#define N 5
struct student

{
	char name[20];
	int roll;
	int marks[6];
	int total;
	char grade;
};
void display(struct student arr);
void calculate(struct student arr[ ]);
void sort(struct student arr[ ]);
main(){
	struct student stu[N],temp;
	int i,j;
	for(i=0;i<N;i++)
	{
		printf("Enter name: ");
		scanf("%s",stu[i].name);
		printf("Enter roll: ");
		scanf("%d",&stu[i].roll);
		stu[i].total=0;
		printf("Enter marks  in 6 subjects :");
		for(j=0;j<6;j++)
			scanf("%d",&stu[i].marks[j]);
	}
	calculate(stu);
	sort(stu);
	for(i=0;i<N;i++)
		display(stu[i]);
}
void calculate(struct student stu[])
{
int i,j;
for(i=0;i<N;i++){
	for(j=0;i<6;j++)
		stu[i].total+=stu[i].marks[j];
	if(stu[i].total>500)
		stu[i].grade='A';
	else if(stu[i].total>400)
		stu[i].grade= 'B';
	else if(stu[i].total>250)
		stu[i].grade='C';
	else
		stu[i].grade='D';
}
}
void sort(struct student stu[])
{
	int i,j;
	struct student temp;
	for(i=0;i<N-1;i++)
		for(j+1;j<N;j++)
			if(stu[i].total<stu[j].total){
				temp=stu[i];
				stu[i]=stu[j];
				stu[j]=temp;
			}
}
void display(struct student stu){
	int i;
	printf("Roll-%d\t",stu.roll);
	printf("Name-%s\t",stu.name);
	printf("Total-%d\t",stu.total);
	printf("Grade- %c\n\n",stu.grade);
}
