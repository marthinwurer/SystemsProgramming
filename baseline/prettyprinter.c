#include <baseline/prettyprinter.h>

void pretty_print(status_t input) {
    switch (input) {
        case E_NO_PCBS:
            cwrites("@Status: No PCBs\n");
            break;
        case E_NO_STACKS:
            cwrites("@Status: No Stacks\n");
            break;
        case E_NO_MORE_PROCS:
            cwrites("@Status: No more Procs\n");
            break;
        case E_NO_CHAR:
            cwrites("@Status: no char\n");
            break;
        case E_NO_CHILD:
            cwrites("@Status: No child\n");
            break;
        case E_BAD_CHAN:
            cwrites("@Status: Bad Channel\n");
            break;
        case E_NO_PROCESS:
            cwrites("@Status: No Process\n");
            break;
        case E_NO_PERM:
            cwrites("@Status: No PERM\n");
            break;
        case E_FAILURE:
            cwrites("@Status: Failure\n");
            break;
        case E_SUCCESS:
            cwrites("@Status: Success!\n");
            break;
        case E_NOT_IMPLEMENTED:
            cwrites("@Status: Not Implemented(IO)\n");
            break;
        case E_BAD_TYPE:
            cwrites("@Status: Bad Type (IO)\n");
            break;
        case E_NO_MORE_HANDLES:
            cwrites("@Status: Out of Handles (IO)\n");
            break;
        case E_BAD_HANDLE:
            cwrites("@Status: Bad handle(IO)\n");
            break;
        case E_NO_DATA:
            cwrites("@Status: No Data (IO)\n");
            break;
        case E_OUT_OF_BOUNDS:
            cwrites("@Status: Out of Bounds(IO)\n");
            break;
        case E_BAD_ARG:
            cwrites("@Status: Bad argument (IO)\n");
            break;
        case E_NO_MATCH:
            cwrites("@Status: No Matching Object (IO) \n");
            break;
        case E_CANCELED:
            cwrites("@Status: Operation Canceled (IO)\n");
            break;
        case E_LOCKED:
            cwrites("@Status: object locked for editing (IO)\n");
            break;
        case E_BAD_PTR:
            cwrites("@Status: bad pointer (IO)\n");
            break;
        case E_OUT_OF_OBJECTS:
            cwrites("@Status: out of IO objects\n");
            break;
        case E_MORE_DATA:
            cwrites("@Status: more data available (IO)\n");
            break;
        default:
            cwrites("unknown status\n");
    }
}
