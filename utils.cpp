#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include "utils.h"


int maho=0;

void myhand(int signo){
  maho=1;
  std::cout << "PETHANA " << getpid() << "\n";
}
