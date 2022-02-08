// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include <poll.h>
#include <unistd.h>
#include "structs_travel.h"

#include "newfunctions_travelmonitor.h"
#include "Functions_travelmonitor.h"
#include "travel_main_questions.h"
#include "bloomFunctions.h"





struct Country *searchCountry(struct Country **rootC, char *str)
{
    struct Country *tmp = *rootC;
    if (tmp == NULL) return NULL;
    while (tmp !=NULL)
    {
        int compare = strcmp(tmp->countryName,str);
        if (compare == 0) return tmp;
        else if (compare <0) tmp=tmp->right;
        else tmp=tmp->left;
    }
    return tmp;
}
struct Country *newCountryNode(char *str,int numMonitor)
{
    struct Country *fresh = malloc(sizeof(struct Country));
    fresh->countryName=strdup(str);
    fresh->numMonitor=numMonitor;
    fresh->next=NULL;
    fresh->left=NULL;
    fresh->right =NULL;
    return fresh;
}
struct Country *addCountry(struct Country *node,char *str,int numMonitor)//viskoume thesh gia na prosthesoume xwra sto dentro kai me thn newCountryNode thn prosthetoume
{
    if (node == NULL) node = newCountryNode(str,numMonitor);
    else
    {
        int comp = strcmp(node->countryName,str);
        if (comp > 0)
            node->left = addCountry(node->left,str,numMonitor);
        else node->right = addCountry(node->right,str,numMonitor);
    }
    return node;
}

void makearrayCountry(struct Country *root,char *Table_of_Countries[], int* i ){//ftiaxnoume sto Table_of_Countries pinaka me tis xwres
     if (root ==NULL) return;
    //printf("%s\n",root->virusName);
     Table_of_Countries[*i]=root->countryName;
     // printf("%s\n", Table_of_Heads[*i]);
     ++*i;
     makearrayCountry(root->left, Table_of_Countries,i);
     makearrayCountry(root->right, Table_of_Countries,i);
}

struct Virus *searchVirus(struct Virus **rootV, char *str)
{
    struct Virus *tmp = *rootV;
    if (tmp == NULL) return NULL;
    while (tmp !=NULL)
    {
        int compare = strcmp(tmp->virusName,str);
        if (compare == 0) return tmp;
        else if (compare <0) tmp=tmp->right;
        else tmp=tmp->left;
    }
    return tmp;
}
struct Virus *newVirusNode(char *str,int noOfVirs)//
{
    struct Virus *fresh = malloc(sizeof(struct Virus));
    fresh->virusName=strdup(str);
    fresh->rank = noOfVirs;
    fresh->left=NULL;
    fresh->right =NULL;
    return fresh;
}
struct Virus *addVirus(struct Virus *node,char *str,int noOfVirs)//viskoume thesh gia na prosthesoume io sto dentro kai me thn newVirusNode thn prosthetoume
{

    if (node == NULL) node = newVirusNode(str,noOfVirs);
    else
    {
        int comp = strcmp(node->virusName,str);
        if (comp > 0)
            node->left = addVirus(node->left,str,noOfVirs);
        else node->right = addVirus(node->right,str,noOfVirs);
    }
    return node;
}

void innorderC(struct Country *root)
{
    if (root ==NULL) return;
    innorderC(root->left);
    printf("[Country: %s numMonitor: %d ]\n",root->countryName,root->numMonitor);
    innorderC(root->right);
}

void innorderV(struct Virus *root)//inorder emfanish bst
{
    if (root ==NULL) return;
    innorderV(root->left);
    printf("%d %s \n",root->rank,root->virusName);
    innorderV(root->right);
}

void freeBSTC (struct Country *root)//eleftherwnoume xwro pou pianei to BST
{
    if (root == NULL) return;
    freeBSTC(root->left);
    freeBSTC(root->right);
    struct LinkedList* list=NULL;
    struct LinkedList* templist=NULL;
    list=root->next;
    while(list!=NULL){
        free(list->accepted_or_no);
        free(list->date_requested);
        free(list->virusName);
        templist=list->next;
        free(list);
        list=templist;
    }
    free(root->countryName);
    free(root);
}

void freeBSTV (struct Virus *root)//eleftherwnoume xwro pou pianei to BST
{
    if (root == NULL) return;
    freeBSTV(root->left);
    freeBSTV(root->right);
    free(root->virusName);
    free(root);
}


struct uInput *checkUserInput()  //0-exit, -1 error, 1-8 valid commands
{
    struct uInput *cmd = malloc(sizeof(struct uInput));
    char line[MAX_LINE];
    char ch;
    int j=0;
    while ((ch=getchar()) !='\n' && j<MAX_LINE) //read all line
    {
        line[j]=ch;
        j++;
    }
    line[j]='\0'; //null terminate
    if (line[0] !='/')  //den ksekina me /, ara error
    {
        cmd->command=-1;
        return cmd;
    }
    char *ptr = strtok(line, " "); //diaspasi grammis
    int count=0; //metra seira orismatos. to proto i entoli, meta ta diafora
    while (ptr !=NULL) //break into tokens
    {
        strcpy(cmd->args[count],ptr); //ston pinaka ta orismata
        ptr=strtok(NULL," ");
        count++;
    }
    cmd->argnum = count; //posa orismata, mazi me tin arxiki entoli
    if (strcmp(cmd->args[0],"/exit") ==0) cmd->command =0; //tipote allo, bgainoume
    else if (strcmp(cmd->args[0],"/travelRequest") ==0)
    {
        if (count == 6) cmd->command =1;
        else cmd->command =-1;
    } //analoga me tin entoli plithos orismaton...
    else if (strcmp(cmd->args[0],"/travelStats") ==0)
    {
        if (count == 5) cmd->command =2;
        else if (count==4) cmd->command=2;
        else cmd->command=-1;
    }
    else if (strcmp(cmd->args[0],"/addVaccinationRecords") ==0)
    {
        if (count == 2) cmd->command =3;
        else cmd->command =-1;
    }
    else if (strcmp(cmd->args[0],"/searchVaccinationStatus") ==0)
    {
        if (count == 2) cmd->command =4;
        else cmd->command =-1;
    }
    //else if (strcmp(cmd->args[0],"/insertCitizenRecord") ==0)
    //{
       // if (count == 9 || count==8) cmd->command =6;
        //else cmd->command =-1;
    //}
    else cmd->command =-1;
    return cmd;
}


