#include <unistd.h>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "UDPServer.hpp"
using namespace std;

int main(int argc, char* argv[]){
    int Port;

    if(argc > 1){
        int i = 0;
        while(i < argc){
            if(strcmp(argv[i],"-port") == 1){
                Port = stoi(argv[i+1]);
                if(Port > 65535 || Port < 0){
                    cout << "Invalid Port Number.\nSetting to default" << endl;
                }else{
                    Port = atoi(argv[i+1]);
                }
                i++;
            }
            i++;
        }

    }

    UDPServer testServer = UDPServer();
    testServer.waiting();
    return -1;
}