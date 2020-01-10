#! /bin/bash

client21=10003
client22=10004
client31=10005
client32=10006

sleep 2

# client21 joins
echo "write client21/igmp.join 224.4.4.4" | telnet localhost $client21
sleep 5

# client31 joins
echo "write client31/igmp.join 224.4.4.4" | telnet localhost $client31
sleep 5

# client32 joins
echo "write client32/igmp.join 224.4.4.4" | telnet localhost $client32
sleep 5

# client21 leaves
echo "write client21/igmp.leave 224.4.4.4" | telnet localhost $client21
sleep 5

# client32 leaves
echo "write client32/igmp.leave 224.4.4.4" | telnet localhost $client32
sleep 5

# client22 joins
echo "write client22/igmp.join 224.4.4.4" | telnet localhost $client22
sleep 5

# client31 leaves
echo "write client31/igmp.leave 224.4.4.4" | telnet localhost $client31
sleep 5

# client22 leaves
echo "write client22/igmp.leave 224.4.4.4" | telnet localhost $client22
sleep 5