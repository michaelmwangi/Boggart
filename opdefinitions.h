#ifndef OPDEFINITIONS_H
#define OPDEFINITIONS_H
#include <string>

// this class defines the operation internal definitions eg Boggart worker/client signatures

struct Signatures{
    static constexpr const char * worker_signature = "BOGW01";
    static constexpr const char * client_signature = "BOGC01";
    static constexpr const char * internal_worker_signature = "BOGI01";
};

// this structure defines the operational commands that are issued by workers and clients

struct OpCommands {
    static constexpr const char * worker_ready = "READY";
    static constexpr const char * worker_resp = "RESP";
    static constexpr const char * client_put_job = "INSJOB";
    static constexpr const char * client_get_result = "GETRES";
};



#endif // OPDEFINITIONS_H
