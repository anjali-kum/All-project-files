// Program of single linked list
#include<stdio.h>
#include<malloc.h>
struct node{
	int info;
	struct node *link;
}
*start;
main(){
	int choice,n,m,position,i;
	while(1){
		printf("1.Create list\n");
		printf("2.Add at Begining\n");
		printf("3.Add After \n");
		printf("4.Delete\n");
		printf("5.Display\n");
		printf("6.search\n");
		printf("7.Quit\n");
		printf("Enter your choice: ");
		scanf("%d",&choice);
		switch(choice){
			case 1:
				start=NULL;
				printf("How many nodes you want: ");
				scanf("%d",&n);
				for(i=0;i<n;i++){
					printf("Enter the element: ");
					scanf("%d",&m);
					create_list(m);
				}
				break;
			case 2:
				printf("Enter the Element:");
				scanf("%d",&m);
				break;
			case 3:
				printf("Enter the element :");
				scanf("%d",&m);
				printf("Enter the position after which this element is inserted: :");
				scanf("%d",&position);
				addafter(m,position);
				break;
			case 4:
				if(start==NULL)
				{
					printf("List is Empty\n");
					continue;
				}
				printf("Enter the element for deletion : ");
				scanf("%d",&m);
				del(m);
				break;
			case 5: 
				display();
				break;
			case 6:
				printf("Enter the element to be searched:");
				scanf("%d",&m);
				search(m);
				break;
			case 7:
				exit();
			default:
				printf("Wrong choice\n");
		}
	}
}

