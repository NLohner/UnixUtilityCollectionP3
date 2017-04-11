# Project 3

Jake Schramm (811-322-582)
Nick Lohner (811-775-576)

## Instructions

1) To clean our code (i.e. remove any compiled files), type:
   $ make clean

2) To compile and link our code, type:
   $ make

3) To run our code (after compiling it), type each of the following:
   $ ./cal [[month] year] 
   $ ./date [+format]
   $ ./head [-n number] [file...]
   $ ./ln [-s] target-file link-file
   $ ./ls [-all] [file...]
   $ ./mkdir [-p] [-m mode] dir...
   $ ./env
   $ ./tail [-f] [-c number | -n number] [file]
   $ ./true
   $ ./false
   $ ./wc [-c | -m] [-lw] [file...]
   $ ./pwd 
     excluding the [ ], |, and ... 
         

	      
## Reflection 

   Man 2 pages were extremely useful for this project, as basically every
utility uses system calls. We learned more about tm structs and how to print
their fields in a specified format using strftime, which is like printf
but has special chars such as %Z for time zone and %Y for year. This was 
the gist of ./date, but tm structs were also used in ./cal and ./ls -l. 
Stat structs were useful for getting a file's permissions (the st_mode field
could be ANDed with various set bits such as S_ISUID and S_ISGUID to get each
permission bit). 
	   Although some utilities were straightforward to implement
(./true, ./false, ./env, ./date, ./ln), others required the consideration of
various scenerios and knowledge of more complex concepts. Head, tail, and 
wc, for example, required acute knowledge of how file i/o and bytes of files
worked; one of the most helpful concepts was determining when a new line 
was encountered, which we generally did by scanning for a '\n' char in a 
loop. Bytes are obviously just the number of chars counted, as a char has
a size of 1 byte. 
  Another important concept (especially for ./ls) was the fact that in order 
to look for direntries, one must first change to the directory in which that 
direntry can be found (i.e. using chdir(2)) and then perhaps change back to 
the original directory if necessary. But without changing directories, the 
file will not be found, and trying to open or read that file in the wrong 
directory will obviously result in an error. This is one way we accessed 
a file in another directory, anyways. To detect -options, in some utilities 
we used getopt(3), while in others we just went through the entire argv array 
and if argv[i] equaled "-r", then set "bool r" to true (as an example). Then, 
the "if" statements that had (r) as a condition would execute, doing whatever 
the -r option was meant to do. 
 
 
