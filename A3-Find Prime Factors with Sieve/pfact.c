#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <math.h>

int main(int argc, char **argv) {
	int n;
	if(argc != 2 || (n = strtol(argv[1],NULL,10)) <= 1){
		fprintf(stderr,"Usage:\n\tpfact n\n");
		exit(1);
	}
	double root = sqrt(n);
	
	// Use filter_key to store the value of filter
	// Use factors to store the factors of input number
	
	//attributes that parent and child share
	int status;
	int filter_key;
	int factors[2];
	int num_factors = 0;

	// Use flag to mark if we continue to create child process
	// Use result to keep the decision on the input number
	//		0: no factors  
	//		1: two equal prime factors
	//		2: two unequal prime factors
	//		3: more than two prime factors

	// attributes that belong to parent
	int flag = 1;
	// attributes that belong to child
	int result;

	int fd1[2];
	int fd2[2];
	int read_status,write_status;
	if(pipe(fd1) == -1){
		perror("pipe");
		exit(-1);
	}

	int p = fork();
	if(p < 0){
		perror("fork");
		exit(-1);
	}else if(p > 0){
		if(close(fd1[0]) == -1){
			perror("close read in main\n");
			return 1;
		}
		// write 2 to n-1 to pipe.
		for(int i = 2; i < n+1; i++){
			if ((write_status = write(fd1[1], &i, sizeof(int))) == -1){
				perror("write in main parent\n");
				return 1;
			}
		}
		if(close(fd1[1]) == -1){
			perror("close write in main parent\n");
			return 1;
		}
		int main_status;
		if(wait(&main_status)!=-1){
			if(WIFEXITED(main_status)){
				printf("Number of filters = %d\n", WEXITSTATUS(main_status));
				return 0;
			}else{
				printf("Process terminates abnormally.\n");
				exit(1);
			}
		}else{
			perror("wait in main parent");
			exit(-1);
		}
	}else{
		while(flag){
			if(p < 0){
				perror("fork");
				exit(-1);
			}else if(p == 0){
				fd2[0] = fd1[0];
				fd2[1] = fd1[1];
				if(close(fd2[1]) == -1){
					perror("close write in child\n");
					exit(-1);
				}
				// read in the filter_key
				if((read_status = read(fd2[0], &filter_key, sizeof(int))) == -1){
					perror("read\n");
					exit(-1);
				}
				if(filter_key < root){
					// the case where filter_key is smaller than the square root of n
					if(n % filter_key == 0){
						if(num_factors == 1){
							// the case where filter_key is the second factor
							int receiver;
							for(int i = 0; i < n - 1; i++){
								if((read_status = read(fd2[0], &receiver, sizeof(int))) == -1){
									perror("read\n");
									exit(-1);
								}
								if(read_status != sizeof(int))
									break;
							}
							if (close(fd2[0]) == -1) {
								perror("close read in child\n");
								exit(-1);
							}
							result = 3;
							flag = 0;
						}else{
							// the case where filter_key is the first factor
							factors[num_factors] = filter_key;
							num_factors++;
							if (pipe(fd1) == -1) {
								perror("pipe\n");
								exit(-1);
							}
							p = fork();
						}
					}else{
						if(pipe(fd1) == -1){
							perror("pipe\n");
							exit(-1);
						}
						p = fork();
					}
				}else if(filter_key == root){
					// the case where filter_key is equal to the square root of n
					int receiver;
          for(int i = 0; i < n - 1; i++) {
            if((read_status = read(fd2[0], &receiver, sizeof(int))) == -1) {
              perror("read\n");
              exit(-1);
            }
						if(read_status != sizeof(int))
							break;
          }
          if (close(fd2[0]) == -1) {
		        perror("close read in child\n");
		        exit(-1);
          }
          flag = 0;
          result = 1;
					factors[0] = filter_key;
					factors[1] = filter_key;
					num_factors = num_factors + 2;
				}else{
					// the case where filter_key is larger than the square root of n
          int possible_factors[n-1];
          possible_factors[0] = filter_key;
					int receiver;
					for(int i = 1; i < n - 1; i++){
						if((read_status = read(fd2[0], &receiver, sizeof(int))) == -1){
							perror("read\n");
							exit(-1);
						}
						if(read_status != sizeof(int))
							break;
						possible_factors[i] = receiver;
					}
					if (close(fd2[0]) == -1) {
						perror("close read in child\n");
						exit(-1);
					}
					flag = 0;
					if(num_factors == 1){
						// look for the other factor 
						int possible_factor = n / factors[0];
						int find = 0;
						for(int i = 0; i < n-1; i++){
							if(possible_factors[i] == possible_factor){
								find = 1;
								break;
							}
						}
						if(find == 1){
							result = 2;
							factors[num_factors] = possible_factor;
							num_factors++;
						}else{
							result = 3;
						}
					}else{
						result = 0;
					}
				}
				// determine whether the loop terminates or not
				if(flag == 0){
					if(result == 3){
						printf("%d is not the product of two primes\n", n);
					}else if(result == 0){
						printf("%d is prime\n", n);
					}else{
						printf("%d %d %d\n", n, factors[0], factors[1]);
					}
					exit(0);
				}
			}else{
				if(close(fd1[0]) == -1){
					perror("close read in parent\n");
					return 1;
				}
				int sender;
				// pipe the numbers that pass the filter
				for(int i = 0; i < n - 1; i++){
					if((read_status = read(fd2[0], &sender, sizeof(int))) == -1){
						perror("read\n");
						exit(-1);
					}
					if(read_status != sizeof(int))
						break;
					if(sender % filter_key != 0){
						if((write_status = write(fd1[1], &sender, sizeof(int))) == -1){
							perror("write\n");
							exit(-1);
						}
					}
				}

				if(close(fd2[0]) == -1){
					perror("close read in child\n");
					exit(-1);
				}
				if(close(fd1[1]) == -1){
					perror("close write in parent\n");
					exit(-1);
				}
				if(wait(&status)!=-1){
					if(WIFEXITED(status)){
						exit(1 + WEXITSTATUS(status));
					}else
						printf("Process terminates abnormally.\n");
						exit(-1);
				}else{
					perror("wait");
					exit(-1);
				}
			}
		}
	}
}
