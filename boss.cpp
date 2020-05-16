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

int glob_counter=0; //gia na perimenei oti ola ta paidia einai ok

//xeirismos SIGUSR1 gia na fanei oti ena paidi etoimasthke
void foo(int signo){
  //signal(signo, foo);
  glob_counter++;
}
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
  //struct sigaction first_act; //gia xeirismo SIGUSR1 arxika
  //sigfillset(&(first_act.sa_mask));
  //first_act.sa_handler = &foo ;//o handler moy
  //sigaction(SIGUSR1, &first_act, NULL); //to orisame!
  //signal(SIGUSR1, foo);

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
    //std::cout << dirs_per_wrk[i];
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

  std::string tool;


  //sigourepsou (mesw blocking pipes) oti de tha proxwrhseis prin ola ta paidia teleiwsoun to parsing
  for(int i=0; i<wnum; i++){
    pipe_fds[2*i].fd = open(pipe_names[2*i].c_str(), O_RDONLY );
    receive_string(pipe_fds[2*i].fd, &tool, bsize);
    if(tool == "ok") //teleiwse to parsing to paidi
      glob_counter++;
    close(pipe_fds[2*i].fd);
  }
  if(glob_counter ==wnum)
    std::cout << "parsing donezo!\n";

  //std::cout << "ola ok!\n";
  std::string line;

  while(getline(std::cin, line)){
    std::cout << "line is " << line << "\n";
    for(int i=0; i<wnum; i++){
      //std::cout << "i am par and i  will wrt block\n";
      pipe_fds[2*i +1].fd = open(pipe_names[2*i +1].c_str(), O_WRONLY);
      //write(pipe_fds[2*i +1].fd, line.c_str(), strlen(line.c_str()) +1);
      //char eleos[200];
      //strcpy(eleos, line.c_str());
      //send_string(pipe_fds[2*i +1].fd, eleos, bsize);
      send_string(pipe_fds[2*i +1].fd, &line, bsize);
      //std::cout << "i wrote\n";
      close(pipe_fds[2*i +1].fd);
    }
    //diabase apo paidi

      for(int i=0; i<wnum; i++){
        //std::cout << "iam par and i will rd blck\n";
        pipe_fds[2*i].fd = open(pipe_names[2*i].c_str(), O_RDONLY);
        //read(pipe_fds[2*i].fd, but, bsize);
        receive_string(pipe_fds[2*i].fd, &tool, bsize);
        std::cout << "diabasa apo paidi " << tool << "\n";
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

return 0;
}
