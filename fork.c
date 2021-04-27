/*---------------------------------------------------------------------------------------------
# Process Creation and Manipulation
#
# Name: fork.c
#
# Written By: Saalar Faisal - 2021-03-08
#
# Purpose: The purpose is to demonstrate knowledge of process creation and manipulation
#		   and write a program that uses  fork, execlp, wait, and waitpid 
#		   to create and control several processes.
#
#		   We will create 3 processes. Two children and a grandchild of a process (parent)
#
# To compile: gcc -o fork fork.c
#
# Usage:  >> ./fork
#  
#-----------------------------------------------------------------------------------------------*/

#include <stdlib.h> //standard library
#include <string.h> //Header for String lib
#include <sys/stat.h> //Header for getting bytes
#include <unistd.h> //required for open, close
#include <sys/types.h> //types
#include <sys/wait.h> // required for waitpd
#include <stdio.h> //standard input and output
#include <fcntl.h> // Used for File read, open, close, and write

//main function
int main() {

  // Create our processes
  pid_t child1, child2, grandChild; 
  
  // What is the output prior to creating the fork
  printf("\n\nIn the main program, just before the fork() statement\n");
  printf("\tmy process ID is %d and my parent's process ID is %d\n\n",getpid(),getppid());
  
 // Lets store the PID's of Parent and its Parent 
 int fatherPID = getpid();
 int grandFatherPID = getppid();
  
  // If the first child is succesfully created
  // we will go ahead and fork for the second child as well
  
  // fork the first child
  if(child1 = fork()) { 
  // fork the second child
    if(child2 = fork()) { 
      printf("PARENT: First fork() worked. Child PID=%d\n", child1);
      printf("PARENT: Second fork() worked. Child PID=%d\n", child2);
	  printf("PARENT: Waiting either child to finish... will it be %d or %d?\n\n", child1, child2);

	  // this will store the process id for the process that finishes
      pid_t wpid; 
	  
	  // holds the status returned by the processes
      int status; 
	
	  // this loop indicates when a processes is completed
      while ((wpid = wait(&status)) > 0){
        if(wpid > 0){
          printf("PARENT: After the long wait, process id %d finished.\n", wpid);
        }
      };
	  
	  // successfully exit
      exit(0); 
	  
	// lets instatiate the second child process  
    } else {
		
      printf(" - Fork was successful. Second child code now starting\n");
	  printf(" - Second child has a grandparent=%d Parent=%d\n", grandFatherPID, fatherPID);
	  printf(" -   Second Child is running... my PID=%d\n", getpid());
	  printf(" -   Second Child: running, copying  document.txt to ./local_copy.txt\n");
      printf(" -   my parent is %d and my grandparent is %d\n", fatherPID, grandFatherPID);
	  
	  
	  // counter for file transfer
	  int bytesCount = 0;
	  
	  // reads the bytes
      ssize_t readBytes = 0; 
	  
	  // buffer with max capacity
      char buffer[2056]; 

      // Read our txt file 
      int fileInput = open("document.txt", O_RDONLY);
      
	  // loop through entire file
	  do {
		  
        //read the bytes in the file
        readBytes = read(fileInput, buffer, (size_t)2056);

        // EOF
        if (readBytes <= 0){
            break; 
        }

        //Counter Addition
        bytesCount = bytesCount + readBytes;
		
      } while (1); 
      
      // Now we will write the file
      char filePath[1024];
	  
	  // get CWD
      getcwd(filePath, sizeof(filePath)); 
	  
	  // string library for ouputing an exact copy to our new text file
      strcat(filePath, "/local_copy.txt");
	  
	  // reference: Unit 4: Process Powerpoint --> read,write, create if it doesn't exist 
      int fd = open(filePath, O_RDWR | O_CREAT, S_IRUSR | S_IRGRP | S_IROTH); 
	  
	  // write to file
      write(fd, buffer, strlen(buffer));
	  
	  // lets close the reading file
      close(fileInput);
	  
	  // lets close the copying file
      close(fd); 

      // print bytes copied
      printf(" -   Second Child: Contents copied from document.txt to./local_copy.txt\t\n\t --> Second Child: %d bytes were copied.\n", bytesCount);

	 // lets exist 
      exit(0);
    }
  }
  
  // The First Child Processes
  else {
    printf(" > Fork was successful. First child code now starting \n");
	
    //  creating the process for grandChild
    if((grandChild = fork()) == 0) {
      printf(" > > Fork was successful. First grandchild code now running\n");
	  printf(" > > Grandchild: ... my PID=%d\n", getpid());
	  printf(" > >   my parent is %d and my grandparent is %d \n", getppid(), fatherPID);  
      
	  //sleep for 3 seconds
      sleep(3); 
 
	  // header for file output
	  printf("[saalarfaisal@loki lab4]$ head -n 20 small_world.txt\n");
	  
	  // execlp is for running the command line simulation
      execlp("head", "head", "-n", "20", "small_world.txt", NULL); 
      
	  // If it fails
	  printf("ERROR: execlp command failed\n");
	  
	  // return error
      exit(1); 
	  
    } else {
		
      // Output information for firstChild
      printf(" > First Child has a grandparent=%d Parent=%d\n", grandFatherPID, fatherPID);
	  printf(" > First Child is running... my PID=%d\n", getpid());
	  printf(" >   my parent is %d and my grandparent is %d\n", fatherPID, grandFatherPID);
      
	  // holds the status returend by the grand-child processes
	  int grandchildStatus;
	  
	  // Poll the child and sleep for 1 second
	  // uses WNOHANG option of the waitpid system call
	  // each time it polls, the process printout that it will
	  // “Try again” after it executes a sleep(1)
      while((waitpid(grandChild, &grandchildStatus, WNOHANG)) != grandChild){
        printf(" > First Child: 'Try again'\n");
        sleep(1);
      };
	  // exit status
      printf(" > First child: Exit status from grandchild was 0 \n");

	  // process has completed
      exit(0); 
    }
  }
}