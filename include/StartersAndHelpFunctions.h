// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __StartersAndHelpFunctions__
#define __StartersAndHelpFunctions__

int argsOK(int argc, char *argv[]);
int isAlpha(char *item);
void checkLine(char *line, int *error, char lineData[9][50]);
struct Citizen *addToList(struct Citizen *headC, char lineData[9][50],struct Country **rootC, struct Virus **rootV, int *noOfVirs,int *noOfCountries);
struct Citizen *searchList(struct Citizen *headC, char * ID);
int errorInDB(char lineData[9][50],struct Citizen *headC);
void printList(struct Citizen *head);
struct Citizen *processFile(char *fname, struct Country **rootC, struct Virus **rootV, int *noOfVirs,int *noOfCountries);



#endif