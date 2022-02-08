// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include "structs.h"

#include "skipListFunctions.h"
#include "StartersAndHelpFunctions.h"
#include "bloomfilter.h"
#include "bloomFunctions.h"
#include "BST.h"

//#include "monitor_main_questions.h"
//elegxoume an to # twn arguments einai swsto
int argsOK(int argc, char *argv[])
{
    if (argc != 3)
    {
        printf("Monitor:Error in # of args\n");
        return 0;
    }
    return 1;
}
//elegxoume firstname, lastname, den prepei na exoun noumera
int isAlpha(char *item)
{
    int fl=1;
    for (int i=0; i<strlen(item); i++)
        if (!isalpha(item[i]))
        {
            fl = 0;
            break;
        }
    return fl;
}
// dexetai mia grammh, sthn opoia periexonte ta dedomena emvoliasmou gia enan citizen, elegxei gia aflarithmitiko first kai last name, hlikia na einai mesa sto orio (0,120],
// to vaccination status na einai eite YES eite NO, kai na mhn exoume NO + date. An kati einai lathos epistrefei 1 mesw ths metavliths error, diaforetika gemize ton pinaka me ta dedomena 
//tou citizen

void checkLine(char *line, int *error, char lineData[9][50])
{
    int counter = 0;
    *error=0;
    for (int i=1; i<9; i++) lineData[i][0]='\0';
    if (line[strlen(line)-1]=='\n')  line[strlen(line)-1] ='\0'; //clear trailing \n read by fgets. CHECK BEHAVIOUR IN LINUX
    char *ptr = strtok(line, " "); //tokenize each line
    counter++;
    while (ptr !=NULL)
    {
        switch (counter)
        {
        case 1:
            //TBD
            strcpy(lineData[1],ptr);
            break;
        case 2:
            if (! isAlpha(ptr))
            {
                printf("---Error in firstname. ");
                *error=1;
            }
            else
                strcpy(lineData[2],ptr);
            break;
        case 3:
            if (! isAlpha(ptr))
            {
                printf("---Error in lastname. ");
                *error=1;
            }
            else
                strcpy(lineData[3],ptr);
            break;
        case 4:
            strcpy(lineData[4],ptr);
            break;
        case 5:
            if (atoi(ptr) <=0 || atoi(ptr)>120)
            {
                printf("---Error in age. ");
                *error=1;
            }
            else strcpy(lineData[5],ptr);
            break;
        case 6:
            strcpy(lineData[6],ptr);
            break;
        case 7:
            if (strcmp(ptr,"NO") !=0 && strcmp(ptr,"YES")!=0)
            {
                printf("---Error in vaccination status (Only YES/NO allowed). ");
                *error=1;
            }
            else
                strcpy(lineData[7],ptr);
            break;
        case 8:
            if (strlen(ptr)>0 && strcmp(lineData[7],"NO")==0)
            {
                printf("---Inconsistent vaccination status. ");
                *error =1;
            }
            else
                strcpy(lineData[8],ptr);
            break;
        default:
            printf("---Line contains # of items > 8. ");
            *error=1;
            break;
        }
        if (*error) break;
        ptr=strtok(NULL," ");
        counter++;
    }
    if (counter < 8)
    {
        printf("---Line contains # of items < 7. ");    //less items in a line
        *error=1;
    }
}

//adds each line at the end of the list
struct Citizen *addToList(struct Citizen *headC, char lineData[9][50],struct Country **rootC, struct Virus **rootV, int *noOfVirs,int *noOfCountries)
{
    struct Citizen *fresh;
    struct Country *tempC;
    struct Virus *tempV;
    fresh = malloc(sizeof(struct Citizen));
    fresh->citizenID=strdup(lineData[1]);
    fresh->firstName =strdup(lineData[2]);
    fresh->lastName =strdup(lineData[3]);
    tempC = searchCountry(&*rootC,lineData[4]); //search BST for country
    if (tempC == NULL){
     *rootC = addCountry(*rootC, lineData[4]); //if country not found, add it to the BST
     (*noOfCountries)++;
    }
    tempC = searchCountry(&*rootC,lineData[4]); //search will find the country and return the pointer
    fresh->country= tempC; //point to citizen's country
    fresh->age = atoi(lineData[5]);
    tempV = searchVirus(&*rootV,lineData[6]); //search BST for virus
    if (tempV == NULL)
    {
        *rootV = addVirus(*rootV, lineData[6], *noOfVirs); //if virus is not found, add it to the BST
        (*noOfVirs)++;

    }
    tempV = searchVirus(&*rootV,lineData[6]); //search will find the virus and return the pointer
    fresh->virusName = tempV; //point to virus
    fresh->vaccinated = strdup(lineData[7]);
    fresh->dateVaccinated = strdup(lineData[8]);
    fresh->next = NULL;
    //insert to citizens' list, preserving sorting.
    if (headC == NULL ) headC = fresh; //first citizen
    else if (strcmp(headC->citizenID,fresh->citizenID) >=0)  //smaller ID than the Head, insert first
    {
        fresh->next=headC;
        headC = fresh;
    }
    else //search for proper position of new citizenID
    {
        struct Citizen *curr = headC;
        while (curr->next !=NULL && strcmp(curr->next->citizenID,fresh->citizenID)<0 ) curr = curr->next; //next node
        fresh->next = curr->next;
        curr->next = fresh;
    }
    return headC;
}

struct Citizen *searchList(struct Citizen *headC, char * ID){
    struct Citizen *tmp = headC;
    while (tmp !=NULL && strcmp(tmp->citizenID, ID) !=0 ){ 
        tmp=tmp->next;
        //printf("citizenID:[%s] ID:[%s]\n",tmp->citizenID,ID );
    }
        if(tmp!=NULL){    
            if (strcmp(tmp->citizenID,ID)==0) 
                return tmp;
        }
    return NULL;
}
//checks for inconsistencies with previous entries
int errorInDB(char lineData[9][50],struct Citizen *headC)
{   

    
    if (headC == NULL)
        return 0;
    else while(headC->next !=NULL){
    if  (strcmp(headC->citizenID,lineData[1]) ==0)//an exw idio id
    {
        
        if (strcmp(headC->firstName,lineData[2]) ==0 && strcmp(headC->lastName,lineData[3])==0 && strcmp(headC->country->countryName,lineData[4])==0 && headC->age == atoi(lineData[5])){//me idio onomatepwnymo kai hlikia kai xwra
            if(strcmp(headC->virusName->virusName,lineData[6])==0)
                return 1;
            else return 0;
        }
        else
            return 1;//diaforetiko onomatepwnymo h xwra h hlikia, ara lathos
    }
    else
        headC=headC->next;
    }
    return 0;//an den exw lathos proxoraw
}



void printList(struct Citizen *head)
{
    struct Citizen *temp = head;
    while (temp !=NULL)
    {
        printf("%s %s %s %s %d %s %s %s \n",temp->citizenID,temp->firstName,temp->lastName, temp->country->countryName,temp->age,temp->virusName->virusName,temp->vaccinated,temp->dateVaccinated);
        temp=temp->next;
    }
    printf("\n\n");
}



//opens a file, reads line by line, calls checkLine, checks for inconsistencies with previous entries, if all ok creates a citizen list item and if needed a new country and/or virus name
struct Citizen *processFile(char *fname, struct Country **rootC, struct Virus **rootV, int *noOfVirs,int *noOfCountries)
{
    FILE *file;
    char line[MAX_LINE];
    char lineData[9][50];// To prwto pou xrhsimopoiw einai sth thesh 1
    int error, dataCounter=0;
    struct Citizen *headC=NULL;
    file = fopen( fname, "r");
    if (! file)
    {
        printf("file not found\n");
        return NULL;
    }
    while (fgets(line, MAX_LINE, file)!=0)
    {
        dataCounter++;
        checkLine(line,&error,lineData );
        if (error)
            printf("ERROR IN RECORD WITH ID %s in line %d\n",lineData[1], dataCounter);
        else
        {
            if (errorInDB(lineData,headC))
                printf("\nERROR - INCONSISTENT RECORD WITH ID %s in line %d\n",lineData[1], dataCounter);
            else
            {
                headC = addToList(headC,lineData, &(*rootC), &(*rootV), noOfVirs,noOfCountries ); //pass pointers addresses in order to be updated
            }
        }
    }
    fclose(file);
    return headC;
}



