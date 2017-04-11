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

#include <iostream>
using std::cout;
using std::endl;

int  writeToBuffer(char*);
int   writeNewLines(int);
int      writeWords(int);
int      writeBytes(int);
int writeCharacters(int);

const unsigned int O_NOOPTS =    0b0;
const unsigned int      O_C =    0b1;
const unsigned int      O_M =   0b10;
const unsigned int      O_L =  0b100;
const unsigned int      O_W = 0b1000;

char buffer[2048*10];

using std::to_string;

int main(int argc, char ** argv){

  /* Source: http://en.cppreference.com/w/cpp/string/multibyte/mbrtowc */
  std::setlocale(LC_ALL, "en_US.utf8");

  int numoptions = 0;

  for(int i = 1; i < argc; i++){

    if((argv[i][0] == '-') && (argv[i][1] != '\0') && (argv[i][1] != '-')){

      numoptions++;

    }//if

  }//for

  char currchar;

  char opts[] = {'c','m','l','w'};

  int copt = O_NOOPTS;

  bool quit = false;

  while((currchar = getopt(argc, argv, opts)) > 0){

    if(currchar == 'c') copt |= O_C;

    if(currchar == 'm') copt |= O_M;

    if(currchar == 'l') copt |= O_L;

    if(currchar == 'w') copt |= O_W;

    else quit = true;

  }//while

  if(quit) return EXIT_FAILURE;

  if((numoptions + 1) == argc){

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

  int totalLines = 0;

  int totalWords = 0;

  int totalBytes = 0;

  int totalChars = 0;

  for(int i = numoptions + 1; i < argc; i++){

    if(writeToBuffer(argv[i]) < 0){}

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

int writeToBuffer(char * filename){

  int count = 0;

  int fd = 0;

  if((filename[0] == '-') && (filename[1] == '\0')){

    fd = STDIN_FILENO;

  }//if

  else if((fd = open(filename, O_RDONLY)) < 0){

    perror("./wc");

    return -1;

  }//if

  int i = 0;

  while((i = read(fd, buffer, 2048*10)) > 0)count += i;

  close(fd);

  buffer[count] = '\0';

  return count;

}//writeToBuffer

int writeNewLines(int opts){

  int numlines = -1;

  if((opts == O_NOOPTS) || ((opts & O_L) == O_L)){

    numlines = 0;

    for(int i = 0; buffer[i] != '\0'; i++){

      if(buffer[i] == '\n') numlines++;

    }//for

    std::string out = to_string(numlines);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return numlines;

}//writeNewLines

int writeWords(int opts){

  int words = -1;

  if((opts == O_NOOPTS) || ((opts & O_W) == O_W)){

    words = 0;

    for(int i = 1; buffer[i] != '\0'; i++){

      if(buffer[i] == ' ' && !iswspace(buffer[i-1])) words++;

      else if(buffer[i] == '\n' && !iswspace(buffer[i-1])) words++;

    }//for

    std::string out = to_string(words);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return words;

}//writeWords

int writeBytes(int opts){

  int bytes = -1;

  if((opts == O_NOOPTS) || ((opts & O_C) == O_C)){

    bytes = 0;

    for(int i = 0; buffer[i] != '\0'; i++){

      bytes++;

    }//for

    std::string out = to_string(bytes);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return bytes;

}//writeBytes

int writeCharacters(int opts){

  int chars = -1;

  if((opts & O_M) == O_M){

    chars = 0;

    /* Source: http://en.cppreference.com/w/cpp/string/multibyte/mbrtowc (slightly edited)*/
    std::mbstate_t state = std::mbstate_t();

    const char * ptr = buffer;

    const char* end = ptr + std::strlen(ptr);

    int len;

    int diff = 0;

    int bytes = 0;

    wchar_t wc;

    while((len = std::mbrtowc(&wc, ptr, end-ptr, &state)) > 0) {

      diff += len - 1;

      bytes += len;

      ptr += len;

    }

    chars = bytes - diff;

    std::string out = to_string(chars);

    write(STDOUT_FILENO, out.c_str(), out.length());

    write(STDOUT_FILENO, " ", 1);

  }//if

  return chars;

}//writeCharacters
