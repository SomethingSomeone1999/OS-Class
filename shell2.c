#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h> //this is for signal processing and handling

#define MAX_LINE 80 /* 80 chars per line, per command, should be enough. */
#define BUFFER_SIZE 50
#define HISTORY_SIZE 10 //for the history array size
static char buffer[BUFFER_SIZE];/* the signal handler function */

char *found; //must be a ptr

int count = 0; //global count for the printing of the history

typedef struct history
{
    char *ent;
    struct history * next;
}history;

history *top = NULL; //represents the top of the linked list

void add(char *s)
{
    count++;
    history *h =  (history *) malloc(sizeof(history)); //always allocate first or segmentation fault may happen
	

    h->ent = strdup(s); //the strdup function us needed to make a copy of the string instead of just a pointer

    h->next = top;
    top = h;
}

void print()
{
	history *i; //pointer iterator
	int stop = NULL; 
	int internalCount = 0; //used to only print the 10 recent commands if commands are greater than 10

	if(top == NULL)
	{
		printf("No commands have been issued! \n");	
	}
	else
	{int temp = count;
    		for (i = top; i != stop; i = i->next){
        		printf("  %d. %s\n", count, i->ent);
			count--;
			internalCount++;
			if(internalCount == 10)
			{
				break;	
			}
		}
		count = temp;
	}
	
	printf("COMMAND->\n");
}

void setup(char inputBuffer[], char *args[],int *background)
{
    int length, /* # of characters in the command line */
        i,      /* loop index for accessing inputBuffer array */
        start,  /* index where beginning of next command parameter is */
        ct;     /* index of where to place the next parameter into args[] */
    
    ct = 0;
    
    /* read what the user enters on the command line */
    length = read(STDIN_FILENO, inputBuffer, MAX_LINE);  

	//the below block of code is very important to get rid of garbage characters
	int itr = 0;
	int itr1 = 0;
	char copy[50];
	memset(&copy[0], 0, sizeof(copy)); //initialize array
	while(inputBuffer[itr] != '\n' && inputBuffer[itr] != '\0')
	{
		copy[itr] = inputBuffer[itr1];
		itr++;
		itr1++;
	}
	
	if(inputBuffer[0] != 'r') {add(copy);} //this is to prevent 'r' from entering the history


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



void handleSigint() //defining the behavior of the SIGINT
{
	print();
}

void searchAndRun(char let) //char let represents the first char in the command
{
	history *i =  (history *) malloc(sizeof(history));
	int stop = NULL; 
	int internalCount = 0; 
	

	if(top == NULL)
	{
		printf("No commands have been issued! \n");	
	}
	else
	{int temp = count;
    		for (i = top; i != stop; i = i->next){
        		
			if(i->ent[0] == let)
			{
				found = i->ent; //this variable is used for execution later
				printf("found it\n"); //works
				break;
			}

			count--;
			internalCount++;
			if(internalCount == 10 && i->ent[0] != let)
			{
				printf("No such command exists \n");
				break;	
			}
		}
		count = temp;
	}
	
	 
}


int main(void)
{
    char inputBuffer[MAX_LINE]; /* buffer to hold the command entered */
    int background;             /* equals 1 if a command is followed by '&' */
    char *args[MAX_LINE/2+1];/* command line (of 80) has max of 40 arguments */

	char check[80]; //check the first argument of a command
	char check1[80]; //check the second argument of a command
	char recentCopy[80]; //copy the most recent command

	//The signal processing code should be defined before the while loop
	struct sigaction handler;
	handler.sa_handler = handleSigint; 
	sigaction(SIGINT, &handler, NULL);
	
	//signal(SIGINT, SIG_IGN); //to stop the SIGINT signal from actually firing
	 signal(SIGINT, handleSigint);
    	

    while (1){            /* Program terminates normally inside setup */
	background = 0;
	printf("COMMAND->\n");
        setup(inputBuffer, args, &background);       /* get next command */

	
	strcpy(check, args[0]);
	//strcpy(check1, args[1]);
	
	/*if(check[0] == 'r') {
		history *i =  (history *) malloc(sizeof(history));
		printf("  Running the most recent command: %s\n", i->ent);
		
	}*/

	


	pid_t iret = fork(); //it seems like I should create a child process before I begin to do the insert function

	if(iret > 0)
		{wait(NULL);} //makes the parent process wait 
	else if (iret == 0)
		{

			//if(inputBuffer[0] == 'r') {printf("Hello \n");} //this code also works down here as well
			//if(*args[1] == 'x') {printf("Hello \n");} //this is the syntax I must use for the args as well
			
			if(check[0] == 'r') { 
				history *i =  (history *) malloc(sizeof(history));
				char *execute[MAX_LINE/2+1];
				
				if(top == NULL)
				{
					printf("No commands have been issued! \n");	
				}
					strcpy(*execute, i->ent);
					add(execute); //add the most recent command again
					execvp(execute[0], execute);
					printf("If you reached this point, something is wrong\n"); 
			}
			/*else if(check[0] == 'r' && isalpha(check[1]))
			{
				history *i =  (history *) malloc(sizeof(history));
				char *execute[MAX_LINE/2+1];
				searchAndRun(check[1]);
				strcpy(*execute, found);
				add(execute); //add the most recent command again
				execvp(execute[0], execute);
				printf("If you reached this point, something is wrong\n");
			}*/

			else{
				execvp(args[0], args); //the args[0] holds the main meat of the command
				printf("If you reached this point, something is wrong\n"); //this should not execute 
			}
			
		} 
	else
		{printf("error\n");} //standard error message
    }
}
