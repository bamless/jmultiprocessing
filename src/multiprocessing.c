#include <jstar.h>
#include <errno.h>
#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/wait.h>

// class Process

#define M_FUNC "__func"
#define M_ARGS "__args"
#define M_PID  "__pid"

static bool Process_start(JStarVM *vm) {
    jsrGetField(vm, 0, M_PID);
    if(!jsrCheckInt(vm, -1, "Process."M_PID)) return false;
    if(jsrGetNumber(vm, -1) != -1) JSR_RAISE(vm, "ProcessException", "Process already started.");
    jsrPop(vm);

    jsrGetField(vm, 0, M_ARGS);
    if(!jsrCheckTuple(vm, -1, "Process."M_ARGS)) return false;
    size_t arity = jsrTupleGetLength(vm, -1);
    if(arity > UINT8_MAX) {
        JSR_RAISE(vm, "TypeException", "Too many arguments to start fucntion (max %d).", (int)UINT8_MAX);
    }
    jsrPop(vm);

    jsrGetField(vm, 0, M_FUNC);
    jsrGetGlobal(vm, JSR_CORE_MODULE, "Function");
    if(!jsrIs(vm, -2, -1)) return false;
    jsrPop(vm);
    jsrPop(vm);

    pid_t pid = fork();
    if(pid < 0) {
        JSR_RAISE(vm, "ProcessException", strerror(errno));
    } 

    // we are on the father, exit the constructor normally
    if(pid) {
        jsrPushNumber(vm, pid);
        jsrSetField(vm, 0, M_PID);
        jsrPushNull(vm);
        return true;
    }

    // We are in the children, push arguments and call start function
    jsrGetField(vm, 0, M_ARGS);
    jsrGetField(vm, 0, M_FUNC);
    jsrEnsureStack(vm, arity);

    for(size_t i = 0; i < arity; i++) {
        jsrTupleGet(vm, i, 1);
    }

    // call and exit
    if(jsrCall(vm, (uint8_t) arity) != VM_EVAL_SUCCESS) {
        fprintf(stderr, "forked process (pid %d):\n", getpid());
        jsrPrintStacktrace(vm, -1);
        exit(EXIT_FAILURE);
    } else {
        exit(EXIT_SUCCESS);
    }   
}

static bool Process_join(JStarVM *vm) {
    jsrGetField(vm, 0, M_PID);
    if(!jsrCheckInt(vm, -1, "Process."M_PID)) return false;
    pid_t pid = jsrGetNumber(vm, -1);

    if(pid <= 0) {
        JSR_RAISE(vm, "ProcessException", "The process hasn't been started.");
    }

    if(waitpid(pid, NULL, 0) == -1) {
        JSR_RAISE(vm, "ProcessException", strerror(errno));
    }

    jsrPushNull(vm);
    return true;
}

// end

static JStarNativeReg registry[] = {
    JSR_REGMETH(Process, start, &Process_start)
    JSR_REGMETH(Process, join, &Process_join)
    JSR_REGEND
};

JSTAR_API JStarNativeReg *jsr_open_multiprocessing() {
    return registry;
}