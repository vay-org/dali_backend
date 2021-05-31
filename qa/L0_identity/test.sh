#!/bin/bash -ex

: ${GRPC_ADDR:=${1:-"localhost:8001"}}

netstat -tuplen

python identity_client.py -u $GRPC_ADDR
