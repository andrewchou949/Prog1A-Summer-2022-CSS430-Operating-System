// Name: Andrew Chou
// Class: CSS 430
// Assignment: progA
// ------------------------------------------ processes.cpp --------------------------------------------
// This processes.cpp file would illustrate the work of creating child, grandchild and great grandchild
// processes; simultaneously, create pipe between the child and grandchild (fds[0]) and pipe between
// grandchild and great grandchild (fds[1]).
// On top of that, it checks for pipe error via if statement when creating pipe and also duplicate some
// of the pipes accordingly. Pipe being duplicated are closed right after.
// -----------------------------------------------------------------------------------------------------

#include <sys/types.h>   // for fork, wait
#include <sys/wait.h>    // for wait
#include <unistd.h>      // for fork, pipe, dup, close
#include <stdio.h>       // for NULL, perror
#include <stdlib.h>      // for exit

#include <iostream>      // for cout

using namespace std;

int main( int argc, char** argv ) {
  int fds[2][2];
  int pid;

  if ( argc != 2 ) {
    cerr << "Usage: processes command" << endl;
    exit( -1 );
  }

  // fork a child
  if ( ( pid = fork( ) ) < 0 ) {
    perror( "fork error" );
  }
  else if ( pid == 0 ) {
    // I'm a child
    cout << "I'm a child!" << endl;
    // create a pipe using fds[0]
    // pipe between child and grandchild
    if(pipe(fds[0]) < 0){
      perror("pipe error");
    }
    // fork a grandchild
    if(pid = fork() < 0){
      perror("fork error");
    }
    // if I'm a grandchild
    else if(pid == 0){
      // create a pipe using fds[1]
      // pipe between grandchild and great grandchild
      if(pipe(fds[1]) < 0){
        perror("pipe error");
      }
      // fork a great grandchild
      if(pid = fork() < 0){
        perror("fork error");
      }
      // if I'm a great grandchild
      else if(pid == 0){
        // pipe (grandchild <--> great grandchild) (1) dup write end (1)
        dup2(fds[1][1], 1);
        close(fds[1][1]);
        // execute "ps" (great grandchild process executing ps -A)
        execlp("ps", "ps", "-A", nullptr);
      }
      // else if I'm a grandchild (when pid is > 0, that is parent process of current process)
      else if(pid > 0){
        // pipe (grandchild <--> great grandchild) (1) dup read end (0)
        dup2(fds[1][0], 0);
        // pipe (child <--> grandchild) (0) dup write end (1)
        dup2(fds[0][1], 1);
        close(fds[1][0]);
        close(fds[0][1]);
        // execute "grep" (grandchild process executing grep argv[1] or grep ssh)
        execlp("grep", "grep", "argv[1]", nullptr);
      }
    }         
    // else if I'm a child
    else if(pid > 0){
      // pipe (child <--> grandchild) (0) dup read end (0) 
      dup2(fds[0][0], 0);
      close(fds[0][0]);
      // execute "wc" (child process executing wc -l)
      execlp("wc", "wc", "-l", nullptr);
    }
  }
  else {
    // I'm a parent
    // Parent wait for child process
    wait( NULL );
    cout << "commands completed" << endl;
  }
}
