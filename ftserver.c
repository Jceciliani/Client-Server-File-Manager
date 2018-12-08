/* Program Name: Project 2
 * Author: Josh Ceciliani
 * Date: November 21, 2018
 * Description: This is the server side C program to handle file transfer.
 * Course: CS 372-400 - Intro To Networking
 * Last Modified: 12:24:13 MST - 12/26/2018
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <fcntl.h>
#include <dirent.h>


#define SIZE 2000

void error(const char *msg) { perror(msg); exit(1); } // Error function used for reporting issues

// Create string array for directories and their files
char ** createFileString(int s)
{
	char ** string = malloc(s * sizeof(char*));
	int i;
	for(i = 0; i < s; i++)
	{
		string[i] = malloc(50*sizeof(char));
		memset(string[i], 0, sizeof(string[i]));
	}

	return string;
}

// Function to get all files from the current directory
// REFERENCE: https://stackoverflow.com/questions/612097/how-can-i-get-the-list-of-files-in-a-directory-using-c-or-c
// https://stackoverflow.com/questions/12489/how-do-you-get-a-directory-listing-in-c
int getFiles(char **files)
{
	DIR *directory;
	directory = opendir(".");
	struct dirent *dir;
	int i = 0;
	if(directory)
	{
		while((dir = readdir(directory)) != NULL)
		{
			if(dir->d_type == DT_REG)
			{
				strcpy(files[i], dir->d_name);
				i++;
			}	
		}
		closedir(directory);
	}	

	return i;
}

int fileExist(char ** files, int fileNum, char * fileName)
{
	int exists = 0;
	int i;
	// Loop through current files and compare it with the file name
	for(i = 0; i < fileNum; i++)
	{
		if(strcmp(files[i], fileName) == 0)
		{
			exists = 1;
		}
	}	

	return exists;
}	

int main(int argc, char *argv[])
{
	int listenSocketFD;
	int establishedConnectionFD;
	int socketFD;	
	int portNumber;
	int newPortNumber;	
	int charsRead;
	socklen_t sizeOfClientInfo;
	char buffer[SIZE];
	char cMessage[50];	
	struct sockaddr_in serverAddress, clientAddress, newServerAddress;
	struct hostent* serverHostInfo;
	char *token = NULL;
	char *newline = " \n";
	const char space[3] = " \n";	

	// Check usage of args
	if (argc < 2) 
	{ 
		fprintf(stderr,"USAGE: %s port\n", argv[0]);
		exit(1); 
	} 

	// Set up the address struct for this process (the server)
	memset((char *)&serverAddress, '\0', sizeof(serverAddress)); // Clear out the address struct
	portNumber = atoi(argv[1]); // Get the port number, convert to an integer from a string
	serverAddress.sin_family = AF_INET; // Create a network-capable socket
	serverAddress.sin_port = htons(portNumber); // Store the port number
	serverAddress.sin_addr.s_addr = INADDR_ANY; // Any address is allowed for connection to this process

	// Set up the socket
	listenSocketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
	if (listenSocketFD < 0) 
	{
		error("ERROR opening socket");
	}
	// Enable the socket to begin listening
	if (bind(listenSocketFD, (struct sockaddr *)&serverAddress, sizeof(serverAddress)) < 0) // Connect socket to port
	{	
		error("ERROR on binding");
	}
	
	listen(listenSocketFD, 1); // Flip the socket on - it can now receive up to 1 connection

	// Print that the server is open on current PORT number
	printf("Server open on %d\n", portNumber);
	while(1)
	{

		// Accept a connection, blocking if one is not available until one connects
		sizeOfClientInfo = sizeof(clientAddress); // Get the size of the address for the client that will connect
		establishedConnectionFD = accept(listenSocketFD, (struct sockaddr *)&clientAddress, &sizeOfClientInfo); // Accept
		if (establishedConnectionFD < 0) 
		{
			error("ERROR on accept");
		}
		// Variables to help keep track of reading the whole message
		int totalBytes = 0;
		int readBytes = 0;
		// Get message and place in buffer
		while(1)
		{	
			char *flip = NULL;
			char *portNum = NULL;
			char *command = NULL;
			char *file = NULL;
			char *secondPort = NULL;					
			char fileString[200];
			// Find the number of bytes read this iteration
			readBytes = recv(establishedConnectionFD, buffer, 1000, 0);
			if(readBytes < 0)
			{
				error("ERROR reading from socket");
			}

			// Iterate 3 times through the string to get to command - First Arg
			token = strtok(buffer, space);
			// Get the flip argument
			flip = strdup(token);
			// Next
			token = strtok(NULL, space);
			// Get the server port argument
			portNum = strdup(token);
			// Next
			token = strtok(NULL, space);
			// Get the command argument
			command = strdup(token);

			// 4 arguments - ERROR HERE
			if(strcmp(command, "-l") == 0)
			{
				// Tokenize the buffer from the client to get all of the information
				// Next		
				token = strtok(NULL, space);
				// Get port that the data goes to
				secondPort = strdup(token);
			}
			// 5 arguments
			else if(strcmp(command, "-g") == 0)
			{
				// Tokenize the buffer from the client to get all of the information
				// Next	
				token = strtok(NULL, space);
				// Get file name from the command line
				file = strdup(token);
				// Next
				token = strtok(NULL, space);
				// Get port that the data goes to
				secondPort = strdup(token);
			}
	
			// Check to see if the python server socket is open and available
			char *serverOk = "OK";
			// Send the ok message to the client to tell it that the data was receive and am now ready for the data connection set up
			send(establishedConnectionFD, serverOk, 3, 0);		
			// Receive message back from client
			recv(establishedConnectionFD, cMessage, 50, 0);

			// Create new socket to send on **********************************************************
 			// Set up server address
        		memset((char*)&newServerAddress, '\0', sizeof(newServerAddress)); // Clear address struct
        		newPortNumber = atoi(secondPort); // Get port number, convert from string to int
        		newServerAddress.sin_family = AF_INET; // Create a network-capable socket
        		newServerAddress.sin_port = htons(newPortNumber); // Store the port number
        		serverHostInfo = gethostbyname(flip); // Get host name from arguments
			

			if(serverHostInfo == NULL)
			{
				fprintf(stderr, "SERVER: ERROR, no such host\n");
				exit(0);
			}

			// If message from client is ready, create new data socket 
			if(strcmp(cMessage, "Ready") == 0)
			{
				
				memcpy((char*)&newServerAddress.sin_addr.s_addr, (char*)serverHostInfo->h_addr, serverHostInfo->h_length); // Copy in the address

				socketFD = socket(AF_INET, SOCK_STREAM, 0); // Create the socket
				// Error test socket
				if(socketFD < 0)
				{
					error("CLIENT: ERROR opening socket");
				}
	
				// Connect to the Server
				if(connect(socketFD, (struct sockaddr*)&newServerAddress, sizeof(newServerAddress)) < 0) //Connect socket to address
				{
					error("CLIENT: ERROR connecting");
				}
				// Sleep so that the client can set up the new socket
				sleep(1);
			}	
			
		
			// Handle commands here
			// Send the directory to the client		
			if(strcmp(command, "-l") == 0)
			{	
				// Print location of connection
				printf("Connection from %s\n", flip);
				// Print what the client wants
				printf("List directory requested on port %s\n", secondPort);
				// Print that the server is sending what the client requested
				printf("Sending directory contents to %s:%s\n", flip, secondPort);
				// Clear file string buffer
				memset(fileString, '\0', sizeof(fileString));
				// Create files string
				char ** files = createFileString(100);
				// Get files from directory
				int numberFiles = getFiles(files);
				// Send the directory to the client on the current connection
				int i;
				for(i = 0; i < numberFiles; i++)
				{
					if(i == 0)
					{
						strcpy(fileString, files[i]);
						strcat(fileString, "\n");
					}		
					else
					{
						strcat(fileString, files[i]);
						strcat(fileString, "\n");
					}
				}
				// Send string to client over data socket
				send(socketFD, fileString, 200, 0);		
			}
			else if(strcmp(command, "-g") == 0)
			{
				// Print location of connection
				printf("Connection from %s\n", flip);
				// Print what the client wants
				printf("File %s requested on port %s\n", file, secondPort);
					
				char **files = createFileString(100);
				int numberFiles = getFiles(files);
				// Check if files exists 
				if(fileExist(files, numberFiles, file) == 1)
				{
					// Create a buffer to hold the contents of the file			
					char buff[SIZE];
					// Reset the buffer
					memset(buff, '\0', sizeof(buffer));
					// Print that the server is sending what the client requested
					printf("Sending %s to %s:%s\n", file, flip, secondPort);
					// Get and open file
					int chosenFile = open(file, O_RDONLY);
					// Place file into buffer
					int totalBytes = read(chosenFile, buff, sizeof(buff)-1);
					// Send file to client
					send(socketFD, buff, totalBytes, 0); 	
					
					// Send confimation
					char * confirmation = "File transfer complete.";
					send(socketFD, confirmation, 25, 0);	
					
				}
				// File was not found
				else
				{	
					printf("File not found. Sending error message to %s:%s\n", flip, secondPort);
					char * nonExist = "FILE NOT FOUND";
					send(socketFD, nonExist, 20, 0);
				}		
			}	
			// Close client socket	
			close(socketFD);
			// Break out of the while loop
			break;	
		}

		close(establishedConnectionFD); // Close the existing socket which is connected to the client


	}	

	close(listenSocketFD); // Close the listening socket
	return 0; 
}

