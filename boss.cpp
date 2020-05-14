#include <signal.h> //sigaction
#include <iostream>
#include <cstring>
#include <errno.h> //perror
#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open k flags
#include <unistd.h> //read, write
#include "boss.h"
#include "utils.h"

//ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
//PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI

int administrate(char * in_dir, int wnum, int bsize, std::string * pipe_names, int * pids){
  /*struct sigaction act, oldact; //gia xeirismo SIGCHLD
  sigaction(SIGCHLD, NULL, &oldact); //kratame thn palia sumperifora gia restoration argotera
  sigfillset(&(act.sa_mask));
  act.sa_handler = &myhand ;//o handler moy
  //sigaction(SIGCHLD, &act, NULL); //to orisame!*/
  struct pollfd pipe_fds[2*wnum]; //ta file descriptors twn pipes poy tha mpoyn kai sthn poll
  memset (pipe_fds, 0, sizeof(pipe_fds)); // not necessary, but I am paranoid
  for(int i=0; i<wnum; i++){
    pipe_fds[2*i +1].fd = open(pipe_names[2*i +1].c_str(), O_WRONLY ); //anoigma kathe pipe pros ta paidia gia grapsimo
    pipe_fds[2*i +1].events = POLLOUT; //arxikopoiw gia thn poll

    //gia paidi i, grafw sto 2*i +1, diabazw apo to 2*
    write(pipe_fds[2*i +1].fd, &pids[i], sizeof(pids[i]));
    std::cout << "child pid is " << pids[i] << "\n";
    close(pipe_fds[2*i +1].fd); //comment me poll
  }
  char but[60];

  for(int i=0; i<wnum; i++){
    pipe_fds[2*i].fd = open(pipe_names[2*i].c_str(), O_RDONLY); //anoigma kathe pipe apo ta paidia gia diabasma
    pipe_fds[2*i].events = POLLIN; //arxikopoiw gia thn poll

    read(pipe_fds[2*i].fd, but, bsize);
    std::cout << but;
    close(pipe_fds[2*i].fd); //comment me poll
  }

  std::string line;

  while(getline(std::cin, line)){
    /*for(int i=0; i<wnum; i++){
      pipe_fds[2*i].events = POLLIN; //arxikopoiw gia thn poll
      pipe_fds[2*i +1].events = POLLOUT; //arxikopoiw gia thn poll
    }

    int rp = poll(pipe_fds, 2*wnum, 3000);
    if(rp !=0){
      for(int i=0; i<wnum; i++){
        if(pipe_fds[2*i +1].revents & POLLOUT){ //exoume writeable fd!
          write(pipe_fds[2*i +1].fd, line.c_str(), strlen(line.c_str()) +1);
        }

      }
      for(int i=0; i<wnum; i++){
        if(pipe_fds[2*i].revents & POLLIN){ //exoume readable fd!
          int nr = read(pipe_fds[2*i].fd, but, bsize);
          std::cout << "diabasa apo paidi " << but <<"\n";
        }
      }

    }
    else if(rp == 0)
      std::cout << "par timeout!\n";
    else
      perror("Poll error:");
      */
    for(int i=0; i<wnum; i++){
      pipe_fds[2*i +1].fd = open(pipe_names[2*i +1].c_str(), O_WRONLY );
      write(pipe_fds[2*i +1].fd, line.c_str(), strlen(line.c_str()) +1);
      close(pipe_fds[2*i +1].fd);
    }
    //diabase apo paidi
    for(int i=0; i<wnum; i++){
      pipe_fds[2*i].fd = open(pipe_names[2*i].c_str(), O_RDONLY );
      read(pipe_fds[2*i].fd, but, bsize);
      std::cout << "diabasa apo paidi " << but << "\n";
      close(pipe_fds[2*i].fd);
    }

    if(line == "/exit"){ //telos
      for(int i=0; i<wnum; i++)
        kill(pids[i], SIGKILL);
      break;
    }

  }

  //sleep(5);
  for(int i=0; i<wnum; i++){
    close(pipe_fds[2*i].fd);
    close(pipe_fds[2*i +1].fd);
  }


}
