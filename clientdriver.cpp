#include <iostream>
#include <stdio.h>
#include <string>
#include <string.h>
#include <stdlib.h>
#include "UDPClient.hpp"

using namespace std;

int main( int argc, char* argv[]){

	int Port = 65535;
	string IP;
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
				i++;
			}			
			i++;
		}
	}
	cout << "Address " << IP << ":"<< Port << endl;
	UDPClient client = UDPClient(IP, Port);
	client.echo();

}
