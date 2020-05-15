#include <signal.h> //sigaction
#include <iostream>
#include <cstring>
#include <errno.h> //perror
#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open k flags
#include <unistd.h> //read, write
#include <dirent.h> //gia DIRECTORIES
#include "boss.h"
#include "utils.h"

//ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
//PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI

int share_dirs(int *dpw, int ndirs, int ws){
  int share=ndirs/ws;
  for(int i=0; i<ws; i++)
    dpw[i] = share;
  share = ndirs % ws;
  for(int i=0; i<share; i++)
    dpw[i] += 1;
  return 0;
}

int detect_subdirs(char * inpdir, int * dleft, std::string ** subdrs){
  DIR *dp;
  struct dirent *dirp;
  //anoigw kai tsekarw oti einai ok
  dp = opendir(inpdir);
  if(dp == NULL)
  {
      std::cout << "Error opening " << inpdir << "\n";
      return errno;
  }
  //metraw ta directories poy exei mesa
  while ((dirp = readdir(dp)) != NULL){
    if((strcmp(dirp->d_name, ".") == 0)||(strcmp(dirp->d_name, "..") == 0))
      continue;
    *(dleft) += 1;
  }
  closedir(dp);

  if(*dleft > 0){
    dp = opendir(inpdir);
    if(dp == NULL)
    {
        std::cout << "Error opening " << inpdir << "\n";
        return errno;
    }
    //metraw ta directories poy exei mesa

    *subdrs = new std::string[*dleft];
    int in =0;
    while ((dirp = readdir(dp)) != NULL){ //krata to dir name
      if((strcmp(dirp->d_name, ".") == 0)||(strcmp(dirp->d_name, "..") == 0))
        continue;
      (*subdrs)[in] = std::string(dirp->d_name);
      in++;
    }
    closedir(dp);
  }
  return 0;
}

//ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
//PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI
int administrate(char * in_dir, int wnum, int bsize, std::string * pipe_names, int * pids){
  /*struct sigaction act, oldact; //gia xeirismo SIGCHLD
  sigaction(SIGCHLD, NULL, &oldact); //kratame thn palia sumperifora gia restoration argotera
  sigfillset(&(act.sa_mask));
  act.sa_handler = &myhand ;//o handler moy
  //sigaction(SIGCHLD, &act, NULL); //to orisame!*/

  char abuf[300]; //ergaleio gia reading apo pipes ktl
  std::string * subdirs = NULL; //tha mpoun ta subdir names
  int * dirs_per_wrk = new int[wnum](); //gia na dw an teleiwse me tous katalogous gia to i paidi, initialized to 0
  int dirs_n=0;
  detect_subdirs(in_dir, &dirs_n, &subdirs); //euresh dirs
  share_dirs(dirs_per_wrk, dirs_n, wnum); //katanomh dirs

  struct pollfd pipe_fds[2*wnum]; //ta file descriptors twn pipes poy tha mpoyn kai sthn poll

  if(dirs_n == 0) //lathos
    {std::cout << "empty dir!!\n"; delete[] subdirs;return 1;}
  //ROUND-ROBIN KATANOMH YPO-KATALOGWN-XWRWN
  int dirs_writ = 0;
  for(int i=0; i<wnum; i++){
    pipe_fds[2*i +1].fd = open(pipe_names[2*i +1].c_str(), O_WRONLY ); //anoigma kathe pipe pros ta paidia gia grapsimo
    //pipe_fds[2*i +1].events = POLLOUT; //arxikopoiw gia thn poll
    //gia paidi i, grafw sto 2*i +1, diabazw apo to 2*
    write(pipe_fds[2*i +1].fd, &(dirs_per_wrk[i]), sizeof(int)); //tou eipame oti diabazei teleutaia fora
    for(int j=0; j< dirs_per_wrk[i]; j++){
      sprintf(abuf, "%s/%s", in_dir,(subdirs[dirs_writ]).c_str() ); //pairnw to dir_name kai to bazw mazi me to inputdir (ftiaxnw path)
      send_string(pipe_fds[2*i +1].fd, abuf, bsize);
      //write(pipe_fds[2*i +1].fd, abuf, strlen(abuf)+1 ); //grapse to directory name
      //std::cout << "egarpsa to " << subdirs[dirs_writ] << "\n";
      dirs_writ++;
    }

    close(pipe_fds[2*i +1].fd);

  }//telos for gia moirasma directories
  delete[] dirs_per_wrk;
  delete[] subdirs; //apodesmeush axreiastou pleon pinaka


  char but[500];

  /*for(int i=0; i<wnum; i++){
    pipe_fds[2*i].fd = open(pipe_names[2*i].c_str(), O_RDONLY); //anoigma kathe pipe apo ta paidia gia diabasma
    pipe_fds[2*i].events = POLLIN; //arxikopoiw gia thn poll

    read(pipe_fds[2*i].fd, but, bsize);
    std::cout << but;
    close(pipe_fds[2*i].fd); //comment me poll
  }*/

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
