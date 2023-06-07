#!/usr/bin/bash

tab="--tab"
foo=""

for i in 1 2 3; do
	cmd="bash -c './tcp_client 127.0.0.1 5555 user$i 1234';bash"
	foo+=($tab -e "$cmd")         
done

gnome-terminal "${foo[@]}"

exit 0
