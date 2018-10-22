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
            if(strcmp(argv[i],"-port") == 0){
                int temp = stoi(argv[i+1],nullptr,10);
                if(temp > 65535 || temp < 0){
                    cout << "Invalid Port Number.\nSetting to default" << endl;
                }else{
                    Port = temp;
                }
                i++;
            }
            i++;
        }

    }

    UDPServer testServer = UDPServer(Port);
    testServer.echo();
    return -1;
}