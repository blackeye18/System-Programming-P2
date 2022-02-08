// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/wait.h> 
#include <unistd.h>
#include <fcntl.h> 
#include <poll.h>
#include <signal.h>
#include "structs_travel.h"
#include "newfunctions_travelmonitor.h"
#include "Functions_travelmonitor.h"
#include "travel_main_questions.h"
#include "bloomFunctions.h"

int sigintflag=0;
int sigquitflag=0;
int sigchldflag=0;

void handle_sig_int(int sig){
    sigintflag=1;
    //printf("caught sigintflag\n");
}

void handle_sig_quit(int sig){
    sigquitflag=1;
    //printf("caught sigquitflag\n");
}

void handle_sig_chld(int sig){
    sigchldflag=1;
    //printf("caught sigchldflag\n");
}


int main(int argc, char *argv[] )
{   
if (!argsOK(argc, argv)) return 0; //error checks on arguments
//printf("%s\n",argv[0] );
long int numMonitors=0;
numMonitors=atol(argv[2]);
long int bufferSize=atol(argv[4]);
char* input_dir=argv[8];
long int bloomSize = atol(argv[6]);
struct dirent **countriesList;
int countries_counter=0;
int scandir_return=0;
scandir_return= scandir(input_dir,&countriesList,NULL,alphasort);
//diavazoume me thn voithia ths scandir alfavitika ta arxeia pou yparxoun sto subdirectory

if(scandir_return<0){
    printf("ERROR WITH SCANDIR\n");
    return 1;
}
countries_counter=scandir_return-2;

printf("Total No Of Countries: %d\n",countries_counter);


int modulus=0;
long int directories_per_monitor=0;
directories_per_monitor= countries_counter/numMonitors;//Kanw prwth moirasia 
if ((countries_counter % numMonitors) != 0 )
    modulus=countries_counter % numMonitors;//Elegxw an den xwrane akrivws gia na kanw round robin


if (directories_per_monitor == 0){//An exw perissotera monitors apo oti xwres, tote den xreiazomai ola ta monitors
    printf("%ld monitors won't take paths! (numMonitors>countries_counter)\n",numMonitors - countries_counter );
    directories_per_monitor=1;
    numMonitors=countries_counter;
    modulus=0;
}
printf("Number Of Monitors:%ld\nBufferSize:%ld\nBloomSize:%ld\nInput Directory:%s\n",numMonitors,bufferSize,bloomSize,input_dir);
struct Country *rootC=NULL; //BST riza gia to Countries
struct Country *tempC=NULL;

//printf("MODULUS::::: [%d]\n",modulus );
int fds[numMonitors];
for(int i=0;i<numMonitors;i++)//apothikevw ta file descriptors tou kathe monitor
    fds[i]=0;
int fds2[numMonitors];
for(int i=0;i<numMonitors;i++)
    fds2[i]=0;
char** sub_directories;
int country_array_list=2;
sub_directories = (char **)malloc(sizeof (char *)*(numMonitors));
//megalyterh xwra poy exw einai h "UnitedArabEmirates", 19 xarakthres dhladh +1 gia to keno sto telos 20.
//+11 gia to "input_dir/". Ara theloume countries_counter*31
for(int i=0; i<numMonitors;i++){
    sub_directories[i]=(char*)malloc((countries_counter*31)*sizeof(char));
    strcpy(sub_directories[i],"");
}
//ftiaxnw to string gia to kathe monitor kai apothikevw tis xwres sto dentro
for (int i=0; i<directories_per_monitor;i++){
    for(int j=0; j<numMonitors;j++){
        tempC=searchCountry(&rootC,countriesList[country_array_list]->d_name);
        if(tempC==NULL){
            rootC=addCountry(rootC,countriesList[country_array_list]->d_name,j);
        }
        //strcat(sub_directories[j],"/");
        strcat(sub_directories[j],input_dir);
        strcat(sub_directories[j],"/");
        strcat(sub_directories[j],countriesList[country_array_list]->d_name);
        strcat(sub_directories[j]," ");
        country_array_list++;

    }   
}
//ama exw perisevma to prosthetw sto telos. Dhladh an exw 4 monitors kai 5 xwres, apo panw pairnei 1 xwra to kathe monitor kai apo katw dinoume thn 5h xwra sto 1o monitor. Round robin
if(modulus!=0){
    for(int i=0;i<modulus;i++){
        //strcat(sub_directories[i],"/");
        tempC=searchCountry(&rootC,countriesList[country_array_list]->d_name);
        if(tempC==NULL){
            rootC=addCountry(rootC,countriesList[country_array_list]->d_name,i);
        }
        strcat(sub_directories[i],input_dir);
        strcat(sub_directories[i],"/");
        strcat(sub_directories[i],countriesList[country_array_list]->d_name);
        strcat(sub_directories[i]," ");
        country_array_list++;
    }
}

//innorderC(rootC);

for(int i=0; i<numMonitors;i++){
    int length=strlen(sub_directories[i]);
    sub_directories[i][length]='\0';//vazw \0 sto telos kathe string
}

Create_All_Fifos(numMonitors);//dhmiourgw ta fifos

char* path="./monitor";
char FIFO_1[100]="Input";
char FIFO_2[100]="Output";//dhmiourgw ta strings gia ta orismata sthn execl
int *id2;
id2=malloc(sizeof(int)*numMonitors);//apothikevw ta proccess id gia ta signals
int  id[numMonitors];
for(int i=0;i<numMonitors;i++)
    id[numMonitors]=0;
id[0]=fork();//kanw to prwto fork
if (id[0]==0){//an eimaste sto paidi
    strcat(FIFO_1,"0");//ftiaxnw ta orismata
    strcat(FIFO_2,"0");
    execl(path,path,FIFO_1,FIFO_2,NULL);// kai ektelw thn monitor
    strcpy(FIFO_1,"Input");//arxikopoiw ta orismata
    strcpy(FIFO_2,"Output");
}
//id[0]=getpid();
//printf("%d \n",id[0] );
//twra eimai ston patera
    for(int i=1; i<numMonitors;i++){//gia kathe allo monitor pou prepei na dhmiourghsw
        char text[100];
        sprintf(text,"%d",i);
        strcat(FIFO_1,text);
        strcat(FIFO_2,text);
        id[i]=fork();// to idio me panw, ftiaxnw ta orismata, kanw fork kai an eimai sto paidi ektelw to monitor
        if(id[i]==0)
            execl(path,path,FIFO_1,FIFO_2,NULL);
        //id[i]=getpid();
       strcpy(FIFO_1,"Input");
       strcpy(FIFO_2,"Output");
    }
//printf("%d \n",id[0] );
for(int i=0;i<numMonitors;i++)//kratw to process id
    id2[i]=id[i];
pid_t temp;
int temp2=0;

int fd=0;
for(int i=0;i<numMonitors;i++){// gia kathe monitor
    char text[100]="";
    sprintf(text,"%d",i);
    strcat(FIFO_1,text);
    strcat(FIFO_2,text);
    fd=open(FIFO_1,O_WRONLY);//anoigw to pipe 
    if(fd<0){
        printf("Error: TravelMonitor Cannot Open FIFO_1\n");
        exit(1);
    }
    fds[i]=fd;//apothikevw to file descriptor
    write(fd,&bloomSize,sizeof(long int));//grafw to bloom kai buffer size
    write(fd,&bufferSize,sizeof(long int));
    int size=0;
    size=strlen(sub_directories[i]);
    write_string(fd,sub_directories[i],size);//grafw me thn voithia ths synarthshs write string ta subdirectories pou tha analavei to monitor
    strcpy(FIFO_1,"Input");//arxikopoiw ta orismata 
    strcpy(FIFO_2,"Output");

}

signal(SIGINT,handle_sig_int);//signal handlers
signal(SIGQUIT,handle_sig_quit);
signal(SIGCHLD,handle_sig_chld);

//Sygkledronw olous tous ious pou yparxoun.
//Diavazw apo kathe monitor to plhthos twn iwn, meta diavazw to onoma tou kathe iou kai an den to exw apothikefsei hdh sto dentro me tous ious to apothikevw
struct Virus *rootV=NULL;
int noOfVirs = 0;
int fd2=0;
int flag=0;
struct pollfd fdarray[1];
for(int i=0;i<numMonitors;i++){
    char text[100]="";
    sprintf(text,"%d",i);
    strcat(FIFO_1,text);
    strcat(FIFO_2,text);
    //printf("%s\n",FIFO_2 );
    fd2=open(FIFO_2,O_RDONLY);
    if(fd2<0){
        printf("Error: TravelMonitor Cannot Open FIFO_2\n");
        exit(1);
    }
    fds2[i]=fd2;
    //printf("fd2=[%d] fds2[%d]=[%d] \n",fd2,i,fds2[i] );
    fdarray[0].fd=fd2;
    fdarray[0].events=POLLIN;
    int viruses_in_monitor=0;
    viruses_in_monitor=read_int(fd2, fdarray);
    //printf("viruses_in_monitor:%d\n",viruses_in_monitor );
    //printf("viruses_in_monitor:%d\n",viruses_in_monitor );
    for(int k=0; k<viruses_in_monitor;k++){
        struct Virus *tempV=NULL;
        int size=0;
        size=read_int(fd2,fdarray);
        char virus_received[size];
        strcpy(virus_received,"");
        int rc=0;
        if(size<=bufferSize){
            rc=poll(fdarray,1,300);
            if(rc==1){
                read(fd2,virus_received,size);
                virus_received[size]='\0';}
            else{
                printf("Error! Poll timeout!\n");
                exit(1);
            }
        }else{
            //strcpy(virus_received,"");
            int division=0,modulus=0;
            char constring[size];
            strcpy(constring,"");
            division=size/bufferSize;
            modulus=size%bufferSize;
            int rc=0;
            //printf("division: %d modulus: %d\n", division,modulus);
            for(int j=0;j<division;j++){
                rc=poll(fdarray,1,300);
                if(rc==1)
                    read(fd2,constring,bufferSize);
                else{
                    printf("Error! Poll timeout!\n");
                    exit(1);
                }
                constring[bufferSize]='\0';
                strcat(virus_received,constring);
                strcpy(constring,"");

            }
            if(modulus>0){
                int rc=poll(fdarray,1,300);
                if(rc==1)
                    read(fd2,constring,modulus);
                else{
                    printf("Error! Poll timeout!\n");
                    exit(1);
                }
                //printf("%s\n",constring );
                strcat(virus_received,constring);
                strcpy(constring,"");
            }
            virus_received[size]='\0';
        }
        //printf("ektypwnw\n");
        //printf("virus_received: %s\n",virus_received );
        tempV = searchVirus(&rootV,virus_received); //search BST for virus
        if (tempV == NULL)
        {
        if (flag==0){
            rootV = addVirus(rootV, virus_received, noOfVirs); //if virus is not found, add it to the BST
            flag=1;
        }
        else
            tempV= addVirus(rootV, virus_received, noOfVirs); //if virus is not found, add it to the BST
        noOfVirs++;

        }
    
    }
    strcpy(FIFO_1,"Input");
    strcpy(FIFO_2,"Output");

}

//printf("fd:[%d] fds2[0]:[%d]\n",fd2,fds2[0] );
//printf("noOfVirs %d\n",noOfVirs );
//innorderV(rootV);

//dhmiourgw kai arxikopoiow me 0 apo 1 bloom filter gia kathe monitor gia kathe io poy yparxei
int ***bArrays;

long int bArrayLength = bloomSize / (long)sizeof(int);
printf("\nParent:There are %ld monitors that have at max %d viruses (rows), each with a bloom 1-d array of size %ld (columns)\n",numMonitors,noOfVirs,bArrayLength);
bArrays=(int***)malloc(numMonitors*sizeof(int**)) ;
for(int i=0; i<numMonitors;i++) {  
    bArrays[i] = (int **)malloc(sizeof (int *)*(noOfVirs));
    for (int v=0; v<noOfVirs; v++) 
        bArrays[i][v] = (int *) malloc(sizeof(int)*bArrayLength);
        for (int lines = 0; lines<noOfVirs; lines++)
            for (long cols=0; cols<bArrayLength; cols++)
                bArrays[i][lines][cols]=0;
}
//innorderV(rootV);
//diavazw ta bloom filter apo kathe monitor
printf("Parent: Now getting BloomFilters from all childs\n");
for(int i=0;i<numMonitors;i++){
    fdarray[0].fd=fds2[i];
    fdarray[0].events=POLLIN;
    printf("Parent: Now copying from child %d\n",i );
get_bloom_from_monitors(rootV,bArrays,bloomSize,noOfVirs,fds2[i],bufferSize,fdarray,i);   
}

//stelnw shma se ola ta paidia gia na ftiaksoun log files
for(int i=0;i<numMonitors;i++){
        
    kill(id2[i],SIGINT);
    kill(id2[i],SIGQUIT);
         
    }


//main loop
struct uInput *command;
int result;
 printf("\nyour wish is my command, bro!>");
    command = checkUserInput();
    while (command->command !=0)
    {   
        
        switch (command->command)
        {
        case 1://travelRequest
            result = travel_request(command ,bArrays, rootV,rootC,bloomSize,fds,fds2,bufferSize);
            break;
        case 2:
            //printf("2nd option\n");
            result = travelStats(command,rootC,countries_counter);
            break;//travelStats 
        case 3:
            printf("3rd option\n");

            result = addVaccinationRecords(command,fds,fds2,numMonitors,rootC,bufferSize,noOfVirs,bloomSize,bArrays,rootV,id2);
            //addVaccinationRecords
            break; 
        case 4:
            result = searchVaccinationStatus(command,fds,fds2,bufferSize,numMonitors);
            break; //searchVaccinationStatus
        default:
            printf("Error in command, try again\n");
            break;
        }
	//ama dexthkes shma enw ektelouses synarthsh termatise apo afou teleiwsei h synarthsh
        if(sigintflag==1 || sigquitflag==1)
            break;
	else
	    free(command);
        printf("your wish is my command, bro!>");
        command = checkUserInput();
        

    }
    int ret1=1;
    create_logFile(rootC,countries_counter);//ftiakse logfiles
    for(int i=0;i<numMonitors;i++){
        write_int(fds[i],0);//leei sta paidia na vgoun apo thn loupa tous
    }
    
    for(int i=0;i<numMonitors;i++){
        ret1=read_int_notimeout(fds2[i]);//Ousiastika dexetai mnm oti ta paidia einai etoima na pethanoun
    }
        

    for(int i=0;i<numMonitors;i++){//sigkill se olata paidia
        kill(id2[i],SIGKILL);     
    }
    
while((temp=wait(&temp2))>0);//perimenei na pethanoun ola

//printf("sigchldflag:%d\n",sigchldflag);
printf("Parent:Exiting....freeing memory...\n");  
close_all_fifos(numMonitors,fds,fds2);
free(command);  
free(id2); 
//sleep(100);
for(int i=0;i<numMonitors;i++){
    for (int v=0; v<noOfVirs; v++){ 
            free(bArrays[i][v]);
        }
    free(bArrays[i]);
}
free(bArrays);
freeBSTV(rootV);
freeBSTC(rootC);
for(int i=0; i<numMonitors;i++)
    free(sub_directories[i]);
free(sub_directories);
for(int i=0;i<scandir_return;i++)
    free(countriesList[i]);
free(countriesList);
Delete_All_Fifos(numMonitors);
return 0;
}

