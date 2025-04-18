#include <stdio.h>
#include <string.h>

void main()
{
    int rl,phy,che,ca,total,math,eng;
    float per;
    char nm[20],div[10];
    printf("Input the Roll Number of the student :");
    scanf("%d",&rl);
    printf("Input the Name of the Student :");
    scanf("%s",nm);
    printf("Input  the marks of Physics, math,English,Chemistry and Computer Application : ");
    scanf("%d%d%d%d%d",&phy,&che,&ca,&math,&eng);
    total = phy+che+ca+math+eng;
    per = total/3.0;
    if (per>=60)
	 strcpy(div,"First");
    else
	if (per<60&&per>=48)
	    strcpy(div,"Second");
	else
	    if (per<48&&per>=36)
		strcpy(div,"Pass");
	     else
		strcpy(div,"Fail");

       printf("\nRoll No : %d\nName of Student : %s\n",rl,nm);
       printf("Marks in Physics : %d\nMarks in Chemistry : %d\nMarks in Computer Application :%d\nMarks in math : %d\nMarks in eng : %d\n",phy,che,ca,math,eng);
       printf("Total Marks = %d\nPercentage = %5.2f\nDivision = %s\n",total,per,div);
}

