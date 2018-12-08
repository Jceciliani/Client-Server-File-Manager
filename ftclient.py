# Program Name: Project 2
# Author: Josh Ceciliani
# Date: November 21, 2018
# Description: This is the client of the client/server connetion.
# Course: CS 372-400 - Intro to Networking
# Last Modified: 12:24:30 MST - 12/26/2018

import socket
import sys
# Function to open a connection
def connection():
	# Get the server name from the arguments
	serverName = sys.argv[1]
	# Get the port
	serverPort = int(sys.argv[2])
	# Create the client socket
	clientsocket = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# Connect
	clientsocket.connect((serverName, serverPort))
	# Return the socket
	return clientsocket

# Functon for transfering data over a new connection
def newTransferSocket():
	# Checker for 4 or 5 arguments
	if sys.argv[3] == "-l":
		args = 4
	elif sys.argv[3] == "-g":
		args = 5
	
	# Create the socket
	sock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
	# Get correct port from agruments
	port = int(sys.argv[args])
	# Bind socket to port
	try:
		sock.bind(('', port))
	except socket.error as msg:
		print ('SERVER: Bind failed')
		sys.exit()
	
	# Make socket listen for connections - currently set to 1 connection
	sock.listen(1)
	# Waiting to accept the connection
	connection, addr = sock.accept()
	# Return transfer socket
	return sock	

# Function to keep connection open
def startConnection(conn):

	# Get message from client after connection
	message = conn.recv(512).decode()
	# Print message to screen
	if sys.argv[3] == "-l":
		print ('Receiving directory structure from ' + sys.argv[1] + ':' + sys.argv[4])
		print (message)

	elif sys.argv[3] == "-g":
		print ('Receiving "' + sys.argv[4] + '" from ' + sys.argv[1] + ':' + sys.argv[5])
		print (message)




# Main function
# Make Client socket
cSocket = connection()

# Place contents of all arguments in the string and seperate by a space
if sys.argv[3] == "-l":
	argsList = [sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4]]

elif sys.argv[3] == "-g":
	argsList = [sys.argv[1], sys.argv[2], sys.argv[3], sys.argv[4], sys.argv[5]]

newList = ' '.join(argsList)	

# Send the string to the server
cSocket.send((newList).encode())
# Receive message back from seerver
serverMessage = cSocket.recv(512).decode()

if serverMessage.find("OK") != -1:
	cSocket.send(("Ready").encode())

# Checker for 4 or 5 arguments
if sys.argv[3] == "-l":
	args = 4
elif sys.argv[3] == "-g":
	args = 5
	
# Create data socket
dataSock = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
# Get correct port from agruments
port = int(sys.argv[args])
# Bind socket to port
try:
	dataSock.bind(('', port))
except socket.error as msg:
	print ('SERVER: Bind failed')
	sys.exit()

# Make socket listen for connections - currently set to 1 connection
dataSock.listen(1)
# Waiting to accept the connection
connect, addr = dataSock.accept()
# Start data connection
startConnection(connect)


