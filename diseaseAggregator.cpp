//MAIN ARXEIO. Apo edw trexei h efarmogh.
#include <iostream>
#include <string> //EPITREPETAI EIPWTHHKE STO PIAZZA
#include <cstring> //strcpy gia ta pathings. genika oxi idiaitera
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <signal.h>

int maho=0;
void myhand(int signo){
  maho=1;
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
  mkfifo("maju", 0666);
  char cbuf[150];
  int val =1;
  int pid = fork();
  if(pid > 0){ //parent
    std::cout << "i am parent and will write to child\n";
    int fd = open("maju", O_WRONLY );
    std::cout << "i am parent and opened to child\n";
    write(fd, &pid, sizeof(pid));
    std::cout << "child pid is " << pid << "\n";
    std::string line;
    close(fd);


    while(1){
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
        fd = open("maju", O_WRONLY  );
        write(fd, cbuf, strlen(line.c_str())+1 );
        //std::cout <<  strlen(line.c_str());
        kill(pid, SIGUSR1);
        //sleep(2);
        close(fd);
      }
      maho=0;

    }
    close(fd);
  }
  else{ //child
    signal(SIGUSR1, myhand);
    std::cout << "i am child and will read from par\n";
    int fd = open("maju", O_RDONLY );
    std::cout << "i am child and opened to par\n";
    int mi;
    read(fd, &mi, 4);
    printf("my pid is %d\n", mi);
    close(fd);

    while(1){
      std::cout << "child openchan\n";
      fd = open("maju", O_RDONLY );
      if(maho==1){
        while(read(fd, cbuf, 156) >0){
          std::cout <<"[" <<cbuf<<"]\n";
        }
        close(fd);
        //int bread = read(fd, cbuf, 156);
        //std::cout << bread<<"\n";
        /*if(bread >0)
          std::cout <<"[" <<cbuf<<"]\n";
        else
          sleep(0);*/
      }
      //maho =0;
    }
    close(fd);
  }
  wait();
  unlink("maju");








    return 0;
}
