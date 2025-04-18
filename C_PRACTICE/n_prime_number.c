#include<stdio.h>
#include<stdbool.h>

bool prime(int n){
if(n==1 || n==0){
return false;
}
for(int i=2;i<= n/2;i++){
if(n%i==0){
return false;
}
return true;
}
int main(){
int n=40;


for(int i=1;i<=n;i++){
if(prime(i)){
printf("%d",i);
}
}
return 0;
}
}
