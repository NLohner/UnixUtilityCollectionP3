#include <time.h>
#include <iostream> 
#include <iomanip> //put_time

using std::cout;
using std::ios; //for unitbuf
using std::put_time;


int main(int argc, const char * argv [])
{
  cout.setf(ios::unitbuf); //UNBUFFERED COUT 
  time_t currtime; 
  time(&currtime); //store num of seconds since the Epoch in currtime

  /* Fill the struct's members based on what currtime (seconds) was */
  struct tm * timestats = localtime(&currtime); 
 
  
  /* put_time adds to the stream the struct's members in the specified format 
     (wkday, month, day of month, clock time, time zone, year) */  
  if(argc == 1)
    cout << put_time(timestats, "%a %b %e %T %Z %Y") << '\n'; 
                                
  
  else {
    if((argv[1])[0] == '+') { //if the first char in argv[1] is '+'
      char arr[100]; 
      strftime(arr, 100, argv[1]+1, localtime(&currtime)); 
               //start at argv[1] + 1 (ignore the '+') and store the next 100 chars in arr
      cout << arr << '\n';
    }
    
  }
                                                                                                      

}
