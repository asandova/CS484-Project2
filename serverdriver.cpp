#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>

using namespace std;

int Port = 65535;
int bufferLen = 512;

int main(int argc, char* argv[]){

    if(argc > 1){
        int i = 0;
        while(i < argc){
            if(strcmp(argv[i],"-port") == 1){
                if(Port > 65535 || Port < 0){
                    cout << "Invalid Port Number.\nSetting to default" << endl;
                }else{
                    Port = atoi(argv[i+1]);
                }
            }
        }

    }
    struct sockaddr_in si_me, si_other;
    int s, i, slen = sizeof(si_other), recv_len;
    char buf[bufferLen];
    if( (s=socket(AF_INET,SOCK_DGRAM,IPPROTO_UDP))==-1){
        perror((char*)s);
        exit(1);
    }
    memset( (char*) &si_me,0,sizeof(si_me) );
    si_me.sin_family = AF_INET;
    si_me.sin_port = htons(Port);
    si_me.sin_addr.s_addr = htonl(INADDR_ANY);

     if( bind(s , (struct sockaddr*)&si_me, sizeof(si_me) ) == -1)
    {
        perror("bind");
        exit(1);
    }
    int close = 0;
    while(!close){
        cout << "waiting for data..." << endl;

        if ((recv_len = recvfrom(s, buf, bufferLen, 0, (struct sockaddr *) &si_other, &slen)) == -1){
            perror("recvfrom()");
            exit(1);
        }

        printf("Received packet from %s:%d\n", inet_ntoa(si_other.sin_addr), ntohs(si_other.sin_port));
        printf("Data: %s\n" , buf);
         
        if (sendto(s, buf, recv_len, 0, (struct sockaddr*) &si_other, slen) == -1){
            perror("sendto()");
            exit(0);
        }
    }
}
