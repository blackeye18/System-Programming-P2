// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __monitor_main_questions__
#define __monitor_main_questions__
int search_in_SkipList_with_virus(struct SkipListHead ** Table_of_Heads,int noOfVirs,char *citizenID,char *virusName,long int bufferSize,int fds2,int *log_total,int *log_accepted,int *log_rejected);
int travel_request(struct SkipListHead ** Table_of_Heads, struct Virus *rootV, struct Country *rootC,int fds,int fds2,struct pollfd* fdarray,long int bufferSize,int noOfVirs,int *log_total,int *log_accepted,int *log_rejecteds);
int search_in_SkipList(struct SkipListHead ** Table_of_Heads,int noOfVirs,char* citizenID,int fd2,long int bufferSize);
struct SkipListHead** NewFileProcess(char *fname, struct Country *rootC, struct Virus *rootV, int *noOfVirs,int *noOfCountries,struct SkipListHead ** Table_of_Heads,struct Citizen *head,int ***bArrays,long int bloomSize);
struct SkipListHead** addVaccinationRecords(char* starting_directory,char ** split_directories,int counter_split,int** files_counter_array,struct LinkedList * filenamelist,struct Country *rootC, struct Virus *rootV, int *noOfVirs,int *noOfCountries,struct SkipListHead ** Table_of_Heads,struct Citizen *head,int ***bArrays,long int bloomSize,long int bufferSize,int fd2);
int searchVaccinationStatus(struct SkipListHead ** Table_of_Heads,int noOfVirs,int fd,int fd2,struct pollfd* fdarray,long int bufferSize,struct Citizen *headC);
void create_logfile(struct Country *rootC,int noOfCountries,int log_total,int log_accepted,int log_rejected);
#endif