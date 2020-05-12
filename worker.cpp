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

int work(char * rd_p, char * wrt_p){
  //pairnw ta onomata twn pipes kai ta krataw gia eykolia
  char read_pipe[64];
  char write_pipe[64];
  strcpy(read_pipe, rd_p);
  strcpy(write_pipe, wrt_p);

  struct sigaction act_us, oldact_us; //gia xeirismo SIGUSR1
  sigaction(SIGUSR1, NULL, &oldact_us); //kratame thn palia sumperifora gia restoration argotera
  sigfillset(&(act_us.sa_mask));
  act_us.sa_handler = &myhand ;//o handler moy
  //sigaction(SIGCHLD, &act_us, NULL); //to orisame!

  int pipe_fd = open(read_pipe, O_RDONLY );
  //std::cout << "i am child and opened to par\n";
  int mi;
  //sleep(1);
  read(pipe_fd, &mi, 4);
  printf("my pid is %d\n", mi);
  close(pipe_fd);

}
