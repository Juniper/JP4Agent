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

import grpc
import jp4cli_pb2
import jp4cli_pb2_grpc

p4_cmds_full = ['inject-l2-pkt <sandbox-index> <port-index>: Inject layer 2 packet',
                'add-ether-encap <src-mac> <dst-mac> <0 or inner-vlan-id> <0 or outer-vlan-id> <output-port-token>',
                'set-input-port-next-node <port-index> <next-node-token>',
                'add-route <rtt-token> <prefix> <next-node-token>',
                'add-receive <receive-code> <context>']

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
        print("Cmd output: " + response.cmdout)

def run_cli():
    """
    Main fn: Loop and get input from user.
    """
    channel = grpc.insecure_channel('localhost:53421')
    stub = jp4cli_pb2_grpc.CmdHandlerStub(channel)
    p4_cmds = ['cross-connect', 'inject-l2-pkt', 'add-ether-encap',
               'set-input-port-next-node']
    p4_cmds_completer = WordCompleter(p4_cmds, sentence=True)
    p4_cmd_history = InMemoryHistory()

    try:
        while True:
            usr_cmd = prompt('JP4Agent> ', completer=p4_cmds_completer,
                             history=p4_cmd_history)
            send_cmd(stub, usr_cmd)
    except (KeyboardInterrupt, EOFError) as e:
        print('Exiting JP4Agent CLI.')


if __name__ == '__main__':
    run_cli()
