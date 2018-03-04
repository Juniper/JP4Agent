#!/bin/bash
#
# Regression scripts
#
# build_push_docker.sh : Build and push Docker to Docker hub repo
#
# Created by Sandesh Kumar Sodhi, January 2018
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

MY_DIR="$(dirname "$0")"
source "$MY_DIR/cmn.sh"

BUILD_DOCKERS=$(xmllint --xpath "//regressionConfig/buildDockers/text()" $REGRESSION_CONFIG_FILE)
PUSH_DOCKERS_TO_DOCKERHUB=$(xmllint --xpath "//regressionConfig/pushDockersToDockerHub/text()" $REGRESSION_CONFIG_FILE)

echo BUILD_DOCKERS=$BUILD_DOCKERS
echo PUSH_DOCKERS_TO_DOCKERHUB=$PUSH_DOCKERS_TO_DOCKERHUB

function docker_tag_n_push_to_docker_hub {
    #docker login

    docker tag $DOCKER_P4_TAG $DOCKER_HUB_P4_TAG
    docker tag $DOCKER_P4_VMX_TAG $DOCKER_HUB_P4_VMX_TAG
    docker push $DOCKER_HUB_P4_TAG
    docker push $DOCKER_HUB_P4_VMX_TAG
}

#
# git clone/pull repo
#
do_repo_git_pull


$ECHO ===============================================
$ECHO ===============================================
$DATE 
$ECHO BUILDING DOCKER CONTAINERS
$ECHO ===============================================
$ECHO ===============================================

if [ "$BUILD_DOCKERS" == "yes" ]; then
    cd $REPO_DIR/tools/docker/
    run_command docker build -f Dockerfile_p4 -t $DOCKER_P4_TAG .
    run_command docker build -f Dockerfile_p4_vmx -t $DOCKER_P4_VMX_TAG .
else
    log_debug "Skipping building of dockers"
fi

if [ "$PUSH_DOCKERS_TO_DOCKERHUB" == "yes" ]; then
    docker_tag_n_push_to_docker_hub $DOCKER_TAG
else
    log_debug "Skipping pushing docker image $DOCKER_TAG to docker hub"
fi

$ECHO ===============================================
$ECHO ===============================================
$DATE 
$ECHO FINISHED BUILDING DOCKER CONTAINERS
$ECHO ===============================================
$ECHO ===============================================

exit 0
