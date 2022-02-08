// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include "structs.h"
//#include "mainQuestionsFunctions.h"
#include "skipListFunctions.h"
#include "StartersAndHelpFunctions.h"
#include "bloomfilter.h"
#include "bloomFunctions.h"
#include "BST.h"
//#include "monitor_main_questions.h"

int flip_a_coin(){
    
    int flip= rand()%2;
    return flip;
}

int rand_depth(){//Ypologizei to max depth gia kathe SkiplistHead
    int depth=DEPTH_MIN_LIMIT;
    while(flip_a_coin()){
        if (depth >= DEPTH_MAX_LIMIT)
            break;
        depth++;
    }
    return depth;
}
//friaxnei mia skiplist gia Eite yes eite no gia enan io
struct SkipListHead *CreateNewSkipList(struct Citizen *headC,char *vaccinated_or_no, char *virusName){
    int depth=rand_depth();
    int i;
    //printf("%d\n",depth );
    struct SkipListHead *headS=malloc(sizeof(struct SkipListHead));//kratame xwro sthn mnhmh
    struct SkipListNode *Nodepointer=NULL;
    struct SkipListNode *Nodepointer2=NULL;
    headS->depth=depth;
    headS->vaccinated_or_no=vaccinated_or_no;
    headS->virusName=virusName;
    for(i=0;i<DEPTH_MAX_LIMIT;i++){
        headS->next[i]=NULL;//arxikopoioume me null gia na mhn exoume tyxon thema (fonazei to valgrind)
    }  
        while(headC!=NULL){//pername apo olh thn citizen lista
            if((strcmp(headC->virusName->virusName,headS->virusName)==0) && (strcmp(headC->vaccinated,headS->vaccinated_or_no)==0) ){//an einai node sthn citizen lista mou mas endiaferei
                
                if(headS->next[0]==NULL){//an eisxwroume ston prwto komvo, dhladh an einai prwth fwra kai den yparxei allo skiplistnode 
                    headS->next[0]=malloc(sizeof(struct SkipListNode));//kratame xwro sthn mnhmh
                    headS->next[0]->citizen_node=headC;//apothikevoume thn diefthinsh tou node sthn citizen lista
                    headS->next[0]->self_depth=0;//vriskomate sto ypsos/vathos 0
                    headS->next[0]->next=NULL;//arxikopoioume me null gia na mhn exoume tyxon thema (fonazei to valgrind)
                }
                else{
                    Nodepointer=headS->next[0];
                    while(Nodepointer->next!=NULL)//an yparxei hdh prwtos komvos
                       Nodepointer=Nodepointer->next;// pame sto telos ths listas 
                   Nodepointer->next=malloc(sizeof(struct SkipListNode));//to idio me apo panw
                    Nodepointer->next->citizen_node=headC;
                    Nodepointer->next->self_depth=0;
                    Nodepointer->next->next=NULL;
                }
            }
            headC=headC->next;
        }

    for(i=0; i<depth;i++){// gia kathe ypsos mexri afto pou prokathorisame me thn rand depth
        
        Nodepointer=headS->next[i];//kratame ton komvo apo to twrino vathos
        while(Nodepointer!=NULL){//gia kathe komvo sthn skiplist
            
            if(flip_a_coin()){//an erthe 1, o komvos anevenei kai sto epomeno ypsos/vathos
                if(headS->next[i+1]==NULL){//ama einai kainourio ypsos, dhladh prwth eggrafh gia afto to ypsos
                    //i+1 gia to epomeno vathos/ypsos
                    headS->next[i+1]=malloc(sizeof(struct SkipListNode));
                    headS->next[i+1]->citizen_node=Nodepointer->citizen_node;
                    headS->next[i+1]->self_depth=i+1;
                    headS->next[i+1]->next=NULL;
                }
                else{
                    
                    // opws kai apo panw, an yparxei hdh prwtos komvos gia afto to ypsos ths listas
                    Nodepointer2=headS->next[i+1];
                    
                    while(Nodepointer2->next!=NULL)
                        Nodepointer2=Nodepointer2->next;//pame sto telos ths 
                    
                    Nodepointer2->next=malloc(sizeof(struct SkipListNode));
                    Nodepointer2->next->citizen_node=Nodepointer->citizen_node;
                    Nodepointer2->next->self_depth=i+1;
                    Nodepointer2->next->next=NULL;
                }
            }
            
            Nodepointer=Nodepointer->next;
        }
    }
    
    return headS;
}

//synarthshs pou ftiaxneis skiplist gia olous tous ious kai gia yes kai gia no
struct SkipListHead **CreateAllSkipLists(struct Citizen *headC,struct Virus *rootV,int noOfVirs){
    int dnoOfVirs=noOfVirs*2;
    int temp=0;
    int i;
    struct SkipListHead **Table_of_Heads;
    char** Table_of_Virus;
    Table_of_Virus=malloc(noOfVirs*sizeof(char*));

    Table_of_Heads=malloc(dnoOfVirs*sizeof(struct SkipListHead *));//mia gia yes mia gia no
    makearray(rootV,Table_of_Virus, &temp );//ftiaxnoume pianaka me tous ious
    temp=0;
   for(i=0; i<noOfVirs;i++){
    //se kathe epanalhpsh ftiaxnoume 2 SkipLists gia ton idio io, mia gia yes mia gia no
        Table_of_Heads[temp]=CreateNewSkipList(headC,"NO",Table_of_Virus[i]);
        temp++;
        Table_of_Heads[temp]=CreateNewSkipList(headC,"YES",Table_of_Virus[i]);
        temp++;
   }
   
   free(Table_of_Virus);
    return Table_of_Heads;

}
//synarthsh gia debug, ektypwnei oles tis skiplists kai anaferei kai se poio ypsos vrisketai
void print_TableOfSkipList(struct SkipListHead ** Table_of_Heads,int noOfVirs){
    int dnoOfVirs=noOfVirs*2;
    int i,j,k;
    struct SkipListHead* Headpointer=NULL;
    struct SkipListNode * Nodepointer=NULL;
    for(i=0;i<dnoOfVirs;i++){
        Headpointer=Table_of_Heads[i];
        printf("\nPrinting for Virus %s with %s on vaccinated\n",Headpointer->virusName,Headpointer->vaccinated_or_no);
        for(j=0;j<Headpointer->depth;j++){
            Nodepointer=Headpointer->next[j];
            if (Nodepointer!=NULL)
                printf("I am on the %d position in the Table_of_Heads array,\non the %d position on SkipListNode next,\nwith self_depth %d and data:  \n\n",i,j,Nodepointer->self_depth );
            while (Nodepointer!=NULL){
                printf("%s %s %s %s %d %s %s %s \n",Nodepointer->citizen_node->citizenID,Nodepointer->citizen_node->firstName,Nodepointer->citizen_node->lastName, Nodepointer->citizen_node->country->countryName,Nodepointer->citizen_node->age,Nodepointer->citizen_node->virusName->virusName,Nodepointer->citizen_node->vaccinated,Nodepointer->citizen_node->dateVaccinated);
                Nodepointer=Nodepointer->next;
                
            }
        }
    }
return;

}



//synarthsh gia vaccinatenow kai insertcitizenrecord
int insert_in_SkipList(struct SkipListHead ** Table_of_Heads,struct Citizen *headC,int noOfVirs){
    struct SkipListHead* Headpointer=NULL;
    struct SkipListNode * Nodepointer=NULL;
    struct SkipListNode * newNode=NULL;//Vazoume max giati h eggrafh mporei na yparxei kai sto megisto ypsos
    struct SkipListNode* newTempNode=NULL;
    int currentMaxHeight=0;
    int dnoOfVirs=noOfVirs*2;
    int i,j;
    int coinflip=0;
    newNode=malloc(sizeof(struct SkipListNode));
    newNode->citizen_node=headC;
    newNode->self_depth=0;
    //vriskoume thn skiplist pou mas endiaverei
    for(i=0;i<dnoOfVirs;i++){
        if(strcmp(Table_of_Heads[i]->virusName,headC->virusName->virusName)==0 && strcmp(Table_of_Heads[i]->vaccinated_or_no,headC->vaccinated)==0){
            //printf("found\n");
            Headpointer=Table_of_Heads[i];
        }
        
    }
    currentMaxHeight=Headpointer->depth;

    newNode->next=Headpointer->next[0];//vazoyme to neo kombo sthn arxh ths listas gia ypsos 0
    Headpointer->next[0]=newNode;
    //kai meta akoulouthoume thn diadikasia poy eidame kai sthn CreateNewSkipList, me thn flip a coin anevenoume epipeda. 
    for(i=0;i<currentMaxHeight-1;i++){
        Nodepointer=Headpointer->next[i];
        coinflip=flip_a_coin();
        if(coinflip){
            newTempNode=malloc(sizeof(struct SkipListNode));
            newTempNode->citizen_node=Nodepointer->citizen_node;
            newTempNode->self_depth=(Nodepointer->self_depth)+1;
            newTempNode->next=Headpointer->next[i+1];
            Headpointer->next[i+1]=newTempNode;
        }
        else
            break;
    }

    return 0;



}

//diagrafh enan komvo apo thn skip list
int delete_from_SkipList(struct SkipListHead ** Table_of_Heads,struct Citizen *headC,int noOfVirs ){
    struct SkipListHead* Headpointer=NULL;
    struct SkipListNode * Nodepointer=NULL;
    struct SkipListNode * previousNodePointer[DEPTH_MAX_LIMIT];//Vazoume max giati h eggrafh mporei na yparxei kai sto megisto ypsos
    int currentMaxHeight=0;
    int dnoOfVirs=noOfVirs*2;
    int i,j;
    
     //vriskoume thn skiplist pou mas endiaverei
    for(i=0;i<dnoOfVirs;i++){
        if(strcmp(Table_of_Heads[i]->virusName,headC->virusName->virusName)==0 && strcmp(Table_of_Heads[i]->vaccinated_or_no,headC->vaccinated)==0){
            Headpointer=Table_of_Heads[i];
        }
    }
    //printf("%s %s\n",Headpointer->virusName, Headpointer->vaccinated_or_no );
    currentMaxHeight=Headpointer->depth;
    //diagrafoume ton komvo apo kathe epipedo pou yparxei
    for(i=0;i<currentMaxHeight;i++){
        if(Headpointer->next[i]==NULL)
            break;
        Nodepointer=Headpointer->next[i];
        
        if(headC==Nodepointer->citizen_node){//ama einai sthn prwth thesh
            Headpointer->next[i]=Nodepointer->next;//vazoume to skiplisthead na deixnei sto epomeno apo afto pou diagrafoume
            free(Nodepointer);//kai eleftherwnoume xwro
        }
        else{//an den einai sthn prwth thesh
            previousNodePointer[i]=Nodepointer;
            Nodepointer=Nodepointer->next;
            while(Nodepointer!=NULL){
                if(Nodepointer->citizen_node==headC){
                   
                    previousNodePointer[i]->next=Nodepointer->next;//diaforetika vazoume to previous na dixnei sto epomeno apo afto pou diagrafoume
                    free(Nodepointer);//kai eleftherwnoume xwro
                    break;
                }
                Nodepointer=Nodepointer->next;
            }
        }
    }
    return 0;
}


//voithikh synarthsh gia thn 6h epilogh, gia thn dhmiourgeia neou bloom filter
int** updateBloom(long int bloomSize,struct Virus *rootV,int *noOfVirs,struct Citizen *head){//oti kanw kai sthn main, afou dimiourgw apo thn arxh bloom
    int **NewbArrays;
    long int bArrayLength = bloomSize / (long)sizeof(int);
    NewbArrays = (int **)malloc(sizeof (int *)*(*noOfVirs));
    
    for (int v=0; v<*noOfVirs; v++) 
        NewbArrays[v] = (int *) malloc(sizeof(int)*bArrayLength);
    
    for (int lines = 0; lines<*noOfVirs; lines++){
        //free(bArrays[lines]);
       
        for (long cols=0; cols<bArrayLength; cols++){
            NewbArrays[lines][cols]=0;
        }
    }
    
   
    NewbArrays = makeBloom(head,rootV,NewbArrays,bloomSize);


    return NewbArrays;

}

//voithikh synarthsh gia thn 6h epilogh, gia periptwseis pou xreiazetai dhmiourgeia neou komvou sthn arxikh single linked list, dhladh exoume kainouria dedomena kai aples allages
struct SkipListHead ** help_insert_c_record(struct SkipListHead ** Table_of_Heads,int *noOfVirs,int *noOfCountries,struct Citizen *head,int ***bArrays, long int bloomSize, struct Virus *rootV,struct Country *rootC,char lineData[9][50]){
    struct Citizen *headholder=head;
    char *citizenID=strdup(lineData[1]);
    char *firstName=strdup(lineData[2]);
    char *lastName=strdup(lineData[3]);
    char *Country=strdup(lineData[4]);
    char *age=strdup(lineData[5]);
    char *virusName = strdup(lineData[6]);
    char *YesOrNo=strdup(lineData[7]);
    char *date=strdup(lineData[8]);
    int VirusFlag=0;
    int VirusCountHolder=0;
    struct Virus *tempV;
    //vlepw an exw hdh ton io gia na kserw an yparxei hdh bloom filter kai skip lists gia afton h oxi.
    tempV = searchVirus(&rootV,lineData[6]); 
    if (tempV == NULL)
    {
        VirusFlag=1;
        VirusCountHolder=*noOfVirs;
    }
    
    head=addToList(head,lineData, &rootC, &rootV, noOfVirs,noOfCountries );// to prosthetw sthn arxiki lista
    *bArrays=updateBloom(bloomSize,rootV,noOfVirs,head);//friaxnw to ananewmeno bloomfilter

    
    if(VirusFlag){//an prepei na ftiaksw skip list epeidh einai neos ios to kanw, diaforetika synexizw
        if(VirusCountHolder<*noOfVirs){//deyteros elegxos gia na eimai sigouros, an h ylopoihsh mou einai swsth afto einai axreiasto
            int dnoOfVirs=*noOfVirs*2;
            int i;
            struct SkipListHead **NewTable_of_Heads;

            NewTable_of_Heads=malloc(dnoOfVirs*sizeof(struct SkipListHead *));
            for(i=0;i<dnoOfVirs-2;i++){
                NewTable_of_Heads[i]=Table_of_Heads[i];//adigrafw ta hdh yparxwn skiplists
            }
            NewTable_of_Heads[dnoOfVirs-2]=CreateNewSkipList(head,"NO",virusName);// kai ftiaxnw gia YES kai gia NO gia ton kainourio io
            NewTable_of_Heads[dnoOfVirs-1]=CreateNewSkipList(head,"YES",virusName);
            free(Table_of_Heads);
            //edw exoume kapoia mikra memory leaks apo tis strdup, den mporw na tis kanw free epeidh tis xrhsimopoiw meta kai tha exw segmentation
            //Me psaksimo ftiaxnete aplws den exw ton xrono......
            // free(citizenID);
            // free(firstName);
            // free(lastName);
            // free(Country);
            // free(age);
            // free(virusName);
            // free(YesOrNo);
            // free(date);
           
            return NewTable_of_Heads;
        }
    }else{
        while(head!=NULL){
            if(strcmp(citizenID,head->citizenID)==0 && strcmp(virusName,head->virusName->virusName)==0)//vriskw to head pou ekana insert
                break;

            head=head->next;
        }
        insert_in_SkipList(Table_of_Heads,head,*noOfVirs);//kai to eisxwrw se hdh yparxwn skip list
        // free(Table_of_Heads);
        // free(citizenID);
        // free(firstName);
        // free(lastName);
        // free(Country);
        // free(age);
        // free(virusName);
        // free(YesOrNo);
        // free(date);
        return Table_of_Heads;
    }

  

}

//6h epilogh
struct SkipListHead ** insert_new_citizen_record(struct SkipListHead ** Table_of_Heads,int *noOfVirs,int *noOfCountries,struct Citizen *head,int ***bArrays, long int bloomSize, struct Virus *rootV,struct Country *rootC, struct uInput *cmd){
    char *citizenID=cmd->args[1];
    char *firstName=cmd->args[2];
    char *lastName=cmd->args[3];
    char *Country=cmd->args[4];
    char *age=cmd->args[5];
    char *virusName = cmd->args[6];
    char *YesOrNo=cmd->args[7];
    char *date;
    int temp=0;
    struct Citizen *headholder=head;
    char lineData[9][50];
    if(strcmp(YesOrNo,"YES")==0)
        date=cmd->args[8];
    else
        date=strdup("");
    //printf("%s\n",date );
    //vlepw an yparxei id hdh, an yparxei vlepw an exei idia stoixeia. An exei diaforetika stop kai error
    while(head!=NULL){
        if(strcmp(citizenID,head->citizenID)==0){
            if((strcmp(firstName,head->firstName)) || (strcmp(lastName,head->lastName)) || (atoi(age)!=head->age) || (strcmp(Country,head->country->countryName)) ){
                printf("ERROR, ID ALREADY EXISTS WITH DIFFERENT NAME|LASTNAME|COUNTRY|AGE\n");
                if(strcmp(date,"")==0)
                    free(date);
                return Table_of_Heads;

            }
            if(strcmp(virusName,head->virusName->virusName)==0){//ama exw hdh eggrafh gia ton io
                if(strcmp("YES",head->vaccinated)==0){//kai exei hdh emvoliastei gia afton
                    printf("ERROR: CITIZEN %s ALREADY VACCINATED ON %s\n",head->citizenID,head->dateVaccinated );
                    if(strcmp(date,"")==0)
                    free(date);
                    return Table_of_Heads;
                }
                else if(strcmp("NO",head->vaccinated)==0 && strcmp("YES",YesOrNo)==0){//ama yparxei hdh me no kai prepei na to kaw yes
                    //prepei diagrapsw apo no skip list kai eisxwrhsw se nai skip list
                    temp=delete_from_SkipList(Table_of_Heads,head,*noOfVirs);
                    strcpy(head->vaccinated,"YES");
                    strcpy(head->dateVaccinated,date);
                    temp=insert_in_SkipList(Table_of_Heads,head,*noOfVirs);
                    printf("Updated an Already Existing Rerord\n");
                    //!!!!!!BLOOM_FILTER!!!!!!!!!!!!!
                    
                    for(int lines=0;lines<*noOfVirs;lines++)
                        free((*bArrays)[lines]);
                    
                   free(*bArrays);
                    *bArrays=updateBloom(bloomSize,rootV,noOfVirs,head);
                    //prepei diagrapsw apo no skip list kai eisxwrhsw se nai skip list
                    if(strcmp(date,"")==0)
                        free(date);
                    return Table_of_Heads;
                }
                else//diaforetika exw hdh NO kai mou ksanaleei NO opote den kanw tipota
                {
                    printf("Record does not need an update\n");
                    if(strcmp(date,"")==0)
                    free(date);
                    return Table_of_Heads;
                }
            }
            
           

            
        }
        head=head->next;
            
    }
   
     //EDW KANW AMA EINAI KAINOURIO ID,IOS KLP DHLADH DEN YPARXEI HDH KAI PREPEI NA DHMIOURGHSW NEO CITIZEN KLP
                //prepei na dhmiourghsw arxika neo record sthn arxikh mou lista me ton neo io
                //meta na ftiaksw neo table of skiplist afksieno kata 2 theseis, mia gia to no kai mia gia to yes
                //meta to bloom filter gia ton io.
                //ola afta ginonte me thn voithia ths help_insert_c_record
                strcpy(lineData[1],citizenID);
                strcpy(lineData[2],firstName);
                strcpy(lineData[3],lastName);
                strcpy(lineData[4],Country);
                strcpy(lineData[5],age);
                strcpy(lineData[6],virusName);
                strcpy(lineData[7],YesOrNo);
                strcpy(lineData[8],date);
                for(int lines=0;lines<*noOfVirs;lines++)
                    free((*bArrays)[lines]);
                free(*bArrays);                
                Table_of_Heads=help_insert_c_record(Table_of_Heads,noOfVirs,noOfCountries,headholder,bArrays,bloomSize,rootV,rootC,lineData);
                //print_TableOfSkipList(Table_of_Heads,*noOfVirs);
                if(strcmp(date,"")==0)
                    free(date);
                return Table_of_Heads;



}


