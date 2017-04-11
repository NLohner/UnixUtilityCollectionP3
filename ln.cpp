#include <unistd.h> //link
#include <stdio.h> //perror
#include <string.h> //strcmp


int main(int argc, char * argv [])
{
  int success = 0;

  if(argc == 1)
    printf("ln: missing file operand\n");

  else if((argc == 4) && strcmp(argv[1], "-s") == 0) { //if argv[1] is "-s"  
    success = symlink(argv[2], argv[3]); //attempt to create symbolic link
    if(success == -1) perror("ln");
  }
  
  else if(argc == 3) {
    success = link(argv[1], argv[2]); //argv[1] is the existing target file (create hard link)
    if(success == -1) perror("ln");
  }
  
  
  else
    printf("Error: check arguments.\n");
}

  
