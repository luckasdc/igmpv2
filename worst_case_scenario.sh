#! /bin/bash

client21=10003
client22=10004
client31=10005
client32=10006

sleep 2

# client21 joins server that does not exist
echo "write client21/igmp.join 224.4.4.5" | telnet localhost $plient21
sleep 6

# client21 leaves group where he is not joined to
echo "write client21/igmp.leave 224.4.4.4" | telnet localhost $client21
sleep 5


# client32 joins
echo "write client32/igmp.join 224.4.4.4" | telnet localhost $client32
sleep 5

# kill client32
echo "write client32/igmp.kill" | telnet localhost $client32
sleep 10

# client21 joins two times
echo "write client21/igmp.join 224.4.4.4" | telnet localhost $client21

# client21 joins
echo "write client21/igmp.join 224.4.4.4" | telnet localhost $client21
sleep 6

# client21 leaves
echo "write client21/igmp.leave 224.4.4.4" | telnet localhost $client21
sleep 5
