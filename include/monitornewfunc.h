// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __monitornewfunc__
#define __monitornewfunc__

void int_to_4_bytes(int number,unsigned char* bytes);
int bytes_to_int(unsigned char* bytes);
void write_int(int fd,int number);
int read_int(int fd,struct pollfd* fdarray);
void write_string(int fd,char *string,long int bufferSize);
int read_int_notimeout(int fd);

#endif