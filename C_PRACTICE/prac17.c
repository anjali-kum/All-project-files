#include<stdio.h>
int main(){
	char ch='A';
	switch(ch)
	{
		case 'A':  case 'B':
			ch++;
			continue;
		case 'C':      case 'D':
			ch++;
	}
}

