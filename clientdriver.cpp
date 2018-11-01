/*
*   File: clientdriver.cpp
*   Author: August B. Sandoval
*   Date: 2018-10-19
*   Purpose: The driver program for the UPDClient
*   Class: CS484
*/
#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "UDPClient.hpp"

using namespace std;

int main( int argc, char* argv[]){

	int Port = 65535;
	string filename = "";
	string IP;
	bool useHost = false;
	if(argc > 1){
		int i = 0;
		while(i < argc){
			if( strcmp(argv[i], "-port") == 0 ){
				Port = atoi(argv[i+1]);
				if(Port > 65535 || Port < 0){
					cout << "Invalid port!\nSetting to default"<< endl;
					Port = 65535;
				}
				i++;
			}
			if(strcmp(argv[i], "-ip") == 0){
				IP = string(argv[i+1]);
				useHost = false;
				i++;
			}
			if(strcmp(argv[i], "-host" ) == 0){
				IP = string(argv[i+1]);
				useHost = true;
				i++;
			}
			if(strcmp(argv[i], "-v") == 0){
				UDPClient::verboseMode = true;
			}	
			if(strcmp(argv[i], "-debug") == 0){
				UDPClient::DebugMode = true;
			}	
			if(strcmp(argv[i], "-file")==0){
                filename = argv[i+1];
                i++;
            }
			i++;
		}
	}

	cout << "Address " << IP << ":"<< Port << endl;
	UDPClient client = UDPClient(IP, Port, useHost);
	if(client.run()){
		client.save(filename);
	}
	return 0;
}
