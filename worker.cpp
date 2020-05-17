#include <signal.h> //sigaction
#include <iostream>
#include <cstring>
#include <errno.h> //perror
#include <sys/types.h> //open
#include <sys/stat.h> //open
#include <fcntl.h> //open k flags
#include <unistd.h> //read, write
#include <fstream>
#include "worker.h"
#include "utils.h"
#include "record.h"


//ARTIOI = GONIOS DIABAZEI, PAIDI GRAFEI
//PERITTOI = GONIOS GRAFEI, PAIDI DIABAZEI
void print_records_in_file(std::string filename, std::string date, std::string folder){
  std::ifstream infile(filename.c_str()); //diabasma apo tis grammes tou arxeiou
  std::string line; //EPITREPETAI H STRING EIPAN STO PIAZZA
  while (std::getline(infile, line)){
    //https://stackoverflow.com/questions/49201654/splitting-a-string-with-multiple-delimiters-in-c
    std::string const delims{ " \t,\r\n" }; //delimiters einai ta: space,tab,comma kai carriage return. TELOS.
    size_t beg, pos = 0;
    int params_count =0;
    std::string true_record_parts[8];
    true_record_parts[5] = "-"; //gia entry d
    true_record_parts[6] = "-"; //gia exit d
    std::string record_parts[6]; //mia thesi gia kathe melos tou record opws prpei na einai sto arxeio
    while ((beg = line.find_first_not_of(delims, pos)) != std::string::npos){
        pos = line.find_first_of(delims, beg + 1);
        record_parts[params_count] = line.substr(beg, pos - beg);
        params_count++;
    }//telos while eksagwghs gnwrismatwn apo grammh
    if(params_count != 6) //kati leipei/pleonazei, akurh h eggrafh!
      {/*std::cout<< "BAD rec\n";*/continue;}
    //fernw thn eggrafh sth morfh ths ergasias 1 gia na einai apodotika kai eukolotera ta queries
    true_record_parts[0] = record_parts[0]; //id
    true_record_parts[1] = record_parts[2]; //first name
    true_record_parts[2] = record_parts[3]; //last name
    true_record_parts[3] = record_parts[4]; //disease
    true_record_parts[4] = folder; //country
    if(record_parts[1] == "ENTER")
      true_record_parts[5] = date; //entrydate to onoma tou arxeiou
    else if(record_parts[1] == "EXIT")
      true_record_parts[6] = date; //exitdate to onoma tou arxeiou
    else //kakh eggrafh, aporripsh k sunexeia
      {/*std::cout<< "BAD rec\n";*/continue;}
    true_record_parts[7] = record_parts[5]; //age
    record * new_rec_ptr = new record(true_record_parts); //dhmiourgia eggrafhs
    //std::cout << new_rec_ptr->get_recordID() << " " << new_rec_ptr->get_patientFirstName() << " " << new_rec_ptr->get_age() << " " << new_rec_ptr->get_entryDate()<< " " << new_rec_ptr->get_country() << "\n";
    //TO PERNAW STIS DOMES ME ELEGXO GIA EXIT AN YPARXEI KTL!!
  }//telos while diabasmatos arxeiou

}

int work(char * read_pipe, char * write_pipe, int bsize){

  int read_fd, write_fd;
  char sbuf[500];
  char jbuf[500];
  int n_dirs=0;
  int n_files=0;
  //oi domes moy. Enas aplos HT gia eggrafes kai oi HTs apo thn ergasia 1
  //record_HT records_htable(50); //o DIKOS MOU HT gia tis eggrafes basei recordID megethous h1+h2. KALUTEROS APO APLH LISTA
  //diseaseHashTable diseases_htable(25, bucketSize); //O erg1 HT GIA DISEASE
  //countryHashTable countries_htable(25, bucketSize); //O erg1 HT GIA COUNTRY



  //DIABAZW DIRECTORIES POY MOY EDWSE O GONIOS
    read_fd = open(read_pipe, O_RDONLY );
    read(read_fd, &n_dirs, sizeof(int));
    std::string * countries = new std::string[n_dirs]; //onomata xwrwn poy exw
    std::string * date_files = NULL; //tha mpoun ta filesnames-hmeromhnies
    for(int i=0; i<n_dirs; i++){
      n_files=0;
      receive_string(read_fd, &(countries[i]), bsize ); //pairnw prwta xwra
      receive_string(read_fd, sbuf, bsize ); //pairnw olo to path
      extract_files(sbuf, &n_files, &date_files); //pairnw plhrofories
      for(int j=0; j<n_files; j++){
        strcpy(jbuf, "");
        sprintf(jbuf, "%s/%s",sbuf, (date_files[j]).c_str());
        print_records_in_file(std::string(jbuf), date_files[j] ,countries[i]);
      }

      //std::cout << getpid() << " diabasa dir ap par " << sbuf << "\n";
      delete[] date_files; //sbhse to new poy egine
    }
    close(read_fd);
    delete[] countries; //svhse to new poy egine



    //enhmerwnw gonio oti teleiwsa to parsing
    write_fd = open(write_pipe, O_WRONLY );
    //write(write_fd, "meow", strlen("mewo") +1);
    send_string(write_fd, "ok", bsize);
    close(write_fd);

  //sleep(4);

  strcpy(sbuf, "");
  char sbuf2[200];
  strcpy(sbuf2, "");
  std::string tool;

  while(1){

      //printf("I am %d and i will rd block\n", getpid());
      read_fd = open(read_pipe, O_RDONLY );

        //int rdb = read(read_fd, sbuf2, bsize);
        //int rdb = receive_string(read_fd, sbuf2, bsize);
        int rdb = receive_string(read_fd, &tool, bsize);
        while(tool == ""){
          //rdb = receive_string(read_fd, sbuf2, bsize);
          rdb = receive_string(read_fd, &tool, bsize);
        }
        std::cout << "diabas apo gonio "<< tool << getpid() <<"\n";

        close(read_fd);
        /*if(strcmp(sbuf2, "mariah") !=0){
          //std::cout << "bout to freak out " << getpid() << "\n";
          write_fd = open(write_pipe, O_WRONLY );
          //write(write_fd, "meow", strlen("mewo") +1);
          send_string(write_fd, "meow", bsize);
          close(write_fd);
        }*/
        if(tool != "mariah"){
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

  return 0;


}
