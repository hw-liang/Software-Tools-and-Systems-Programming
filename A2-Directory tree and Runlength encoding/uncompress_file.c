#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void UNCOMPRESS(FILE *original,FILE *uncompress){
    unsigned char count = 0;
    char value;
    int exist1 = 0;
    int exist2 = 0;

    exist1 = fread(&count,sizeof(char),1,original);
    exist2 = fread(&value,sizeof(char),1,original);
    if(exist1!=1||exist2!=1){
        fprintf(stderr, "Input file is empty\n");
        exit(1);
    }
	
    while(exist1==1 && exist2==1){
        for(;count>0;count--){
            fwrite(&value,sizeof(char),1,uncompress);
        }
        exist1 = fread(&count,sizeof(char),1,original);
        exist2 = fread(&value,sizeof(char),1,original);
    }
}

int main(int argc, char **argv) {
    if(argc != 2){
        fprintf(stderr, "Invalid arguments\n");
        exit(1);
    }

    char check[4];
    if(strlen(argv[1])<5){
        fprintf(stderr, "Invalid argument\n");
        exit(1);		
    }else{
        for(int i=0;i<4;i++) 
            check[i] = argv[1][strlen(argv[1])-1-i];
        check[4] = '\0';
        if(strcmp(check,"elr.")!=0){
            fprintf(stderr, "Invalid argument\n");
            exit(1);
        }
    }

    FILE* original = fopen(argv[1], "rb");
    if(original == NULL){
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }

    char* uncompress_file = argv[1];
    uncompress_file[strlen(argv[1])-4] = '\0';
    FILE* uncompress = fopen(uncompress_file,"wb");
    if(uncompress == NULL){
        fprintf(stderr, "Fail to create file\n");
        exit(1);
    }
    UNCOMPRESS(original,uncompress);

    if(fclose(original)!=0 || fclose(uncompress)!=0){
        fprintf(stderr,"ERROR:Fail to close the file\n");
        return 1;
    }

    return 0;
}

