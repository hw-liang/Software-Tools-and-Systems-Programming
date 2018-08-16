#include<stdio.h>
#include <stdlib.h>

int check_permissions(char*testdata, char*groudtruth){
    for (int i = 0; i < 9; i++){
    	if (testdata[i] == '-' && testdata[i] != groudtruth[i]){
	    return 1;
	}
    }
    return 0; 
}


int main(int argc, char** argv){
    if (argc != 2 && argc != 3){
        fprintf(stderr, "USAGE: count_small size [permissions]\n");
        return 1;
    }
    
    int i=2;
    while(i){
        scanf("%*s");
        i--;
    }

    char string1[11];
    char string2[32];
    char string3[32];
    char string4[32];
    int  test_size;
    char string6[32];
    char string7[32];
    char string8[32];
    char string9[32];
    int count =0;
    int size = strtol(argv[1],NULL,10);
    while (scanf("%s %s %s %s %d %s %s %s %s\n", string1, string2, string3, string4, &test_size, string6, string7, string8, string9) != EOF){
	if (argc == 2){
	    if (test_size < size){
	        count++;
	    }
	} 
        else{
	    int permission = check_permissions(string1+1, argv[2]);
	    if (test_size < size && permission == 0){
	        count++;
            }
	}
    }
    printf("%d\n", count);
    return 0;
}	
