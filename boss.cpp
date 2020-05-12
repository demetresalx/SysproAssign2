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

int administrate(char * in_dir, int wnum, int bsize, std::string * pipe_names, int * pids, int * pipe_fds){
  struct sigaction act, oldact; //gia xeirismo SIGCHLD
  sigaction(SIGCHLD, NULL, &oldact); //kratame thn palia sumperifora gia restoration argotera
  sigfillset(&(act.sa_mask));
  act.sa_handler = &myhand ;//o handler moy
  //sigaction(SIGCHLD, &act, NULL); //to orisame!

  for(int i=0; i<wnum; i++){
    pipe_fds[2*i +1] = open(pipe_names[2*i +1].c_str(), O_WRONLY ); //anoigma kathe pipe pros ta paidia gia grapsimo
    //gia paidi i, grafw sto 2*i +1, diabazw apo to 2*i
    write(pipe_fds[2*i +1], &pids[i], sizeof(pids[i]));
    std::cout << "child pid is " << pids[i] << "\n";
  }

}
