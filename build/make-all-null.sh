#!/bin/bash
#
# build/clean all
#
# Created by Sandesh Kumar Sodhi, February 2018
# Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
#
# All rights reserved.
#
# Notice and Disclaimer: This code is licensed to you under the Apache
# License 2.0 (the "License"). You may not use this code except in compliance
# with the License. This code is not an official Juniper product. You can
# obtain a copy of the License at http://www.apache.org/licenses/LICENSE-2.0
#
# Third-Party Code: This code may depend on other components under separate
# copyright notice and license terms. Your use of the source code for those
# components is subject to the terms and conditions of the respective license
# as noted in the Third-Party source code file.
#

#
# Usage:
#   make-all-null.sh clean
#   make-all-null.sh
#   make-all-null.sh install --destdir <destdir> --prefix <subdir> --confdir <subdir>
#     destdir => installation directory
#     prefix  => subdirectory under destdir
#                JP4Agent lib64 and include sub-directories are created below this
#     confdir => subdirectory under destdir
#                JP4Agent config sub-directory is created below this
#     Example command:
#       make-all-null.sh install --destdir /opt --prefix /usr --confdir /etc
#

me=`basename "$0"`
REPO_DIR=..

components=(
    $REPO_DIR/AFI
    $REPO_DIR/src/pi/protos
    $REPO_DIR/src/jp4agent/src
    $REPO_DIR/src/utils/src
    $REPO_DIR/src/pi/src
    $REPO_DIR/src/afi/src
    $REPO_DIR/test/controller/src
    $REPO_DIR/test/gtest/src
    $REPO_DIR/src/targets/null/null/src
    $REPO_DIR/src/targets/null/src
)

# Parse arguments
cmd=""

while (( "$#" )); do
  case "$1" in
    --destdir)
      destdir=$2
      shift 2
      ;;
    --prefix)
      prefix=$2
      shift 2
      ;;
    --confdir)
      confdir=$2
      shift 2
      ;;
    --) # end
      shift
      break
      ;;
    -*|--*=) # unsupported flags
      echo "Error: Unsupported flag $1" >&2
      exit 1
      ;;
    *) # positional arguments
      cmd="$cmd $1"
      shift
      ;;
  esac
done

# set positional arguments in their proper place
eval set -- "$cmd"

if [ $cmd == "clean" ]
then
    for component in "${components[@]}"
    do
        echo "===== Running make clean under ${component} ====="
        make clean -C ${component} UBUNTU=1
    done
    echo "===== Deleting all obj directories ====="
    cd $REPO_DIR; find . -name obj | xargs rm -rf
elif [ $cmd == "install" ]
then
    for component in "${components[@]}"
    do
        echo "===== Running make install under ${component} ====="
        make install -C ${component} DESTDIR=${destdir} PREFIX=${prefix} CONFDIR=${confdir}
        if [ $? -ne 0 ]
        then
          echo "$me: !!!!!!!!!!!ERROR!!!!!!!!!!" 
          echo "$me: !!!!!!!!!!!ERROR!!!!!!!!!!" 
          echo "$me: !!!!!!!!!!!ERROR!!!!!!!!!!" 
          exit 1
        fi
    done
    echo "Installation done!!!"
else
    make srcs -C $REPO_DIR/AFI
    for component in "${components[@]}"
    do
        echo "===== Running make under ${component} ====="
        make -j4 -C ${component} UBUNTU=1 DEBUG_BUILD=1 CODE_COVERAGE=1
        if [ $? -ne 0 ]
        then
          echo "$me: !!!!!!!!!!!ERROR!!!!!!!!!!" 
          echo "$me: !!!!!!!!!!!ERROR!!!!!!!!!!" 
          echo "$me: !!!!!!!!!!!ERROR!!!!!!!!!!" 
          exit 1
        fi
    done
    echo "$me: SUCCESS!!!"
    #
    # Regression script 'docker_run_ut.expect' use this message to detect compilation status.
    # If you change this message, please make the same change in 'docker_run_ut.expect'.
    #
    echo "JP4Agent binaries compilation success!!"
fi
exit 0
