// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __travel_main_questions__
#define __travel_main_questions__

int *changeBit(int *A,long k);
int checkBit(int *A, long k);

int doFirstOption(struct uInput *cmd , int ***bArrays, struct Virus *rootV, long int bloomSize,int monitor_for_country);
int DateInRange_6_month(char* sdate0,char* sdate1,char* sdate2);

int travel_request(struct uInput *cmd , int ***bArrays, struct Virus *rootV, struct Country *rootC, long int bloomSize,int *fds,int *fds2,long int bufferSize);
int DateInRange(char* sdate0,char* sdate1,char* sdate2);
int travelStats(struct uInput *cmd,struct Country *rootC,int noOfCountries);
int addVaccinationRecords(struct uInput *cmd,int *fds,int *fds2,int numMonitors,struct Country *rootC,long int bufferSize,int noOfVirs,long int bloomSize,int ***bArrays,struct Virus *rootV,int * ids2);
int searchVaccinationStatus(struct uInput *cmd,int *fds,int *fds2,long int bufferSize,int numMonitors);
void create_logFile(struct Country *rootC,int noOfCountries);
#endif