// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include <poll.h>
#include "structs.h"
//#include "mainQuestionsFunctions.h"
#include "skipListFunctions.h"
#include "StartersAndHelpFunctions.h"
#include "bloomfilter.h"
#include "bloomFunctions.h"
#include "BST.h"
#include "monitornewfunc.h"
#include "monitor_main_questions.h"


int *changeBit(int *A,long k)
{
    int bitsPerInt=8 * sizeof(int); //8 bits per byte, sizeofint may be different for a system, normally bitsPerInt = 32
    /* example: for a 32 bit int, if bloomsize is 32bytes, this means 32*8 = 256 bits
    We need 8 ints for the 256 bytes.
    If we want to change bit no 78, then 78/32 = 2 and 78%32 = 14, this means we go to the 3rd int, A[2] and set bit no 14 to 1
    */
    long i = k/bitsPerInt; //find proper int W in the Array
    long pos = k%bitsPerInt; //find which bit to change in the int W
    unsigned long mask = 1; //000...0001
    mask = mask << pos; //shift 1 to it's proper position
    A[i] = A[i] | mask; //change the int W. set bit to 1
    return A;
}
int checkBit(int *A, long k){
     int bitsPerInt=8 * sizeof(int);
     long i = k/bitsPerInt;
    long pos = k%bitsPerInt;
    unsigned long mask = 1;
    mask = mask << pos;
    if (A[i] & mask) return 1; else return 0;
}
//each bloom filter per virus contains info about vaccinated citizens, so we can find these that are NOT
int **makeBloom(struct Citizen *headCitizen,struct Virus *rootV,int **bArrays, long bloomSize)
{
    struct Citizen *curr=headCitizen;

    while ( curr !=NULL)
    {
        if (strcmp(curr->vaccinated,"YES")==0)   //exei kanei emvolio
        {
            int arrayIndex = curr->virusName->rank; //kathe ios exei ena arithmitiko id->index grammis ston pinaka bloom
            for (unsigned int idx = 0; idx < NO_OF_HASH_FUNCTIONS; idx++) //16 functions
            {
                unsigned long bit = hash_i((unsigned char *) curr->citizenID, idx) % bloomSize; //poio bit tha alaksei
                bArrays[arrayIndex] = changeBit(bArrays[arrayIndex],bit); //allagi tou bit stin antistoixi grammi tou bloom
            }
        }
        curr=curr->next;
    }
    return bArrays;
}

int doFirstOption(struct uInput *cmd , int **bArrays, struct Virus *rootV, struct Citizen *head, long int bloomSize){
    char *ID = cmd->args[1];
    char *vName = cmd->args[2];
    printf("ID%s vName%s\n",ID,vName );
    struct Virus *tmp = searchVirus(&rootV,vName);
    if (tmp==NULL) {printf("error virus\n");return -1;}
    struct Citizen *tmp1 = searchList(head,ID);
    if (tmp1 ==NULL){ printf("error id\n");return -1; }
    for (unsigned int idx = 0; idx < NO_OF_HASH_FUNCTIONS; idx++) //16 functions
            {
                unsigned long bit = hash_i((unsigned char *) tmp1->citizenID, idx) % bloomSize;
                int chk = checkBit(bArrays[tmp->rank],bit);
                if (chk ==0) return 0;
            }
 return 1;
}
//oi prohgoymenes synarthseis einai apo thn prwth ergasia
void send_bloom_filter(struct Virus *rootV,int **bArrays, long bloomSize,int noOfVirs,int fd, long int bufferSize){//stelnei to bloom filter ston patera
    struct Virus *temp=NULL;
    struct Virus *temp2=rootV;
    //printf("Child: bloomSize%ld bufferSize%ld\n",bloomSize,bufferSize );
    long int bArrayLength = bloomSize / (long)sizeof(int);
    write_int(fd,noOfVirs);//enhmwrwnei gia to megethos tou patera
   // printf("Monitor: noOfVirs:%d\n",noOfVirs );
    for(int i=0;i<noOfVirs;i++){
       // printf("child i:%d\n",i );
        temp=searchVirusRank(temp2,i);//vriskei to io pou adistixei sto rank i
        if (temp==NULL)
            printf("child: ERROR TEMP=NULL for rank:%d\n",i);
        //printf("Child rank:%d tempvirusname:%s\n",i,temp->virusName );
        write_string(fd,temp->virusName,bufferSize);//grafei to onoma tou iou, gia na kserei o pateras gia poion io milame, afou mporei na exei allo rank sto travel monitor apo oti exei sto monitor
        for(int j=0;j<bArrayLength;j++){
            //printf("Child: Int sent: %d\n",bArrays[i][j] );
            write_int(fd,bArrays[i][j]);//grafei to bloomfilter
            //write_int(fd,2);
        }
        temp=NULL;
        temp2=rootV;
    }
}



