//Assignment 9: TCP server
//CSCI 330: Unix and Network Programming
//Carly Dobie
//May 5, 2019

//Completed the extra credit

#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <cstdlib>
#include <cstring>
#include <cstdio>
#include <iostream>
#include <sys/types.h>
#include <dirent.h>
#include <sys/stat.h>
using namespace std;

int main (int argc, char* argv[])
{
	//check number of arguments:
	//cout << "Checking number of arguments..." << endl;
	if (argc != 3)
	{
		cout << "USAGE: port directory" << endl;
		exit(EXIT_FAILURE);
	}

	//create socket:
	cout << "Attempting to create socket..." << endl;
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock < 0)
	{
		perror("Failed to create socket");
		exit(EXIT_FAILURE);
	}

	//create address structures:
	struct sockaddr_in server_address; //server address
	struct sockaddr_in client_address; //client address
	unsigned int addrlen = sizeof(client_address);

	//construct server:
	memset(&server_address, 0, sizeof(server_address)); //clear struct
	server_address.sin_family = AF_INET; //internet/IP
	server_address.sin_addr.s_addr = INADDR_ANY; //any IP address
	//server port: convert to integer then convert from host representation to network representation
	server_address.sin_port = htons(atoi(argv[1]));

	//bind the socket: attatches a local address to the socket
	cout << "Attempting to bind server socket..." << endl;
	if (bind(sock, (struct sockaddr *) &server_address, sizeof(server_address)) < 0)
	{
		perror("Failed to bind server socket");
		exit(EXIT_FAILURE);
	}
	cout << "Server socket binded" << endl;

	//listen: make socket passive and set length of queue
	cout << "Attempting to listen on socket: " << argv[1] << "..." << endl;
	if (listen(sock, 64) < 0) //length of backup queue: 64
	{
		perror("Listen failed");
		exit(EXIT_FAILURE);
	}
	cout << "Listening on: " << argv[1] << endl;

	//wait for input from client (run until cancelled):
	while (true)
	{
		//accept socket:
		int newSock = accept(sock, (struct sockaddr *) &client_address, &addrlen);
		if (newSock < 0)
		{
			perror("Accept failed");
			exit(EXIT_FAILURE);
		}

		//fork process:
		if (fork()) //parent process: close connection socket
			close(newSock);
		else //child process: process client request
		{
			char request[1024]; //client request
			char buffer[1024]; //to send to client

			//read request from the client:
			cout << "Reading message from client..." << endl;
			int received = read(newSock, request, sizeof(request));
			if (received < 0)
			{
				perror("Read failed");
				exit(EXIT_FAILURE);
			}

			if (strncmp("GET /", request, 5) == 0) //if path begins with "GET /"
			{
				//process client request:
				char path[1024];
				strcpy(path, request + 5); //string after "GET /" now becomes path
				cout << "Path: " << path << endl;
				//check if path entered is a directory or a file:
				struct stat pathStat;
				if (stat(path, &pathStat) == 0)
				{
					//if path goes to a directory:
					if (pathStat.st_mode& S_IFDIR)
					{
						//open directiory:
						cout << "Opening directory " << path << "..." << endl;
						DIR *dirp = opendir(path);

						//if directory fails to open:
						if (dirp == 0)
						{
							cout << "Failed to open directory. Notifying client..." << endl;
							strcpy(buffer, path); //copy path into buffer
							//tell client error occured
							strcat(buffer, ": could not open directory\n");
							//display on client side
							if (write(newSock, buffer, strlen(buffer)) < 0)
							{
								perror("Write failed");
								exit(EXIT_FAILURE);
							}
							exit(EXIT_SUCCESS);
						}
						struct dirent *dirEntry;
						//while there are entries in the directory
						cout << "Sending directory contents to client..." << endl;
						while ((dirEntry = readdir(dirp)) != NULL)
						{
							//put name of entry into buffer
							strcpy(buffer, dirEntry->d_name); //copy file name into buffer
							strcat(buffer, "\n");
							//send buffer to client side
							if (strncmp(".", buffer, 1) != 0) //if the file doesn't begin with a period
							{
								if (write(newSock, buffer, strlen(buffer)) < 0) //write to client
								{
									perror("Write failed");
									exit(EXIT_FAILURE);
								}
								cout << "sent: " << buffer; //display what was sent on server
							}

						}//end of loop reading directory
						//close directory:
						cout << "Closing directory..." << endl;
						closedir(dirp);
					}//directory request processed

					//if path goes to a file:
					else if (pathStat.st_mode&  S_IFREG)
					{
						cout << "Path goes to a file" << endl;
						FILE *file = fopen(path, "r"); //open file in read-only mode
						size_t size = 0; //variable and initialization for the size of file
						fseek(file, 0, SEEK_END); //find the end of the file
						size = ftell(file); //set size
						rewind(file); //start at begining of file
						fread(buffer, size, 1, file); //read file into buffer
						if (write(newSock, buffer, strlen(buffer)) < 0) //write buffer to client
						{
							perror("Write failed");
							exit(EXIT_FAILURE);
						}
					}
				}//finished processing directory or file request
			}//finished processing "GET /" request
			//otherwise, if user enters "INFO" as a request:
			else if (strncmp("INFO", request, 5) == 0) //display date and time
			{
				time_t current_time; //stores values for current time
				time(&current_time); //stores the time since the Epoch in current_time

				if (write(newSock, ctime(&current_time), strlen(ctime(&current_time))) < 0) //write time and date to client
				{
					perror("Write failed");
					exit(EXIT_FAILURE);
				}
			}

			//client request completed:
			cout << "Done with client request" << endl;
			cout << "Closing connection socket..." << endl;
			//close connection socket
			if (close(newSock) < 0)
			{
				perror("Failed to close socket");
				exit(EXIT_FAILURE);
			}
			cout << "Socket closed" << endl;
			exit(EXIT_SUCCESS); //end child process
		}//end of child process fork
	}//end of while loop

	cout << "Closing socket..." << endl;
	if (close(sock) < 0)
	{
		perror("Failed to close socket");
		exit(EXIT_FAILURE);
	}
	cout << "Socket closed" << endl;
	return 0;
}
