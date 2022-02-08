// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#ifndef __structs__
#define __structs__
#define DEPTH_MAX_LIMIT 14 //Exoume mexri 9999 polites, ara 2^14~=16k einai yperarketo
#define DEPTH_MIN_LIMIT 2// 1 einai mono h arxikh seira,dhladh vathos 0 ousiastika mia aplh list, 2 me max vathos 1 klp
#define MAX_LINE 300
#define NO_OF_HASH_FUNCTIONS 16 //Plhthos hasfunctions gia to bloom filter, mas dothike sto piazza

struct LinkedList{//apothikevw ta txt poy exw anoiksei kathws kai se poio fakelo einai

    int folder;
    char* txtname;
    struct LinkedList *next;
};

//structs for two Binary Search Tree Nodes
struct Country
{
    char *countryName;
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
//struct of a citizen, node of a singly linked list. country, virusName point to the relevant BS Tree nodes
struct Citizen
{
    char *citizenID;
    char  *firstName;
    char  *lastName;
    struct Country *country;
    int age;
    struct Virus *virusName;
    char *vaccinated;
    char *dateVaccinated;
    struct Citizen *next;
};
//struct for a user input
struct uInput
{
    int command;
    int argnum;
    char args[9][50];
};

//kathe ios mporei na exei polla SkipListNode.
struct SkipListNode
{  
   struct Citizen *citizen_node;//Dinxei se komvo ths arxikhs single linked list
   int self_depth;//to vathos pou vrisketai to sygkekrimeno node sthn lista, to xrhisimopoiw gia efkolia
   struct SkipListNode *next; //single linked list 
};

//Kathe ios exei mono apo 2 skiplisthead. Ena gia YES kai ena gia NO
struct  SkipListHead
{   
    char *vaccinated_or_no;// "NO" an einai gia oxi emvoliasmenous, "YES" an einai gia emvoliasmenous
    char *virusName;// Gia kathe io
    int depth;// apothikevoume to megisto "vathos" tis sygekrimenhs listas
    struct SkipListNode *next[DEPTH_MAX_LIMIT];//kathe SkipListHead exei next, pou dixnei se SkipListNode, gia kathe pithano epipedo. 
    //Ousiastika h kathe skiplist periexei apo 1 single skip list gia kathe epipedo pou exei.
    //Gia megalytero apo depth(perigrafete akrivws apo panw) einai NULL.
    

};

#endif
