#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void COMPRESS(FILE *original,FILE *compress){
    char current_ch;
    char formal_ch;
    int exist = 0;
    unsigned char count;

    if((exist=fread(&formal_ch,sizeof(char),1,original))!=1){
        fprintf(stderr, "Input file is empty\n");
        exit(1);
    }

    for(count=0;exist==1;count=0){
        do{
            count++;
            exist=fread(&current_ch,sizeof(char),1,original);
        }while(exist==1 && current_ch==formal_ch);

        fwrite(&count,sizeof(char),1,compress);
        fwrite(&formal_ch,sizeof(char),1,compress);
        formal_ch = current_ch;
    }
}

int main(int argc, char **argv){
    if(argc != 2){
        fprintf(stderr, "Invalid arguments\n");
		exit(1);
	}

    FILE *original = fopen(argv[1], "rb");
    if(original == NULL){
        fprintf(stderr, "Cannot open file\n");
        exit(1);
    }
	
    char *compress_file = argv[1];
    strcat(compress_file,".rle");
    FILE *compress = fopen(compress_file,"wb");
    if(compress == NULL){
        fprintf(stderr, "Cannot create file\n");
        exit(1);
    }

    COMPRESS(original,compress);

    if(fclose(original)!=0 || fclose(compress)!=0){
        fprintf(stderr,"ERROR:Fail to close the file\n");
        return 1;
    }

    return 0;
}

 
