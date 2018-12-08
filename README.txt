How to run Project 2:

1) Unzip the zipped folders (File includes: ftserver.c ftclient.py, makefile and README.txt)
2) Place the folder contents into the flip using a file transfer application(i.e. FileZilla)
3) Login to flip, either with access, flip1, flip2 or flip3
4) Go to the location you placed the files
5) Type "make" into the command line to create the execuatable of the ftserver.c file
6) Start the server: type "ftserver [port]" into the command line. I would recommend any number from 30000-31000. NOTE: Do this in the first instance of putty
7) Use the client for commands: type "python3 ftclient.py flip(1-3) [port] [command] [different port]" to get the directory. NOTE: Do this in the second instance of putty. Flip# must be the same as the servers flip#.
8) Use the client for commands: type "python3 ftclient.py flip(1-3) [port] [command] [file] [differntport]" to get a file from the server. NOTE: Do this in the second instance of putty. Flip# must be the same as he servers flip#.
9) If you wish to close down the server, press Ctrl-C.
