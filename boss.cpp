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

int glob_counter=0; //gia na perimenei oti ola ta paidia einai ok

//xeirismos SIGUSR1 gia na fanei oti ena paidi etoimasthke
void foo(int signo){
  //signal(signo, foo);
  glob_counter++;
}
//ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
//PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI

//sunarthsh poy kanei ROUND-ROBIN share ta directories sta paidia-workers
int share_dirs(int *dpw, int ndirs, int ws){
  int share=ndirs/ws; //posa tha parei toulaxiston kathe paidi
  for(int i=0; i<ws; i++)
    dpw[i] = share;
  share = ndirs % ws; //posa meinane
  for(int i=0; i<share; i++)
    dpw[i] += 1; //round robin ena o kathenas twra
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
  extract_files(in_dir, &dirs_n, &subdirs); //euresh dirs
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
      //std::cout << "ok";
      send_string(pipe_fds[2*i +1].fd, &(subdirs[dirs_writ]), bsize); //steile onoma xwras sketo
      send_string(pipe_fds[2*i +1].fd, abuf, bsize); //steile to path
      //write(pipe_fds[2*i +1].fd, abuf, strlen(abuf)+1 ); //grapse to directory name
      //std::cout << "egarpsa to " << subdirs[dirs_writ] << "\n";
      dirs_writ++;
    }

    //close(pipe_fds[2*i +1].fd);//afhnw anoixta

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
    //close(pipe_fds[2*i].fd);//afhnw anoixta
  }
  if(glob_counter ==wnum)
    std::cout << "parsing donezo!\n";

  //ARXIZOUN OI ENTOLES
  std::string line;
  int failed=0;
  int successful=0;

  while(getline(std::cin, line)){
    //std::cout << "line is " << line << "\n";
    if(line == "/exit"){ //telos
      //for(int i=0; i<wnum; i++)
        //kill(pids[i], SIGKILL);
      for(int i=0; i<wnum; i++)
        send_string(pipe_fds[2*i +1].fd, &line, bsize);
      break;
    }
    else{ //arxizoun oi entoles
      std::string const delims{ " \t\r\n" }; //delimiters einai ta: space,tab kai carriage return. TELOS.
      size_t beg, pos = 0;
      std::string requ[12]; //o arithmos orismatwn einai sugkekrimenos gia sugkekrimenes entoles opote de mas noiazei to orio
      int ind=0; //arithmos orismatwn + tou onomatos entolhs
      while ((beg = line.find_first_not_of(delims, pos)) != std::string::npos){
          pos = line.find_first_of(delims, beg + 1);
          requ[ind] = line.substr(beg, pos - beg);
          ind++;
      }//telos while eksagwghs gnwrismatwn apo entolh
      if(requ[0] == "/diseaseFrequency"){
          if(ind == 4){ //xwris to proairetiko country
            if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
              std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_fds[2*i +1].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            if((requ[2] == "-") || requ[3]== "-"){
              std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_fds[2*i +1].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            //prow9hse to aithma sta children mesw pipe
            for(int i=0; i<wnum; i++){
              send_string(pipe_fds[2*i +1].fd, "/diseaseFrequency1", bsize);//steile thn entolh
              send_string(pipe_fds[2*i +1].fd, &requ[1], bsize);//steile disease
              send_string(pipe_fds[2*i +1].fd, &requ[2], bsize);//steile date1
              send_string(pipe_fds[2*i +1].fd, &requ[3], bsize);//steile date2
            }
            //pare apanthsh
            int intreader=0;
            int intreader2=0;
            for(int i=0; i<wnum; i++){ //pare ton arithmo
                read(pipe_fds[2*i].fd, &intreader, sizeof(int));
                intreader2 += intreader;
            }
            std::cout << intreader2 << "\n";
            successful++;//epituxia
          }
          else if(ind ==5){ //me proairetiko orisma country
            if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
              std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_fds[2*i +1].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            if((requ[2] == "-") || requ[3]== "-"){
              std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_fds[2*i +1].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            //prow9hse to aithma sta children mesw pipe
            for(int i=0; i<wnum; i++){
              send_string(pipe_fds[2*i +1].fd, "/diseaseFrequency2", bsize);//steile thn entolh
              send_string(pipe_fds[2*i +1].fd, &requ[1], bsize);//steile disease
              send_string(pipe_fds[2*i +1].fd, &requ[2], bsize);//steile date1
              send_string(pipe_fds[2*i +1].fd, &requ[3], bsize);//steile date2
              send_string(pipe_fds[2*i +1].fd, &requ[4], bsize);//steile country
            }
            //pare apanthsh
            int intreader=0;
            int intreader2=0;
            for(int i=0; i<wnum; i++){ //pare ton arithmo
                read(pipe_fds[2*i].fd, &intreader, sizeof(int));
                intreader2 += intreader;
            }
            std::cout << intreader2 << "\n";
            successful++;//epituxia
          }
          else{//ekana lathos sthn entolh
            std::cout << "Lathos sta orismata. try again...\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_fds[2*i +1].fd, "bad", bsize);
            failed++;//apotuxia
          }
      }//telos if diseaseFrequency
      else if(requ[0] == "/listCountries"){
        if(ind == 1){
          //steile to aithma sta paidia
          for(int i=0; i<wnum; i++)
            send_string(pipe_fds[2*i +1].fd, "/listCountries", bsize);

          //pare tis apanthseis tous
          std::string countryandchild = "";
          int countries_per_child =0;
          for(int i=0; i<wnum; i++){
            read(pipe_fds[2*i].fd, &countries_per_child, sizeof(int));
            for(int j=0; j< countries_per_child; j++){
              receive_string(pipe_fds[2*i].fd, &countryandchild ,bsize);
              std::cout << countryandchild << "\n";
            }//telos for gia kathe xwra tou paidiou
          }//telos for gia kathe paidi
          successful++;//epituxia
        }//telos if an einai swsta h entolh
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_fds[2*i +1].fd, "bad", bsize);
          failed++;//apotuxia
        }

      }//telos if listCountries
      else if(requ[0] == "/searchPatientRecord"){
        if(ind == 2){ //apodektos arithmos orismatwn
          //steile to aithma sta paidia
          for(int i=0; i<wnum; i++){
            send_string(pipe_fds[2*i +1].fd, "/searchPatientRecord", bsize);
            send_string(pipe_fds[2*i +1].fd, &requ[1], bsize); //steile to id pros anazhthsh
          }
          //pare tis apanthseis
          std::string requested_record = "";
          for(int i=0; i<wnum; i++){
            receive_string(pipe_fds[2*i].fd, &requested_record ,bsize);
            if(requested_record == "nope")
              continue;
            else
              std::cout << requested_record << "\n";
          }
          successful++;//epituxia
        }
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_fds[2*i +1].fd, "bad", bsize);
          failed++;//apotuxia
        }

      }//telos searchPatientRecord
      else if(requ[0] == "/topk-AgeRanges"){
        if(ind == 6){ //apodektos arithmos orismatwn
          if((dates_compare(requ[4], requ[5]) != "smaller") && (dates_compare(requ[4], requ[5]) != "equal") ){ //kakws orismeno date
            std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_fds[2*i +1].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          if((requ[4] == "-") || requ[5]== "-"){
            std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_fds[2*i +1].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          int kapa = stoi(requ[1]); // h timh tou k
          if((kapa < 1)||(kapa > 4)){ //lathos timh k
            std::cout << "k must be integer in range [1, 4]\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_fds[2*i +1].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          //steile to aithma stous workers
          for(int i=0; i<wnum; i++){
            send_string(pipe_fds[2*i +1].fd, "/topk-AgeRanges", bsize);//steile thn entolh
            write(pipe_fds[2*i +1].fd, &kapa, sizeof(int));//steile k
            send_string(pipe_fds[2*i +1].fd, &requ[2], bsize);//steile country
            send_string(pipe_fds[2*i +1].fd, &requ[3], bsize);//steile disease
            send_string(pipe_fds[2*i +1].fd, &requ[4], bsize);//steile date1
            send_string(pipe_fds[2*i +1].fd, &requ[5], bsize);//steile date2
          }
          successful++;//epituxia
        }
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_fds[2*i +1].fd, "bad", bsize);
          failed++;//apotuxia
        }

      }//telos topk
      else{
        std::cout << "kakws orismenh entolh\n";
        for(int i=0; i<wnum; i++)
          send_string(pipe_fds[2*i +1].fd, "bad", bsize);
        failed++;//apotuxia
      }//telos if gia to poia kai pws einai h nonexit entolh
    }//telos else gia to an einai nonexit entolh


    //diabase apo paidi gia apanthseis, sumfwna me to prwtokollo ths ka8e mias






  }//telos while poy diabazei entoles

  //sleep(5);
  for(int i=0; i<wnum; i++){
    close(pipe_fds[2*i].fd);
    close(pipe_fds[2*i +1].fd);
  }

return 0;
}
