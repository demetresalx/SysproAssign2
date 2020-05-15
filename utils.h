#ifndef UTILS_H_
#define UTILS_H_

#define PERMS 0666
#include <poll.h> //tha xreiastei opote to kanw include edw kai mesw autou tou header ta ginetai kai auto include

extern int maho;

void myhand(int ); //o handler

//grapse string se pipe me akribh tropo. isws de xreiastei
int send_string(int , char *, int );

//diabase string se pipe me akribh tropo. isws de xreiastei
int receive_string(int, char * , int);

#endif
