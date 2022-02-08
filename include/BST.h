// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __BST__
#define __BST__


struct Country *searchCountry(struct Country **rootC, char *str);
struct Country *newCountryNode(char *str);
struct Country *addCountry(struct Country *node,char *str);
struct Virus *searchVirus(struct Virus **rootV, char *str);
struct Virus *searchVirusRank(struct Virus *rootV, int rank_needed);
struct Virus *newVirusNode(char *str,int noOfVirs);
struct Virus *addVirus(struct Virus *node,char *str,int noOfVirs);
void innorderC(struct Country *root);
void innorderV(struct Virus *root);
void writeInOrder(struct Virus *root,int fd2,long int bufferSize);
void makearray(struct Virus *root,char *Table_of_Heads[], int* i );
void makearrayCountry(struct Country *root,char *Table_of_Countries[], int* i );
void freeBSTC (struct Country *root);
void freeBSTV (struct Virus *root);

#endif