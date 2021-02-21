//Northern Illinois University
//CSCI 330 Spring 2019
//Assignment: Assignment 1
//Author: Carly Dobie
//Z ID: Z1859292
//Date: 27 January 2019
//Description: sample program using Geany in Linux

#include <stdlib.h>
#include <iostream>
using namespace std;

int main(int argc, char* argv[])
{
	int count = 1;
	
	cout << "Welcome to the sample C++ program." << endl;
	
	//show all command line arguments
	for (int i=0; i < argc; i++)
	{
		cout << "argv[" << i << "] = " << argv[i] << endl;
	}
	
	//if no argument was given, prompt the user	
	if (argc == 1)
	{
		cout << "Enter number to repeat: ";
		cin >> count;
	}
	else
	{
		count = atoi(argv[1]);
	}
	
	//show line a few times
	for (int i=0; i < count; i++)
	{
		cout << "C++ is fun!" << endl;
	}
		
	return 0;
}
