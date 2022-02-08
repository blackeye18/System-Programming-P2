// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h> 
#include <unistd.h>
#include <poll.h>
#include <dirent.h>
#include <limits.h>
#include <ctype.h>
#include<time.h>
#include<signal.h>
#include "structs.h"
#include "skipListFunctions.h"
#include "StartersAndHelpFunctions.h"
#include "bloomfilter.h"
#include "bloomFunctions.h"
#include "BST.h"
#include "monitornewfunc.h"
#include "monitor_main_questions.h"

//flags gia ta signals kai oi antistoixes synarthseis
int sigintflag=0;
int sigquitflag=0;
int sigusr1flag=0;

void handle_sig_int(int sig){
	sigintflag=1;
	//printf("caught sigintflag\n");
}

void handle_sig_quit(int sig){
	sigquitflag=1;
	//printf("caught sigquitflag\n");
}

void handle_sig_usr1(int sig){
	sigusr1flag=1;
	//printf("caught sigusr1flag\n");
}




int main(int argc, char *argv[] ){
	char starting_directory[PATH_MAX];
	if (getcwd(starting_directory, sizeof(starting_directory)) != NULL) {//vriskoume to twrino directory poy eimaste kata thn ektelesh, to xreiazomaste na gia na mporoume na epistrepsoume se afto argotera
	    ;
	}else{
	    printf("Error with getcwd!");
	    return 1;
	}
	//printf("%s\n",argv[0] );
	if (!argsOK(argc, argv)) return 0;//elegxos tou plhthous twn arguments
	char FIFO_1[100];
	strcpy(FIFO_1,argv[1]);//apothikevw ta onomata twn named pipe pou pairnei h efarmogh san orismata
	char FIFO_2[100];
	strcpy(FIFO_2,argv[2]);
	long int bloomSize;
	long int bufferSize;
	struct pollfd fdarray[1];

	signal(SIGINT,handle_sig_int);//dhlwnw tis synarthseis poy kaloude otan dexetai h aformogh to antistoixo shma
	signal(SIGQUIT,handle_sig_quit);
	signal(SIGUSR1,handle_sig_usr1);

	//printf("FIFO_1: %s FIFO_2: %s",FIFO_1,FIFO_2);
	int fd=open(FIFO_1,O_RDONLY);//anoigma twn named pipes
	if(fd<0){
	        printf("Error: Monitor Cannot Open FIFO_1\n");
	        exit(1);
	    }
	fdarray[0].fd=fd;//Gia thn poll gia na diabazoume mono afou  exei ginei write
	fdarray[0].events=POLLIN;
	int rc=poll(fdarray,1,300);
	if(rc==1)
		read(fd,&bloomSize,sizeof(long int));//diavazoume to bloomsize
	else{
		printf("Error! Poll timeout!\n");
		exit(1);
	}
	//printf("bloomSize: %ld\n",bloomSize );
	rc=poll(fdarray,1,300);
	if(rc==1)
		read(fd,&bufferSize,sizeof(long int));//diavazoume to buffersize
	else{
		printf("Error! Poll timeout!\n");
		exit(1);
	}
	//printf("bufferSize: %ld\n",bufferSize );

	//theloume na diavasoume ena string pou periexei ta subdirectories pou tha diaxirizetai to monitor
	//omws theloume na diavazoume/grafoume to poly buffersize thn fora. Opote prwta diavazoume to megethos tou string pou theloume na diabasoume
	int size=read_int(fd,fdarray);
	char sub_directories[size];

	//printf("number=%d\n",number );
	//an to string einai mikroterou megetthous tou buffersize, aplws diavazoume to string 
	if(size<=bufferSize){
		rc=poll(fdarray,1,300);
		if(rc==1){
			read(fd,sub_directories,size);
			sub_directories[size]='\0';}
		else{
			printf("Error! Poll timeout!\n");
			exit(1);
		}
	}
	else{//diaforetika vriskoume poses fores prepei na diavasoume me megethos buffersize, kai an xreiazetai sto telos na diavasoume kai <buffersize.(division kai modulus)
		//meta apo kathe diavasma kanoume concat to string gia na vgei enomeno
		strcpy(sub_directories,"");
		int division=0,modulus=0;
		char constring[size];
		division=size/bufferSize;
		modulus=size%bufferSize;
		for(int j=0;j<division;j++){
			rc=poll(fdarray,1,300);
			if(rc==1)
				read(fd,constring,bufferSize);
			else{
				printf("Error! Poll timeout!\n");
				exit(1);
			}
			constring[bufferSize]='\0';
			strcat(sub_directories,constring);
			strcpy(constring,"");
		}
		if(modulus>0){
			rc=poll(fdarray,1,300);
			if(rc==1)
				read(fd,constring,modulus);
			else{
				printf("Error! Poll timeout!\n");
				exit(1);
			}
			strcat(sub_directories,constring);
			strcpy(constring,"");
		}
		sub_directories[size]='\0';
	}

	//printf("Child: sub_directories:[%s]\n",sub_directories );


	//kathe subdirectory sto string xwrizetai apo to allo me keno
	char * temp_subdirectories;
	temp_subdirectories=strdup(sub_directories);
	int counter=0;

	char *token, *tok;
	token=strtok(temp_subdirectories," ");
	//ypologizw to plhthos twn subdirectories xrhsimopoiwdas thn strtok
	while(token!=NULL){
		counter++;
		token=strtok(NULL," ");
	}
	//printf("counter[%d]\n",counter );
	char **split_directories;
	//desmevw ton antistoixo xwro kai ta apothikevw ksexwrista ston pinaka
	split_directories=(char**)malloc(counter * sizeof(char*));
	int counter2=0;
	tok=strtok(sub_directories," ");
	while(tok!=NULL){
		split_directories[counter2]=strdup(tok);
		//printf("Token:[%s]",tok);
		counter2++;
		tok=strtok(NULL," ");
	}

	srand(time(NULL));
	struct Citizen *headCitizen=NULL; //single linked list gia tous Citizens
	struct Country *rootC=NULL; //BST riza gia to Countries
	struct Virus *rootV=NULL; //BST riza gia to Viruses
	int noOfVirs = 0, noOfCountries = 0; //plithos viruses and countries
	int **bArrays;
	
	int result;
	int flag=0;
	struct Virus *tempV=NULL;
	int scandir_return=0;
	struct dirent **FilesList;
	int *files_counter_array;
	files_counter_array=malloc(counter*sizeof(int));
	struct LinkedList * filenamelist=NULL;
	
	for(int i=0;i<counter;i++)
		files_counter_array[i]=0;
	for(int i=0;i<counter;i++){
		int ret=chdir(split_directories[i]);//mpainoume sto subdirectory
		//printf("split_directories[%d]:[%s]\n",i,split_directories[i] );
		if(ret==-1){
			printf("ERROR WITH CHDIR: split_directories[%d]:[%s]\n",i,split_directories[i] );
		}
		//diavazoume me thn voithia ths scandir alfavitika ta arxeia pou yparxoun sto subdirectory
		scandir_return= scandir(".",&FilesList,NULL,alphasort);
		int files_counter=0;
		files_counter=scandir_return-2;
		files_counter_array[i]=files_counter;
		
		//for (int i=2; i<scandir_return;i++){
	    //printf("%s\n",FilesList[i]->d_name );
		//}

		//printf("files_counter= %d\n",files_counter);
		//gia kathe txt pou diavasame se kathe subdirectory
		for(int j=2;j<scandir_return;j++){
			//name_of_files_per_folder[i][j-2]=strdup(FilesList[j]->d_name);
			//to apothikevoume sthn lista
			if (filenamelist==NULL){
				filenamelist=malloc(sizeof(struct LinkedList));
				filenamelist->folder=i;
				filenamelist->txtname=strdup(FilesList[j]->d_name);
				filenamelist->next=NULL;
			}
			else{
				struct LinkedList * filenamelistPointer=filenamelist;
				while(filenamelistPointer->next!=NULL)
					filenamelistPointer=filenamelistPointer->next;
				filenamelistPointer->next=malloc(sizeof(struct LinkedList));
				filenamelistPointer->next->folder=i;
				filenamelistPointer->next->txtname=strdup(FilesList[j]->d_name);
				filenamelistPointer->next->next=NULL;
			}
			//printf("reading file contents\n");
			//kai me tis synarthseis apo thn prwth ergasia to anoigoume to diavazoume kai dhmiourgoume thn arxikh lista headcitizen, gemizontas taytxrona kai ta antistoixa dentra gia country virus
			if(headCitizen==NULL){//ama einai to prwto txt poy diavazoume
	    		headCitizen = processFile(FilesList[j]->d_name,&rootC, &rootV, &noOfVirs,&noOfCountries); //process citizens input file
				tempV=rootV;
			}
	    	else{//an den eina to prwto txt pou diavazoume
	    		struct Citizen *headCitizenPointer=headCitizen;
	    		while(headCitizenPointer->next!=NULL)
	    			headCitizenPointer=headCitizenPointer->next;
	    		headCitizenPointer->next = processFile(FilesList[j]->d_name,&rootC, &rootV, &noOfVirs,&noOfCountries); //process citizens input file
	    	}
	    	if (headCitizen == NULL)
	    	{
	        	printf("No records loaded. Exiting...\n");
	        	return 0;
	    	}
			
		}
		ret=chdir(starting_directory);// epistrofh sto arxiko directory
		for(int i=0;i<scandir_return;i++)
    		free(FilesList[i]);//eleftherwnoume ton xwro pou desmefse h scandir
    	free(FilesList);
	}
	rootV=tempV;
	
	//display citizen list, # of viruses for debug
	//printf("\nList Contains (sorted by ID ascending):\n");
	//printList(headCitizen);
	//printf("\nCountries\n");
	//innorderC(rootC);
	//printf("\n# of viruses: %d\n",noOfVirs);

	// traverse BSTs for debug
	 // innorderV(rootV);

	//Bloom filter apo prwth ergasia:
	//bloom: Xrhsimopoiw int. Kathe int exei sizeof(int) bytes. Ara an to bloomsize einai stai 100.000 bytes kai sizeof(int)=4, xreiazomaste pinaka me 25.000 akeraious
	//long int bloomSize = atol(argv[4]); //parameter
	long int bArrayLength = bloomSize / (long)sizeof(int);
	//printf("\nChild:There are %d viruses (rows), each with a bloom 1-d array of size %ld (columns)\n",noOfVirs,bArrayLength);
	bArrays = (int **)malloc(sizeof (int *)*(noOfVirs));
	for (int v=0; v<noOfVirs; v++) bArrays[v] = (int *) malloc(sizeof(int)*bArrayLength);
	for (int lines = 0; lines<noOfVirs; lines++)
	    for (long cols=0; cols<bArrayLength; cols++)
	        bArrays[lines][cols]=0;

	bArrays = makeBloom(headCitizen,rootV,bArrays,bloomSize);// dimiourgeia toy bloomfilter
	//Skip list apo prwth ergasia
	struct SkipListHead **Table_of_Heads;
	Table_of_Heads=CreateAllSkipLists(headCitizen,rootV,noOfVirs);//dimiourgeia twn SkipList
	//print_TableOfSkipList(Table_of_Heads,noOfVirs); // emfanizei ola ta skipList, xrhsimopoeithike gia debug
	
	int fd2=open(FIFO_2,O_WRONLY);
	if(fd2<0){
	        printf("Error: Monitor Cannot Open FIFO_2\n");
	        exit(1);
	    }

	//printf("Child noOfVirs before writing:%d\n",noOfVirs );
	write_int(fd2,noOfVirs);//grafoume to plithos twn iwn pou exei to monitor

	writeInOrder(rootV,fd2,bufferSize);//kai epeita metaferoume to onoma tou kathe iou
	//innorderV(rootV);
	
	
	
	//synarthsh steile to bloomfilter sou
	send_bloom_filter(rootV,bArrays,bloomSize,noOfVirs,fd2, bufferSize);//stelnoume to bloom filter tou monitor ston patera
	int log_total=0,log_accepted=0,log_rejected=0;//gia to log file
	int command;
	command=read_int_notimeout(fd);//diavazei akeraio xwris na kanei timeout
	result=0;
	while (command!=0)
    {
        switch (command)
        {
        case 1://travelRequest
            //printf("Child:1st option\n");
            result = travel_request(Table_of_Heads,rootV,rootC,fd,fd2,fdarray,bufferSize,noOfVirs,&log_total,&log_accepted,&log_rejected);
            break;
        case 2:
            //printf("child: 2nd option\n");
            result = searchVaccinationStatus(Table_of_Heads,noOfVirs,fd,fd2,fdarray,bufferSize,headCitizen);
            break;//searchVaccinationStatus
        case 3:
            //printf("Child:3rd option\n");
        	if(sigusr1flag==1){
            	Table_of_Heads = addVaccinationRecords(starting_directory,split_directories,counter,&files_counter_array,filenamelist,rootC, rootV,&noOfVirs,&noOfCountries,Table_of_Heads,headCitizen,&bArrays,bloomSize,bufferSize,fd2);
        		sigusr1flag=0;
        	}else
        		printf("Error! Signal SIGUSR1 was not sent\n");
            break; //addVaccinationRecords
        default:
            printf("Error in command, try again\n");
            break;
        }
        command=read_int_notimeout(fd);
        
    }
    
    if(sigintflag==1 || sigquitflag==1){//ama exei dextei ena apo ta 2, h kai ta 2, signals tote ftiaxnei to logfile
    	create_logfile(rootC,noOfCountries,log_total,log_accepted,log_rejected);
    	write_int(fd2,0);//enhmerwnei oti einai etoimo to monitor na pethanei
    	sleep(10);// perimenei gia na faei sigkill diaforetika aplws kanei free thn mnhmh kai termatizei
    }
    else{
    	write_int(fd2,0);//enhmerwnei oti einai etoimo to monitor na pethanei
    	sleep(10);// perimenei gia na faei sigkill diaforetika aplws kanei free thn mnhmh kai termatizei
    }
    
   

   //se periptwsh poy perasei o xronos kai den exei faei sigkill to programma eleftherwnei thn mnhmh kai termatizei
    
	printf("Child:Exiting....freeing memory...\n");



	close(fd);
	close(fd2);
	freeBSTC (rootC);
    freeBSTV(rootV);
    for (int v=0; v<noOfVirs; v++) 
        free(bArrays[v]);
    free(bArrays);
    struct SkipListHead *Headpointer=NULL;
    struct SkipListNode *Nodepointer=NULL;
    struct SkipListNode *NodepointerHolder=NULL;
    int dnoOfVirs=noOfVirs*2;
    int maxdepth=0;
    for(int i=0;i<dnoOfVirs;i++){
        if(Table_of_Heads[i]!=NULL){
            Headpointer=Table_of_Heads[i];
            maxdepth=Headpointer->depth;
            for(int j=0;j<DEPTH_MAX_LIMIT;j++){
                if(Headpointer->next[j]!=NULL){    
                    Nodepointer=Headpointer->next[j];
                    while(Nodepointer!=NULL){
                        NodepointerHolder=Nodepointer->next;
                        free(Nodepointer);
                        Nodepointer=NodepointerHolder;
                    }
                }
            }
            free(Headpointer);
        }
    }
    free(Table_of_Heads);
    free(NodepointerHolder);

    struct Citizen *headCitizenHold=NULL;
    //free Citizen single linked list
    while(headCitizen!=NULL)
    {
        free(headCitizen->citizenID);
        free(headCitizen->firstName);
        free(headCitizen->lastName);
        free(headCitizen->vaccinated);
        free(headCitizen->dateVaccinated);
        headCitizenHold=headCitizen;
        headCitizen=headCitizen->next;
        free(headCitizenHold);
    }
    free(temp_subdirectories);
    for(int i=0;i<counter;i++){
		free(split_directories[i]) ;
	}
	free (split_directories);
	
	free(files_counter_array);

	struct LinkedList *linkedlisthold=NULL;
	while(filenamelist!=NULL){
		free(filenamelist->txtname);
		linkedlisthold=filenamelist;
		filenamelist=filenamelist->next;
		free(linkedlisthold);
	}

	return 0;
	}