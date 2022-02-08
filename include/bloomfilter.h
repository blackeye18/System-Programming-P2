// 1h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __bloomfilter__
#define __bloomfilter__

int *changeBit(int *A,long k);
int checkBit(int *A, long k);
int **makeBloom(struct Citizen *headCitizen,struct Virus *rootV,int **bArrays, long bloomSize);
int doFirstOption(struct uInput *cmd , int **bArrays, struct Virus *rootV, struct Citizen *head, long int bloomSize);
void send_bloom_filter(struct Virus *rootV,int **bArrays, long bloomSize,int noOfVirs,int fd, long int bufferSize);



#endif