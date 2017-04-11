#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <dirent.h>
#include <string.h> //strcmp
#include <unistd.h> //chdir
#include <pwd.h>
#include <grp.h>
#include <ctime>
#include <time.h>

using namespace std;

const char getFileType(char *); 
int printContents(const char *, bool); //for printing the non-"." files in a DIRECTORY
int printDotContents(const char *, bool); //for printing "." files of a DIRECTORY 
int getFileCount(int, char **); //num of items on cmdline other than ./ls and -options
void printStats(char *); //for -l output
int getTotalBlocks(const char *, bool); //for -l output 





int main(int argc, char * argv [])
{
  bool aopt = false; //indicator for whether the -a option is included or not
  bool lopt = false; //indicator for the -l option
  
  setvbuf(stdout, NULL, _IONBF, 0); //disable output buffering (for printf)     

  /* Look for -a or -l options (or both) */
  for(int i = 1; i < argc; i++) {
    if(strcmp(argv[i], "-a") == 0)
      aopt = true;;
    if(strcmp(argv[i], "-l") == 0)
      lopt = true;
    if((strcmp(argv[i], "-al") == 0) || (strcmp(argv[i], "-la") == 0)) {
      aopt = true;
      lopt = true;
   }
 }
    
  int filecount = getFileCount(argc, argv); //omit options and ./ls 
  
  /* IF NO FILE ARGUMENTS */
  if(filecount == 0)  {
    if(lopt) printf("total %d\n", getTotalBlocks(".", aopt));
    if(aopt)
      printDotContents(".", lopt);
    printContents(".", lopt);
  }

  /* IF ONLY ONE FILE */
  if(filecount == 1) {
    for(int i = 1; i< argc; i++) { //there's no guaruntee about where -l or -a is in argv
      if((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "-a") == 0)) {} //don't count -l or -a as files                                                                                   
      else if((strcmp(argv[i], "-la") == 0) || (strcmp(argv[i], "-al") == 0)) {} //don't count -la or -al as files                                                                         
      else if(getFileType(argv[i]) == '\0') perror(argv[i]); //make sure the file actually exists before continuing
      
      else if(getFileType(argv[i]) == 'd') { //if a directory
	if(lopt) printf("total %d\n", getTotalBlocks(argv[i], aopt));
	if(aopt)
	  printDotContents(argv[i], lopt); //. and .. inside argv[i]
	printContents(argv[i], lopt);
      }
      
      else if(getFileType(argv[i]) != '\0') { //non-directories (ELSE if) 
	printf("%s\n", argv[i]);
      }
    } //for
  }
  
  
  /* IF MORE THAN ONE FILE */
  else if(filecount > 1) {
    /* First print all non-directories */
    for(int i = 1; i<argc; i++) {                                                                                                                          
      if((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "-a") == 0)) {} //don't count -l or -a as files                                                                            
      else if((strcmp(argv[i], "-la") == 0) || (strcmp(argv[i], "-al") == 0)) {} //don't count -la or -al as files        
      else if(getFileType(argv[i]) == '\0') perror(argv[i]); //make sure the file actually exists before continuing
      
      else if((getFileType(argv[i]) != 'd') && (getFileType(argv[i]) != '\0')) { //while not a dir or NULL                                                                                 
	if(lopt) //if -l option is on, print stats
	  printStats(argv[i]);
	else
	  printf("%s\n", argv[i]);
      }
    } //for
    
      /* Now print all directories */
    for(int i = 1; i < argc; i++) {
      if((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "-a") == 0)) {} //don't count -l or -a as files                                                                                  
      else if((strcmp(argv[i], "-la") == 0) || (strcmp(argv[i], "-al") == 0)) {} //don't count -la or -al as files                                                                    
      else if(getFileType(argv[i]) == '\0') {} 
      
      else if(getFileType(argv[i]) == 'd') {
	printf("\n%s:\n", argv[i]); //print title of directory first
	if(lopt) printf("total %d\n", getTotalBlocks(argv[i], aopt));
	if(aopt)
	  printDotContents(argv[i], lopt);
	printContents(argv[i], lopt);
      }
    } //for
  } //filecount >1
  
  return EXIT_SUCCESS;
 
}



/* EXTRA FUNCTIONS: */



/* Returns a char describing the type of the file named [filename] */
const char getFileType(char * filename)
{
  struct stat buffer;
  if(lstat(filename, &buffer) == -1) return '\0';

  char filetype;

  switch (buffer.st_mode & S_IFMT) {
  case S_IFBLK:  filetype = 'b';        break;
  case S_IFCHR:  filetype = 'c';        break;
  case S_IFDIR:  filetype = 'd';        break;
  case S_IFIFO:  filetype = 'p';        break;
  case S_IFLNK:  filetype = 'l';        break;
  case S_IFREG:  filetype = '-';        break;
  case S_IFSOCK: filetype = 's';        break;
  default:       filetype = '?';        break;
  }
  return filetype;
}



/* For printing all the non-"." contents of DIRECTORIES */
int printContents(const char * name, bool lopt) {
  
  DIR * dir;
  struct dirent * direntry;
  //struct stat buffer;
 
  if((dir = opendir(name)) == NULL) { //attempt to open current directory                         
    perror("Can't open directory");
    return -1;
  }
  
  char * abspath = realpath(".", NULL); //remember where you started
  chdir(name);

  while((direntry = readdir(dir)) != NULL) //while there's still another direntry in dir                         
    {
      if((direntry->d_name)[0] != '.') {
        if(getFileType(direntry->d_name) == '\0') 
          perror(direntry->d_name);
	else{
	  if(lopt)
	    printStats(direntry->d_name);
	  else
	    printf("%s\n", direntry->d_name);
	}
      } //if    
    }	//while
  
  chdir(abspath);//note: this is outside the while loop
  free(abspath);
  closedir(dir);  

  return 0;
}

/* Prints all the contents of directory [name] that start with '.' */
int printDotContents(const char * name, bool lopt) {

  DIR * dir;
  struct dirent * direntry;
  //struct stat buffer;

  if((dir = opendir(name)) == NULL) { //attempt to open current directory                                                                               
    perror("Can't open directory");
    return -1;
  }
                                                                                                                                                          
  char * abspath = realpath(".", NULL); //passing NULL makes it call malloc to allocate up to PATH_MAX bytes for abspath
  chdir(name);

  while((direntry = readdir(dir)) != NULL) //while there's still another direntry in dir                                
    {
      if((direntry->d_name)[0] == '.') {
        if(getFileType(direntry->d_name) == '\0')
          perror(direntry->d_name);
	else {
	  if(lopt) 
	    printStats(direntry->d_name);
	  
	  else
	    printf("%s\n", direntry->d_name);
	}
      } //outer if
    } //while
  
  chdir(abspath);
  free(abspath);
  closedir(dir);
  return 0; //successful
}


/* Return number of items on the cmdline other than ./ln and -options */
int getFileCount(int argc, char * argv[]) 
{
  int filecount = 0;
  for(int i = 1; i< argc; i++) {
    if((strcmp(argv[i], "-l") == 0) || (strcmp(argv[i], "-a") == 0)) {} //don't count -l or -a as files                                                               
  else if((strcmp(argv[i], "-la") == 0) || (strcmp(argv[i], "-al") == 0)) {} //don't count -la or -al as files                                                                  
  else
    filecount++;
  }                                                                                                                                                                              
  return filecount;
}



void printPerms(mode_t); //prototype (helper method for printStats)

/* Responsible for printing all of the extra -l info for a file */
void printStats(char * fileName) {

  struct stat buf;                                                                                                                                                                    
  int n = lstat(fileName, &buf);                                                                                                      
  if(n == -1) {
    perror("stat");
    exit(EXIT_FAILURE);
  }

  else { 
    printf("%c", getFileType(fileName));
    printPerms(buf.st_mode);
    printf(".");
    
    /* Number of links */
    printf(" %1ld", (long) buf.st_nlink);
    
    /* User and group ID */
    struct passwd * userid = getpwuid(buf.st_uid);
    struct group * groupid = getgrgid(buf.st_gid);
    printf(" %s", userid->pw_name);
    printf(" %s", groupid->gr_name);
    printf(" %6lld", (long long) buf.st_size); //lld for long long int in decimal, 6 for preferred output length                                                            
                                                                                                                                                       
    /* Time */
    time_t modtime = buf.st_mtime; //convert the val from the stat struct to a time_t
    struct tm timeinfo; 
    localtime_r(&modtime, &timeinfo); //use that time_t to make a tm struct
    char * arr = new char[100];
    strftime(arr, 100, "%b %e %H:%M", &timeinfo); //store string w/ month, day of month, and hr::min time 
    printf(" %s", arr); //print the string
   
    /* File name */
    printf(" %s\n", fileName);
  }
}

/* Returns number of 1024 byte blocks in directory [name] */
int getTotalBlocks(const char * name, bool aopt) {

  int numBlocks = 0;
  DIR * dir;
  struct dirent * direntry;
  struct stat buffer;

  if((dir = opendir(name)) == NULL) { //attempt to open current directory                                                                                                   
    return -1;
  }
  char * abspath = realpath(".", NULL); //passing NULL makes it call malloc to allocate up to PATH_MAX bytes for abspath                                                  
  chdir(name);

  while((direntry = readdir(dir)) != NULL) //while there's still another direntry in dir                                                                             
    {
      if(aopt) {
	if(((direntry->d_name)[0] == '.') && (getFileType(direntry->d_name) != '\0')) {
	  if(lstat(direntry->d_name, &buffer) == -1)
	    return -1;
	  else
	    numBlocks += buffer.st_blocks;
	}
      }
      //do this always, regardless of aopt:
      if(((direntry->d_name)[0] != '.') && (getFileType(direntry->d_name) != '\0')) {
	if(lstat(direntry->d_name, &buffer) == -1)
	  return -1;
	else
	  numBlocks += buffer.st_blocks;
      } 
    } //while                                                                                                                                                               
  chdir(abspath);
  free(abspath);
  closedir(dir);

  return numBlocks / 2; //because this whole time, I was counting 512 byte blocks, and I want 1024 byte blocks to match normal ls output
}


/* From lab 09 (stat.cpp) */
void printPerms(mode_t mode) {                                                                                                                               

  printf( (mode & S_IRUSR) ? "r" : "-");
  printf( (mode & S_IWUSR) ? "w" : "-");
  bool usersticky = mode & S_ISUID;
  if(usersticky)
    printf( (mode & S_IXUSR) ? "s" : "S");
  else if(!usersticky)
    printf( ((mode & S_IXUSR))  ? "x" : "-");

  printf( (mode & S_IRGRP) ? "r" : "-");
  printf( (mode & S_IWGRP) ? "w" : "-");
  bool groupsticky = mode & S_ISGID;
  if(groupsticky)
    printf( (mode & S_IXGRP) ? "s" : "S");
  else if(!groupsticky)
    printf( (mode & S_IXGRP) ? "x" : "-");

  printf( (mode & S_IROTH) ? "r" : "-");
  printf( (mode & S_IWOTH) ? "w" : "-");
  bool othsticky = mode & S_ISVTX;
  if(othsticky)
    printf( (mode * S_IXOTH) ? "t" : "T");
  else if(!othsticky)
    printf( (mode & S_IXOTH) ? "x" : "-");
}








