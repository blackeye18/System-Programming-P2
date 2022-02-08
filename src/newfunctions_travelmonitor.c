// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/errno.h>
#include "structs_travel.h"

#include "newfunctions_travelmonitor.h"
#include "Functions_travelmonitor.h"


void int_to_4_bytes(int number,unsigned char* bytes){//spaei ena akeraio, poy exei megethos 4 bytes, se 4 diaforetika bytes
    
    bytes[3]=(number >> 24) & 0xFF;
    bytes[2]=(number >> 16) & 0xFF;
    bytes[1]=(number >> 8) & 0xFF;
    bytes[0]=number & 0xFF;
    return;
}

int bytes_to_int(unsigned char* bytes){//enwnei 4 ksexwrista bytes se enan akeraio megethous 4wn bytes
    int x=0;
    x=*(int*)bytes;
    //printf("%d\n",x );
    return x;
}
void write_int(int fd,int number){//grafei sto namedpipe enan akeraio 4wn bytes, alla ana byte etsi wste na mhn yparxei thema me to buffersize
    unsigned char bytes[4];
    for (int i=0;i<4;i++)
        bytes[i]=0;
    int_to_4_bytes(number,bytes);
    write(fd,&bytes[0],1);
    write(fd,&bytes[1],1);
    write(fd,&bytes[2],1);
    write(fd,&bytes[3],1);
    return;
}
int read_int(int fd,struct pollfd* fdarray){//diavazei enan akeraio apo to namedpipe poy einai spasmenos se ksexwrista bytes, ton enwnei kai ton epistrefei
    unsigned char bytes[4];
    for (int i=0;i<4;i++)
        bytes[i]=0;
    int rc=0;
    rc=poll(fdarray,1,300);
    if(rc==1)
        read(fd,&bytes[0],1);
    else{
        printf("Error! Poll timeout!\n");
        exit(1);
    }
    rc=poll(fdarray,1,300);
    if(rc==1)
        read(fd,&bytes[1],1);
    else{
        printf("Error! Poll timeout!\n");
        exit(1);
    }
    rc=poll(fdarray,1,300);
    if(rc==1)
        read(fd,&bytes[2],1);
    else{
        printf("Error! Poll timeout!\n");
        exit(1);
    }
    rc=poll(fdarray,1,300);
    if(rc==1)
        read(fd,&bytes[3],1);
    else{
        printf("Error! Poll timeout!\n");
        exit(1);
    }
    int size=0;
    size=bytes_to_int(bytes);
    return size;
}

int read_int_notimeout(int fd){//to idio me thn synarthsh apo panw alla xwris thn xrhsh ths poll gia na mhn kanei timeout. Xrhsimopoieitai sthn kyria loupa tou programmatos
    unsigned char bytes[4];
    
    read(fd,&bytes[0],1);
    
    
    read(fd,&bytes[1],1);
    
    
    read(fd,&bytes[2],1);
   
    
    read(fd,&bytes[3],1);
    
    int size=bytes_to_int(bytes);
    return size;
}

int argsOK(int argc, char *argv[])//arguments check
{
    if (argc != 9)
    {
        printf("Error in # of args\nusage: vaccineMonitor -c citizenRecordsFile -b bloomSize");
        return 0;
    }
    if (strcmp(argv[1],"-m")!=0 || strcmp(argv[3],"-b" )!=0 || strcmp(argv[5],"-s" )!=0 || strcmp(argv[7],"-i" )!=0)
    {
        printf("Error in args\nusage: /travelMonitor –m numMonitors -b bufferSize -s sizeOfBloom -i input_dir");
        return 0;
    }
    return 1;
}

void write_string(int fd,char *string,long int bufferSize){//idia synarthsh poy yparxei kai sto monitor
    int size=strlen(string);//yparxoyn ekei ta sxolia, sto arxeio monitornewfunc.c
    write_int(fd,size);
    if(size<=bufferSize){
        write(fd,string,size);
    }
    else{
        int division=1,modulus=0;
        division=size/bufferSize;
        modulus=size%bufferSize;
        //printf("division: %d modulus: %d\n", division,modulus);
        char sub_buf[division][bufferSize];
        long int location=0;

        for(int j=0;j<division;j++){
            strcpy(sub_buf[j],"");
            memcpy( sub_buf[j], &string[location], bufferSize );
            sub_buf[j][bufferSize]='\0';
            location=location+bufferSize;
            
        }
        for(int j=0;j<division;j++){
            //printf("[%s]\n",sub_buf[j] );
            write(fd,sub_buf[j],bufferSize);
        }
        if(modulus>0){
            char sub_buf_mod[modulus];
            memcpy( sub_buf_mod, &string[location], modulus );
            sub_buf_mod[modulus]='\0';
            //printf("%s\n",sub_buf_mod );
            write(fd,sub_buf_mod,modulus);
            //writes klp prepei na kovw to string
        }

    }
}

//ftiaxnwi apo 2 fifos/namedpipes gia kathe monitor
void Create_All_Fifos(int num_of_monitors){
    //ftiaxnw 2 namedpipes/fifos gia kathe monitor
    char FIFO_1[100]="";//read
    char FIFO_2[100]="";//write
    int i=0;
    for(i=0;i<num_of_monitors;i++){
        
        sprintf(FIFO_1,"Input%d", i);
        sprintf(FIFO_2,"Output%d",i);

        if((mkfifo(FIFO_1,permissions_for_fifos)<0) && (errno!=EEXIST)){
            printf("Error! Didn't create fifo\n");
        }
        if((mkfifo(FIFO_2,permissions_for_fifos)<0) && (errno!=EEXIST)){
            printf("Error! Didn't create fifo\n");
        }

    }


}
//kleinei ola ta namedpipes
void close_all_fifos(int num_of_monitors,int *fds,int *fds2){
    for(int i=0;i<num_of_monitors;i++){
        close(fds[i]);
        close(fds2[i]);
    }

}
//diagrafei ola ta named pipes
void Delete_All_Fifos(int num_of_monitors){
    char FIFO_1[100]="";//read
    char FIFO_2[100]="";//write
    int i=0;
    for(i=0;i<num_of_monitors;i++){
        
        sprintf(FIFO_1,"Input%d", i);
        sprintf(FIFO_2,"Output%d",i);

        if(remove(FIFO_1)!=0){
            printf("Error! Didn't Delete fifo\n");
        }
        if(remove(FIFO_2)!=0){
            printf("Error! Didn't Delete fifo\n");
        }

    }

}

void get_bloom_from_monitors(struct Virus *rootV,int ***bArrays, long int bloomSize,int noOfVirs,int fd, long int bufferSize,struct pollfd* fdarray,int currentNumMonitor){
    struct Virus *temp=NULL;
    //printf("Parent: bloomSize%ld bufferSize%ld\n",bloomSize,bufferSize );
    long int bArrayLength = bloomSize / (long)sizeof(int);
    int noOfVirsOfMonitor=read_int(fd,fdarray);//diavazei to plithos twn iwn sto monitor
    int rc,size;
   // printf("noOfVirsOfMonitor%d\n",noOfVirsOfMonitor);
    for(int i=0;i<noOfVirsOfMonitor;i++){
        size=read_int(fd,fdarray);//diavazei to kathe onoma tou iou me ton gnwsto tropo
        char virusName[size];
        if(size<=bufferSize){
            //printf("Child: size<=bufferSize\n");
            rc=poll(fdarray,1,300);
            if(rc==1){
            read(fd,virusName,size);
            virusName[size]='\0';}
            else{
            printf("Error! Poll timeout!\n");
            exit(1);
            }
        }else{
            strcpy(virusName,"");
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
                strcat(virusName,constring);
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
                strcat(virusName,constring);
                strcpy(constring,"");
            }
            virusName[size]='\0';

        }
        temp=searchVirus(&rootV,virusName);//psaxnei sto diko tou dentro gia ton io
        if(temp==NULL){
            printf("VIRUS NAME NOT FOUND: [%s]\n",virusName );
            printf("Parent:Error! Virus Not Found! Exiting\n");
            exit(1);
        }
        int rank=temp->rank;//krataei to rank tou ioy
        for(int j=0;j<bArrayLength;j++){
            int tempint=0;
            tempint=read_int(fd,fdarray);//apothikevei ta stoixeia tou bloomfilter
            bArrays[currentNumMonitor][rank][j]=tempint;
        }



    }
    return;
}
