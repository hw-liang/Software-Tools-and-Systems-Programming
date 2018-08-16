#include<stdio.h>
#include <stdlib.h>

int populate_array(int sin_num,int*sin_array){
    int digit=0;
    for(int m=sin_num;m!=0;m/=10)
	digit++;
    if(digit!=9) 
        return 1;
    for(int j=0;j<9;j++){
	int i = sin_num % 10;
	sin_num = sin_num/10;
	sin_array[8-j] = i;	    
    }
    return 0;
}

int check_sin(int*sin_array){
    int result[9];
    int inter[2];
    int product = 0;
    for(int i=0;i<9;i++){
        int multiplier = 1;
	if(i%2 !=0) 
	    multiplier = 2;

	product = multiplier*sin_array[i];
	for(int j=0;j<2;j++){
	    inter[j] = product % 10;
            product = product/10;
	}

	if(inter[1]==0) 
	    result[i]=inter[0];
	else 
	    result[i] = inter[0]+inter[1];    
    }
	int sum = 0;
	for(int i=0;i<9;i++)
	    sum = sum + result[i];
	if(sum%10==0)
	    return 0;
	else
	    return 1;
}

