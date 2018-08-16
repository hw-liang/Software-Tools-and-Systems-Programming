#include<stdio.h>

int main()
{
char str[11];
int i;

scanf("%s",str);
scanf("%d",&i);

if(i==0)
{printf("%s\n",str);return 0;}
else if(i<=9 && i>=1)
{printf("%c\n",str[i-1]);return 0;}
else 
{printf("Error\n");return 1;}

}
