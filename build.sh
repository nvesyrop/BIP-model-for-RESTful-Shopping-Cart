#!/bin/bash
FDIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )"

set -e
set -x verbose

mkdir -p output build

bipc.sh -I . -p global_rest -d "SystemProcess()" \
   --gencpp-output-dir output  \
--gencpp-follow-used-packages \
--gencpp-enable-bip-debug \
--gencpp-cc-I $PWD/src-ext 
#--gencpp-enable-marshalling \
#--gencpp-cc-extra-src ~/Tools/gsoap-2.8/gsoap/stdsoap2.c \

cd build

cmake $(ls -d ${FDIR}/output)
make

echo "Finished. Run: ./build/system"
