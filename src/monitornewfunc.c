// 2h Ergasia SysPro - MAVROMMATIS PANAGIOTIS - sdi1800115 //
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <poll.h>
#include <sys/stat.h> 
#include <sys/types.h>
#include <sys/errno.h>
#include "monitornewfunc.h"

void int_to_4_bytes(int number,unsigned char* bytes){//spaei ena akeraio, poy exei megethos 4 bytes, se 4 diaforetika bytes
    
    bytes[3]=(number >> 24) & 0xFF;
    bytes[2]=(number >> 16) & 0xFF;
    bytes[1]=(number >> 8) & 0xFF;
    bytes[0]=number & 0xFF;
    return;
}

int bytes_to_int(unsigned char* bytes){//enwnei 4 ksexwrista bytes se enan akeraio megethous 4wn bytes
    int x=*(int*)bytes;
    return x;
}


void write_int(int fd,int number){//grafei sto namedpipe enan akeraio 4wn bytes, alla ana byte etsi wste na mhn yparxei thema me to buffersize
    unsigned char bytes[4];
    int_to_4_bytes(number,bytes);
    write(fd,&bytes[0],1);
    write(fd,&bytes[1],1);
    write(fd,&bytes[2],1);
    write(fd,&bytes[3],1);
    return;
}

int read_int(int fd,struct pollfd* fdarray){//diavazei enan akeraio apo to namedpipe poy einai spasmenos se ksexwrista bytes, ton enwnei kai ton epistrefei
    unsigned char bytes[4];
    int rc=poll(fdarray,1,300);
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
    int size=bytes_to_int(bytes);
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


void write_string(int fd,char *string,long int bufferSize){// h synarthsh ayth grafei se namedpipe to string string xwris na ksepernaei to buffersize, me ton idio tropo pou eidame sthn main.
    int size=strlen(string);//vriskei to megethos
    write_int(fd,size);//enhmerwnei
    if(size<=bufferSize){//an size mikrotero tou buffersize aplws grafei me megethos size
        write(fd,string,size);
    }
    else{
        int division=1,modulus=0;//diaforetika ypologizei pose fores prepei na grapsei megethos buffersize kai an xreiastei ti megethos akoma
        division=size/bufferSize;
        modulus=size%bufferSize;
        //printf("division: %d modulus: %d\n", division,modulus);
        char sub_buf[division][bufferSize];
        long int location=0;

        for(int j=0;j<division;j++){//spaei to string division fores
            strcpy(sub_buf[j],"");
            memcpy( sub_buf[j], &string[location], bufferSize );
            sub_buf[j][bufferSize]='\0';
            location=location+bufferSize;
            
        }
        for(int j=0;j<division;j++){// grafei sto namedpipe
            //printf("[%s]\n",sub_buf[j] );
            write(fd,sub_buf[j],bufferSize);
        }
        if(modulus>0){
            char sub_buf_mod[modulus];// kai an exei perisepsei, grafei oti exei perisepsei apo to spasimo
            memcpy( sub_buf_mod, &string[location], modulus );
            sub_buf_mod[modulus]='\0';
            //printf("%s\n",sub_buf_mod );
            write(fd,sub_buf_mod,modulus);
            
        }

    }
}