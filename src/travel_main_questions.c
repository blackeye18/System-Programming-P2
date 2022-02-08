// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include<time.h>
#include <poll.h>
#include <unistd.h>
#include <signal.h>
#include "structs_travel.h"
#include "newfunctions_travelmonitor.h"
#include "Functions_travelmonitor.h"
#include "travel_main_questions.h"
#include "bloomFunctions.h"

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

int doFirstOption(struct uInput *cmd , int ***bArrays, struct Virus *rootV, long int bloomSize,int monitor_for_country){
    char *ID = cmd->args[1];
    char *vName = cmd->args[5];
    //printf("ID%s vName%s\n",ID,vName );
    struct Virus *tmp = searchVirus(&rootV,vName);
    if (tmp==NULL) {return -1;}
    //struct Citizen *tmp1 = searchList(head,ID);
    //if (tmp1 ==NULL){ printf("error id\n");return -1; }
    for (unsigned int idx = 0; idx < NO_OF_HASH_FUNCTIONS; idx++) //16 functions
            {
                unsigned long bit = hash_i((unsigned char *) ID, idx) % bloomSize;
                int chk = checkBit(bArrays[monitor_for_country][tmp->rank],bit);
                if (chk ==0) return 0;
            }
 return 1;
}
//tropoihmenh synarthsh apo 1h ergasia
//Elegxei an to date0 einai anamesa sto date1 kai date2 
int DateInRange_6_month(char* sdate0,char* sdate1,char* sdate2){//to date0 einai afto poy theloyme na doyme an einai endimesa sto date1 kai sto date2
    char * date0;
    char* date1;
    char* date2;
    date0=strdup(sdate0);
    date1=strdup(sdate1);
    date2=strdup(sdate2);
//spame to string se day month year vash ths - kai meta to kanoume noumera kai sygkrinoume
    char* tmp=strtok(date0,"-");
    char* day0=strdup(tmp);
    tmp=strtok(NULL,"-");
    char* month0=strdup(tmp);
    tmp=strtok(NULL,"-");
    char* year0=strdup(tmp);
    tmp=strtok(NULL,"-");

    char* tmp1=strtok(date1,"-");
    char* day1=strdup(tmp1);
    tmp1=strtok(NULL,"-");
    char* month1=strdup(tmp1);
    tmp1=strtok(NULL,"-");
    char* year1=strdup(tmp1);
    tmp1=strtok(NULL,"-");

    char* tmp2=strtok(date2,"-");
    char* day2=strdup(tmp2);
    tmp2=strtok(NULL,"-");
    char* month2=strdup(tmp2);
    tmp2=strtok(NULL,"-");
    char* year2=strdup(tmp2);
    tmp2=strtok(NULL,"-");

    int iday0=atoi(day0),iday1=atoi(day1),iday2=atoi(day2);
    int imonth0=atoi(month0),imonth1=atoi(month1),imonth2=atoi(month2);
    int iyear0=atoi(year0),iyear1=atoi(year1),iyear2=atoi(year2);
    int flag1=0,flag2=0;

    imonth1=imonth1-6;
    if(imonth1<1){
    	imonth1=12+imonth1;
    	iyear1--;
    }
    //printf("Date 6months ago:%d-%d-%d\n",iday1,imonth1,iyear1 );
    if(iyear1<iyear0)
        flag1=1;
    if(iyear1==iyear0 && imonth1<imonth0)
        flag1=1;
    if(iyear1==iyear0 && imonth1==imonth0 && iday1<iday0)
        flag1=1;

    if(iyear0<iyear2)
        flag2=1;
    if(iyear0==iyear2 && imonth0<imonth2)
        flag2=1;
    if(iyear0==iyear2 && imonth0==imonth2 && iday0<iday2)
        flag2=1;
    free(date0);
    free(date1);
    free(date2);
    free(day0);
    free(month0);
    free(year0);
    free(day1);
    free(month1);
    free(year1);
    free(day2);
    free(month2);
    free(year2);
    if(flag1==1 && flag2==1)
        return 1;
    else
        return 0;

    return 0;
}

//travel request 
int travel_request(struct uInput *cmd , int ***bArrays, struct Virus *rootV, struct Country *rootC, long int bloomSize,int *fds,int *fds2,long int bufferSize){
	char *ID = cmd->args[1];
	char *date = cmd->args[2];
	char *countryFrom = cmd->args[3];
	char *countryTo = cmd->args[4];
    char *vName = cmd->args[5];
    struct Country *tempC=NULL;
    tempC=searchCountry(&rootC, countryFrom);//vriskei thn xwra sto dentro
    if (tempC==NULL){
    	printf("Error! %s doesn't exist in records\n",countryFrom );
    	return -1;
    }
    int monitor_for_country=tempC->numMonitor;//vriskei apo to dentro poio monitor einai ypefthinh gia afth
    int ret=0;
    int flag=0;
    ret=doFirstOption(cmd,bArrays,rootV,bloomSize,monitor_for_country);//psaxnei to bloom filter
    if (ret ==-1) 
    	printf("Virusname not exist\n"); 
    else if (ret) {
    	//printf("MAYBE\n");
    	flag=1;
    }
    else{ 
        printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");//ama einai no sto bloom filter emfanizei analogo mnm kai prosthetei to record sthn lista linked list 
        tempC=searchCountry(&rootC, countryTo);
        if (tempC==NULL){
            printf("Error! %s doesn't exist in records\n",countryTo );
            return -1;
        }
        if (tempC->next==NULL){
            //printf("1\n");
            tempC->next=malloc(sizeof(struct LinkedList));
            tempC->next->accepted_or_no=strdup("NO");
            tempC->next->date_requested=strdup(date);
            tempC->next->virusName=strdup(vName);
            tempC->next->next=NULL;
        }
        else{
            //printf("2\n");
            struct LinkedList* list;
            list=tempC->next;
            while(list->next!=NULL)
                list=list->next;
            list->next=malloc(sizeof(struct LinkedList));
            list->next->accepted_or_no=strdup("NO");
            list->next->date_requested=strdup(date);
            list->next->virusName=strdup(vName);
            list->next->next=NULL;
        }
    }
    if (flag==0)
    	return 0;//ama h apadhhsh tou bloomfilter htan maybe tote to travel monitor prepei na epikoinwnhsei me to monitor gia pareisigourh  apadhsh
    struct pollfd fdarray[1];
    fdarray[0].fd=fds2[monitor_for_country];
    fdarray[0].events=POLLIN;
    int intid=atoi(ID);
    write_int(fds[monitor_for_country],1);//grafei epilogh ena sthn loupa tou monitor
    write_int(fds[monitor_for_country],intid);//grafei to id poy mas endiaferei
    write_string(fds[monitor_for_country],vName,bufferSize);//kathos kai ton io
    sleep(1);
    int size=0;
	size=read_int(fds2[monitor_for_country],fdarray);// kai diavazei eite YES eite NO
    char Yes_No[size];
    strcpy(Yes_No,"");
    int rc=0;
    if(size<=bufferSize){
        rc=poll(fdarray,1,300);
        if(rc==1){
            read(fds2[monitor_for_country],Yes_No,size);
            Yes_No[size]='\0';}
        else{
            printf("Error! Poll timeout!\n");
            exit(1);
        }
    }
    else{
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
                read(fds2[monitor_for_country],constring,bufferSize);
            else{
                printf("Error! Poll timeout!\n");
                exit(1);
            }
            constring[bufferSize]='\0';
            strcat(Yes_No,constring);
            strcpy(constring,"");
            }
            if(modulus>0){
                int rc=poll(fdarray,1,300);
                if(rc==1)
                    read(fds2[monitor_for_country],constring,modulus);
                else{
                    printf("Error! Poll timeout!\n");
                    exit(1);
                }
                //printf("%s\n",constring );
                strcat(Yes_No,constring);
                strcpy(constring,"");
            }
        Yes_No[size]='\0';
    }
    char * Date_Vaccinated=NULL;

    if(strcmp(Yes_No,"YES")==0){//An einai yes tote diavazei kai thn hmeromhnia 

	   	int size=0;
		size=read_int(fds2[monitor_for_country],fdarray);
	    //char Date_Vaccinated[size];
	    Date_Vaccinated=(char*)malloc((size+10)*sizeof(char));
	    strcpy(Date_Vaccinated,"");
	    int rc=0;
	    if(size<=bufferSize){
	        rc=poll(fdarray,1,300);
	        if(rc==1){
	            read(fds2[monitor_for_country],Date_Vaccinated,size);
	            Date_Vaccinated[size]='\0';}
	        else{
	            printf("Error! Poll timeout!\n");
	            exit(1);
	        }
	    }
	    else{
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
	                read(fds2[monitor_for_country],constring,bufferSize);
	            else{
	                printf("Error! Poll timeout!\n");
	                exit(1);
	            }
	            constring[bufferSize]='\0';
	            strcat(Date_Vaccinated,constring);
	            strcpy(constring,"");
	            }
	            if(modulus>0){
	                int rc=poll(fdarray,1,300);
	                if(rc==1)
	                    read(fds2[monitor_for_country],constring,modulus);
	                else{
	                    printf("Error! Poll timeout!\n");
	                    exit(1);
	                }
	                //printf("%s\n",constring );
	                strcat(Date_Vaccinated,constring);
	                strcpy(constring,"");
	            }
	        Date_Vaccinated[size]='\0';
	    }
    }
    else{
    	Date_Vaccinated=(char*)malloc(3*sizeof(char));
    	strcpy(Date_Vaccinated,"");
    }
    //printf("Yes_No:[%s] Date_Vaccinated:[%s]\n",Yes_No,Date_Vaccinated );
    if (strcmp(Yes_No,"NO")==0){//ama einai no emfanizetai analogo mnm kai eisxwrhte sthn lista
    	printf("REQUEST REJECTED – YOU ARE NOT VACCINATED\n");
        tempC=searchCountry(&rootC, countryTo);
        if (tempC==NULL){
            printf("Error! %s doesn't exist in records\n",countryTo );
            return -1;
        }
        if (tempC->next==NULL){
            tempC->next=malloc(sizeof(struct LinkedList));
            tempC->next->accepted_or_no=strdup("NO");
            tempC->next->date_requested=strdup(date);
            tempC->next->virusName=strdup(vName);
            tempC->next->next=NULL;
        }
        else{
            struct LinkedList* list;
            list=tempC->next;
            while(list->next!=NULL)
                list=list->next;
            list->next=malloc(sizeof(struct LinkedList));
            list->next->accepted_or_no=strdup("NO");
            list->next->date_requested=strdup(date);
            list->next->virusName=strdup(vName);
            list->next->next=NULL;
        }
    }
    else if(strcmp(Yes_No,"YES")==0){//ama einai yes
    	int ret2=0;
    	ret2=DateInRange_6_month(Date_Vaccinated,date,date);//kai den einai mesa sta oria twn 6 mhnwn apo tote poy thelei na taksidepsei
    	if(ret2==0){
    		printf("REQUEST REJECTED – YOU WILL NEED ANOTHER VACCINATION BEFORE TRAVEL DATE\n");//emfanizei analogo mnm kai to prosthetei sthn lista
            tempC=searchCountry(&rootC, countryTo);
            if (tempC==NULL){
                printf("Error! %s doesn't exist in records\n",countryTo );
                return -1;
            }
            if (tempC->next==NULL){
                tempC->next=malloc(sizeof(struct LinkedList));
                tempC->next->accepted_or_no=strdup("NO");
                tempC->next->date_requested=strdup(date);
                tempC->next->virusName=strdup(vName);
                tempC->next->next=NULL;
            }
            else{
                struct LinkedList* list;
                list=tempC->next;
                while(list->next!=NULL)
                    list=list->next;
                list->next=malloc(sizeof(struct LinkedList));
                list->next->accepted_or_no=strdup("NO");
                list->next->date_requested=strdup(date);
                list->next->virusName=strdup(vName);
                list->next->next=NULL;
            }
        }
        else{
    		printf("REQUEST ACCEPTED – HAPPY TRAVELS\n");//ama einai edws 6 mhnwn pali emfanizei analogo mnm kai to prosthetei sthn lista
            tempC=searchCountry(&rootC, countryTo);
            if (tempC==NULL){
                printf("Error! %s doesn't exist in records\n",countryTo );
                return -1;
            }
            if (tempC->next==NULL){
                tempC->next=malloc(sizeof(struct LinkedList));
                tempC->next->accepted_or_no=strdup("YES");
                tempC->next->date_requested=strdup(date);
                tempC->next->virusName=strdup(vName);
                tempC->next->next=NULL;
            }
            else{
                struct LinkedList* list;
                list=tempC->next;
                while(list->next!=NULL)
                list=list->next;
                list->next=malloc(sizeof(struct LinkedList));
                list->next->accepted_or_no=strdup("YES");
                list->next->date_requested=strdup(date);
                list->next->virusName=strdup(vName);
                list->next->next=NULL;
            }
        }
    }
    else
    	printf("ERROR!!!!\n");




    free(Date_Vaccinated);
    sleep(1);
    return 0;

}
//synarthsh apo 1h ergasia
//Elegxei an to date0 einai anamesa sto date1 kai date2 
int DateInRange(char* sdate0,char* sdate1,char* sdate2){//to date0 einai afto poy theloyme na doyme an einai endimesa sto date1 kai sto date2
    char * date0;
    char* date1;
    char* date2;
    date0=strdup(sdate0);
    date1=strdup(sdate1);
    date2=strdup(sdate2);
//spame to string se day month year vash ths - kai meta to kanoume noumera kai sygkrinoume
    char* tmp=strtok(date0,"-");
    char* day0=strdup(tmp);
    tmp=strtok(NULL,"-");
    char* month0=strdup(tmp);
    tmp=strtok(NULL,"-");
    char* year0=strdup(tmp);
    tmp=strtok(NULL,"-");

    char* tmp1=strtok(date1,"-");
    char* day1=strdup(tmp1);
    tmp1=strtok(NULL,"-");
    char* month1=strdup(tmp1);
    tmp1=strtok(NULL,"-");
    char* year1=strdup(tmp1);
    tmp1=strtok(NULL,"-");

    char* tmp2=strtok(date2,"-");
    char* day2=strdup(tmp2);
    tmp2=strtok(NULL,"-");
    char* month2=strdup(tmp2);
    tmp2=strtok(NULL,"-");
    char* year2=strdup(tmp2);
    tmp2=strtok(NULL,"-");

    int iday0=atoi(day0),iday1=atoi(day1),iday2=atoi(day2);
    int imonth0=atoi(month0),imonth1=atoi(month1),imonth2=atoi(month2);
    int iyear0=atoi(year0),iyear1=atoi(year1),iyear2=atoi(year2);
    int flag1=0,flag2=0;

    if(iyear1<iyear0)
        flag1=1;
    if(iyear1==iyear0 && imonth1<imonth0)
        flag1=1;
    if(iyear1==iyear0 && imonth1==imonth0 && iday1<iday0)
        flag1=1;

    if(iyear0<iyear2)
        flag2=1;
    if(iyear0==iyear2 && imonth0<imonth2)
        flag2=1;
    if(iyear0==iyear2 && imonth0==imonth2 && iday0<iday2)
        flag2=1;
    free(date0);
    free(date1);
    free(date2);
    free(day0);
    free(month0);
    free(year0);
    free(day1);
    free(month1);
    free(year1);
    free(day2);
    free(month2);
    free(year2);
    if(flag1==1 && flag2==1)
        return 1;
    else
        return 0;

    return 0;
}

int travelStats(struct uInput *cmd,struct Country *rootC,int noOfCountries){
    int total_requests=0;
    int accepted=0;
    int rejected=0;
    int ret=0;
    if(cmd->argnum==5){//ama theloume sygkekrimenh xwra
        char *vName = cmd->args[1];
        char *date1 = cmd->args[2];
        char *date2 = cmd->args[3];
        char *countryTo = cmd->args[4];
        struct Country *tempC=NULL;
        tempC=searchCountry(&rootC, countryTo);//vriskoume thn xwra pou mas endiaferei
        if (tempC==NULL){
            printf("Error! %s doesn't exist in records\n",countryTo );
            return -1;
        }
        struct LinkedList* list=NULL;
        list=tempC->next;
        while(list!=NULL){
            //printf("aaaa\n");
            if(strcmp(list->virusName,vName)==0){//ama vroume komvo sthn lista gia ton io pou mas endiaferei
                ret=DateInRange(list->date_requested,date1,date2);//kai einai anamesa stis hmeromhnies pou theloume
                //printf("aaa2");
                if(ret==1){
                    total_requests++;//enhmrwnoume tis metavlites adistoixa
                    if(strcmp(list->accepted_or_no,"YES")==0)
                        accepted++;
                    else
                        rejected++;
                }
            }
            list=list->next;
        }
        printf("TOTAL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n",total_requests,accepted,rejected );//otan teleiwsei h lista gia thn xwra emfanizote ta statistika
    }else{//ama theloume gia oles tis xwres
        char *vName = cmd->args[1];
        char *date1 = cmd->args[2];
        char *date2 = cmd->args[3];
        struct Country *tempC=NULL;
        int j=0;
        char** Table_of_Countries=NULL;// h idia diadikasi me panw aplws twra ginetai gia kathe xwra
        Table_of_Countries=malloc(noOfCountries*sizeof(char*));
        //makearrayCountry(root,Table_of_Countries,&j);
        makearrayCountry(rootC,Table_of_Countries, &j );
        for(int i=0;i<noOfCountries;i++){//gia kthe xwra

            tempC=searchCountry(&rootC, Table_of_Countries[i]);
            if (tempC==NULL){
                printf("Error! %s doesn't exist in records\n",Table_of_Countries[i] );
                return -1;
            }
            struct LinkedList* list=NULL;
            list=tempC->next;
            while(list!=NULL){
                //printf("aaaa\n");
                if(strcmp(list->virusName,vName)==0){
                    ret=DateInRange(list->date_requested,date1,date2);
                    //printf("aaa2");
                    if(ret==1){
                        total_requests++;
                        if(strcmp(list->accepted_or_no,"YES")==0)
                            accepted++;
                        else
                            rejected++;
                    }
                }
                list=list->next;
            }
        }
        printf("TOTAL REQUESTS %d\nACCEPTED %d\nREJECTED %d\n",total_requests,accepted,rejected );
        free(Table_of_Countries);
    }
}

int addVaccinationRecords(struct uInput *cmd,int *fds,int *fds2,int numMonitors,struct Country *rootC,long int bufferSize,int noOfVirs,long int bloomSize,int ***bArrays,struct Virus *rootV,int * ids2){
    char *country = cmd->args[1];
    struct Country *tempC=NULL;
    struct pollfd fdarray[1];
    tempC=searchCountry(&rootC, country);//psaxnei gia thn xwra pou endiaferomaste
    if (tempC==NULL){
        printf("Error! %s doesn't exist in records\n",country );
        return -1;
    }
    int numMonitorCountry=0;
    numMonitorCountry=tempC->numMonitor;//vriskoume poio nummonitor einai ypefthino gia afth
    kill(ids2[numMonitorCountry],SIGUSR1);//stelnei sigusr1 shma sto adistoixw monitor
    write_int(fds[numMonitorCountry],3);//kai na kanei thn 3h epilogh
    fdarray[0].fd=fds2[numMonitorCountry];
    fdarray[0].events=POLLIN;//pinakas gia poll
    int ret=read_int_notimeout(fds2[numMonitorCountry]);//enhmerwnetai apo to monitor pote einai etoimo na steilei to ananeomeno bloomfilter
    //printf("pethainw\n");
    get_bloom_from_monitors(rootV,bArrays,bloomSize,noOfVirs,fds2[numMonitorCountry],bufferSize,fdarray,numMonitorCountry);//diavazei to neo bloom filter
    return 0;
   // printf("hahah pethana\n");
    //diavasw bloom filter 

}

int searchVaccinationStatus(struct uInput *cmd,int *fds,int *fds2,long int bufferSize,int numMonitors){
    char *ID = cmd->args[1];
    int intid=atoi(ID);
    struct pollfd fdarray[1];
    int flag=0;
    int division=0,modulus=0;
    for(int i=0;i<numMonitors;i++){//stelnei se kathe monitor thn defterh epilogh kai to id pou mas endiaferei
        write_int(fds[i],2);
        write_int(fds[i],intid);
        fdarray[0].fd=fds2[i];
        fdarray[0].events=POLLIN;
        int received=read_int(fds2[i],fdarray);// an diavazei arithmo diaforetiko tou 0 tote shmainei oti vrhkame to monitor pou mas endiaferei 
        if(received!=0){//diavazoume to onoma tou record pou mas endiaferei
            flag=1;
            int size=0;
            size=read_int(fds2[i],fdarray);
            char firstName[size];
            strcpy(firstName,"");
            int rc=0;
            if(size<=bufferSize){
                rc=poll(fdarray,1,300);
                if(rc==1){
                    read(fds2[i],firstName,size);
                    firstName[size]='\0';}
                else{
                    printf("Error! Poll timeout!\n");
                    exit(1);
                }
            }
            else{
                //strcpy(virus_received,"");
                division=0;
                modulus=0;
                char constring[size];
                strcpy(constring,"");
                division=size/bufferSize;
                modulus=size%bufferSize;
                rc=0;
                //printf("division: %d modulus: %d\n", division,modulus);
                for(int k=0;k<division;k++){
                    rc=poll(fdarray,1,300);
                    if(rc==1)
                        read(fds2[i],constring,bufferSize);
                    else{
                        printf("Error! Poll timeout!\n");
                        exit(1);
                    }
                    constring[bufferSize]='\0';
                    strcat(firstName,constring);
                    strcpy(constring,"");
                }
                if(modulus>0){
                    rc=poll(fdarray,1,300);
                    if(rc==1)
                        read(fds2[i],constring,modulus);
                    else{
                        printf("Error! Poll timeout!\n");
                        exit(1);
                    }
                    //printf("%s\n",constring );
                    strcat(firstName,constring);
                    strcpy(constring,"");
                }
                firstName[size]='\0';
            }

            size=0;
            size=read_int(fds2[i],fdarray);
            char lastName[size];//diavazoume to epitheto tou record mou mas endiaferei
            strcpy(lastName,"");
            rc=0;
            if(size<=bufferSize){
                rc=poll(fdarray,1,300);
                if(rc==1){
                    read(fds2[i],lastName,size);
                    lastName[size]='\0';}
                else{
                    printf("Error! Poll timeout!\n");
                    exit(1);
                }
            }
            else{
                //strcpy(virus_received,"");
                division=0;
                modulus=0;
                char constring1[size];
                strcpy(constring1,"");
                division=size/bufferSize;
                modulus=size%bufferSize;
                rc=0;
                //printf("division: %d modulus: %d\n", division,modulus);
                for(int k=0;k<division;k++){
                    rc=poll(fdarray,1,300);
                    if(rc==1)
                        read(fds2[i],constring1,bufferSize);
                    else{
                        printf("Error! Poll timeout!\n");
                        exit(1);
                    }
                    constring1[bufferSize]='\0';
                    strcat(lastName,constring1);
                    strcpy(constring1,"");
                    }
                    if(modulus>0){
                        rc=poll(fdarray,1,300);
                        if(rc==1)
                            read(fds2[i],constring1,modulus);
                        else{
                            printf("Error! Poll timeout!\n");
                            exit(1);
                        }
                        //printf("%s\n",constring );
                        strcat(lastName,constring1);
                        strcpy(constring1,"");
                    }
                lastName[size]='\0';
            }

            size=0;
            size=read_int(fds2[i],fdarray);
            char countryName[size];
            strcpy(countryName,"");
            rc=0;
            if(size<=bufferSize){
                rc=poll(fdarray,1,300);//diavazoume to onoma ths xwra tou record pou mas endiaferei
                if(rc==1){
                    read(fds2[i],countryName,size);
                    countryName[size]='\0';}
                else{
                    printf("Error! Poll timeout!\n");
                    exit(1);
                }
            }
            else{
                //strcpy(virus_received,"");
                division=0;
                modulus=0;
                char constring2[size];
                strcpy(constring2,"");
                division=size/bufferSize;
                modulus=size%bufferSize;
                rc=0;
                //printf("division: %d modulus: %d\n", division,modulus);
                for(int k=0;k<division;k++){
                    rc=poll(fdarray,1,300);
                    if(rc==1)
                        read(fds2[i],constring2,bufferSize);
                    else{
                        printf("Error! Poll timeout!\n");
                        exit(1);
                    }
                    constring2[bufferSize]='\0';
                    strcat(countryName,constring2);
                    strcpy(constring2,"");
                }
                if(modulus>0){
                    rc=poll(fdarray,1,300);
                    if(rc==1)
                        read(fds2[i],constring2,modulus);
                    else{
                        printf("Error! Poll timeout!\n");
                        exit(1);
                    }
                    //printf("%s\n",constring );
                    strcat(countryName,constring2);
                    strcpy(constring2,"");
                }
                    countryName[size]='\0';
            }

            int age=read_int(fds2[i],fdarray);//diavazoume thn hlikia tou record pou mas endiaferei
            printf("%s %s %s %s\nAGE %d\n",ID,firstName,lastName,countryName,age);
            
            int times=0;
            times=read_int(fds2[i],fdarray);//diavazoume posa records yparxoun gia ton sygkekrimeno anthrwpo
            for(int j=0;j<times;j++){//gia kathe record

                //diavazoume to onoma tou iou

                size=0;
                size=read_int(fds2[i],fdarray);
                char virusName[size];
                strcpy(virusName,"");
                rc=0;
                if(size<=bufferSize){
                    rc=poll(fdarray,1,300);
                    if(rc==1){
                        read(fds2[i],virusName,size);
                        virusName[size]='\0';}
                    else{
                        printf("Error! Poll timeout!\n");
                        exit(1);
                    }
                }
                else{
                    //strcpy(virus_received,"");
                    division=0;
                    modulus=0;
                    char constring3[size];
                    strcpy(constring3,"");
                    division=size/bufferSize;
                    modulus=size%bufferSize;
                    rc=0;
                    //printf("division: %d modulus: %d\n", division,modulus);
                    for(int k=0;k<division;k++){
                        rc=poll(fdarray,1,300);
                        if(rc==1)
                            read(fds2[i],constring3,bufferSize);
                        else{
                            printf("Error! Poll timeout!\n");
                            exit(1);
                        }
                        constring3[bufferSize]='\0';
                        strcat(virusName,constring3);
                        strcpy(constring3,"");
                        }
                        if(modulus>0){
                            rc=poll(fdarray,1,300);
                            if(rc==1)
                                read(fds2[i],constring3,modulus);
                            else{
                                printf("Error! Poll timeout!\n");
                                exit(1);
                            }
                            //printf("%s\n",constring );
                            strcat(virusName,constring3);
                            strcpy(constring3,"");
                        }
                    virusName[size]='\0';
                }

                int yesnoflag=read_int(fds2[i],fdarray);//diavazoume arithmo. to 1 shmainei NO kai to 0 YES
                if(yesnoflag==1){
                    printf("%s NOT YET VACCINATED\n",virusName);
                }
                else if(yesnoflag==0){// ama einai YES diavazei kai thn hmeromhnia
                    size=0;
                    size=read_int(fds2[i],fdarray);
                    char date[size];
                    strcpy(date,"");
                    rc=0;
                    if(size<=bufferSize){
                        rc=poll(fdarray,1,300);
                        if(rc==1){
                            read(fds2[i],date,size);
                            date[size]='\0';}
                        else{
                            printf("Error! Poll timeout!\n");
                            exit(1);
                        }
                    }
                    else{
                        //strcpy(virus_received,"");
                        division=0;
                        modulus=0;
                        char constring4[size];
                        strcpy(constring4,"");
                        division=size/bufferSize;
                        modulus=size%bufferSize;
                        rc=0;
                        //printf("division: %d modulus: %d\n", division,modulus);
                        for(int k=0;k<division;k++){
                            rc=poll(fdarray,1,300);
                            if(rc==1)
                                read(fds2[i],constring4,bufferSize);
                            else{
                                printf("Error! Poll timeout!\n");
                                exit(1);
                            }
                            constring4[bufferSize]='\0';
                            strcat(date,constring4);
                            strcpy(constring4,"");
                            }
                            if(modulus>0){
                                rc=poll(fdarray,1,300);
                                if(rc==1)
                                    read(fds2[i],constring4,modulus);
                                else{
                                    printf("Error! Poll timeout!\n");
                                    exit(1);
                                }
                                //printf("%s\n",constring );
                                strcat(date,constring4);
                                strcpy(constring4,"");
                            }
                        date[size]='\0';
                    }
                    printf("%s VACCINATED ON %s\n",virusName,date );//kai emfanizei analogo mnm

                }


            }
        }
    }
    if(flag==0){
        printf("Error ID not found!!\n");
        return 1;
    }
    return 0;

}


// xrhsimopoihei thn lista LinkedList gia na dei posa request exoun ginei kai an eginan dekta h oxi gia na vrei ta statistika. Epeita dimiourgeite to logfile opws zhteitai apo thn ekfwnhsh
void create_logFile(struct Country *rootC,int noOfCountries){

    int total_requests=0,accepted=0,rejected=0;
    pid_t pid=getpid();
    char ch_pid[10];
    char citizenID[17];
    sprintf(ch_pid,"%d",pid);
    strcpy(citizenID,"");
    strcpy(citizenID,"log_file.");//fitaxnw to onoma tou arxeiou
    strcat(citizenID,ch_pid);
    //printf("citizenID\n");
    FILE *fp = NULL;
    fp = fopen(citizenID ,"w");//dhmiourgw to arxeio

    struct Country *tempC=NULL;
    int j=0;
    char** Table_of_Countries=NULL;//ftiaxnw enan pinaka apo tis xwres
    Table_of_Countries=malloc(noOfCountries*sizeof(char*));
    //makearrayCountry(root,Table_of_Countries,&j);
    makearrayCountry(rootC,Table_of_Countries, &j );
    for(int i=0;i<noOfCountries;i++){
        fprintf(fp,"%s\n",Table_of_Countries[i]);//grafw sto arxeio thn kathe xwra
        tempC=searchCountry(&rootC, Table_of_Countries[i]);
        if (tempC==NULL){
            printf("Error! %s doesn't exist in records\n",Table_of_Countries[i] );
            return ;
        }
        struct LinkedList* list=NULL;
        list=tempC->next;
        while(list!=NULL){//ypologizei ta stistika 
            total_requests++;
            if(strcmp(list->accepted_or_no,"YES")==0)
                accepted++;
            else
                rejected++;  
            list=list->next;
            }
        }
    fprintf(fp,"TOTAL TRAVEL REQUESTS %d\n",total_requests);//ta grafei sto arxeio
    fprintf(fp,"ACCEPTED %d\n",accepted);
    fprintf(fp,"REJECTED %d\n",rejected);
    free(Table_of_Countries);
    fclose(fp);
    return;
}