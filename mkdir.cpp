#include <sys/stat.h>
#include <sys/types.h>
#include <cstdlib>
#include <unistd.h>
#include <stdio.h>
#include <ctype.h>
#include <stdlib.h>

#include <iostream>
using std::cout;
using std::endl;

bool isNum(char*);

const unsigned int O_NOOPTS =  0b0;
const unsigned int      O_M =  0b1;
const unsigned int      O_P = 0b10;

int main(int argc, char ** argv){

  char ch;

  int prevMask = umask(0);

  char opts[] = {'m','p'};

  int copt = O_NOOPTS;

  bool quit = false;

  int mpos = 0;

  while((ch = getopt(argc, argv, opts)) > 0){

    if(ch == 'p') copt |= O_P;

    if(ch == 'm'){

      copt |= O_M;

      mpos = optind - 1;

    }//if

    else if(ch == '?') quit = true;

  }//while

  if(quit) return EXIT_FAILURE;

  int numopts = 0;

  for(int i = 1; i < argc; i++){

    if((argv[i][0] == '-') && (argv[i][1] != '\0')) numopts++;

  }//for

  for(int i = numopts + 1; i < argc; i++){

    if(copt == O_NOOPTS){

      if(mkdir(argv[i], 0755) < 0) perror("./mkdir");

    }//if

    else if(((copt & O_M) == O_M) && ((copt & O_P) != O_P)){

      if((i == (mpos + 1)) && isNum(argv[mpos + 1])){}

      else if(isNum(argv[mpos + 1])){

	if(mkdir(argv[i], strtoll(argv[mpos + 1], NULL, 8)) < 0) perror("./mkdir");

      }//if

      else{

	write(STDOUT_FILENO, "./mkdir: option requires operand -- '-m'\n", 41);

      }//else

    }//else if

    if(((copt & O_P) == O_P) && ((copt & O_M) != O_M)){

      //the number of sub-directories to create
      int numDirs = 0;

      for(int j = 1; argv[i][j] != '\0'; j++){

	if((argv[i][j] == '/') && (argv[i][j + 1] != '\0')) numDirs++;

      }//for

      cout << numDirs << endl;

    }//else if

  }//for

  if((numopts + 1) == argc){

    write(STDOUT_FILENO, "./mkdir: missing operand\n", 25);

  }//if

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
