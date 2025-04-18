#include<stdio.h>

void accept(int [][50], int ,int);
void display(int [][50], int, int);
void mult(int [][50],int [][50],int [][50],int,int,int);
void main(){
	int a[50][50], b[50][50],c[50][50];
	int r1,r2,c1,c2;
	printf("Enter the first matrix size: ");
	scanf("%d %d", &r1,&c1);
	printf("Enter the second matrix size:");
	scanf("%d %d",&r2,&c2);
	if((c1==r2)){
		printf("Multiplication is possible...\n");
		printf("Enter the elements in first matrix...\n");
		accept(a,r1,c1);
		printf("Enter the elements in second ...\n");
		accept(b,r2,c2);
		printf("The Element of the first matrix form...\n");
		display(a,r1,c1);
		printf("The second matrix...\n");
		display(b,r2,c2);
		printf("calling the multiplication function...\n");
		mult(a,b,c1,r1,c2,r2);
		printf("Display the third matrix...\n");
		display(c,r1,c2);
	}
	else
	
		printf("Multiplication is not possible..\n");
		//getch();
	}
	void accept(int mul[][50],int r,int c){
		int i,j;
		for(i=0;i<r;i++){
			for(j=0;j<c;j++)
				scanf("%d", *(mul+i)+j);
			}
		}
		void display(int mul[][50],int r,int c)
		{
			int i,j;
			for(i=0;i<r;i++,printf("\n"))
			{
				for(j=0;j<c;j++)
					printf("%3d",*(mul[i]+j));
			}
		}
					void mult(int a[][50],int b[][50],int c[][50],int r1,int c2,int r2)
					{
						int i,j,k,sum=0;
						for(i=0;i<r1;i++)
						{
							for(j=0;j<c2;j++)
							{
								sum=0;
								for(k=0; k<r2; k++)
					sum= sum + *(*(a+i)+k) * *(b[j]+k); 
					c[i][j]= sum;
					}
					}
					}
					



