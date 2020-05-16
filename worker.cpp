#include <signal.h> //sigaction
#include <iostream>
#include <cstring>
#include <errno.h> //perror
#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open k flags
#include <unistd.h> //read, write
#include "worker.h"
#include "utils.h"


//ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
//PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI

int work(char * read_pipe, char * write_pipe, int bsize){

  int read_fd, write_fd;
  char sbuf[500];
  int n_dirs=0;


  //DIABAZW DIRECTORIES POY MOY EDWSE O GONIOS
    read_fd = open(read_pipe, O_RDONLY );
    read(read_fd, &n_dirs, sizeof(int));
    for(int i=0; i<n_dirs; i++){
      receive_string(read_fd, sbuf, bsize );
      //std::cout << getpid() << " diabasa dir ap par " << sbuf << "\n";
    }
    close(read_fd);


    //enhmerwnw gonio oti teleiwsa to parsing
    write_fd = open(write_pipe, O_WRONLY );
    //write(write_fd, "meow", strlen("mewo") +1);
    send_string(write_fd, "ok", bsize);
    close(write_fd);

  //sleep(4);

  strcpy(sbuf, "");
  char sbuf2[200];
  strcpy(sbuf2, "");
  std::string tool;

  while(1){

      //printf("I am %d and i will rd block\n", getpid());
      read_fd = open(read_pipe, O_RDONLY );

        //int rdb = read(read_fd, sbuf2, bsize);
        //int rdb = receive_string(read_fd, sbuf2, bsize);
        int rdb = receive_string(read_fd, &tool, bsize);
        while(tool == ""){
          //rdb = receive_string(read_fd, sbuf2, bsize);
          rdb = receive_string(read_fd, &tool, bsize);
        }
        std::cout << "diabas apo gonio "<< tool << getpid() <<"\n";

        close(read_fd);
        /*if(strcmp(sbuf2, "mariah") !=0){
          //std::cout << "bout to freak out " << getpid() << "\n";
          write_fd = open(write_pipe, O_WRONLY );
          //write(write_fd, "meow", strlen("mewo") +1);
          send_string(write_fd, "meow", bsize);
          close(write_fd);
        }*/
        if(tool != "mariah"){
          //std::cout << "bout to freak out " << getpid() << "\n";
          write_fd = open(write_pipe, O_WRONLY );
          //write(write_fd, "meow", strlen("mewo") +1);
          send_string(write_fd, "meow", bsize);
          close(write_fd);
        }

      //close(read_fd);



  }



  close(read_fd);
  close(write_fd);

  return 0;


}
