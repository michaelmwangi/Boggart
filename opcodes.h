#ifndef OPCODES_H
#define OPCODES_H


// this structure defines the return opertion return codes to the bogggart client
enum class OpCodes{
    pending = 202,
    ok = 200,
    created = 201,
    not_found = 404,
    error = 500
};

#endif // OPCODES_H
