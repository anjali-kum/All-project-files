#include<stdio.h>
#include<stdlib.h>
//#include<string.h>
//struct student display(struct student s);// declaration
struct student 
{
char name[20];
int rollno;
float marks;
};
main(){
struct student stu={"anjali",3,78.00};
//display(stu.name,stu.rollno,stu.marks);
display(stu);
}
//display(char *pointer_name,int rollno,float marks)
display(struct student s)
{
strcpy(s.name,"sweeti");
s.rollno=4;
s.marks=88.00;
printf("%s",s.name);
printf("%d",s.rollno);
printf("%f",s.marks);
}

