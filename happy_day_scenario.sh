#! /bin/bash

pclient21=10003
pclient22=10004
pclient31=10005
pclient32=10006

sleep 2

# client21 joins
echo "write client21/igmp.join 224.4.4.4" | telnet localhost $pclient21
sleep 5

# client31 joins
echo "write client31/igmp.join 224.4.4.4" | telnet localhost $pclient31
sleep 5

# client32 joins
echo "write client32/igmp.join 224.4.4.4" | telnet localhost $pclient32
sleep 5

# client21 leaves
echo "write client21/igmp.leave 224.4.4.4" | telnet localhost $pclient21
sleep 5

# client32 leaves
echo "write client32/igmp.leave 224.4.4.4" | telnet localhost $pclient32
sleep 5

# client22 joins
echo "write client22/igmp.join 224.4.4.4" | telnet localhost $pclient22
sleep 5

# client31 leaves
echo "write client31/igmp.leave 224.4.4.4" | telnet localhost $pclient31
sleep 5

# client22 leaves
echo "write client22/igmp.leave 224.4.4.4" | telnet localhost $pclient22
sleep 5