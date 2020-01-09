## IGMP - Telecom Project 2019-20
#### Stan Schepers & Luckas Declerck

This project implements IGMPv3 on the Click Modular Router. The solution is interoperable
using the given reference platform. The implementation can executed by running ```sudo ./start_click.sh``` 
in the ```click/scripts``` folder. Make sure to comment out the elements in this file when running both this and 
the reference implementation. 

### Basic testing
Clients can join/leave multicasts group by calling the following handlers: ``write clientXX/igmp.join [multicast IP]`` and
  ``write clientXX/igmp.leave [multicast IP]``. These lines can be executed in the telnet panel
  ``telnet localhost {portnumber}`` where portnumber is:
- client21: 10003
- client22: 10004
- client31: 10005
- client31: 10006

The router will handle these state changes internally, output will be visible in the console or in the ```.pcap``` files.

### Scenarios
Our project comes with 2 scenario's: the ```happy_day_scenario.sh``` end the ```worst_case_scenario.sh```. In the first 
one, the given scenario (blackboard) will be executed. The second one will stress-test the router and 
clients by executing the following: 
- xxx
- xxx 
- xxx

### Extra Handlers
We used the given handlers of the reference implementation to test our created elements. But for some specific reasons
we created our own.
- xxx
- xxx
- xxx



### TODO GENERAL
- [ ] Router Alert option
- [ ] add expired timer cheking (p 23 en 24)
