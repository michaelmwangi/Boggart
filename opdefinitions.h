#ifndef OPDEFINITIONS_H
#define OPDEFINITIONS_H
#include <string>

// this class defines the operation internal definitions eg Boggart worker/client signatures

struct OpDefinitions{
    std::string worker_signature = "BOGW01";
    std::string client_signature = "BOGC01";
    std::string internal_worker_signature = "BOGI01";
};



#endif // OPDEFINITIONS_H
