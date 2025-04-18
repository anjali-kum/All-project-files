#include<stdio.h>
int main(){
	char ch= 'A';
	while(ch<='D')
	{
		switch(ch)
		{
			case 'A':
			case 'B':
				ch++;
				continue;
			case 'C':
			case 'D':
				ch++;
		}
		printf("%c",ch);
	}
}

