#ifndef UTILS_H_
#define UTILS_H_

#define PERMS 0666
#include <poll.h> //tha xreiastei opote to kanw include edw kai mesw autou tou header ta ginetai kai auto include
#include <string>
#include <dirent.h> //gia DIRECTORIES
#include <sstream>

void myhand(int ); //o handler
//OI PARAKATW SUNARTHSEIS EINAI OLES LOW LEVEL I/O APO PIPIES. DES TO .CPP GIA DETAILS
//grapse string se pipe me akribh tropo. isws de xreiastei
int send_string(int , char *, int );
int send_string(int , std::string *, int );

//diabase string se pipe me akribh tropo. isws de xreiastei
int receive_string(int, char *,int);
int receive_string(int, std::string *,int);

//gia na pairnw ta files h ta directories poy yparxoyn se ena path
int extract_files(char * , int * , std::string **);

//h hash function moy gia hash tables
unsigned hash_str(std::string );

//gia sugkrish dates
std::string dates_compare(std::string , std::string );

//elegxei an ena date einai ok
bool is_date_ok(std::string );

#endif
