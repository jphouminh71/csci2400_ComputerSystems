// 
// tsh - A tiny shell program with job control
// 
// <Jonathan Phouminh joph0114>
//  null -> nullptr 

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
// waitfg, sigchld_handler, sigstp_handler, sigint_handler
//
// The code below provides the "prototypes" for those functions
// so that earlier code can refer to them. You need to fill in the
// function bodies below.
// 

void eval(char *cmdline);  // done 
int builtin_cmd(char **argv);  // done 
void do_bgfg(char **argv);  // done , understand this 
void waitfg(pid_t pid);   // done, look at lecture slides to figure out how to correctly implement this. sleep(100) is bad. 

void sigchld_handler(int sig);  // done
void sigtstp_handler(int sig);  // done 
void sigint_handler(int sig); // done 

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

  //
  // This one provides a clean way to kill the shell
  //
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
void eval(char *cmdline) //     QUESTION: If we fork child processes with conditions, does it make any variables that
{                        // try to make use of these variables throw a warning of being possibly "uninitialized? line 209"
  /* Parse command line */
  //
  // The 'argv' vector is filled in by the parseline
  // routine below. It provides the arguments needed
  // for the execve() routine, which you'll need to
  // use below to launch a process.
  //
  char *argv[MAXARGS];
  pid_t processID;
  //
  // The 'bg' variable is TRUE if the job should run
  // in background mode or FALSE if it should run in FG
  //

  int bg = parseline(cmdline, argv); 
  if (argv[0] == NULL)  
    return;   /* ignore empty lines */
    
  // need to start handling the two cases where the input is either a built in command or not 
  /* if first argument passed isn't a built in command we need to fork a child process and run the job in the context of child. 
      need to change the newly forked child processes group ID so it wont get terminated. setgpid(0,0) 
          need to handle this new process the two cases where it can be a background process or foreground. 
      when we add child processes to the job lists we need to make sure we avoid all possible race conditions, 
          1. block the parents bit vector signals before the child process is exectued, then the child will inherit these properties. 
          2. fork the child process, change its group id then unblock its signals. 
          3. then unblock the parent signals. 
              if we don't do this we might run into problems when deleting / adding jobs because of unknown time of child processes. 
          
  */
      // install all the signals that we are going to have to use in this function 
  sigset_t mask;              // creates object for mask that will be used when blocking signals in parent process
  sigemptyset(&mask);         // empties the mask
  sigaddset(&mask, SIGCHLD);  // signal when child terminates
  sigaddset(&mask, SIGINT);   // signal that is used when we want to terminate a process
  sigaddset(&mask, SIGTSTP);  // signal that we want to use when we want to suspend or put process to sleep
    
  if (!builtin_cmd(argv)){                                                   // if its not a built in command, make a fork process. 
                                                                            // handle the first case of it being a background task specified by & at the end of the argument 
      sigprocmask(SIG_BLOCK, &mask, NULL);                                  // block parent from recieving any signals ,unblock this at the end
      if (bg){                                                                     // should have returned 1 if it was a background task 
          if ((processID = fork()) == 0){                                     // fork a child process
              sigprocmask(SIG_UNBLOCK, &mask, NULL);                           // unblock the bit vector for the child process
              setpgid(0,0);                                                        // need to change the group id so it wont get terminated. 
        
                                                                              // in the case where the argument is not found print and terminate
              if (execv(argv[0],argv) < 0){
                  printf("%s: Command not found.\n", argv[0]);
                  exit(0);
              }
              return;
       }
          
                                                                         // Add job to job list
      addjob(jobs,processID,BG,cmdline);                                  // figure out what needs to change HERE, giving you warning for uninitizalied processID
                                                                           // Prints out specific job information
      struct job_t* tempJob = getjobpid(jobs,processID);
      printf("[%d] (%d) %s",tempJob -> jid,tempJob -> pid,tempJob -> cmdline);
      }
    
                                             // now we need to handle it for the case of it being a foreground task, we need to make sure that this process finishes before
                                            // we can move on to another prompt to the command. 
  else{
      // fork a child process 
      if ((processID = fork()) == 0){
          // do the same thing and unblock all the signals and change it's group ID and check if its a built in command
          processID = sigprocmask(SIG_UNBLOCK, &mask, NULL);
          setpgid(0,0); //changes it process group 
          
          if (execv(argv[0], argv) < 0){
              printf("%s: Command not found.\n", argv[0]);
              exit(0); 
          }
          return;
      }
      // adds the job to the job list
      addjob(jobs,processID,FG,cmdline); 
      // Unblock the parent process and then we will not return until the processID has be returned(terminated) 
      sigprocmask(SIG_UNBLOCK, &mask, NULL);
      waitfg(processID); // wait for the process to finish, now implement this function 
  }
  return;
  
}
}


/////////////////////////////////////////////////////////////////////////////
//                    DONE
// builtin_cmd - If the user has typed a built-in command then execute
// it immediately. The command name would be in argv[0] and
// is a C string. We've cast this to a C++ string type to simplify
// string comparisons; however, the do_bgfg routine will need 
// to use the argv array as well to look for a job number.
//
int builtin_cmd(char **argv)  // function will essentially return true (1) if its a built in command for the eval function 
{
  
  string argument1(argv[0]);
  if (argument1 == "quit")
  {
      exit(0); 
  }
  else if ( argument1 == "fg")   // pass the argument to the do_bgfg to execute
  {
      do_bgfg(argv);
      return 1; // return true 
  }
  else if ( argument1 == "bg")  // same thing here 
  {
      do_bgfg(argv); 
      return 1; 
  }
  else if ( argument1 == "jobs" )   // call the function that lists all the current jobs 
  {
      listjobs(jobs);   // function is already implemented in jobs.cc 
      return 1; 
  }
  return 0;     /* not a builtin command */ // returns false
}

/////////////////////////////////////////////////////////////////////////////
//
// do_bgfg - Execute the builtin bg and fg commands
//
void do_bgfg(char **argv) 
{
  struct job_t *jobp=NULL;
    
  /* Ignore command if no argument */
  if (argv[1] == NULL) {
    printf("%s command requires PID or %%jobid argument\n", argv[0]);
    return;
  }
    
  /* Parse the required PID or %JID arg */
  if (isdigit(argv[1][0])) {
    pid_t pid = atoi(argv[1]);
    if (!(jobp = getjobpid(jobs, pid))) {
      printf("(%d): No such process\n", pid);
      return;
    }
  }
  else if (argv[1][0] == '%') {
    int jid = atoi(&argv[1][1]);
    if (!(jobp = getjobjid(jobs, jid))) {
      printf("%s: No such job\n", argv[1]);
      return;
    }
  }	    
  else {
    printf("%s: argument must be a PID or %%jobid\n", argv[0]);
    return;
  }

  //    ask for help FINISH THIS ONE NEXT 
  // You need to complete rest. At this point,
  // the variable 'jobp' is the job pointer
  // for the job ID specified as an argument.
  //
  // Your actions will depend on the specified command
  // so we've converted argv[0] to a string (cmd) for
  // your benefit.
  //
  string cmd(argv[0]);
  
  // send the continue signals to suspended processes 
  if (cmd == "fg"){
      if (jobp -> state == ST){
          kill(-jobp -> pid, SIGCONT); // send the continue signal with the kill function 
      }
      jobp -> state = FG; // the job state to be a foreground process then wait for it to finish 
      waitfg(jobp -> pid);
  }
  else if (cmd == "bg"){
      // do the same for background processes
      kill(-jobp -> pid, SIGCONT);
      jobp -> state = BG; 
      printf("[%d] (%d) %s", jobp -> jid, jobp -> pid, jobp -> cmdline); 
  }
  
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// waitfg - Block until process pid is no longer the foreground process
//
void waitfg(pid_t pid)
{
  // 
  while(1){
      // first we need check if a foreground process exist, shouldn't happen though because it should only be called during fg task
      struct job_t* job = getjobpid(jobs,pid);
      if (job == NULL)   // edge case for when the job doesn't exist 
      {
          return;
      }
      if (job -> state != FG)
      {
          return;
      }
      sleep(100);   // theres another way to do this, look in lecture notes. This is inefficient and doesn't always guarentee
      //int sigsuspend(const sigset_t *mask);
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
   int status = -1; 
   pid_t processID ;   // process ID o
   
       
   while ( (processID = waitpid(-1, &status, WNOHANG|WUNTRACED)) > 0) // this loop will reap the process by checking the status
   {
       // delete the child process in the job list
       if (WIFEXITED(status))
       {
           deletejob(jobs,processID);  // deletes the child from the job list, it gets checked then reaped
       }
       // stop the signal for sigstop, prints out the shell whatever child proccess got stoped then changes that state to stopped
       else if (WIFSIGNALED(status))
       {
           printf("Job [%d] (%d) terminated by signal %d \n",pid2jid(processID),processID,WTERMSIG(status));
           deletejob(jobs, processID); /* Delete the child from the job list */
       }
       else if (WIFSTOPPED(status))
       {
          printf("Job [%d] (%d) stopped by signal %d \n",pid2jid(processID),processID,WSTOPSIG(status));
          job_t* tempjob = getjobpid(jobs, processID);
          tempjob -> state = ST;
       }
       // checks the status if any other signal was thrown at the child process , then reaps
      
   }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//
// sigint_handler - The kernel sends a SIGINT to the shell whenver the
//    user types ctrl-c at the keyboard.  Catch it and send it along
//    to the foreground job.  
// this should be very similar to how sigstp_handler functions but just send it the signal that terminates the process 
void sigint_handler(int sig) 
{
  pid_t processID = fgpid(jobs); 
  if (processID != 0) 
  {
      kill(-processID, sig); 
  }
  return;
}

/////////////////////////////////////////////////////////////////////////////
//                      DONE 
// sigtstp_handler - The kernel sends a SIGTSTP to the shell whenever
//     the user types ctrl-z at the keyboard. Catch it and suspend the
//     foreground job by sending it a SIGTSTP.  
//   ONLY SUSPEND FOREGROUND PROCESSES , including everything that has the same process group ID
void sigtstp_handler(int sig)   
{
        // get access the process ID or the JID and check if there even exists one, then if there is send the signal SIGSTP
  pid_t processID = fgpid(jobs); // jobs.cc has this implemented, returns the process id of current process or 0 if null. 
                                // we know there is a foreground job if the processID is greater than 0. 
  if ( processID != 0 ) // send it the signal 
  {
      kill(-processID, sig);   // kill function is used to send signal to program , - is used because it sends it to everything                            // in the same group as whatever processID is in. 
  }
      
  return;
}

/*********************
 * End signal handlers
 *********************/


/* 
    NOTES : 
        The first word in the command line is either a built-in command or a pathname to an executable and the remaining words
    serve as command line arguements (argv[1], argv[2], etc..). 
        if the first word is a built in command then the shell will execute that command in the current process. 
         for our shell, when a built-in command needs to be executed, it will fork a child process and then runs and loads that 
         program in the context of the child process 
    - child processes that are created as a result of interpretting a single command line argument are known as jobs. 
        - a job can consist of multiple child processes connected by unix pipes 
        
    IF the command line argument ends with , &, then this job will take place in the background process. 
        - Background processes are when the shell doesn't wait for a job to terminate before printing the prompt and awaiting the next command line. 
            - so you execute the process and then let the shell do its thing like normal and ask for more inputs 
            
    OHTERWISE every process that needs to be executed will be executed in the foreground 
        - processes must execute completely before the shell prompts the user for another command
        
Unix Shells support Job Control 
    Job control allows the user to move jobs back and forth between foreground processes and background processes and also 
        allow the user to change the state of jobs { running , stopped, or terminated }. 
        
What each command should do 
    typing ctrl-c should send the signal , SIGINT, to each proccess in the foreground job. 
                                                The default action for SIGINT is to terminate the process
                                                    so you need to implement ctrl-c signal to terminate all foreground processes.
                                  if there is no foreground processes than this should do nothing 
    typing ctrl-z should send the signal , SIGSTP, te each process in the foreground job 
                                                The default action of SIGSTP is to put the process into stopped state until it is 
                                                awakened by a SIGCONT signal. 
                                                    so you need to implement ctrl-z to signal to stop all foreground processes.
                                                    
                                                    
Features that you shell should have : 
    The prompt string should print out "tsh> "  , yes it already does this 
    Strings entered from the command line will be entered in this format 
        name , (other arguments) 
      if name is a built in command than it should be executed immediately. and wait for the next command line 
          if name isn't a built in command line than it should assume that, name, is an executable PATH which it will load
          load and run in the context of a child process. Have to fork and make it run the path 


Other Notes 
    Each job can be identified by either its PID or JID, which is a positive integer given by tsh
        job id's are used because some scripts use them to manipulate certain jobs, and PID can change across runs. 
        JID's should be noted by % . 
            Ex ) %5 denotes JID and 5 denotes PID
            
            
tsh should also support the following commands 
    quit , this command should terminate the shell 
    jobs , this command will list all the current jobs 
    bg <job> , this command restarts the <job> by sending it a SIGCONT signal, then runs it in the background
        argument can be given as a PID or JID
    fg <job> , this commaands restarts the <job> by sending it a SIGCONT signal, then runs it in the foreground
        argument can also be given as PID or JID 
      
      
     *** TSH SHOULD REAP ALL ITS CHILDREN 
         If any job terminates because it recieves a signal that it didn't catch, then TSH should recognize this event and 
             print the message with the jobs PID and a description of the offending signal 

*/



