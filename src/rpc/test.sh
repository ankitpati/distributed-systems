#!/usr/bin/env bash

myname="$(basename "$0")"
mydir="$(dirname "$0")/"

test "$#" -ne 2 && echo "Usage: $myname <number 1> <number 2>" && exit 1

cd "$mydir" || exit 2

source '/etc/os-release'

case "$NAME" in
'Fedora')
    sudo dnf install gcc rpcbind libtirpc-devel
    ;;
'Ubuntu')
    sudo apt install gcc rpcbind libtirpc-dev
    ;;
*)
    echo 'Installation unsupported on this platform. May still run, though.'
    ;;
esac

sudo systemctl start rpcbind.service

sudo -k # drop privileges

rpcgen 'add.x' && \
gcc -Wall -Wextra \
    -Wpedantic -Wno-unused-result -Wno-unused-parameter -Wno-unused-variable \
               -Wno-cast-function-type \
    -o 'add_server' 'addnum.c' 'add_xdr.c' 'add_svc.c' \
    -I'/usr/include/tirpc' -ltirpc && \
gcc -Wall -Wextra \
    -Wpedantic -Wno-unused-result -Wno-unused-parameter -Wno-unused-variable \
               -Wno-cast-function-type \
    -o 'add_clnt' 'passnum.c' 'add_xdr.c' 'add_clnt.c' \
    -I'/usr/include/tirpc' -ltirpc && \
rpcinfo &> /dev/null && \
./add_server &

sleep 1

./add_clnt localhost "$@"
killall add_server

: '
OUTPUT

./test 43.5 5.53
43.500000 + 5.430000 = 48.930000
'
