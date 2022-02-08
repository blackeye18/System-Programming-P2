// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __newfunctions_travelmonitor__
#define __newfunctions_travelmonitor__
#define permissions_for_fifos 0777
void int_to_4_bytes(int number,unsigned char* bytes);
int bytes_to_int(unsigned char* bytes);
void write_int(int fd,int number);
int read_int(int fd,struct pollfd* fdarray);
int read_int_notimeout(int fd);
int argsOK(int argc, char *argv[]);
void write_string(int fd,char *string,long int bufferSize);
void Create_All_Fifos(int num_of_monitors);
void close_all_fifos(int num_of_monitors,int *fds,int *fds2);
void Delete_All_Fifos(int num_of_monitors);
void get_bloom_from_monitors(struct Virus *rootV,int ***bArrays, long int bloomSize,int noOfVirs,int fd, long int bufferSize,struct pollfd* fdarray,int currentNumMonitor);
#endif