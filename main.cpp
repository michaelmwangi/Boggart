#include <iostream>
#include "network/boggartserver.h"

using namespace std;

int main(int argc, char *argv[])
{    
    cout<<"Started"<<endl;
    BoggartServer boggartserver("127.0.0.1", "8090");
    boggartserver.run();
    return 0;
}



