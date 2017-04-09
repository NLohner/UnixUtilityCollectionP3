#include <cstdlib>
#include <unistd.h>

int main(int argc, char ** argv, char ** envp){

  for(int i = 0; envp[i] != nullptr; i++){

    //write the variable to standard output
    write(STDOUT_FILENO, envp[i], 32);

    //write a new line to standard output if we aren't on the last entry
    if(envp[i+1] != nullptr) write(STDOUT_FILENO, "\n", 1);

  }//for

}//main
