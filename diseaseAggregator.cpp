//MAIN ARXEIO. Apo edw trexei h efarmogh.
#include <iostream>
#include <string> //EPITREPETAI EIPWTHHKE STO PIAZZA
#include <cstring> //strcpy gia ta pathings. genika oxi idiaitera
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/poll.h>
#include <fcntl.h>
#include <signal.h>

#define PERMS 0666

int maho=0;
void myhand(int signo){
  maho=1;
  std::cout << "PETHANA " << getpid() << "\n";
}

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
  for(int i=0; i<2*numWorkers; i++){
    std::string namer = "fifo" + std::to_string(i);
    strcpy(pipe_names[i], namer.c_str()); //onomata ths morfhs fifo1,fifo2 ktl
    mkfifo(pipe_names[i], PERMS);
  }

  char cbuf[150];
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

  //ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
  //PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI
  int pipe_fds[2*numWorkers]; //ta file descriptors twn pipes
  if(pid > 0){ //parent
    //signal(SIGCHLD, myhand);
    //std::cout << "i am parent and will write to children\n";
    for(int i=0; i<numWorkers; i++){
      pipe_fds[2*i +1] = open(pipe_names[2*i +1], O_WRONLY ); //anoigma kathe pipe pros ta paidia gia grapsimo
      //std::cout << "i am parent and opened to child\n";
      write(pipe_fds[2*i +1], &pids[i], sizeof(pids[i]));
      std::cout << "child pid is " << pids[i] << "\n";
    }
    /*int fd = open("maju", O_WRONLY );
    std::cout << "i am parent and opened to child\n";
    write(fd, &pid, sizeof(pid));
    std::cout << "child pid is " << pid << "\n";*/
    std::string line;
    //close(fd);


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
    close(fd);*/
  }
  else{ //child
    signal(SIGUSR1, myhand);
    //std::cout << "i am child and will read from par\n";
    //anoigw to 2*index + 1 gt einai to katallhlo pipe gia diabasma apo ton worker
    int fd = open(pipe_names[2*child_index +1], O_RDONLY );
    //std::cout << "i am child and opened to par\n";
    int mi;
    //sleep(1);
    read(fd, &mi, 4);
    printf("my pid is %d\n", mi);
    //close(fd);

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
    //close(fd);
  }

  if(pid >0){ //parent
    for(int i=0; i<numWorkers; i++){
      wait();
      unlink(pipe_names[2*i]);
      unlink(pipe_names[2*i +1]);
    }

  }




    return 0;
}
