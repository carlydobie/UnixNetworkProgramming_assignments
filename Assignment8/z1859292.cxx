#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <stdio.h>

using namespace std;

int main()
{
	char commandLine[80]; //holds user input
	pid_t pid; //process id for child process
	int i; //argv index
	int argc; //argument counter

	do
	{
		char *argv[] = { (char*)0, (char*)0, (char*)0, (char*)0, (char*)0, (char*)0, (char*)0 }; //stores parsed arguments

		i = 0; //initialize argv index
		argc = 0; //initialize argument counter

		cout << "Enter command: "; //prompt user for command
		cin.getline(commandLine, 80); //store user input string

		//parse arguments into array argv:
		for (char *p = strtok(commandLine, " "); p; p = strtok(NULL, " ")) //store each user argument as a token into p
		{
			argc++; //tally arguments
			argv[i] = p; //set current index in array to current token in p
			i++; //move to next spot in array
		}

		//check user input:
		if (argc > 5) //more than 5 arguments are entered
		{
			cout << "Too many arguments." << endl;

		}
		else if (argc == 0) //no arguments entered
		{
			cout << "No arguments." << endl;

		}
		else if (strncmp(commandLine, "quit", 5)) //make sure sufficient argument(s) isn't "quit" before trying to execute
		{
			pid = fork(); //create child process with pid = 0 (if successful in creating one)
			if (pid == 0) //child process will do this:
			{
				execvp(argv[0], argv); //execute the command
				if ((execvp(argv[0], argv) < 0)) //if the command was not successfully executed
					perror(argv[0]); //display first command along with error message
				exit(0); //end child process when it finishes executing command
			}
			else //parent process will do this:
				wait(NULL); //parent waits until child finishes
		}

	}while (strncmp(commandLine, "quit", 5)); //repeat loop unless user enters "quit"

	return 0;
}
