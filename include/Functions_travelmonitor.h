// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __Functions_travelmonitor__
#define __Functions_travelmonitor__



struct Country *searchCountry(struct Country **rootC, char *str);
struct Country *newCountryNode(char *str,int numMonitor);
struct Country *addCountry(struct Country *node,char *str,int numMonitor);
void makearrayCountry(struct Country *root,char *Table_of_Countries[], int* i );
void innorderC(struct Country *root);
void freeBSTC (struct Country *root);
struct Virus *searchVirus(struct Virus **rootV, char *str);
struct Virus *newVirusNode(char *str,int noOfVirs);
struct Virus *addVirus(struct Virus *node,char *str,int noOfVirs);
void innorderV(struct Virus *root);
void freeBSTV (struct Virus *root);

struct uInput *checkUserInput();
#endif