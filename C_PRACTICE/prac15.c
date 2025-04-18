#include<stdio.h>
int main(){
	int i=1;
	while(i<10)
	{
		if(i==3)
			continue;
		printf("%d ",i);
		//printf("%d",printf);
		i++;
	}
}

