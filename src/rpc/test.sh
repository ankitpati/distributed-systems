#!/bin/bash

test $# -ne 2 && {
    printf 'Usage:\n\t ./test.sh <number 1> <number 2>\n' >&2
    exit 1
}

rpcgen add.x && \
gcc -Wall -Wextra \
    -Wpedantic -Wno-unused-result -Wno-unused-parameter -Wno-unused-variable \
    -o add_server addnum.c add_xdr.c add_svc.c -lnsl && \
gcc -Wall -Wextra \
    -Wpedantic -Wno-unused-result -Wno-unused-parameter -Wno-unused-variable \
    -o add_clnt passnum.c add_xdr.c add_clnt.c -lnsl && \
rpcinfo &> /dev/null &&
./add_server &

sleep 1

./add_clnt localhost $@
killall add_server
