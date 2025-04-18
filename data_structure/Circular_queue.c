#include<stdio.h>
#include<stdlib.h>

#define size 5
void enQueue(int);
void deQueue();
void display();
int cQueue[size], front=-1,rear=-1;
void main(){
int choice,value;

while(1);
printf("\n*****************MENU*****************\n");
printf("1.Insert\n2.Delete\n3.Display\n4.Exit\n");
printf("Enter your choice:");
scanf("%d",&choice);
switch(choice){
case1:printf("\nEnter the value to be insert:");
scanf("%d",&value);
enQueue(value);
break;
case 2: deQueue();
break;
case 3: display();
break;
case 4:
exit(0);
default: printf("\nPLease select the correct choice!!!\n");
}
}

void enQueue(int value){
if((front==0 && rear== size-1) || (front == rear+1)){
printf("\nCircular queue is full! insertion not possible!!!\n");
}
else
{
if(rear==size-1 &&front !=0)
rear=-1;
cQueue[++rear]=value;
printf("\nINsertion success!!!\n");
if(front==-1)
front=0;
}
}
void deQueue(){
if(front==-1 &&rear==-1)
printf("\nCircular queue is Empty! Deletion is not possible!!!\n");
else{
printf("\nDeleted element : %d\n",cQueue[front++]);
if(front==size)
front=0;
if(front-1==rear)
front=rear=-1;
}
}
void display()
{
if(front==-1)
printf("\nCircular Queue is Empty!!!\n");
else{
int i= front;
printf("\nCircular queue is elements are:\n");
if(front <= rear){
while(i<= rear)
printf("%d\t",cQueue[i++]);
i=0;
while(i<=rear)
printf("%d\t",cQueue[i++]);
}
}
}










































