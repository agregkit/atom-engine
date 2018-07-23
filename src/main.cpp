#include "swapserver.h"

int main() {
    CSwapServer swapServer;
    if (!swapServer.run()) {
        return -1;
    }
    return 0;
}
