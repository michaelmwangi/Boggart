#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include "broker.h"

using namespace std;

int main(int argc, char *argv[])
{
    Broker broker("tcp://*", "5777");
    broker.StartBroker();
    return 0;
}
