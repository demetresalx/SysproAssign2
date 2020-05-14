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

int work(char * rd_p, char * wrt_p, int bsize){
  //pairnw ta onomata twn pipes kai ta krataw gia eykolia
  int pipe_fd, pipe_fd2;
  char sbuf[500];
  char read_pipe[64];
  char write_pipe[64];
  strcpy(read_pipe, rd_p);
  strcpy(write_pipe, wrt_p);


  /*struct sigaction act_us, oldact_us; //gia xeirismo SIGUSR1
  sigaction(SIGUSR1, NULL, &oldact_us); //kratame thn palia sumperifora gia restoration argotera
  sigfillset(&(act_us.sa_mask));
  act_us.sa_handler = &myhand ;//o handler moy
  //sigaction(SIGCHLD, &act_us, NULL); //to orisame!*/

  //int pipe_fd = open(read_pipe, O_RDONLY );


  //std::cout << "i am child and opened to par\n";

  /*struct pollfd pipe_fds[2]; //ta file descriptors twn pipes poy tha mpoyn kai sthn poll
  memset (pipe_fds, 0, sizeof(pipe_fds)); // not necessary, but I am paranoid
  //int mi = poll(pipe_fd, 1, 3000);
  */
  int n_dirs=0;
  //pipe_fds[0].fd = pipe_fd;
  //pipe_fds[0].events = POLLIN;
  //sleep(1);

  //DIABAZW DIRECTORIES POY MOY EDWSE O GONIOS

    pipe_fd = open(read_pipe, O_RDONLY );
    read(pipe_fd, &n_dirs, sizeof(int));

    for(int i=0; i<n_dirs; i++){
      //pipe_fd = open(read_pipe, O_RDONLY );
      read(pipe_fd, sbuf, 7);
      std::cout << getpid() << " diabasa dir ap par " << sbuf << "\n";
      //close(pipe_fd);
    }
    close(pipe_fd);





  /*int pipe_fd2 = open(write_pipe, O_WRONLY);
  pipe_fds[1].fd = pipe_fd2;
  pipe_fds[1].events = POLLOUT;
  write(pipe_fd2, "he" ,3);
  close(pipe_fd2);
  */
  char but[500];


  //pipe_fd = open(read_pipe, O_RDONLY | O_NONBLOCK );
  while(1){
    /*int rp = poll(pipe_fds, 2, 3000);
    if(rp !=0){

      if(pipe_fds[0].revents & POLLIN){ //exoume readable fd!
        read(pipe_fds[0].fd, but2, bsize);
        std::cout << "diabas apo gonio "<< but2 <<"\n";
        if(strcmp(but, "/exit") ==0) //telos
          break;
        //int jikan = write(pipe_fds[1].fd, but, strlen(but) +1);

        //perror("chld write");
      }
      if(pipe_fds[1].revents & POLLOUT){ //exoume writeable fd!
        int ji = write(pipe_fds[1].fd, but, strlen(but) +1);
        std::cout << ji << " egrapsa ws paidi\n";
      }

    }
    else if(rp == 0)
      std::cout << "ch timeout!\n";
    else
      perror("Poll error:");
      */
      pipe_fd = open(read_pipe, O_RDONLY );
      int rdb = read(pipe_fd, but, bsize);
      std::cout << "diabas apo gonio "<< but << getpid() <<"\n";
      close(pipe_fd);
      if(strcmp(but, "/laugh") !=0){
        pipe_fd2 = open(write_pipe, O_WRONLY);
        write(pipe_fd2, "meow", strlen("mewo") +1);
        close(pipe_fd2);
      }


  }



  close(pipe_fd);
  close(pipe_fd2);






}
