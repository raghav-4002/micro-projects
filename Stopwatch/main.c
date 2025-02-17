/* 
 * 'termios.h', 'time.h' and 'unistd.h' header files are only available 
 * in POSIX Compliant Operating Systems, like Unix and Unix-like OSes (including Linux and MacOs)
 * Thus this program won't run on Windows (as it is not POSIX Compliant).
 * In future, I will try to add compatibility for Windows also.
 */

#include <termios.h> /*Only in POSIX*/
#include <unistd.h>  /*Only in POSIX*/
#include <time.h>    /*Only in Posix*/
#include <stdlib.h>
#include <stdio.h>


void enable_raw_mode(void);
void disable_raw_mode(void);
void clear_screen(void);
void format_time(time_t seconds);


/*Hold original terminal attributes*/
struct termios orig_termios;


int main(void)
{
    /*Enable non-canonical mode*/
    enable_raw_mode();

    struct timespec start, end; /*Holds initial time and final time*/
    char status;                /*Holds status checking variable*/

    /*Get the starting time*/
    clock_gettime(CLOCK_MONOTONIC, &start);

    while(1) {
        /*Clear screen after every iteration of the loop*/
        clear_screen();
        
        /*Get the final time*/
        clock_gettime(CLOCK_MONOTONIC, &end);

        /*Find the time elapsed*/
        format_time(end.tv_sec - start.tv_sec);

        /*
         * 'read' is a system-call (provided by unistd.h)

         * I've configured read to wait only for one second.
         * If the user enters any character, then it breaks from loop,
         * else, it returns without waiting any further.

         * Thus, it acts just like 'sleep()' function from 'time.h'
          
         * Check function definition of 'enable_raw_mode()'
         */
        if(read(STDIN_FILENO, &status, 1) == 1)
            break;
    }
    
    return 0;
}


void enable_raw_mode(void)
{
    tcgetattr(STDIN_FILENO, &orig_termios);
    
    /*Enable canonical mode when the program exits*/
    atexit(disable_raw_mode);

    struct termios raw = orig_termios;

    raw.c_lflag &= ~(ICANON | ECHO); /*Disable canonical mode and input echoing*/
    raw.c_cc[VTIME] = 1;             /*Set waiting time for 'read()' to be 1 sec*/
    raw.c_cc[VMIN] = 0;              /*Set min characters to be read by 'read()' as 0*/

    tcsetattr(STDIN_FILENO, TCSAFLUSH, &raw);    
}


void disable_raw_mode(void)
{
    /*Restore the original terminal attributes*/
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &orig_termios);
}


void format_time(time_t seconds)
{
    time_t minutes, hours;

    minutes = seconds / 60;
    seconds = seconds % 60;

    hours = minutes / 60;
    minutes = minutes % 60;

    printf("======================================\n");
    printf("\tElapsed time: %.2ld:%.2ld:%.2ld\n", hours, minutes, seconds);
    printf("======================================\n");
        
    printf("\tEnter any key to exit\n");   
}


void clear_screen(void)
{
    system("clear");
}