#ifndef UTILS_H_
#define UTILS_H_

#define PERMS 0666
#include <poll.h> //tha xreiastei opote to kanw include edw kai mesw autou tou header ta ginetai kai auto include
#include <string>

void myhand(int ); //o handler
//OI PARAKATW SUNARTHSEIS EINAI OLES LOW LEVEL I/O APO PIPIES. DES TO .CPP GIA DETAILS
//grapse string se pipe me akribh tropo. isws de xreiastei
int send_string(int , char *, int );
int send_string(int , std::string *, int );

//diabase string se pipe me akribh tropo. isws de xreiastei
int receive_string(int, char *,int);
int receive_string(int, std::string *,int);

#endif
