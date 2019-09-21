// 
// tsh - A tiny shell program with job control
// 
// Jonathan Phouminh
//

using namespace std;

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string>

#include "globals.h"
#include "jobs.h"
#include "helper-routines.h"

//
// Needed global variable definitions
//

static char prompt[] = "tsh> ";
int verbose = 0;

//
// You need to implement the functions eval, builtin_cmd, do_bgfg,
// waitfg, _handler, sigstp_handler, sigint_handler
//
// The code below provides the "prototypes" for those functions
// so that earlier code can refer to them. You need to fill in the
// function bodies below.
// 

void eval(char *cmdline);
int builtin_cmd(char **argv);
void do_bgfg(char **argv);
void waitfg(pid_t pid);

void sigchld_handler(int sig);
void sigtstp_handler(int sig);
void sigint_handler(int sig);

// Helper prototypes
int pid2jid(pid_t pid);

//
// main - The shell's main routine 
//
int main(int argc, char **argv) 
{
    int emit_prompt = 1; // emit prompt (default)

    //
    // Redirect stderr to stdout (so that driver will get all output
    // on the pipe connected to stdout)
    //
    dup2(1, 2);

    /* Parse the command line */
    char c;
    while ((c = getopt(argc, argv, "hvp")) != EOF) {
    switch (c) {
    case 'h':             // print help message
        usage();
        break;
    case 'v':             // emit additional diagnostic info
        verbose = 1;
        break;
    case 'p':             // don't print a prompt
        emit_prompt = 0;  // handy for automatic testing
        break;
    default:
        usage();
    }
    }

    //
    // Install the signal handlers
    //

    //
    // These are the ones you will need to implement
    //
    Signal(SIGINT,  sigint_handler);   // ctrl-c
    Signal(SIGTSTP, sigtstp_handler);  // ctrl-z
    Signal(SIGCHLD, sigchld_handler);  // Terminated or stopped child

    // This one provides a clean way to kill the shell
  
    Signal(SIGQUIT, sigquit_handler); 

    //
    // Initialize the job list
    //
    initjobs(jobs);

    //
    // Execute the shell's read/eval loop
    //
    for(;;) {
    //
    // Read command line
    //
    if (emit_prompt) {
        printf("%s", prompt);
        fflush(stdout);
    }

    char cmdline[MAXLINE];

    if ((fgets(cmdline, MAXLINE, stdin) == NULL) && ferror(stdin)) {
        app_error("fgets error");
    }
    //
    // End of file? (did user type ctrl-d?)
    //
    if (feof(stdin)) {
        fflush(stdout);
        exit(0);
    }

    //
    // Evaluate command line
    //
    eval(cmdline);
    fflush(stdout);
    fflush(stdout);
    } 

    exit(0); //control never reaches here
}
  
/////////////////////////////////////////////////////////////////////////////
//
// eval - Evaluate the command line that the user has just typed in
// 
// If the user has requested a built-in command (quit, jobs, bg or fg)
// then execute it immediately. Otherwise, fork a child process and
// run the job in the context of the child. If the job is running in
// the foreground, wait for it to terminate and then return.  Note:
// each child process must have a unique process group ID so that our
// background children don't receive SIGINT (SIGTSTP) from the kernel
// when we type ctrl-c (ctrl-z) at the keyboard.
//
void eval(char *cmdline) 
{
  
    /* Parse command line */
    // Parseline routine fills in argv vector which provides args needed for execve which is required to launch a process.
    char *argv[MAXARGS];
    pid_t pid;

    // bg is true if job should be in background, false if it should run in foreground
    int bg = parseline(cmdline, argv); 
    if (argv[0] == NULL) return;   /* ignore empty lines */

    /* handle background and forground
    * tasks, if foregounds, send control to terminal
    * also check if its a builtin_cmd
    */ 
  
    // Sets up a mask that blocks signals when adding a job
    // Job should not be deleted before it's added
    sigset_t mask;
    sigemptyset(&mask);
    sigaddset(&mask, SIGCHLD);
    sigaddset(&mask, SIGINT);
    sigaddset(&mask, SIGTSTP);
  
    if(!builtin_cmd(argv))
    {
        // blocks signals
        sigprocmask(SIG_BLOCK, &mask, NULL);
        // Not a built in command
        if(bg) // Background task
    {
      if((pid=fork()) == 0){// Unblocks and sets a pid for child
                sigprocmask(SIG_UNBLOCK, &mask, NULL);
                setpgid(0,0);
          
                // if it can't find the command at all
                if(execv(argv[0],argv) < 0){
                    printf("%s: Command not found. \n", argv[0]);
                    exit(0);
                }
                    return;
        }
        // Parent
        // Add the job
        addjob(jobs,pid,BG,cmdline);
        // Prints out specific job information
        struct job_t* job = getjobpid(jobs,pid);
        printf("[%d] (%d) %s",job->jid,job->pid,job->cmdline);
      
      
    }
    else{ // Foreground task
      
        if((pid=fork()) == 0){
        // unblocks and sets pid for child again
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        setpgid(0,0);
        
        // Checks for valid command again
        if(execv(argv[0],argv) < 0){
                printf("%s: Command not found. \n", argv[0]);
                exit(0);
                }
                return;
            
        }
        
        
        addjob(jobs,pid,FG,cmdline);
        // Unblock and wait till done in foreground
        sigprocmask(SIG_UNBLOCK, &mask, NULL);
        waitfg(pid);
    }
  } 

    return;
}


/////////////////////////////////////////////////////////////////////////////
//
// builtin_cmd - If the user has typed a built-in command then execute
// it immediately. The command name would be in argv[0] and
// is a C string. We've cast this to a C++ string type to simplify
// string comparisons; however, the do_bgfg routine will need 
// to use the argv array as well to look for a job number.
//
int builtin_cmd(char **argv) 
{
    // gives you the element of the array that holds the potential built in command
    string cmd(argv[0]);
  
    // checks to see if quit or exit first
    if(cmd == "quit") exit(0);
  
    //checks if it is a foreground or background task
    else if(cmd == "fg" || cmd == "bg"){
    do_bgfg(argv);
    return 1;
    }
    // Prints job list
    else if(cmd == "jobs"){
    listjobs(jobs);
    return 1;
    }

    return 0;
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv) 
{
    struct job_t *jobp=NULL;
    
    //ignores if no arg
    if (argv[1] == NULL) {
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }
    
    // if it is a pid
    if (isdigit(argv[1][0])) {
    pid_t pid = atoi(argv[1]);
    if (!(jobp = getjobpid(jobs, pid))) {
        printf("(%d): No such process\n", pid);
        return;
        }
      }
    //if it is a jid
    else if (argv[1][0] == '%') {
    int jid = atoi(&argv[1][1]);
    if (!(jobp = getjobjid(jobs, jid))) {
        printf("%s: No such job\n", argv[1]);
        return;
    }
    }     
    //if it is not pid or jid
    else {
        printf("%s: argument must be a PID or %%jobid\n", argv[0]);
        return;
    }
    
    // You need to complete rest. At this point,
    // the variable 'jobp' is the job pointer
    // for the job ID specified as an argument.
      // Your actions will depend on the specified command
      // so we've converted argv[0] to a string (cmd) for
      // your benefit.
      string cmd(argv[0]);

    if(cmd == "fg"){
        // continues job if it is stopped
        if(jobp->state==ST){
            kill(-jobp -> pid, SIGCONT); //cotinues the job, and then kills the continue signal
            }
        jobp -> state = FG;//sets state to foreground job
        waitfg(jobp -> pid);// wait for it to finish
        
        }
        else if(cmd == "bg"){
        //continue if it is background job
        kill(-jobp -> pid, SIGCONT);
        //changes the state to background
        jobp -> state = BG;
        printf("[%d] (%d) %s",jobp -> jid, jobp -> pid, jobp->cmdline);
    }
  
}

/////////////////////////////////////////////////////////////////////////////
//
// waitfg - Block until process pid is no longer foreground
//
void waitfg(pid_t pid)
{
    for(;;) {
    // Set up a new structure to check that it will wait for a valid job and that its a foreground job
    struct job_t *job = getjobpid(jobs,pid);
    if ( job == NULL ) {
        return;
        }
    if (job -> state != FG) {
        return;
        }
    sleep(100);
  
    }
  
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// Signal handlers
//


/////////////////////////////////////////////////////////////////////////////
//
// sigchld_handler - The kernel sends a SIGCHLD to the shell whenever
//     a child job terminates (becomes a zombie), or stops because it
//     received a SIGSTOP or SIGTSTP signal. The handler reaps all
//     available zombie children, but doesn't wait for any other
//     currently running children to terminate.  
//
void sigchld_handler(int sig) 
{
    pid_t pid;
    int status = -1;
  
    while((pid = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0){
    
    // delete job signal for sigchld
    if ( WIFEXITED(status) ) { 
        deletejob(jobs, pid); /* Delete the child from the job list */
      
    // delete signal for sigint
    }
        else if( WIFSIGNALED(status) ){ 
            printf("Job [%d] (%d) terminated by signal %d \n",pid2jid(pid),pid,WTERMSIG(status));
            deletejob(jobs, pid); /* Delete the child from the job list */
      
          // stop signal for sigtstp
    }
    else if( WIFSTOPPED(status) ){
      printf("Job [%d] (%d) stopped by signal %d \n",pid2jid(pid),pid,WSTOPSIG(status));
      job_t* temp = getjobpid(jobs, pid);
      temp -> state = ST;
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//
// sigint_handler - The kernel sends a SIGINT to the shell whenver the
//    user types ctrl-c at the keyboard.  Catch it and send it along
//    to the foreground job.  
//
void sigint_handler(int sig) 
{
    // Saves foreground task in temp structure
    pid_t temp = fgpid(jobs);
  
    // Kills foreground task if there is any
    if(temp != 0)
    {
        kill(-temp, SIGINT);
    }
  
    return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
//     the user types ctrl-z at the keyboard. Catch it and suspend the
//     foreground job by sending it a SIGTSTP.  
//
void sigtstp_handler(int sig){
  
    //Saves FG task in temp structure
    pid_t temp = fgpid(jobs);
  
    // Pauses foreground task if there is any
    if(temp != 0)
    {
        kill(-temp, SIGTSTP);
    }
  
    return;
}

/*********************
 * End signal handlers
 *********************/





