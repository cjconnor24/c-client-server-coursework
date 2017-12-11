# Systems Programming - TCP Client / Server - C Coursework

This repo is for a University Coursework in the C language.

## Overview
We were given the task of implementing a very simple TCP Client / Server program using the C language and TCP sockets. The client should me a menu driven program which will send and recieve various options to the server.

## Functions

* Get student name and IP
* Get server time
* Get server file list
* Download file from server (including binary files)

## Installation
```
git clone https://www.github.com/cjconnor24/sp-c-coursework ccoursework
```
### Server
```bash
cd ccoursework/server
make
```
### Client
```bash
cd ccoursework/client
make
```

## Running
To test the program, first launch the server program:
```bash
cd ccoursework/server
./server
```
The launch the client:
```bash
cd ccoursework/client
./client
```
You will see a screen like below - simply follow the options:
```
  _______ _____ _____   _______ _      _____ ______ _   _ _______
 |__   __/ ____|  __ \ / / ____| |    |_   _|  ____| \ | |__   __|
    | | | |    | |__) / / |    | |      | | | |__  |  \| |  | |
    | | | |    |  ___/ /| |    | |      | | |  __| | . ` |  | |
    | | | |____| |  / / | |____| |____ _| |_| |____| |\  |  | |
    |_|  \_____|_| /_/   \_____|______|_____|______|_| \_|  |_|
--------------------------------------------------------------------------

	Student: Chris Connor
	    SID: S1715477
	  Email: cconno208@caledonian.ac.uk / chris@chrisconnor.co.uk
	 GitHub: https://www.github.com/cjconnor24/sp-c-coursework (PRIVATE REPO)

--------------------------------------------------------------------------

Successfully connected to server...

+-----------------------------------+
|                MENU               |
+-----------------------------------+
|  [0]	Re-display menu             |
+-----------------------------------+
|  [1]	Get Student Information     |
|  [2]	Get server timestamp        |
|  [3]	Get server information      |
|  [4]	Get server file list        |
|  [5]	Get a file from the server  |
+-----------------------------------+
|  [6]	Exit                        |
+-----------------------------------+

Enter your choice>
```


