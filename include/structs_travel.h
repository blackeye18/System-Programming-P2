// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __structs_travel__
#define __structs_travel__
#define MAX_LINE 300
#define NO_OF_HASH_FUNCTIONS 16

struct LinkedList{//gia to log file

    char * accepted_or_no;
    char * date_requested;
    char * virusName;
    struct LinkedList *next;
};

struct Country
{	
	int numMonitor;//se poio monitor anoikei
    char *countryName;
    struct LinkedList *next;
    struct Country *left;
    struct Country *right;
};

struct Virus
{
    char *virusName;
    int rank;//kathe ios exei ena arithmitiko id (rank) grammis ston pinaka bloom
    struct Virus *left;
    struct Virus *right;
};

struct uInput
{
    int command;
    int argnum;
    char args[9][50];
};


#endif