#include <cstdlib>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string>
#include <string.h>
#include <wctype.h>
#include <clocale>
#include <cstring>
#include <cwchar>

//prototypes
int  writeToBuffer(char*);
int   writeNewLines(int);
int      writeWords(int);
int      writeBytes(int);
int writeCharacters(int);

//option bits
const unsigned int O_NOOPTS =    0b0;
const unsigned int      O_C =    0b1;
const unsigned int      O_M =   0b10;
const unsigned int      O_L =  0b100;
const unsigned int      O_W = 0b1000;

//the buffer that the file will be stored in
char buffer[2048*100];

using std::to_string;

int main(int argc, char ** argv){

  /* Source: http://en.cppreference.com/w/cpp/string/multibyte/mbrtowc */
  std::setlocale(LC_ALL, "en_US.utf8");

  //we want to find how many option parameters there are so that
  //we can easily loop through all of the files
  int numoptions = 0;

  //looping through each parameter
  for(int i = 1; i < argc; i++){

    //if the parameter starts with a dash and is not - or --, increment
    //the number of option parameters
    if((argv[i][0] == '-') && (argv[i][1] != '\0') && (argv[i][1] != '-')){

      numoptions++;

    }//if

  }//for

  //setup for getopt
  char currchar;

  char opts[] = {'c','m','l','w'};

  int copt = O_NOOPTS;

  //if we have a weird option, we want to quit the program
  bool quit = false;

  while((currchar = getopt(argc, argv, opts)) > 0){

    if(currchar == 'c') copt |= O_C;

    if(currchar == 'm') copt |= O_M;

    if(currchar == 'l') copt |= O_L;

    if(currchar == 'w') copt |= O_W;

    //if we have an unkown option
    else if(currchar == '?') quit = true;

  }//while

  //quit
  if(quit) return EXIT_FAILURE;

  //if argc is equal to 1 + the number of option parameters,
  //we are reading from standard input
  if((numoptions + 1) == argc){

    //writeToBuffer handles an input of - to read from
    //standard input
    char stdchar[] = {'-','\0'};

    writeToBuffer(stdchar);

    //newlines
    writeNewLines(copt);

    //words
    writeWords(copt);

    //characters
    writeCharacters(copt);

    //bytes
    writeBytes(copt);

    //write the filename
    write(STDOUT_FILENO, "-", 1);

    //write a new line
    write(STDOUT_FILENO, "\n", 1);

  }//if

  //totals to print at the end
  int totalLines = 0;

  int totalWords = 0;

  int totalBytes = 0;

  int totalChars = 0;

  //loop from the end of the option parameters through to all of the rest of the parameters
  for(int i = numoptions + 1; i < argc; i++){

    //try to write the file to the buffer.
    //if it doesn't work, don't print anything in case we can move on to
    //another parameter
    if(writeToBuffer(argv[i]) < 0){}

    //if writing the file to the buffer worked, print everything for the file
    //and increment each of the totals by the numbers returned
    else{

      //newlines
      totalLines += writeNewLines(copt);

      //words
      totalWords += writeWords(copt);

      //characters
      totalChars += writeCharacters(copt);

      //bytes
      totalBytes += writeBytes(copt);

      //write the filename
      write(STDOUT_FILENO, argv[i], strlen(argv[i]));

      //write a new line
      write(STDOUT_FILENO, "\n", 1);

    }//else

  }//for

  //if we have more than one file parameter, print the totals
  if(argc > numoptions + 2){

    if(((copt & O_L) == O_L) || (copt == O_NOOPTS)){
      const char * lineout = to_string(totalLines).c_str();
      write(STDOUT_FILENO, lineout, strlen(lineout));
      write(STDOUT_FILENO, " ", 1);
    }//if

    if((copt & O_W) == O_W || (copt == O_NOOPTS)){
      const char * wordout = to_string(totalWords).c_str();
      write(STDOUT_FILENO, wordout, strlen(wordout));
      write(STDOUT_FILENO, " ", 1);
    }//if

    if((copt & O_M) == O_M){
      const char * charout = to_string(totalChars).c_str();
      write(STDOUT_FILENO, charout, strlen(charout));
      write(STDOUT_FILENO, " ", 1);
    }//if

    if((copt & O_C) == O_C || (copt == O_NOOPTS)){
      const char * byteout = to_string(totalBytes).c_str();
      write(STDOUT_FILENO, byteout, strlen(byteout));
      write(STDOUT_FILENO, " ", 1);
    }//if

    write(STDOUT_FILENO, "total\n", 6);

  }//if

}//main

/*
 * Writes the input file to the buffer, including standard input
 * @param filename the file to write to the buffer
 * @return -1 on error and the number of bytes written on success
 */
int writeToBuffer(char * filename){

  int count = 0;

  int fd = 0;

  //handle standard input
  if((filename[0] == '-') && (filename[1] == '\0')){

    fd = STDIN_FILENO;

  }//if

  //if we're not reading from standard input, try to read the file
  else if((fd = open(filename, O_RDONLY)) < 0){

    perror("./wc");

    return -1;

  }//if

  int i = 0;

  //read from the file into the buffer, and keep track of how many bytes we read
  while((i = read(fd, buffer, 2048*100)) > 0)count += i;

  close(fd);

  //add a null character just in case
  buffer[count] = '\0';

  return count;

}//writeToBuffer

/*
 * Writes the number of lines found in the file to standard output
 * @param opts the option bits
 * @return -1 if there are options other than -l without -l
 */
int writeNewLines(int opts){

  int numlines = -1;

  //if no options are specified or -l is specified, continue
  if((opts == O_NOOPTS) || ((opts & O_L) == O_L)){

    numlines = 0;

    //loop through the buffer, and if we encounter a newline character,
    //increment the number of lines
    for(int i = 0; buffer[i] != '\0'; i++){

      if(buffer[i] == '\n') numlines++;

    }//for

    //convert the number to a string and then a c string so that we can pring it
    std::string out = to_string(numlines);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return numlines;

}//writeNewLines

/*
 * Writes the number of words found in the file to standard output
 * @param opts the option bits
 * @return -1 on failure and the number of words found otherwise
 */
int writeWords(int opts){

  int words = -1;

  //if we have no options or -w, continue
  if((opts == O_NOOPTS) || ((opts & O_W) == O_W)){

    words = 0;

    //loop though the buffer
    for(int i = 1; buffer[i] != '\0'; i++){

      //if the character is a space and the previous character is not whitespace,
      //we have a word
      if(buffer[i] == ' ' && !iswspace(buffer[i-1])) words++;

      //if the character is a newline character, and the previous character is
      //not whitespace, we have a word
      else if(buffer[i] == '\n' && !iswspace(buffer[i-1])) words++;

    }//for

    //convert the number to a string and then a c string so we can print it
    std::string out = to_string(words);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return words;

}//writeWords

/*
 * Writes the number of bytes in the file to standard output
 * @param opts the option bits
 * @return -1 on failure and the number of bytes in the file on success
 */
int writeBytes(int opts){

  int bytes = -1;

  //if we have no options or -c, continue
  if((opts == O_NOOPTS) || ((opts & O_C) == O_C)){

    bytes = 0;

    //each index of buffer is one byte
    for(int i = 0; buffer[i] != '\0'; i++){

      bytes++;

    }//for

    //convert the number to string and then c string so we can print it
    std::string out = to_string(bytes);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return bytes;

}//writeBytes

/*
 * Writes the number of characters in the file to standard output
 * @param opts the option bits
 * @return -1 on failure and the number of characters on success
 */
int writeCharacters(int opts){

  int chars = -1;

  //if we have -m, continue
  if((opts & O_M) == O_M){

    chars = 0;

    /* Source: http://en.cppreference.com/w/cpp/string/multibyte/mbrtowc (slightly edited)*/
    std::mbstate_t state = std::mbstate_t();

    //we're reading from the buffer
    const char * ptr = buffer;

    const char* end = ptr + std::strlen(ptr);

    int len;

    //we want to find the difference between the number of bytes in
    //the file and the number of bytes each normal character would have
    int diff = 0;

    int bytes = 0;

    wchar_t wc;

    //loop through buffer and find the size in bytes of each character
    while((len = std::mbrtowc(&wc, ptr, end-ptr, &state)) > 0) {

      //find the difference between the character's number of bytes and
      //a normal character's number of bytes
      diff += len - 1;

      //keep track of the total number of bytes
      bytes += len;

      //move along buffer
      ptr += len;

    }//while

    //the number of characters will be the number of bytes in the file
    //minus the number of extra bytes that are used for in each multibyte character
    chars = bytes - diff;

    //convert the number to string then c string so we can print it
    std::string out = to_string(chars);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return chars;

}//writeCharacters
