#include<stdio.h>
#include <stdlib.h>

int populate_array(int, int *);
int check_sin(int *);

int main(int argc,char**argv){
    if(argc!=2)
        return 1;
    int sin_array[9];
    int sin_num = strtol(argv[1],NULL,10);
    int condition1 = populate_array(sin_num,sin_array);
    int condition2 = check_sin(sin_array);
    if(condition1==0 && condition2==0){
        printf("Valid SIN\n");
        return 0;
    }else{
        printf("Invalid SIN\n");
        return 1;
    }
}


