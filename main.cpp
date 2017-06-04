#include <iostream>
#include "network/boggartserver.h"

using namespace std;

int main(int argc, char *argv[])
{    
    cout<<"Started"<<endl;
    Network network("127.0.0.1", "8090");
    network.run();
    return 0;
}



