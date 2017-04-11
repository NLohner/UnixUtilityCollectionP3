#include <iostream>
#include <string.h> //strcmp 
#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

using std::cout; //only used to print headers and error messages, not file contents 
using std::ios;

int getLines(const char*, int); //prints specified number of lines from a file to stdout
int getStdin(int); //prints specified number of lines from stdin to stdout


int main(int argc, const char * argv [])
{
  cout.setf(ios::unitbuf); //UNBUFFERED COUT       

  int number = 10; //the arg placed after -n (10 by default if no -n)  
  int success = 1; //whether a valid number was found after -n or not (1 if yes or if no -n existed to begin with) 
  int optionindex = -1; //index of -n in argv

  
  /* look for -n and a valid int directly after it */
  for(int i = 1; i < argc; i++)
    {
      if(strcmp(argv[i], "-n") == 0) {  //if "-n" is found
	optionindex = i; //store where -n was found
	success = -1; //in case the following line doesn't succeed:  
	if(argv[i+1] != nullptr) //if there's an argument after the -n 
	  success = sscanf(argv[i+1], "%d",  &number); //success = 1 if an int was found at argv[i+1]. Store it in 'number'
	
	else //no argument found after -n
	  success = -1;
	

	break; //stop searching for "-n" since one was already found
      } 
      
    }
  
  
  /* If a valid number was placed after -n OR -n wasn't included */ 
  if((success == 1) && (number >= 0))
    {
      if(argc == 3 || argc == 1)  // argc == 3 means just "./head [-n number]", no file specified after it. argc == 1 is just ./head 
	getStdin(number); //read 'number' many lines from stdin and write to stdout
      
      
      else {
	for(int i = 1; i < argc; i++) { 
	  if(i != optionindex && i != optionindex + 1) { //don't want to read from "-n 10" for instance  
	    if(*argv[i] == '-') { //notice: * before argv[i]
	      if(argc > 4) //if more than one file to show 
		cout << "==> standard input <==" << '\n';
              getStdin(number); 
	      cout << '\n';
	    }
	    
	    else {
	      if(argc > 4)  
		cout << "==> " << argv[i] << " <==" << '\n'; 
	      getLines(argv[i], number); //read 'number' many lines from argv[i] and write to stdout
	      cout << '\n';
	    }
	  } //if (!optionindex)...
	} //for
      } //else (if argc != 3)
    } //if (success == 1)...
  
  
  
  /* Else (-n was included without a valid int after it) */
  else 
    cout << "head: invalid number of lines" << '\n';
  
}


/* Return -1 on failure, 0 on success */
int getLines(const char * filename, int amount) {
  int fd;
  if((fd = open(filename, O_RDONLY)) == -1) {                                                   
    perror("head");
    return -1;
  }

  else { //if able to open                                                                                                                       
    off_t fileSize = lseek(fd, 0, SEEK_END); //file offset is now at the end for the sake of finding file size. You'll want to set the offset back to 0 before reading                       
    char * buffer = new char[fileSize]; //dynamically allocated; remember to delete later!                                                                                                    
    lseek(fd, 0, SEEK_SET); //set offset back to 0 (beginning of file)   
    
    if((fd = read(fd, buffer, fileSize)) == -1) { //read all the bytes (chars) from file associated with fd into buffer 
      perror("head");
      return -1;
    }
    
    //Find out number of lines in file:                                                                                                                                                      
    int flines = 0;
    for(int i = 0; i < fileSize; i++) { //for every char in buffer,                                                                                                                    
      if(buffer[i] == '\n')
	flines++;
    }
    
    int limit = amount; //what line to stop at
    if(flines < amount) //if file doesn't have enough lines
      limit = flines;
    
    
    int c = 0;
    int linesCounted = 0;
    while(linesCounted < limit) {
      write(1, &buffer[c], 1); //write chars to stdout
      if(buffer[c] == '\n')
	linesCounted++;
      
      c++;
    } //while

    delete[] buffer;
  } //else
  return 0;
}

/* Return -1 on failure, 0 on success */
int getStdin(int lines)
{
                                                                                                                                                           
  char ch; //where each char from stdin will be stored (one by one)
  int count = 0; //number of lines passed so far 
  int success;
  
  while(count < lines) { //while successfully read the next char and haven't reached lines 
    if((success = read(0, &ch, 1)) == -1) {
      perror("head");
      return -1;
    }

    success = write(1, &ch, 1); //write the char that was just read
    if(success == -1) {
      perror("head");
      return -1;
    }
    
    if(ch == '\n')
      count++; //while loop stops when this reaches 'lines'
  }
  return 0;
}
