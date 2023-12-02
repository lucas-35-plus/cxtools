#!/bin/bash
CURRENT_DIR=$(cd $(dirname $0); pwd)

build_linux() {
  if [ ! -d ${CURRENT_DIR}/build ]
  then
    mkdir ${CURRENT_DIR}/build
  fi
  cd ${CURRENT_DIR}/build
  rm -rf *
  cmake -DCMAKE_BUILD_TYPE=Release ../
  make
}

build_win() {
  if [ ! -d ${CURRENT_DIR}/build_win ]
  then
    mkdir ${CURRENT_DIR}/build_win
  fi
  cd ${CURRENT_DIR}/build_win
  rm -rf *
  cmake -DCMAKE_TOOLCHAIN_FILE=CMake/x86_64-w64-mingw32.cmake \
        -DMINGW32=1 \
        -DCMAKE_BUILD_TYPE=Release \
        ../
  make
}

for i in "$@"; do
    if [ ${i} == 'linux' ]
    then
      build_linux
    elif [ ${i} == 'win' ]
    then
      build_win
    else
      echo "${i} not impl"
    fi
done