
#include <stdio.h>
#include <string>
#include <stdlib.h>

#include "UDPClient.hpp"


int main(){

    UDPClient client = UDPClient("192.168.1.7", 65535);

    client.waiting();

}
