//
// Created by hackeris on 16-5-30.
//

#include <iostream>
#include <sys/ucontext.h>
#include <ucontext.h>
#include <cstring>

using namespace std;

int main() {

    ucontext_t context;

    memset(&context, 0, sizeof(context));

    getcontext(&context);

    printf("Hello World!\n");

    setcontext(&context);

    return 0;
}