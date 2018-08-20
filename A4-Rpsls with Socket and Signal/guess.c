#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <time.h>

int to_close = 0;
// Set SIGINT to show correct guess and try to kill parent and child
void end(int sig) {
  to_close = 1;
}
void init_sigint_end(){
  struct sigaction sigint;
  sigint.sa_handler = end;
  sigint.sa_flags = 0;
  sigemptyset(&sigint.sa_mask);
  sigaction(SIGINT,&sigint,NULL);
}

int main(){
  int pid = fork();
  if(pid < 0){
    perror("fork");
    exit(1);
  }else if(pid == 0){
    init_sigint_end();

    int secret;
    int valid_secret = 0;
    // Read until you get a valid secret number
    while(valid_secret == 0){
      fprintf(stdout,"Please enter your secret number(0-1023):");
      scanf("%d",&secret);
      if(secret>=0 && secret<=1023)
        valid_secret = 1;
    }

    // Read judgement and send signal
    char judge[10];
    for(;;){
      if(to_close == 0){
        sleep(1);
        fprintf(stdout,"Judge the guess(hi, lo or correct): ");
        scanf("%s",judge);
        if(strcmp(judge,"hi") == 0){
          fprintf(stderr,"%d # SIGUSR1 # %d\n",getpid(),getppid());
          if(kill(getppid(),SIGUSR1) < 0){
            perror("send signal");
            exit(1);
          }
        }else if(strcmp(judge,"lo") == 0){
          fprintf(stderr,"%d # SIGUSR2 # %d\n",getpid(),getppid());
          if(kill(getppid(),SIGUSR2) < 0){
            perror("send signal");
            exit(1);
          }
        }else if(strcmp(judge,"correct") == 0){
          fprintf(stderr,"%d # SIGINT # %d\n",getpid(),getppid());
          if(kill(getppid(),SIGINT) < 0){
            perror("send signal");
            exit(1);
          }
          pause(); //wait for sigint from parent
        }
      }else{
        fprintf(stderr, "%d # SIGKILL # %d\n",getpid(),getppid());
        if(kill(getppid(), SIGKILL) < 0){
          perror("send signal");
          exit(1);
        }
        break;
      }
    }
    return 0;
  }else{
    init_sigint_end();
    int guess;
    int lowest = 0;
    int higest = 1023;
    // Set SIGUSR1 to show higher guess
    void high(int sig){
      higest = guess - 1;
		}
    void init_sigusr1(){
      struct sigaction sigusr1;
      sigusr1.sa_handler = high;
      sigusr1.sa_flags = 0;
      sigemptyset(&sigusr1.sa_mask);
      sigaddset(&sigusr1.sa_mask, SIGINT);
      sigaction(SIGUSR1, &sigusr1, NULL);
    }
    // Set SIGUSR2 to show lower guess
    void low(int sig){
      lowest = guess + 1;
    }
    void init_sigusr2(){
      struct sigaction sigusr2;
      sigusr2.sa_handler = low;
      sigusr2.sa_flags = 0;
      sigemptyset(&sigusr2.sa_mask);
      sigaddset(&sigusr2.sa_mask, SIGINT);
      sigaction(SIGUSR2, &sigusr2, NULL);
    }
    init_sigusr1();
    init_sigusr2();

    int num_guess = 0;
    sleep(3);
    // Receive signal, adjust range and continue guessing
    for(;;){
      if(to_close == 0){
        num_guess += 1;	
        srand((unsigned)time(NULL));
        guess = (rand()%(higest+1-lowest)) + lowest;
        fprintf(stdout,"Guess the secret number is: %d\n",guess);
        pause();//wait for feedback from child before next guess
      }else{
        fprintf(stdout,"Number of guesses is: %d\n", num_guess);
        fprintf(stderr, "%d # SIGINT # %d\n",getpid(),pid);  
        if(kill(pid, SIGINT) < 0){
          perror("send signal");
          exit(1);
        }
        pause();
      }
    }
  }
	
  return 1; // something is wrong if we ever get here!
}
