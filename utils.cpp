#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include "utils.h"


int maho=0;

void myhand(int signo){
  maho=1;
  std::cout << "PETHANA " << getpid() << "\n";
}

int send_string(int fd, char * str, int b){
  int totalwr =0;
  int size = strlen(str) +1; //to mhkos poy prepei na steilei prwta
  int bwrit = write(fd, &size, sizeof(int));
  //stelnoyme twra to string

  /*while( (bwrit = write(fd, str, size)) < size )
    totalwr += bwrit;
  */
  bwrit = write(fd, str, size);
  //std::cout << "egrapsa " << totalwr << "bytes\n";
  return totalwr;
}

int receive_string(int fd, char * buf, int b){
  strcpy(buf, "");
  char tool[200];
  int totalrd =0;
  int size =0;
  //pare mhkos erxomenhs sumvoloseiras
  int brd = read(fd, &size, sizeof(int));
  //pare twra to string
  /*while( (brd = read(fd, tool, size)) > 0 ){
    strcat(buf, tool);
    totalrd += brd;
  }*/
  brd = read(fd, buf, size);
  //std::cout << "diabasa " << totalrd << "bytes\n";
  return totalrd;

}
