#include <iostream>
#include "umthread.h"

using namespace std;


void hello(void *p) {

    for (long long i = 0; i < 160000000; i++) {
        if (i % 10000000 == 0) {
            printf("%lli\n", i);
        }
    }
}

void world(void *p) {

    for (long long i = 0; i < 320000000; i++) {
        if (i % 10000000 == 0) {
            printf("%lli\n", i);
        }
    }
}

int main() {

    umthread_create(hello, NULL);
    umthread_create(hello, NULL);
    umthread_create(world, NULL);

    return 0;
}
