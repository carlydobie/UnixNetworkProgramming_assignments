#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <cstring>

using namespace std;

void permissionCheck(char* fn); //check to make sure user has permission and file is secure
void writeMessage(int fd, char* msg); //writes message to file

int main(int argc, char *argv[]) //allows program to take arguments
{
	if (argc < 3) //if no file name and/or message was given
	{
		exit(1); //terminate program
	}
	if (strcmp(argv[1], "-c") == 0) //if first argument is -c
	{
		char* fileName = argv[2]; //file name must be second argument
		char* message = argv[3]; //message must be the thrid argument

		//open file in write only mode and create the file if it doesn't already
		//exist. set permissions of file to 0600 (only user can read and write file)
		int fd = open(fileName, O_WRONLY | O_CREAT, 00600);

		if (fd < 0) //if the file failed to open
			perror("File could not be opened: "); //display error

		permissionCheck(fileName); //check to make sure user has permission and file is secure
		writeMessage(fd, message); //write message to file
	}
	else //no option
	{
		char* fileName = argv[1]; //file name must be first argument
		char* message = argv[2]; //message must be the second argument

		//open file in write only mode and create the file if it doesn't already
		//exist. set permissions of file to 0600 (only user can read and write file).
		//message is also set to append to the file if it already exists.
		int fd = open(fileName, O_WRONLY | O_APPEND | O_CREAT, 00600);

		if (fd < 0) //if the file failed to open
			perror("File could not be opened: "); //display error

		permissionCheck(fileName); //check to make sure user has permission and file is secure
		writeMessage(fd, message); //append message to file
	}

	return 0;
}

void permissionCheck(char* fn) //check to make sure user has permission and file is secure
{
	struct stat filestat; //declare variable to hold infomation for file
	stat(fn, &filestat); //hold information for fn in filestat

	//if file can be read by others or by a group
	if ((S_IROTH & filestat.st_mode) || (S_IRGRP & filestat.st_mode))
	{
		cout << fn << " is not secure. Ignoring." << endl;
		exit(1); //terminate program
	}
	//if user doesn't have read or write access on the file
	if (access(fn, R_OK) || access(fn, W_OK))
	{
		perror("Error"); //permission denied
		exit(1); //terminate program
	}
	else //file is secure and can be modified
		return; //proceed
}
void writeMessage(int fd, char* msg) //writes message to file
{
	write(fd, msg, strlen(msg)); //write message to file
	write(fd, "\n", 2); //write new line to file after message
	close(fd); //close file
}

