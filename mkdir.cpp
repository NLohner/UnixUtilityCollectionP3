#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>
#include <string.h>

//prototype
bool isNum(char*);

//option bits
const unsigned int O_NOOPTS =  0b0;
const unsigned int      O_M =  0b1;
const unsigned int      O_P = 0b10;

int main(int argc, char ** argv){

  char ch;

  //get the current mask and set it to 0
  int prevMask = umask(0);

  char opts[] = {'m','p'};

  int copt = O_NOOPTS;

  bool quit = false;

  //we need to keep track of the position of the option after m,
  //so that we can later test if it is a valid number parameter
  int mpos = 0;

  //find the position of the -m
  for(int i = 1; i < argc; i++){

    if((argv[i][0] == '-') && (argv[i][1] == 'm') && (argv[i][2] == '\0')){

      mpos = i;

    }//if

  }//for

  //work through our options unsing getopt
  while((ch = getopt(argc, argv, opts)) > 0){

    if(ch == 'p') copt |= O_P;

    if(ch == 'm') copt |= O_M;

    //if we have an option we can't handle, quit the program
    else if(ch == '?') quit = true;

    //if the number after -m comes before another option, getopt
    //will move it, so we need to account for that
    if(optind > mpos){

      mpos++;

    }//if

  }//while

  //quit
  if(quit) return EXIT_FAILURE;

  //get the number of options so we can skip them when we loop through argv
  int numopts = 0;

  for(int i = 1; i < argc; i++){

    if((argv[i][0] == '-') && (argv[i][1] != '\0')) numopts++;

  }//for

  //loop through argv
  for(int i = numopts + 1; i < argc; i++){

    //if we have no options, try to make the file with mode 0755
    if(copt == O_NOOPTS){

      if(mkdir(argv[i], 0755) < 0) perror("./mkdir");

    }//if

    //if we have -m but not -p
    else if(((copt & O_M) == O_M) && ((copt & O_P) != O_P)){

      //if we're at the index of the number and it is indeed a number, skip it
      if((i == (mpos)) && isNum(argv[mpos])){}

      //if we're not at the number but it is a number, try to make the directory
      //with the mode specified by the number
      else if(isNum(argv[mpos])){

	if(mkdir(argv[i], strtoll(argv[mpos], NULL, 8)) < 0) perror("./mkdir");

      }//if

      //if we have something else, -m doesn't have a number operand, so tell the user and
      //quit the program
      else{

	write(STDOUT_FILENO, "./mkdir: option requires operand -- '-m'\n", 41);

	return EXIT_FAILURE;

      }//else

    }//else if

    //if we have -p and not -m
    else if(((copt & O_P) == O_P) && ((copt & O_M) != O_M)){

      //find the number of directories we need to create
      int numDirs = 0;

      for(int j = 1; argv[i][j] != '\0'; j++){

	if((argv[i][j] == '/') && (argv[i][j + 1] != '\0')) numDirs++;

      }//for

      //save the position of the last directory in dirName
      int lastDirPos = 0;

      //loop enough times to create as many directories as is neccessary
      for(int j = 0; j < (numDirs + 1); j++){

	//c string to save the directory name to create
	char dirName[1000];

	//copy the current parameter into dirName
	for(unsigned int k = 0; k < strlen(argv[i]); k++){

	  dirName[k] = argv[i][k];

	}//for

	//just to be sure, add an escape character
	dirName[strlen(argv[i])] = '\0';

	//starting from the last directory's position, loop through dirName
	for(int k = lastDirPos; dirName[k] != '\0'; k++){

	  //if we find a /, set the current position to a null character
	  //and set the position of the last directory to one past the 
	  //location of the /
	  if(dirName[k] == '/'){

	    dirName[k] = '\0';

	    lastDirPos = k + 1;

	    break;

	  }//if

	}//for

	//if we can access the directory, do nothing
	if(access(dirName, F_OK) == 0){}
	
	//if we can't, try to make it with the normal permissions
	else if(mkdir(dirName, 0755) < 0){

	  perror("./mkdir");

	}//if

      }//for

    }//else if

    //if we have both -p and -m
    else if(((copt & O_P) == O_P) && ((copt & O_M) == O_M)){

      //if we're at the position of the number, and it is a number, do nothing
      if((i == mpos) && isNum(argv[mpos])){}

      //if we're not at the position of the number, and it is a number,
      //try to create the directory with the permissions specified by the number
      else if(isNum(argv[mpos])){

	//the number of directories to create
	int numDirs = 0;

	//find the number of directories to create
	for(int j = 1; argv[i][j] != '\0'; j++){

	  if((argv[i][j] == '/') && (argv[i][j + 1] != '\0')) numDirs++;

	}//for

	int lastDirPos = 0;

	//loop enough times to create the right ammount of directories
	for(int j = 0; j < (numDirs + 1); j++){

	  char dirName[1000];

	  //copy the filename into dirName
	  for(unsigned int k = 0; k < strlen(argv[i]); k++){

	    dirName[k] = argv[i][k];

	  }//for

	  //just to be safe
	  dirName[strlen(argv[i])] = '\0';

	  //get the directory name we need
	  for(int k = lastDirPos; dirName[k] != '\0'; k++){

	    if(dirName[k] == '/'){

	      dirName[k] = '\0';

	      lastDirPos = k + 1;

	      break;

	    }//if

	  }//for

	  //if we can access the directory, do nothing
	  if(access(dirName, F_OK) == 0){}

	  //if we're not at the last directory, create the directory
	  //with the normal permissions
	  else if(j < numDirs){

	    if(mkdir(dirName, 0755) < 0){

	      perror("./mkdir");

	      return EXIT_FAILURE;

	    }//if

	  }//else if

	  //if we are at the last directory, create the directory with the permissions
	  //specified by the number
	  else{

	    if(mkdir(argv[i], strtoll(argv[mpos], NULL, 8)) < 0){

	      perror("./mkdir");

	      return EXIT_FAILURE;

	    }//if

	  }//else

	}//for

      }//else if

      //if we have anything else, -m does not have a number operand, so tell the user
      //and quit the program
      else{

        write(STDOUT_FILENO, "./mkdir: option requires operand -- '-m'\n", 41);

	return EXIT_FAILURE;

      }//else

    }//else if

  }//for

  //if we have no parameters outside of the options, tell the user and quit the program
  if((numopts + 1) == argc){

    write(STDOUT_FILENO, "./mkdir: missing operand\n", 25);

    return EXIT_FAILURE;

  }//if

  //reset the umask
  umask(prevMask);

  return EXIT_SUCCESS;

}//main

/*
 * Determines whether or not a c-string is a number
 * @param ch the c-string to examine
 * @return true if the c-string is a number and false otherwise
 */
bool isNum(char * ch){

  for(int i = 0; ch[i] != '\0'; i++){

    if(!isdigit(ch[i])) return false;

  }//for

  return true;

}//isNum
