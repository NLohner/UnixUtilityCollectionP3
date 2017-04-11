#include <iostream>
#include <string>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <iomanip>

using namespace std;      

int getFirstDay(int, int); //month, year 
int getMoDays(int, int);
void printMonth(int, int);
int julianDate(int, int, int);

int main(int argc, char * argv [])
{
  setvbuf(stdout, NULL, _IONBF, 0); //unbuffered io
  string months[12] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
  

  /* Get current year and month */
  time_t currtime;
  time(&currtime); //store num of seconds since the Epoch in currtime 
  struct tm * timeinfo = localtime(&currtime); //store struct's members based on what currtime (seconds) was 
  int currYear = timeinfo->tm_year + 1900; //tm_year is yrs since 1900
  int currMonth = timeinfo->tm_mon + 1; //tm_mon is 0-11, I want 1-12 for jan-dec 
  int currDay = timeinfo->tm_mday; //day of the month



  if(argc == 1) { //default
    printf("%11s", months[currMonth-1].c_str()); //months[1] is February
    printf(" %s\n", to_string(currYear).c_str());
    printMonth(currMonth, currYear);
    printf("\nThe current day of the month is: %s\n", to_string(currDay).c_str());
  }

  if(argc == 2) { //argv[1] is the year
    printf("%26s\n\n", argv[1]); //print year title   
 
    for(int i = 1; i <= 12; i++) { //for every month...                                                                                                                                                      
      if(i != 1) printf("\n"); //space btwn each month
      printf("%13s\n", months[i-1].c_str()); //month title (ex. January)
      printMonth(i, atoi(argv[1]));
    }
  }
  
  if(argc == 3) { //month AND year
    int monthnum = atoi(argv[1]);
    printf("%11s", months[monthnum-1].c_str()); //months is an array, so if user enters 1, treat it as 0 for 0th ele in array                                                                               
    printf(" %s\n", argv[2]); //./cal [month] [year]. argv[2] is year 
    printMonth(atoi(argv[1]), atoi(argv[2]));
  }
} //main




/* 
 * Source: http://www.cplusplus.com/forum/beginner/55721/ 
 * @return returns the first day in the month (not always a Sunday, obviously). 
 * 0 for Sunday, 1 for Monday, etc.
 */
int getFirstDay(int month, int year) {
  
  int day = 1;
  int y = year - (14-month)/12;
  int m = month +12 * ((14-month) / 12) -2;
  int firstday = (day + y + y / 4 - y / 100 + y / 400 + (31 * m / 12)) % 7; 
  
  return firstday;  
}

/* Returns number of days in that month (accounts for leap years for Feb) */ 
int getMoDays(int mo, int year)
{
  switch(mo)
    {
    case 1: //Jan
    case 3: //March
    case 5: //May
    case 7: //July
    case 8: //August
    case 10: //Oct
    case 12: //December
      return 31;
    break;
    
    case 2: //Feb
      /* Centuries: 1600 is a leap year, but 1700 isn't even though both are div by 4. That's why both parts are necessary */
      if(year % 400 == 0 || (year % 4 == 0 && year % 100 != 0))
	return 29; //leap year
      else
	return 28;
      break;
      
    case 4: //April
    case 6: //June
    case 9: //Sept
    case 11: //November
      return 30;
    break;
    
    default:
      return 0;
      break;
    } //switch
}


/* 
 * Prints the specified month in the year. 
 * This section was INSPIRED by http://www.cplusplus.com/forum/beginner/55721/ if anything looks similar   
 */
void printMonth(int mo, int yr)
{                                                                            
  printf("Su Mo Tu We Th Fr Sa\n");
  
  int numDaysInMo = getMoDays(mo, yr);
  int firstday = getFirstDay(mo, yr); //first day of the month (0 for Sunday, 1 for Monday, etc.)                                                                                                           
  if(mo == 9 && yr == 1752) firstday -= 3; //Sept 1752 special case 
  
  for (int d = 0; d < firstday; ++d) //move over to Tues if firstday is 2, for example                                                                                                                             
    {
      printf("%2s ", " ");
    }
                                                                                                                                                   
  int wkdaycounter = firstday; //to know when to go to next line
  
  for(int daynumcounter = 1; daynumcounter <= numDaysInMo; ++daynumcounter) {
    if(mo == 9 && yr == 1752) //september 1752 Gregorian calendar adopted     
      if(daynumcounter == 3)
	daynumcounter = 14; //go from 2 to 14 immediately 
  
    printf("%2s ", to_string(daynumcounter).c_str()); //print each day (1, 2, 3..) in the month                                                                                                                    
    ++wkdaycounter;
    if(wkdaycounter > 6)
      {
	printf("\n");
	wkdaycounter = 0;
      }
  } //for                                                                                                                                                                    
  printf("\n");
}                                                                                                                                                                                    
  
