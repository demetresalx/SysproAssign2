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
  int read_fd, write_fd;
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

    read_fd = open(read_pipe, O_RDONLY );
    read(read_fd, &n_dirs, sizeof(int));
    //std::cout << "sbuf " << sbuf;
    for(int i=0; i<n_dirs; i++){
      //read_fd = open(read_pipe, O_RDONLY );
      receive_string(read_fd, sbuf, bsize );
      //read(read_fd, sbuf, 7);
      std::cout << getpid() << " diabasa dir ap par " << sbuf << "\n";
      //close(pipe_fd);
    }
    close(read_fd);





  /*int pipe_fd2 = open(write_pipe, O_WRONLY);
  pipe_fds[1].fd = pipe_fd2;
  pipe_fds[1].events = POLLOUT;
  write(pipe_fd2, "he" ,3);
  close(pipe_fd2);
  */
  //sleep(4);

  strcpy(sbuf, "");
  char sbuf2[200];
  strcpy(sbuf2, "");

  //pipe_fd = open(read_pipe, O_RDONLY | O_NONBLOCK );

  while(1){

      //printf("I am %d and i will rd block\n", getpid());
      read_fd = open(read_pipe, O_RDONLY );

        //int rdb = read(read_fd, sbuf2, bsize);
        int rdb = receive_string(read_fd, sbuf2, bsize);
        while(strcmp(sbuf2, "") ==0){
          rdb = receive_string(read_fd, sbuf2, bsize);
        }
        std::cout << "diabas apo gonio "<< sbuf2 << getpid() <<"\n";

        close(read_fd);
        if(strcmp(sbuf2, "mariah") !=0){
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






}
