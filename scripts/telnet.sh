echo "write client21/igmp.join 224.4.4.4" | telnet localhost 10003
sleep 20
echo "write client21/igmp.leave 224.4.4.4" | telnet localhost 10003