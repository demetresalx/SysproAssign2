#include <iostream>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#include "utils.h"


int send_string(int fd, char * str, int b){
  int totalwr =0;
  int size = strlen(str) +1; //to mhkos poy prepei na steilei prwta
  int bwrit = write(fd, &size, sizeof(int));
  //stelnoyme twra to string

  int offs=0;
  /*while( (bwrit = write(fd, str+offs, size)) < size ){
    size = size - bwrit;
    offs = offs +bwrit;
    totalwr += bwrit;
  }*/


  bwrit = write(fd, str, size);
  //std::cout << "parsize is " << size << "\n";
  //std::cout << "egrapsa " << bwrit << "bytes\n";
  return totalwr;
}

//to stelnei apo string gia anti gia char array
int send_string(int fd, std::string * str, int b){
  char a[300];
  strcpy(a, (*str).c_str());
  int totalwr =0;
  int size = strlen(a) +1; //to mhkos poy prepei na steilei prwta
  int bwrit = write(fd, &size, sizeof(int));
  //stelnoyme twra to string

  int offs=0;
  /*while( (bwrit = write(fd, str+offs, size)) < size ){
    size = size - bwrit;
    offs = offs +bwrit;
    totalwr += bwrit;
  }*/


  bwrit = write(fd, a, size);
  //std::cout << "parsize is " << size << "\n";
  //std::cout << "egrapsa " << bwrit << "bytes\n";
  return totalwr;
}


int receive_string(int fd, char * buf, int b){
  strcpy(buf, "");
  char tool[300];
  int totalrd =0;
  int size =0;
  //pare mhkos erxomenhs sumvoloseiras
  int brd = read(fd, &size, sizeof(int));
  //std::cout << "size is " << size << "\n";
  //pare twra to string
  brd = read(fd, buf, size);
  /*while(brd < size ){
    //std::cout << "nai";
    size = size - brd;
    brd = read(fd, tool, size);
    strcat(buf, tool);
    totalrd += brd;
    if(brd == 0)
      break;
  }*/
  //brd = read(fd, buf, size);
  //std::cout << "diabasa " << brd << "bytes\n";
  return totalrd;

}

//to krataei se string anti gia gia char array
int receive_string(int fd, std::string * str, int b){
  char tool[300];
  int totalrd =0;
  int size =0;
  //pare mhkos erxomenhs sumvoloseiras
  int brd = read(fd, &size, sizeof(int));
  //std::cout << "size is " << size << "\n";
  //pare twra to string
  brd = read(fd, tool, size);
  *str = std::string(tool);
  /*while(brd < size ){
    //std::cout << "nai";
    size = size - brd;
    brd = read(fd, tool, size);
    strcat(buf, tool);
    totalrd += brd;
    if(brd == 0)
      break;
  }*/
  //brd = read(fd, buf, size);
  //std::cout << "diabasa " << brd << "bytes\n";
  return totalrd;

}
