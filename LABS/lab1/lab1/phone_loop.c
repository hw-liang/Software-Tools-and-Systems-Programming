#include<stdio.h>

int main()
{
char str[11];
int i;
scanf("%s",str);
while(scanf("%d",&i)!=EOF)
{
if(i==0)
{printf("%s\n",str);}
else if(i<=9 && i>=1)
{printf("%c\n",str[i-1]);}
else 
{printf("ERROR.\n");}
}
if(i<0||i>9) 
return 1;
else 
return 0;
}
