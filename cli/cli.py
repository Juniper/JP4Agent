#!/usr/bin/env python3

# Created by Karthikeyan Sivaraj, February 2018
# Copyright (c) [2018] Juniper Networks, Inc. All rights reserved.
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

"""
CLI for JP4Agent commands.
"""
from prompt_toolkit import prompt
from prompt_toolkit.history import InMemoryHistory
from prompt_toolkit.contrib.completers import WordCompleter

import sys
import grpc
import jp4cli_pb2
import jp4cli_pb2_grpc

p4_cmds_full = ['add-table <table-name> <key-field> <protocol-num> <default-next-obj> <table-size>',
                'add-table-entry <table-name> <prefix> <prefix-length>',
                'show-afi-objects']

cli_cmds = ['help', 'quit']

def display_usage():
    for cmd in p4_cmds_full:
        print(cmd)

def send_cmd(stub, usr_cmd):
    try:
        response = stub.SendCmd(jp4cli_pb2.CmdRequest(cmdstr=usr_cmd))
    except grpc.RpcError as e:
        status_code = e.code()
        if grpc.StatusCode.UNAVAILABLE == status_code:
            print('Couldn\'t connect to JP4Agent. Please ensure JP4Agent is up and running.')
        else:
            print(e.details())
    else:
        print(response.cmdout)

def run_cli():
    """
    Present a prompt and get input from the user.
    """

    # Connect to command handler grpc server
    cli_serv_addr = 'localhost:53421'
    channel = grpc.insecure_channel(cli_serv_addr)
    stub = jp4cli_pb2_grpc.CmdHandlerStub(channel)

    # Setup prompt
    p4_cmds = [cmd.split()[0] for cmd in p4_cmds_full] + cli_cmds
    p4_cmds_completer = WordCompleter(p4_cmds, sentence=True)
    p4_cmd_history = InMemoryHistory()

    while True:
        # Get user command
        try:
            usr_cmd = prompt('JP4Agent> ', completer=p4_cmds_completer, history=p4_cmd_history)
        except (KeyboardInterrupt, EOFError) as e:
            continue

        if not usr_cmd:
            continue
        elif usr_cmd in ['help', 'h', '?']:
            display_usage()
            continue
        elif usr_cmd in ['quit', 'exit']:
            print('Exiting JP4Agent CLI.')
            sys.exit()
        elif usr_cmd.strip().split()[0] not in p4_cmds:
            print('Invalid command. Supported commands are:')
            display_usage()
            continue
        # Valid cmd. Send to cmd server.
        send_cmd(stub, usr_cmd)

if __name__ == '__main__':
    run_cli()
