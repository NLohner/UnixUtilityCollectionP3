#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <ctype.h>

//prototypes
int writeToBuffer(char*,int);
int writeToSTDOUT(int,int);
int linesToChar(int,int);
bool isNum(char*);
bool isDir(char*);

//the buffer that will store the file's contents
//made global so that we don't have to pass it around functions
char buffer[2048*10];

//option bits
const unsigned int O_NOOPTS =   0b0;
const unsigned int      O_F =   0b1;
const unsigned int      O_C =  0b10;
const unsigned int      O_N = 0b100;

int main(int argc, char ** argv){

  //the three possible options
  char opts[] = {'f','c','n'};

  char curropt;

  int currentoptions;

  //if one of the options is not recognised, we want to quit the program
  bool quit = false;

  //loop through the options using getopt(3) and set the option bits accordingly
  while((curropt = getopt(argc, argv, opts)) > 0){

    if(curropt == 'f') currentoptions |= O_F;

    else if(curropt == 'c') currentoptions |= O_C;

    else if(curropt == 'n') currentoptions |= O_N;

    //if we get something weird, quit
    else quit = true;

  }//while

  //quit
  if(quit) return EXIT_FAILURE;

  //if we have both the -n and -c options specified, tell the user to just use one
  //this isn't how tail works, but I think it makes more sense, as tail just ignores
  //the first input which is less helpful to the user
  if(((currentoptions & O_N) == O_N) && ((currentoptions & O_C) == O_C)){

    write(STDOUT_FILENO, "./tail: please specify either -c or -n, not both\n", 49);

    return EXIT_FAILURE;

  }//if

  //struct used to test if STDIN_FILENO is a pipe
  struct stat filestat;

  //used to direct the program to handle standard input
  bool stdin = true;

  //number of offset characters, used when we are looking at standard input
  int off = 0;


  /*                               THIS CODE IS PRETTY NASTY, TONS OF IF-ELSE STATEMENTS AHEAD                               */


  //if we have no parameters, we're reading from standard input
  if(argc == 1){

    stdin = true;

  }//if

  //cases where we have 1 parameter
  else if(argc == 2){

    //if the parameter is a dash, we're reading from standard input
    if((argv[1][0] == '-') && (argv[1][1] == '\0')){

      stdin = true;

    }//if

    //if the parameter is just -f, we're reading from standard input
    //(the standard input section handles the -f)
    else if((currentoptions & O_F) == O_F){

      stdin = true;

    }//if

    //if the parameter is just -n or -c, we need a number as well, so print an
    //error and then quit the program
    else if(((currentoptions & O_N) == O_N) || ((currentoptions & O_C) == O_C)){

      stdin = false;

      write(STDOUT_FILENO, "./tail: option requires an integer argument\n", 44);

      return EXIT_FAILURE;

    }//else if

    //the last case is if we have a file (or at least what we assume is a file)
    else{

      //we can't read directories
      if(isDir(argv[1])) return EXIT_FAILURE;

      stdin = false;

      int len = 0;

      //write the contents of the file to the buffer
      if((len = writeToBuffer(argv[1], O_NOOPTS)) < 0) return EXIT_FAILURE;

      //write the contents of the buffer to standard output, with an offset
      //of ten lines
      if(writeToSTDOUT(linesToChar(10,len), len) < 0) return EXIT_FAILURE;

    }//else

  }//else if

  //if we have 2 parameters
  else if(argc == 3){

    //if the second parameter is - and the first is -f, we read from standard input
    if((argv[2][0] == '-') && (argv[2][1] == '\0') && ((currentoptions & O_F) == O_F)){

      stdin = true;

    }//if

    //if we have -c or -n and not a number, we need a number so print an 
    //error and quit the program
    else if((((currentoptions & O_N) == O_N) || ((currentoptions & O_C) == O_C)) && !isNum(argv[2])){

      write(STDOUT_FILENO, "./tail: option requires an integer argument\n", 44);

      return EXIT_FAILURE;

    }//else if

    //if we just have -f and another option, we need a number, so print
    //an error and quit the program
    else if(((currentoptions & O_F) == O_F) && (((currentoptions & O_N) == O_N) || ((currentoptions & O_C) == O_C))){

      write(STDOUT_FILENO, "./tail: option requires an integer argument\n", 44);

      return EXIT_FAILURE;

    }//if

    //if we have -f and what we assume is a file
    else if(((currentoptions & O_F) == O_F)){

      //we can't read directories
      if(isDir(argv[2])) return EXIT_FAILURE;

      stdin = false;

      int len = 0;

      //write the contents of the file to the buffer
      if((len = writeToBuffer(argv[2], O_NOOPTS)) < 0) return EXIT_FAILURE;

      //print the contents of the buffer to the screen, with an offset of 10 lines
      if(writeToSTDOUT(linesToChar(10,len), len) < 0) return EXIT_FAILURE;

      //now for the -f, keep reading from the file as it is appended to
      if((len = writeToBuffer(argv[2], O_F)) < 0) return EXIT_FAILURE;

      //then print the file once the above has ended
      //(I dont think this ever gets called, as the only way to quit
      // the above is to type C-c, which quits the program)
      if(writeToSTDOUT(linesToChar(10,len), len) < 0) return EXIT_FAILURE;

    }//else if

    //if we have -n and a number, read from standard input with an offset of argv[2] lines
    else if(((currentoptions & O_N) == O_N) && isNum(argv[2])){

      stdin = true;

      off = atoi(argv[2]);

    }//else if

    //if we have -c and a number, read from standard input with an offset of argv[2] characters
    else if(((currentoptions & O_C) == O_C) && isNum(argv[2])){

      stdin = true;

      off = atoi(argv[2]);

    }//else if

    //if we have something else entirely, print the correct usage of the program,
    //then quit the program
    else{

      write(STDOUT_FILENO, "usage: ./tail [-f] [-c number | -n number] [file]\n", 50);

      return EXIT_FAILURE;

    }//else

  }//else if

  //three parameters
  else if(argc == 4){

    //if we have -, either -c or -n, and a number, we're reading from standard input with an offest of argv[2]
    if((argv[3][0] == '-') && (argv[3][1] == '\0') && (((currentoptions & O_N) == O_N) || ((currentoptions & O_C) == O_C)) && isNum(argv[2])){

      stdin = true;

      off = atoi(argv[2]);

    }//if

    //if we have -f, -n, and not a number, ask for a number and quit the program
    else if(((currentoptions & O_N) == O_N) && ((currentoptions & O_F) == O_F) && !isNum(argv[3])){

      write(STDOUT_FILENO, "./tail: option requires an integer argument\n", 44);

      return EXIT_FAILURE;

    }//if

    //if we have -n or -c, a number, and what we assume is a file, try to read from the file
    //with an offest of argv[2] bytes/lines
    else if((((currentoptions & O_N) == O_N) || ((currentoptions & O_C) == O_C)) && isNum(argv[2])){

      //we can't read directories
      if(isDir(argv[3])) return EXIT_FAILURE;

      stdin = false;

      int len = 0;

      //write the file's contents to the buffer
      if((len = writeToBuffer(argv[3], O_NOOPTS)) < 0) return EXIT_FAILURE;

      //if we have -c, we need the offset to be in bytes rather than lines
      if((currentoptions & O_C) == O_C){

	int offset = atoi(argv[2]);

	//the largest the offset can be is the size of the file,
	//so if the offset is bigger, set it to the length of the file
        if(offset > len) offset = len;

	//write to the screen
        if(writeToSTDOUT(offset, len) < 0) return EXIT_FAILURE;

      }//if

      //if we have -n, the offset is in lines
      else{

        if(writeToSTDOUT(linesToChar(atoi(argv[2]), len), len) < 0) return EXIT_FAILURE;

      }//else

    }//else if

    //if we have -n, -f, and a number, we're reading from standard input with offset argv[3]
    else if(((currentoptions & O_N) == O_N) && ((currentoptions & O_F) == O_F) && isNum(argv[3])){

      stdin = true;

      off = atoi(argv[3]);

    }//else if

    //if we have -c, -f, and not a number, we need a number, so quit the program
    else if(((currentoptions & O_C) == O_C) && ((currentoptions & O_F) == O_F) && !isNum(argv[3])){

      write(STDOUT_FILENO, "./tail: option requires an integer argument\n", 44);

      return EXIT_FAILURE;

    }//if

    //if we have -c, -f, and a number, we're reading from standard input with offset argv[3]
    else if(((currentoptions & O_C) == O_C) && ((currentoptions & O_F) == O_F) && isNum(argv[3])){

      stdin = true;

      off = atoi(argv[3]);

    }//else if

    //if we have a weird case, tell the user the correct usage
    else{

      write(STDOUT_FILENO, "usage: ./tail [-f] [-c number | -n number] [file]\n", 50);

      return EXIT_FAILURE;

    }//else

  }//else if

  //4 parameters
  else if(argc == 5){

    //if we have -f, -c or -n, a number, and -, we're reading from standard input with an offset of argv[3]
    if((argv[4][0] == '-') && (argv[4][1] == '\0') && isNum(argv[3]) && 
       (((currentoptions & O_C) == O_C) || ((currentoptions & O_N) == O_N)) && ((currentoptions & O_F) == O_F)){

      stdin = true;

      off = atoi(argv[3]);

    }//if

    //if we have -c or -n, -f, a number, and what we assume is a file, try to read from the file with offset argv[3],
    //remembering to loop read so that we read any appends to the file
    else if(isNum(argv[3]) && (((currentoptions & O_C) == O_C) || ((currentoptions & O_N) == O_N)) && ((currentoptions & O_F) == O_F)){

      //we can't read directories
      if(isDir(argv[4])) return EXIT_FAILURE;

      stdin = false;

      int len = 0;

      //write the file to the buffer
      if((len = writeToBuffer(argv[4], O_NOOPTS)) < 0) return EXIT_FAILURE;

      //if we have -c, handle the offset as bytes
      if((currentoptions & O_C) == O_C){

	int offset = atoi(argv[3]);

        if(offset > len) offset = len;

	if(writeToSTDOUT(offset, len) < 0) return EXIT_FAILURE;

      }//if

      //if we have -n, handle the offset as lines
      else{

	if(writeToSTDOUT(linesToChar(atoi(argv[3]), len), len) < 0) return EXIT_FAILURE;

      }//else

      //because we have -f, we now need to loop our read to catch any appends to the file
      if((len = writeToBuffer(argv[4], O_F)) < 0) return EXIT_FAILURE;

      //again, I'm pretty sure these never get called because
      //we use C-c to get out of the above loop
      if((currentoptions & O_C) == O_C){

	int offset = atoi(argv[3]);

	if(offset > len) offset = len;

        if(writeToSTDOUT(offset, len) < 0) return EXIT_FAILURE;

      }//if

      else{

        if(writeToSTDOUT(linesToChar(atoi(argv[3]), len), len) < 0) return EXIT_FAILURE;

      }//else

    }//else if

    //if we have a weird case, tell the user the correct usage
    else{

      write(STDOUT_FILENO, "usage: ./tail [-f] [-c number | -n number] [file]\n", 50);

      return EXIT_FAILURE;

    }//else

  }//else if

  //if argc > 5, we have a case we can't handle, so ell the user the correct usage
  else{

    write(STDOUT_FILENO, "usage: ./tail [-f] [-c number | -n number] [file]\n", 50);

    return EXIT_FAILURE;

  }//else


  /*                               OKAY NOW THAT THAT IS OVER                               */


  //handle standard input
  if(stdin){

    //fstat to test if STDIN_FILENO is a pipe
    if(fstat(STDIN_FILENO, &filestat) < 0){

      perror("./tail");

      return EXIT_FAILURE;

    }//if

    int i = 0;

    int count = 0;

    //infinitely read from standard input
    while((i = read(STDIN_FILENO, buffer, 32)) > 0){count += i;}

    //if we have -n, the offset is in lines
    if((currentoptions & O_N) == O_N){if(writeToSTDOUT(linesToChar(off, count),count) < 0){return EXIT_FAILURE;}}

    //if we have -c, the offset is in characters
    if((currentoptions & O_C) == O_C){if(writeToSTDOUT(off, count) < 0){return EXIT_FAILURE;}}

    //if we have neither, the offset is 10 lines
    else{if(writeToSTDOUT(linesToChar(10, count),count) < 0){return EXIT_FAILURE;}}

  }//if

  //if nothing went wrong
  return EXIT_SUCCESS;

}//main

/*
 * Writes the contents of the specified file to the buffer
 * @param filename the file to read from
 * @param opts the option bits
 * @return the number of bytes read on success, and -1 on failure
 */
int writeToBuffer(char * filename, int opts){

  int fd;

  int count = 0;

  //open the file
  if((fd = open(filename, O_RDONLY)) < 0){

    perror("./tail");

    return -1;

  }//if

  //if -f is specified, infinitely loop the read
  if((opts & O_F) == O_F){

    while((count += read(fd, buffer, 2048*10)) > 0){};

  }//if

  //if -f is not specified, only read until the end of the file
  else {

    int i = 0;

    //I don't know if I'm using read wrong, but if the number of bytes to read
    //is smaller than the filesize, read will just infinitely loop through the file,
    //going from the first byte to the specified length over and over again
    //I have no clue why this happens so I just made the size to read as large as buffer possibly can be
    while((i = read(fd, buffer, 2048*10)) > 0 && count < 2048*10){count += i;}

    //just in case
    buffer[count] = '\0';

    close(fd);

  }//else

  return count;

}//writeToBuffer

/*
 * Writes the buffer to standard output
 * @param offest the offest (in bytes) from the end of the file
 * @param length the length of the buffer
 * @return 0 on success and -1 on failure
 */
int writeToSTDOUT(int offset, int length){

  //write the buffer, starting at the point [length - offest] to standard output
  if(write(STDOUT_FILENO, buffer + length - offset, length) < 0){

    perror("./tail");

    return -1;

  }//if

  return 0;

}//writeToSTDOUT

/*
 * Reads through the buffer end-first and finds the point at which the
 * specified nubmer of lines ends
 * @param lines the number of lines to traverse
 * @param end the index of the null character of the buffer
 * @return the position (byte) that, when passed to writeToSTDOUT will 
 * print the buffer with the correct offset
 */
int linesToChar(int lines, int end){

  int numlines = 0;

  int pos = 0;

  //we're determining what a line is based on newline characters,
  //so we need to go one newline character past the number specified
  //so that we write that last line, too
  for(int i = end; numlines <= lines && i >= 0; i--){

    //if we have a newline character, increment the number of newline characters traversed
    if(buffer[i] == '\n'){

      //save the position of this newline character
      pos = i;

      numlines++;

    }//if

    //if we get to the beginning of the file before we find enough lines,
    //we set the position to the beginning of the file and exit the loop
    if(i == 0) pos = 0;

  }//for

  //if we're at the beginning of the file, we want to read the whole file,
  //so return end, which is the number of bytes in the file
  if(pos == 0) return end;

  //if we're not at the beginning of the file, pos points to a newline character,
  //and we don't want to print that, so move one character down from that newline
  return end - pos - 1;

}//lineToChar

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

/*
 * Determines whether or not the file is a directory
 * @param filename the file to check
 * @return true if the file is a directory and false otherwise
 */
bool isDir(char * filename){

  struct stat sta;

  if(lstat(filename, &sta) < 0) perror("./tail");

  if(S_ISDIR(sta.st_mode)) {

    write(STDOUT_FILENO, "./tail: cannot open directory\n", 30);

    return true;

  }//if

  return false;

}//isDir
