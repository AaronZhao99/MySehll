/* ************************************************************************
> File Name:     myshell.c 
> Author:        Binhao Zhao 
> Created Time:  March 2022
> Description:   shell project for OS course 
 ************************************************************************/
 
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <dirent.h>

#ifndef FALSE
#define FALSE 0
#endif 

#ifndef TRUE
#define TRUE 1
#endif

#define MAX_BUFFER 1024      // max line buffer
#define MAX_ARGS 64          // max # args
#define MAX_PATH_LENGTH 256  // max path length
#define SEPARATORS " \t\n"   // token sparators
#define README "readme"      // help file name
#define CLEAR "\e[1;1H\e[2J" // clear

struct shellstatus_st
{
    int foreground;  // foreground execution flag
    char *infile;    // input redirection flag & file
    char *outfile;   // output redirection flag & file
    char *outmode;   // output redirection mode
    char *shellpath; // full pathname of shell
};
typedef struct shellstatus_st shellstatus;

extern char **environ;

void check4redirection(char **, shellstatus *); // check command line for i/o redirection
void errmsg(char *, char *);                    // error message printout
void execute(char **, shellstatus);             // execute command from arg array
char *getcwdstr(char *, int);                   // get current work directory string
FILE *redirected_op(shellstatus);               // return required o/p stream
char *stripath(char *);                         // strip path from filename
void syserrmsg(char *, char *);                 // system error message printout

/*******************************************************************/

int main(int argc, char **argv)
{
    FILE *ostream = stdout;   // (redirected) o/p stream
    FILE *instream = stdin;   // batch/keyboard input
    char linebuf[MAX_BUFFER]; // line buffer
    char cwdbuf[MAX_BUFFER];  // cwd buffer
    char *args[MAX_ARGS];     // pointers to arg strings
    char **arg;               // working pointer thru args
    char *prompt = "==>";     // shell prompt
    char *readmepath;         // readme pathname
    shellstatus status;       // status structure
    int execargs;             // execute command in args

    // parse command line for batch input
    switch (argc)
    {
    case 1:
    {
        // keyboard input
        // TODO
        /*get cwd*/
	    getcwdstr(cwdbuf, MAX_PATH_LENGTH);
        break;
    }

    case 2:
    {
        // possible batch/script
        // TODO
        /* redirection to the batch/script */
        getcwdstr(cwdbuf, MAX_PATH_LENGTH);
	    freopen(argv[1], "r", instream); 
	    break;
    }
    default: // too many arguments
        fprintf(stderr, "%s command line error; max args exceeded\n"
                        "usage: %s [<scriptfile>]",
                stripath(argv[0]), stripath(argv[0]));
        exit(1);
    }

    // get starting cwd to add to readme pathname
    // TODO
    char tempPath[MAX_PATH_LENGTH];
    strcat(strcpy(tempPath, cwdbuf), "/readme");
    readmepath = tempPath;

    // get starting cwd to add to shell pathname
    // TODO
    char shellPath[MAX_PATH_LENGTH];
    strcat(strcpy(shellPath, cwdbuf), "/myshell");
    status.shellpath = shellPath;

    
    // set SHELL= environment variable, malloc and store in environment
    // TODO
    setenv("SHELL",status.shellpath,1);

    // prevent ctrl-C and zombie children
    signal(SIGINT, SIG_IGN);  // prevent ^C interrupt
    signal(SIGCHLD, SIG_IGN); // prevent Zombie children

    // keep reading input until "quit" command or eof of redirected input
    while (!feof(instream))
    {
        // (re)initialise status structure
        status.foreground = TRUE;
        
        // set up prompt
        // TODO
        /* cout when argc == 1 */
        if(argc == 1){
            printf("%s%s ", cwdbuf,prompt);
            fflush(stdout);
        }
        
        execargs = TRUE;
        memset(args,0,sizeof(args));
        
        // get command line from input
        if (fgets(linebuf, MAX_BUFFER, instream))
        {
            // read a line
            // tokenize the input into args array
            arg = args;
            *arg++ = strtok(linebuf, SEPARATORS); // tokenize input
            while ((*arg++ = strtok(NULL, SEPARATORS)));

            // last entry will be NULL
            if (args[0])
            {
                // check for i/o redirection
                check4redirection(args, &status);

                // check for internal/external commands
                // "cd" command
                if (!strcmp(args[0], "cd"))
                {
                    // TODO
                    /*-----------------------------------------------------------------------
                    1. Generate a dirpath variable, judging according to different situations
                        1.1 . or ./
                        1.2 .. or ../
                        1.3 else (clear dir)
                    2. set env
                    3. not need to further exec
                    -----------------------------------------------------------------------*/
                    char dirPath[MAX_PATH_LENGTH];
                    memset(dirPath, 0, MAX_PATH_LENGTH);

                    if(!args[1]){
                        printf("%s\n",cwdbuf);
                    }else{
                        if (!strcmp(args[1], ".") || !strcmp(args[1], "./"))
                        {
                            strcpy(dirPath, cwdbuf);
                        }
                        else if (!strncmp(args[1], "..", 2) || !strncmp(args[1], "../", 3))
                        {
                            int i = strlen(cwdbuf) - 2;
                            while (i >= 0 && cwdbuf[i] != '/'){
                                --i;
                            }
                            strncpy(dirPath, cwdbuf, i + 1);
                        }
                        else{
                            strcpy(dirPath, args[1]);
                        }
                        setenv("PWD", dirPath, 1);
                    }
                    execargs = FALSE;
                }
                // "clr" command
                else if (!strcmp(args[0], "clr"))
                {
                    // TODO
                    /* 
                    1. CLEAR 
                    2. no need to further exec 
                    */
                    printf(CLEAR);
                    execargs = FALSE;
                }
                // "dir" command
                else if (!strcmp(args[0], "dir"))
                {
                    // TODO
                    /*-----------------------------------------------------------------------
                    1. set argv variable
                        1.1 argv[1] : ls
                        1.2 argv[2] : -al
                        1.3 argv[3] : dir path
                    -----------------------------------------------------------------------*/
                    int idx = 0;
                    while (args[idx] && strcmp(args[idx], ">") && strcmp(args[idx], ">>"))
                        ++idx;
                    if (args[idx])
                        args[idx] = NULL;
                    
                    char argv1[MAX_ARGS], argv2[MAX_ARGS], argv3[MAX_ARGS];
                    if (!args[1]){
                        strcpy(argv2, ".");
                    } else if (args[1]) {
                        strcpy(argv2, args[1]);
                    } else if (args[2]){
                        strcpy(argv3, args[2]);
                        args[3] = argv3;
                    }

                    strcpy(args[0], "ls");
                    strcpy(argv1, "-al");
                    args[1] = argv1;
                    args[2] = argv2;
                }
                // "echo" command
                else if (!strcmp(args[0], "echo"))
                {
                    // TODO
                    /*-------------------------------------------
                    1. judge whether need to redirection or not
                    2. Write in /dev/tty, to recover redirection
                    3. no need to further exec
                    -------------------------------------------*/
                    ostream = redirected_op(status);

                    int idx = 0;    
                    while (args[++idx] && strcmp(args[idx], ">") && strcmp(args[idx],">>")){
                        fprintf(ostream, "%s", args[idx]);
                    }
                    fprintf(ostream, "\n");

                    execargs = FALSE;
                }
                // "environ" command
                else if (!strcmp(args[0], "environ"))
                {
                    // TODO
                    /*-------------------------------------------
                    1. judge whether need to redirection or not
                    2. print environ (for loop)
                    3. Write in /dev/tty, to recover redirection
                    3. no need to further exec
                    -------------------------------------------*/
		            ostream = redirected_op(status);

                    int idx;
                    for (idx = 0; environ[idx] != NULL; idx++)
                        fprintf(ostream, "%s\n", environ[idx]);

                    execargs = FALSE; 
                    
                }
                // "help" command
                else if (!strcmp(args[0], "help"))
                {
                    args[0] = "more";
                    args[1] = readmepath;
                    args[2] = NULL;
                }
                // "pause" command - note use of getpass - this is a made to measure way of turning off
                //  keyboard echo and returning when enter/return is pressed
                else if (!strcmp(args[0], "pause"))
                {
                    // TODO
                    /* getchar() or getpass() */
                    getchar();
		            execargs = FALSE;
                }
                // "quit" command
                else if (!strcmp(args[0], "quit"))
                {
                    break;
                }
                // else pass command on to OS shell
                // TODO
                /* excute when excute = true*/
                if(execargs){
                    execute(args, status);
                }
            }
        }
    }
    return 0;
}

/***********************************************************************

void check4redirection(char ** args, shellstatus *sstatus);

check command line args for i/o redirection & background symbols
set flags etc in *sstatus as appropriate

***********************************************************************/

void check4redirection(char **args, shellstatus *sstatus)
{
    sstatus->infile = NULL; // set defaults
    sstatus->outfile = NULL;
    sstatus->outmode = NULL;

    while (*args)
    {
        // input redirection
        /*
        1. < : infile
        2. > / >> : outfile outmode
        3. & : foreground
        */
        if (!strcmp(*args, "<"))
        {
            // TODO
            sstatus->infile = *(args+1);
        }
        // output direction
        else if (!strcmp(*args, ">") || !strcmp(*args, ">>"))
        {
            // TODO
            sstatus->outfile = *(args+1);
            if(!strcmp(*args, ">")){
                sstatus->outmode = ">";
            }else{
                sstatus->outmode = ">>";
            }
        }
        else if (!strcmp(*args, "&"))
        {
            // TODO
            sstatus->foreground = FALSE;
        }
        args++;
    }
}

/***********************************************************************

  void execute(char ** args, shellstatus sstatus);

  fork and exec the program and command line arguments in args
  if foreground flag is TRUE, wait until pgm completes before
    returning

***********************************************************************/

void execute(char **args, shellstatus sstatus)
{
    int status;
    pid_t child_pid;

    switch (child_pid = fork())
    {
    case -1:
        syserrmsg("fork", NULL);
        break;
    case 0:
        // execution in child process
        // reset ctrl-C and child process signal trap
        signal(SIGINT, SIG_DFL);
        signal(SIGCHLD, SIG_DFL);

        // i/o redirection */
        // TODO
        if (sstatus.infile){
            freopen (sstatus.infile, "r", stdin);
        }
	    redirected_op(sstatus);

        int idx=0;
        while (args[idx] && strcmp(args[idx], "<") && strcmp(args[idx], ">") && strcmp(args[idx], ">>") && strcmp(args[idx], "&")){
            ++idx;
        }
        if (args[idx]){
            args[idx] = NULL;
        } 
          

        // set PARENT = environment variable, malloc and put in nenvironment
        // TODO
	    setenv("PARENT", getenv("SHELL"), 1);

        // final exec of program
        execvp(args[0], args);
        syserrmsg("exec failed -", args[0]);
        exit(127);
    }

    // continued execution in parent process
    // TODO
    if (sstatus.foreground){
        waitpid(child_pid, &status, 0); 
    }else{
	    waitpid(child_pid, &status, WNOHANG );
    }
}

/***********************************************************************

 char * getcwdstr(char * buffer, int size);

return start of buffer containing current working directory pathname

***********************************************************************/

char *getcwdstr(char *buffer, int size)
{
    // TODO
    strcpy(buffer, getenv("PWD"));
    return buffer;
}

/***********************************************************************

FILE * redirected_op(shellstatus ststus)

  return o/p stream (redirected if necessary)

***********************************************************************/

FILE *redirected_op(shellstatus status)
{
    FILE *ostream = stdout;

    // TODO
    /*
    chack outfile:
    1. > : w
    2. >> : a
    */
    if (status.outfile) {
        if  (!strcmp(status.outmode, ">"))
	        freopen(status.outfile, "w", ostream);
	else
	    freopen(status.outfile, "a", ostream);
    }
    return ostream;
}

/*******************************************************************

  char * stripath(char * pathname);

  strip path from file name

  pathname - file name, with or without leading path

  returns pointer to file name part of pathname
            if NULL or pathname is a directory ending in a '/'
                returns NULL
*******************************************************************/

char *stripath(char *pathname)
{
    char *filename = pathname;

    if (filename && *filename)
    {                                      // non-zero length string
        filename = strrchr(filename, '/'); // look for last '/'
        if (filename)                      // found it
            if (*(++filename))             //  AND file name exists
                return filename;
            else
                return NULL;
        else
            return pathname; // no '/' but non-zero length string
    }                        // original must be file name only
    return NULL;
}

/********************************************************************

void errmsg(char * msg1, char * msg2);

print an error message (or two) on stderr

if msg2 is NULL only msg1 is printed
if msg1 is NULL only "ERROR: " is printed
*******************************************************************/

void errmsg(char *msg1, char *msg2)
{
    fprintf(stderr, "ERROR: ");
    if (msg1)
        fprintf(stderr, "%s; ", msg1);
    if (msg2)
        fprintf(stderr, "%s; ", msg2);
    return;
    fprintf(stderr, "\n");
}

/********************************************************************

  void syserrmsg(char * msg1, char * msg2);

  print an error message (or two) on stderr followed by system error
  message.

  if msg2 is NULL only msg1 and system message is printed
  if msg1 is NULL only the system message is printed
 *******************************************************************/

void syserrmsg(char *msg1, char *msg2)
{
    errmsg(msg1, msg2);
    perror(NULL);
    return;
}
