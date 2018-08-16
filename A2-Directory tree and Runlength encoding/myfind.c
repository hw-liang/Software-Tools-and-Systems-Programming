#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<dirent.h>
#include<sys/time.h>
#include<unistd.h>
#include<limits.h>

void searchdir(char *dirname,char *findme){
	char fullpath[100];
	DIR *dir;
	struct dirent *ptr;
	if((dir=opendir(dirname))!=NULL){
 		while((ptr=readdir(dir))!=NULL){
			if(ptr->d_type==4 && strcmp(ptr->d_name,".")!=0 && strcmp(ptr->d_name,"..")!=0){
				strcpy(fullpath,dirname);               
				
				if(fullpath[strlen(fullpath)-1]!='/'){
					strcat(fullpath,"/");
					strcat(fullpath,ptr->d_name);
				}else{
					strcat(fullpath,ptr->d_name);
				}
				if(strcmp(ptr->d_name,findme)==0){
					char printpath[1024];					
					realpath(fullpath,printpath);
					printf("%s\n",printpath);              
				}

				searchdir(fullpath,findme);                     
			}
			else if(strcmp(ptr->d_name,".")!=0 && strcmp(ptr->d_name,"..")!=0 && strcmp(ptr->d_name,findme)==0){
				strcpy(fullpath,dirname);
				if(fullpath[strlen(fullpath)-1]!='/')
					sprintf(fullpath,"%s/%s",fullpath,ptr->d_name);
				else{
					strcat(fullpath,ptr->d_name);
				}
				char printpath[1024];					
				realpath(fullpath,printpath);
				printf("%s\n",printpath); 
			}
		}
	closedir(dir);
	}
	else{
		printf("Failed to open %s!\n",dirname);
	}
}

int main(int argc,char**argv)
{
	if(argc!=3){
		printf("Invalid arguments\n");
		return 1;	
	}
	searchdir(argv[1],argv[2]);
	return 0;
}
