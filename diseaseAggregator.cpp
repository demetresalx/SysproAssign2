//MAIN ARXEIO. Apo edw trexei h efarmogh.
#include <iostream>
#include <cstring> //strcpy gia ta pathings. genika oxi idiaitera
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/wait.h>
#include "worker.h"
#include "boss.h"
#include "utils.h"


int main(int argc, char** argv){

  //GIA TIS PARAMETROUS APO ARGC
  char input_path[256]; //to path tou patientRecordsFile poy tha parei apo to command line
  int numWorkers=0;
  int bufferSize=0;
  for (int i = 0; i < argc; i++){
    if (strcmp("-i", argv[i]) == 0){
      strcpy(input_path, argv[i + 1]); //akoloythei to path
    }

    if (strcmp("-w", argv[i]) == 0){
      numWorkers = atoi(argv[i + 1]); //akoloythei h timh
    }

    if (strcmp("-b", argv[i]) == 0){
      bufferSize = atoi(argv[i + 1]); //akoloythei h timh
    }

  } //telos for command line args
  if(bufferSize < 1) //prepei na einai toulaxiston 1 byte
    bufferSize = 1;

  //mkfifo("maju", PERMS);

  //Dhmiourgia named pipes
  char pipe_names[2*numWorkers][100];
  std::string pip_names[2*numWorkers];
  for(int i=0; i<2*numWorkers; i++){
    pip_names[i] = "fifo" + std::to_string(i);
    strcpy(pipe_names[i], pip_names[i].c_str()); //onomata ths morfhs fifo1,fifo2 ktl
    mkfifo(pipe_names[i], PERMS);
  }

  int pids[numWorkers]; //pinakas me ta pids twn paidiwn
  //dhmiourgia paidiwn-workers
  int pid;
  int child_index=0; //poio paidi eimai
  for(int i=0; i<numWorkers; i++){
    pid = fork();
    pids[i] = pid;
    if(pid == 0){ //worker, feugei ap th loypa dhmiourgias
      child_index=i;
      break;
    }

  }

  if(pid > 0){ //parent


    //douelia sto boss.cpp
    administrate(input_path, numWorkers , bufferSize ,pip_names, pids);


    /*while(1){
      getline(std::cin, line);
      //line = "wendy";
      strcpy(cbuf, line.c_str());
      if(line == "/exit"){
        kill(pid, SIGKILL);
        break;
        //wait();
        //exit(1);
      }
      else{
        //std::cout << "mphka loypa\n";
        //fd = open("maju", O_WRONLY  );
        write(fd, cbuf, strlen(line.c_str())+1 );
        //std::cout <<  strlen(line.c_str());
        kill(pid, SIGUSR1);
        //sleep(2);
        //close(fd);
      }
      maho=0;

    }
    */
  }
  else{ //child

    //douleia sto worker.cpp
    work(pipe_names[2*child_index +1],pipe_names[2*child_index], bufferSize);

    //while(1){
      //std::cout << "child openchan\n";
      //fd = open("maju", O_RDONLY );
      //if(maho==1){
        //while(read(fd, cbuf, 156) >0){
          //std::cout <<"[" <<cbuf<<"]\n";
        //}
        //close(fd);
        //int bread = read(fd, cbuf, 156);
        //std::cout << bread<<"\n";
        /*if(bread >0)
          std::cout <<"[" <<cbuf<<"]\n";
        else
          sleep(0);*/
      //}
      //maho =0;
    //}
  }

  //Perimene kathe paidi kai sbhse ta pipes tou
  if(pid >0){ //parent
    for(int i=0; i<numWorkers; i++){
      kill(pids[i], SIGKILL); //Mhdeia paidoktonos!!
      int stats;
      int fpid= waitpid(pids[i], &stats, 0);
      //unlink gia sbhsimo pipes tou kathe paidiou. Meta th wait to paidi tha exei teleiwsei kai den ta xrhsimopoiei pleon
      if( unlink(pipe_names[2*i]) < 0)
            perror("Error: Cannot unlink worker");
        if( unlink(pipe_names[2*i +1]) < 0)
            perror("Error: Cannor unlink jobExecutor");
      //printf("teleiwse o %d\n", fpid);
    }

  }


    return 0;
}
