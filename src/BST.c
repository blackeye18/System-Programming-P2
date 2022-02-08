// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include <unistd.h>
#include <poll.h>
#include "structs.h"
#include "skipListFunctions.h"

#include "StartersAndHelpFunctions.h"
#include "bloomfilter.h"
#include "bloomFunctions.h"
#include "BST.h"
#include "monitornewfunc.h"
#include "monitor_main_questions.h"
/*
functions to process Binary Search Trees, for Viruses and Countries
innorder traversals of BSTs for debugging and extract all viruses for bloom filters
*/
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
struct Country *newCountryNode(char *str)
{
    struct Country *fresh = malloc(sizeof(struct Country));
    fresh->countryName=strdup(str);
    fresh->left=NULL;
    fresh->right =NULL;
    return fresh;
}
struct Country *addCountry(struct Country *node,char *str)//viskoume thesh gia na prosthesoume xwra sto dentro kai me thn newCountryNode thn prosthetoume
{
    if (node == NULL) node = newCountryNode(str);
    else
    {
        int comp = strcmp(node->countryName,str);
        if (comp > 0)
            node->left = addCountry(node->left,str);
        else node->right = addCountry(node->right,str);
    }
    return node;
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

struct Virus *searchVirusRank(struct Virus *rootV, int rank_needed){
    struct Virus *temp = rootV;
    struct Virus *temp2=NULL;
    if(temp==NULL )
        return NULL;
    if(temp->rank==rank_needed )
        return temp;
    
    temp2=searchVirusRank(temp->left,rank_needed);
    if(temp2!=NULL)
        return temp2;
    temp2=searchVirusRank(temp->right,rank_needed);
    if(temp2!=NULL)
        return temp2;
    return NULL;
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
    printf("%s ",root->countryName);
    innorderC(root->right);
}
void innorderV(struct Virus *root)//inorder emfanish bst
{
    if (root ==NULL) return;
    innorderV(root->left);
    printf("%d %s \n",root->rank,root->virusName);
    innorderV(root->right);
}

void writeInOrder(struct Virus *root,int fd2,long int bufferSize)//inorder grapsimo bst pros ton parent
{
    if (root ==NULL) return;
    writeInOrder(root->left,fd2,bufferSize);
    char * virusName=root->virusName;
    write_string(fd2, virusName,bufferSize);
    //printf("%d %s \n",root->rank,root->virusName);
    writeInOrder(root->right,fd2,bufferSize);
}

void makearray(struct Virus *root,char *Table_of_Heads[], int* i ){//ftiaxnoume sto Table_of_Heads pinaka me tis xwres(Sygkekrimena Sto Table_of_Virus, xrhsimopoieitai sthn createallskiplists)
     if (root ==NULL) return;
    //printf("%s\n",root->virusName);
     Table_of_Heads[*i]=root->virusName;
     // printf("%s\n", Table_of_Heads[*i]);
     ++*i;
     makearray(root->left, Table_of_Heads,i);
     makearray(root->right, Table_of_Heads,i);
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


void freeBSTC (struct Country *root)//eleftherwnoume xwro pou pianei to BST
{
    if (root == NULL) return;
    freeBSTC(root->left);
    freeBSTC(root->right);
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
