#include <iostream>
#include <memory>
#include "network/boggartserver.h"
#include "boggartconfig.h"

using namespace std;

int main(int argc, char *argv[])
{    
    cout<<"Started"<<endl;
    std::shared_ptr<BoggartConfig> boggartconfig = std::make_shared<BoggartConfig>();
    boggartconfig->heart_beat_timeout = 3;
    boggartconfig->host = "0.0.0.0";
    boggartconfig->port = "8090";
    BoggartServer boggartserver(boggartconfig);
    boggartserver.run();
    return 0;
}



