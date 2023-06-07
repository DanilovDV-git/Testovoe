#!/usr/bin/bash

#Start server and auto client in another terminal tabs
tab="--tab"
foo=""

cmd1="bash -c './server/tcp_server 5555';bash"
foo+=($tab -e "$cmd1")

cmd2="bash -c './auto_client/tcp_client 127.0.0.1 5555 user1 1234';bash"
foo+=($tab -e "$cmd2")

cmd3="bash -c './auto_client/tcp_client 127.0.0.1 5555 user1 1234';bash"
foo+=($tab -e "$cmd3")

cmd4="bash -c './auto_client/tcp_client 127.0.0.1 5555 user2 1234';bash"
foo+=($tab -e "$cmd4")

gnome-terminal "${foo[@]}"

exit 0
