#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> //this is for signal processing and handling

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define BUFFER_SIZE 50
#define HISTORY_SIZE 10 //for the history array size
static char buffer[BUFFER_SIZE];/* the signal handler function */

char *history[HISTORY_SIZE][BUFFER_SIZE]; //reference to array that holds all of the previous 10 commands
int index = 0; //index is for the history buffer

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
    
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  


    start = -1;
    if (length == 0)
        exit(0);            /* ^d was entered, end of user command stream */
    if (length < 0){
        perror("error reading the command");
	exit(-1);           /* terminate with error code of -1 */
    }
    
    /* examine every character in the inputBuffer */
    for (i = 0; i < length; i++) { 
        switch (inputBuffer[i]){
        case ' ':
        case '\t' :               /* argument separators */
            if(start != -1){
                args[ct] = &inputBuffer[start];    /* set up pointer */
                ct++;
            }
            inputBuffer[i] = '\0'; /* add a null char; make a C string */
            start = -1;
            break;
            
        case '\n':                 /* should be the final char examined */
            if (start != -1){
                args[ct] = &inputBuffer[start];     
                ct++;
            }
            inputBuffer[i] = '\0';
            args[ct] = NULL; /* no more arguments to this command */
            break;

        case '&':
            *background = 1;
            inputBuffer[i] = '\0';
            break;
            
        default :             /* some other character */
            if (start == -1)
                start = i;
	} 
    }    
    args[ct] = NULL; /* just in case the input line was > 80 */

} 


int printHistory(char *history[][BUFFER_SIZE], int index)
{
 	//int i = index;
        //int cmdNum = 1;

        //implement print down here

    int j = 0;
    int cmdNum = index;
    
    
    for (int i = 0; i<10;i++)
    {
        
        printf("%d.  ", cmdNum);
        while (history[i][j] != '\n' && history[i][j] != '\0')
        {	
            printf("%s", history[i][j]);
            j++;
        }
        printf("\n");
        j = 0;
        cmdNum--;
        if (cmdNum ==  0)
            break;
    }
    printf("\n");

        return 0;
}


void handleSigint() //defining the behavior of the SIGINT
{
	printHistory(history, index);
	
}

void insertCmd(char inputBuffer[]) { //inserts new command into the history buffer ; 
     //history[index][BUFFER_SIZE] = strdup(inputBuffer); //I had inputBuffer here originally, but it only copied the command, 
     history[index][BUFFER_SIZE]=malloc(strlen(inputBuffer)+1); //allocate memory
     history[index][BUFFER_SIZE] = inputBuffer;
     index = (index + 1) % HISTORY_SIZE; //implements the circular buffer
 }


int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */

	//The signal processing code should be defined before the while loop
	struct sigaction handler;
	handler.sa_handler = handleSigint; 
	sigaction(SIGINT, &handler, NULL);
	strcpy(buffer,"Caught <ctrl><c>\n");/* wait for <control> <C> */
	//signal(SIGINT, SIG_IGN); //to stop the SIGINT signal from actually firing
	 signal(SIGINT, handleSigint);
    	

    while (1){            /* Program terminates normally inside setup */
	background = 0;
	printf("COMMAND->\n");
        setup(inputBuffer, args, &background);       /* get next command */


	pid_t iret = fork(); //it seems like I should create a child process before I begin to do the insert function

	insertCmd(inputBuffer);

	if(iret > 0)
		{wait(NULL);} //makes the parent process wait 
	else if (iret == 0)
		{

			execvp(args[0], args); //the args[0] holds the main meat of the command
			printf("If you reached this point, something is wrong\n"); //this should not execute if the program runs as attended
		} 
	else
		{printf("error\n");} //standard error message
    }
}
