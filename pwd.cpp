#include <cstdlib>
#include <unistd.h>
#include <stdlib.h>

int main(){

  char * ch = nullptr;

  //get the present working directory
  ch = getcwd(ch ,0);

  int size = 0;

  //get the size of the string we need to print
  while(ch[size] != '\0') size++;

  //write the whole string to standard output
  write(STDOUT_FILENO, ch, size);

  //write a new line to standard output
  write(STDOUT_FILENO, "\n" , 1);

  //getcwd(3) uses malloc(3), so we need to free(3) that memory
  free(ch);

  return EXIT_SUCCESS;

}//main
