/* The purpose of this program is to practice writing signal handling
 * functions and observing the behaviour of signals.
 */

#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>

/* Global variables to store number of read operations and seconds elapsed. 
 */
long num_reads, seconds;

/* Message to print in the signal handling function. */
#define MESSAGE "%ld reads were done in %ld seconds.\n"
void print_message(){
    printf(MESSAGE,num_reads,seconds);
    exit(0);
}
/* The first command line argument is the number of seconds to set a timer to run.
 * The second argument is the name of a binary file containing 100 ints.
 * Assume both of these arguments are correct.
 */
void init_sigaction(void){
    struct sigaction act;
    act.sa_handler = print_message;
    act.sa_flags = 0;
    sigemptyset(&act.sa_mask);
    sigaction(SIGPROF,&act,NULL);
}

void init_time(){
    struct itimerval value;
    value.it_value.tv_sec = seconds;
    value.it_value.tv_usec = 0;
    value.it_interval = value.it_value;
    setitimer(ITIMER_PROF,&value,NULL);
}

int main(int argc, char **argv) {
    if (argc != 3) {
        fprintf(stderr, "Usage: time_reads s filename\n");
        exit(1);
    }
    seconds = strtol(argv[1], NULL, 10);
    
    FILE *fp;
    if ((fp = fopen(argv[2], "r")) == NULL) {
      perror("fopen");
      exit(1);
    }

    /* In an infinite loop, read an int from a random location in the file,
     * and print it to stderr.
     */
    init_sigaction();
    init_time();

    num_reads = 0;
    for (;;) {
        num_reads++;
        srand((unsigned)time(NULL));
        int offset = (rand()%100)*4;
        fseek(fp,offset,SEEK_SET);
        int number;
        fread(&number,sizeof(int),1,fp);
        rewind(fp);
        printf("%d\n",number);
    }
    return 1; // something is wrong if we ever get here!
}
