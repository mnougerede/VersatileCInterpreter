#ifndef ENV_SCOPE_GUARD_H
#define ENV_SCOPE_GUARD_H

#include "Environment.h"

class EnvScopeGuard {
public:
    EnvScopeGuard(Environment*& currentEnv)
      : envRef(currentEnv), oldEnv(currentEnv) {
        // Create a new environment with the current one as its parent.
        currentEnv = oldEnv->pushScope();
    }
    ~EnvScopeGuard() {
        // Restore the original environment and free the created scope.
        Environment* temp = envRef;
        envRef = oldEnv;
        delete temp;
    }
private:
    Environment*& envRef;    // Reference to the current environment pointer.
    Environment* oldEnv;       // The original environment.
};

#endif // ENV_SCOPE_GUARD_H
