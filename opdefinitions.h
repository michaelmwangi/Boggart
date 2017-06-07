#ifndef OPDEFINITIONS_H
#define OPDEFINITIONS_H
#include <string>

// this class defines the operation internal definitions eg Boggart worker/client signatures

struct OpDefinitions{
    static constexpr const char * worker_signature = "BOGW01";
    static constexpr const char * client_signature = "BOGC01";
    static constexpr const char * internal_worker_signature = "BOGI01";
};



#endif // OPDEFINITIONS_H
