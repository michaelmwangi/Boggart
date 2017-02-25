#include <iostream>
#include <zmqpp/zmqpp.hpp>
#include "broker.h"

using namespace std;

int main(int argc, char *argv[])
{
    Broker broker("tcp://*", "5557");
    broker.StartBroker();
    cout << "Hello World!" << endl;
    return 0;
}
