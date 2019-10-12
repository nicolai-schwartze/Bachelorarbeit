#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
 
int main( void )
{
   int status;
   int parent_pid;
   int child_pid;
 
   printf( "[fork-exec-test] start\n" );
 
   // so the child can send a signal to the parent if needed
   parent_pid = getpid();
 
   child_pid = fork();
   switch( child_pid )
   {
      case -1:
         perror( "[fork-exec-test] fork failed" );
         exit( EXIT_FAILURE );
         break;
 
      case 0:
         // the program should receive its own command as argv[0]
         execlp( "./print-argv", "./print-argv", "foo", "bar", NULL );
         // should't return
         perror( "[fork-exec-test] exec failed" );
         exit( EXIT_FAILURE );
         break;
 
      default:
         // no errors
         break;
   }
 
   // informational messages
   printf( "[fork-exec-test] parent PID: %d\n", parent_pid );
   printf( "[fork-exec-test] child PID: %d\n", child_pid );
 
   // wait for state changes in the child (aka for it to return)
   wait( &status );
 
   if( WIFEXITED( status ) )
   {
      // no errors occurred in the child
      printf( "[fork-exec-test] print-argv terminated  normally\n" );
 
      // which doesn't mean the program returned success (zero)
      printf( "[fork-exec-test] print-argv exit status: %d\n", WEXITSTATUS( status ) );
   }
   else
   {
      // something went wrong
      fprintf( stderr, "[fork-exec-test] print-argv wasn't executed\n" );
   }
 
   printf( "[fork-exec-test] end\n" );
 
   return EXIT_SUCCESS;
}