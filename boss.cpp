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

int quitflag =0; //gia na kserw an tha grapsw log kai kleinw

void quit_hdl(int signo){
  quitflag=1; //gia na kserei sth megalh while ti tha kanei to paidi
}



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



int administrate(char * in_dir, int wnum, int bsize, std::string * pipe_names, int * pids){
  //SIGNAL HANDLERS MOY gia SIGINT/SIGQUIT
  struct sigaction actquit;
  sigfillset(&(actquit.sa_mask)); //otan ekteleitai o handler thelw na blockarw ta panta
  actquit.sa_handler = quit_hdl;
  sigaction(SIGINT, &actquit, NULL); //to orisame!
  sigaction(SIGQUIT, &actquit, NULL); //to orisame!

  char abuf[300]; //ergaleio gia reading apo pipes ktl
  std::string * subdirs = NULL; //tha mpoun ta subdir names
  int * dirs_per_wrk = new int[wnum](); //gia na dw an teleiwse me tous katalogous gia to i paidi, initialized to 0
  int dirs_n=0;
  extract_files(in_dir, &dirs_n, &subdirs); //euresh dirs
  share_dirs(dirs_per_wrk, dirs_n, wnum); //katanomh dirs
  std::string dirsofeach[wnum][dirs_n]; //xwres kathe paidiou gia log
  struct pollfd pipe_rfds[wnum]; //ta read fds twn pipes poy tha mpoyn kai sthn poll
  struct pollfd pipe_wfds[wnum]; //ta write fds antistoixa, auta mallon de tha xreiastoun poll giati ta paidia diabazoun mono apo enan
  //anoigw ta pipes kai krataw tous fds tous
  for(int i=0; i< wnum; i++){
    pipe_wfds[i].fd = open(pipe_names[2*i +1].c_str(), O_WRONLY ); //anoigma kathe pipe pros ta paidia gia grapsimo
    pipe_rfds[i].fd = open(pipe_names[2*i].c_str(), O_RDONLY );
  }


  if(dirs_n == 0) //lathos
    {std::cout << "empty dir!!\n"; delete[] subdirs;return 1;}
  //ROUND-ROBIN KATANOMH YPO-KATALOGWN-XWRWN
  int dirs_writ = 0;
  for(int i=0; i<wnum; i++){
    //pipe_wfds[i].events = POLLOUT; //arxikopoiw gia thn poll
    //gia paidi i, grafw sto 2*i +1, diabazw apo to 2*
    write(pipe_wfds[i].fd, &(dirs_per_wrk[i]), sizeof(int)); //tou eipame oti diabazei teleutaia fora
    //std::cout << dirs_per_wrk[i];
    for(int j=0; j< dirs_per_wrk[i]; j++){
      sprintf(abuf, "%s/%s", in_dir,(subdirs[dirs_writ]).c_str() ); //pairnw to dir_name kai to bazw mazi me to inputdir (ftiaxnw path)
      //std::cout << "ok";
      send_string(pipe_wfds[i].fd, &(subdirs[dirs_writ]), bsize); //steile onoma xwras sketo
      send_string(pipe_wfds[i].fd, abuf, bsize); //steile to path
      //write(pipe_wfds[i].fd, abuf, strlen(abuf)+1 ); //grapse to directory name
      //std::cout << "egarpsa to " << subdirs[dirs_writ] << "\n";
      dirsofeach[i][j] = subdirs[dirs_writ]; //to krataw gia log
      dirs_writ++;
    }

  }//telos for gia moirasma directories

  std::string tool;
  int kids_read =0;
  //EDW DIABAZW SUMMARY STATISTICS KAI EKTYPWNW, ISWS POLL
  /*for(int i=0; i<wnum; i++){
    for(int j=0; j< dirs_per_wrk[i]; j++){
      int nfls =0;
      read(pipe_rfds[i].fd, &nfls, sizeof(int));
      for(int k=0; k<nfls; k++)
        receive_and_print_file_summary(pipe_rfds[i].fd, bsize);
    }
  }*/
  //KANW POLL GIA SUMMARIES!! Etsi mporw na diabazw prwta ta summaries twn paidiwn poy exoun teleiwsei
  //meta to kathe summary sigourepsou mesw named pipe oti to i paidi teleiwse
  int already_read[wnum]; //mh diabaseis ksana to idio paidi
  int already_ok[wnum]; //twra gia na parei kai to mhyma oti to paidi teleiwse
  memset(already_read, 0, sizeof(already_read)); // arxika ola adiabasta
  memset(already_ok, 0, sizeof(already_read)); // arxika ola adiabasta

  while(kids_read < wnum){
    //arxikopoihsh se kathe loupa gia thn poll
      reset_poll_parameters(pipe_rfds, wnum);

    int rc = poll(pipe_rfds, wnum, 2000); //kanw poll
    if(rc == 0)
      std::cout << "timeout\n";
    else{//tsekarw poioi einai etoimoi
      for(int i=0; i<wnum; i++){
        //an einai etoimo kai den to exw ksanadiabasei
        if(pipe_rfds[i].revents == POLLIN){
          if(already_read[i] == 0){ //pame gia ta summaries
            for(int j=0; j< dirs_per_wrk[i]; j++){
              int nfls =0;
              read(pipe_rfds[i].fd, &nfls, sizeof(int));
              for(int k=0; k<nfls; k++)
                receive_and_print_file_summary(pipe_rfds[i].fd, bsize); //ektupwse to summary
            }
            already_read[i] = 1;
          }
          //tsekarw kai to oti to paidi teleiwse genika
          if(already_ok[i] == 0){
            receive_string(pipe_rfds[i].fd, &tool, bsize);
            if(tool == "ok") //teleiwse to parsing to paidi
              {kids_read++; already_ok[i] = 1;}
          }

        } //telos elegxou diathesimothtas fd
      }//telos for gia paidia
    } //telos else gia timeout ths poll
  }//telos while


  delete[] dirs_per_wrk;
  delete[] subdirs; //apodesmeush axreiastou pleon pinaka

  //sigourepsou (mesw blocking pipes) oti de tha proxwrhseis prin ola ta paidia teleiwsoun to parsing
  /*for(int i=0; i<wnum; i++){
    receive_string(pipe_rfds[i].fd, &tool, bsize);
    if(tool == "ok") //teleiwse to parsing to paidi
      glob_counter++;
  }*/
  if(kids_read ==wnum)
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
        send_string(pipe_wfds[i].fd, &line, bsize);
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
                send_string(pipe_wfds[i].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            if((requ[2] == "-") || requ[3]== "-"){
              std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_wfds[i].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            //prow9hse to aithma sta children mesw pipe
            for(int i=0; i<wnum; i++){
              send_string(pipe_wfds[i].fd, "/diseaseFrequency1", bsize);//steile thn entolh
              send_string(pipe_wfds[i].fd, &requ[1], bsize);//steile disease
              send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile date1
              send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile date2
            }
            //pare apanthsh
            int intreader=0;
            int intreader2=0;
            for(int i=0; i<wnum; i++){ //pare ton arithmo
                read(pipe_rfds[i].fd, &intreader, sizeof(int));
                intreader2 += intreader;
            }
            std::cout << intreader2 << "\n";
            successful++;//epituxia
          }
          else if(ind ==5){ //me proairetiko orisma country
            if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
              std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_wfds[i].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            if((requ[2] == "-") || requ[3]== "-"){
              std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
              for(int i=0; i<wnum; i++)
                send_string(pipe_wfds[i].fd, "bad", bsize);
              failed++;//apotuxia
              continue;
            }
            //prow9hse to aithma sta children mesw pipe
            for(int i=0; i<wnum; i++){
              send_string(pipe_wfds[i].fd, "/diseaseFrequency2", bsize);//steile thn entolh
              send_string(pipe_wfds[i].fd, &requ[1], bsize);//steile disease
              send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile date1
              send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile date2
              send_string(pipe_wfds[i].fd, &requ[4], bsize);//steile country
            }
            //pare apanthsh
            int intreader=0;
            int intreader2=0;
            for(int i=0; i<wnum; i++){ //pare ton arithmo
                read(pipe_rfds[i].fd, &intreader, sizeof(int));
                intreader2 += intreader;
            }
            std::cout << intreader2 << "\n";
            successful++;//epituxia
          }
          else{//ekana lathos sthn entolh
            std::cout << "Lathos sta orismata. try again...\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
          }
      }//telos if diseaseFrequency
      else if(requ[0] == "/listCountries"){
        if(ind == 1){
          //steile to aithma sta paidia
          for(int i=0; i<wnum; i++)
            send_string(pipe_wfds[i].fd, "/listCountries", bsize);

          //pare tis apanthseis tous
          std::string countryandchild = "";
          int countries_per_child =0;
          for(int i=0; i<wnum; i++){
            read(pipe_rfds[i].fd, &countries_per_child, sizeof(int));
            for(int j=0; j< countries_per_child; j++){
              receive_string(pipe_rfds[i].fd, &countryandchild ,bsize);
              std::cout << countryandchild << "\n";
            }//telos for gia kathe xwra tou paidiou
          }//telos for gia kathe paidi
          successful++;//epituxia
        }//telos if an einai swsta h entolh
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_wfds[i].fd, "bad", bsize);
          failed++;//apotuxia
        }

      }//telos if listCountries
      else if(requ[0] == "/searchPatientRecord"){
        if(ind == 2){ //apodektos arithmos orismatwn
          //steile to aithma sta paidia
          for(int i=0; i<wnum; i++){
            send_string(pipe_wfds[i].fd, "/searchPatientRecord", bsize);
            send_string(pipe_wfds[i].fd, &requ[1], bsize); //steile to id pros anazhthsh
          }
          //pare tis apanthseis
          std::string requested_record = "";
          for(int i=0; i<wnum; i++){
            receive_string(pipe_rfds[i].fd, &requested_record ,bsize);
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
            send_string(pipe_wfds[i].fd, "bad", bsize);
          failed++;//apotuxia
        }

      }//telos searchPatientRecord
      else if(requ[0] == "/topk-AgeRanges"){
        if(ind == 6){ //apodektos arithmos orismatwn
          if((dates_compare(requ[4], requ[5]) != "smaller") && (dates_compare(requ[4], requ[5]) != "equal") ){ //kakws orismeno date
            std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          if((requ[4] == "-") || requ[5]== "-"){
            std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          int kapa = stoi(requ[1]); // h timh tou k
          if((kapa < 1)||(kapa > 4)){ //lathos timh k
            std::cout << "k must be integer in range [1, 4]\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          //steile to aithma stous workers
          for(int i=0; i<wnum; i++){
            send_string(pipe_wfds[i].fd, "/topk-AgeRanges", bsize);//steile thn entolh
            write(pipe_wfds[i].fd, &kapa, sizeof(int));//steile k
            send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile country
            send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile disease
            send_string(pipe_wfds[i].fd, &requ[4], bsize);//steile date1
            send_string(pipe_wfds[i].fd, &requ[5], bsize);//steile date2
          }
          //pare kai ektupwse ta apotelesmata
          for(int i=0; i<wnum; i++)
            read_and_present_topk(pipe_rfds[i].fd);
          successful++;//epituxia
        }
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_wfds[i].fd, "bad", bsize);
          failed++;//apotuxia
        }

      }//telos topk
      else if(requ[0] == "/numPatientAdmissions"){
        if(ind == 4){ //xwris to proairetiko country
          if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
            std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          if((requ[2] == "-") || requ[3]== "-"){
            std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          //prow9hse to aithma sta children mesw pipe
          for(int i=0; i<wnum; i++){
            send_string(pipe_wfds[i].fd, "/numPatientAdmissions1", bsize);//steile thn entolh
            send_string(pipe_wfds[i].fd, &requ[1], bsize);//steile disease
            send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile date1
            send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile date2
          }
          //pare apanthsh
          for(int i=0; i<wnum; i++){ //pare ton arithmo
              read_and_present_num_adms_disch(pipe_rfds[i].fd, bsize);
          }
          successful++;//epituxia
        }
        else if(ind ==5){ //me proairetiko orisma country
          if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
            std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          if((requ[2] == "-") || requ[3]== "-"){
            std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          //prow9hse to aithma sta children mesw pipe
          for(int i=0; i<wnum; i++){
            send_string(pipe_wfds[i].fd, "/numPatientAdmissions2", bsize);//steile thn entolh
            send_string(pipe_wfds[i].fd, &requ[1], bsize);//steile disease
            send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile date1
            send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile date2
            send_string(pipe_wfds[i].fd, &requ[4], bsize);//steile country
          }
          //pare apanthsh
          int intreader=0;
          int intreader2=0;
          for(int i=0; i<wnum; i++){ //pare ton arithmo
              read(pipe_rfds[i].fd, &intreader, sizeof(int));
              intreader2 += intreader;
          }
          std::cout << intreader2 << "\n";
          successful++;//epituxia
        }
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_wfds[i].fd, "bad", bsize);
          failed++;//apotuxia
        }
      } //telos numPatientAdmissions
      else if(requ[0] == "/numPatientDischarges"){
        if(ind == 4){ //xwris to proairetiko country
          if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
            std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          if((requ[2] == "-") || requ[3]== "-"){
            std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          //prow9hse to aithma sta children mesw pipe
          for(int i=0; i<wnum; i++){
            send_string(pipe_wfds[i].fd, "/numPatientDischarges1", bsize);//steile thn entolh
            send_string(pipe_wfds[i].fd, &requ[1], bsize);//steile disease
            send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile date1
            send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile date2
          }
          //pare apanthsh
          for(int i=0; i<wnum; i++){ //pare ton arithmo
              read_and_present_num_adms_disch(pipe_rfds[i].fd, bsize);
          }
          successful++;//epituxia
        }
        else if(ind ==5){ //me proairetiko orisma country
          if((dates_compare(requ[2], requ[3]) != "smaller") && (dates_compare(requ[2], requ[3]) != "equal") ){ //kakws orismeno date
            std::cout << "Date1 must be earlier or equal to Date2 or bad date\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          if((requ[2] == "-") || requ[3]== "-"){
            std::cout << "Date1 and Date2 can't be - , it's supposed to be an INTERVAL\n";
            for(int i=0; i<wnum; i++)
              send_string(pipe_wfds[i].fd, "bad", bsize);
            failed++;//apotuxia
            continue;
          }
          //prow9hse to aithma sta children mesw pipe
          for(int i=0; i<wnum; i++){
            send_string(pipe_wfds[i].fd, "/numPatientDischarges2", bsize);//steile thn entolh
            send_string(pipe_wfds[i].fd, &requ[1], bsize);//steile disease
            send_string(pipe_wfds[i].fd, &requ[2], bsize);//steile date1
            send_string(pipe_wfds[i].fd, &requ[3], bsize);//steile date2
            send_string(pipe_wfds[i].fd, &requ[4], bsize);//steile country
          }
          //pare apanthsh
          int intreader=0;
          int intreader2=0;
          for(int i=0; i<wnum; i++){ //pare ton arithmo
              read(pipe_rfds[i].fd, &intreader, sizeof(int));
              intreader2 += intreader;
          }
          std::cout << intreader2 << "\n";
          successful++;//epituxia
        }
        else{//ekana lathos sthn entolh
          std::cout << "Lathos sta orismata. try again...\n";
          for(int i=0; i<wnum; i++)
            send_string(pipe_wfds[i].fd, "bad", bsize);
          failed++;//apotuxia
        }
      }//telos numPatientDischarges
      else{
        std::cout << "kakws orismenh entolh\n";
        for(int i=0; i<wnum; i++)
          send_string(pipe_wfds[i].fd, "bad", bsize);
        failed++;//apotuxia
      }//telos if gia to poia kai pws einai h nonexit entolh
    }//telos else gia to an einai nonexit entolh

    if(quitflag > 0) //fagame SIGINT/QUIT
      break;

  }//telos while poy diabazei entoles
  //grapse to log sou ws gonios
  create_logfile(successful, failed, subdirs, dirs_writ);
  //sleep(5);
  for(int i=0; i<wnum; i++){
    close(pipe_rfds[i].fd);
    close(pipe_wfds[i].fd);
  }

return 0;
}

//pare kai parousiase ta apotelesmata topk apo ena pipe paidiou
//GIA THN EKTYPWSH POSOSTWN EVALA 0 DEKADIKA PSHFIA GIATI ETSI EINAI STHN EKFWNHSH
void read_and_present_topk(int rfd){
  int fetched=0;
  read(rfd, &fetched, sizeof(int));
  if(fetched ==0) //to paidi auto den exei tpt. mh sunexiseis
    return;

  int age_cat;
  float pososto;
  std::string onoma_kat = "";
  //diabazw ta topk tou paidiou (mono ena paidi tha einai)
  for(int i=0; i< fetched; i++){
    read(rfd, &age_cat, sizeof(int)); //pare omada hlikias
    read(rfd, &pososto, sizeof(float)); //pare pososto
    if(age_cat == 0)
      printf("0-20: %.0f%\n",pososto*100);
    else if(age_cat == 1)
      printf("21-40: %.0f%\n",pososto*100);
    else if(age_cat == 2)
      printf("41-60: %.0f%\n",pososto*100);
    else
      printf("60+: %.0f%\n",pososto*100);
  }
}

//pare kai parousiase ta apotelesmata topk apo ena pipe paidiou
void read_and_present_num_adms_disch(int rfd, int bsize){
  int nc =0;
  int adms=0;
  read(rfd, &nc, sizeof(int));
  std::string cname;
  for(int i=0; i< nc; i++){
    //pare onoma xwras
    receive_string(rfd, &cname, bsize);
    //pare timh
    read(rfd, &adms, sizeof(int));
    std::cout << cname << " " << adms << "\n";
  }

}
