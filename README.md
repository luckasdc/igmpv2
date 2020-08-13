## IGMP - Telecom Project 2019-20
#### Stan Schepers & Luckas Declerck (Zit 1)
#### Luckas Declerck (Zit 2)

This project implements IGMPv3 on the Click Modular Router. The solution is interoperable
using the given reference platform. The implementation can executed by running ```sudo ./start_click.sh``` 
in the ```click/scripts``` folder. Make sure to comment out the elements in this file when running both this and 
the reference implementation. 

All the checksums of in- and outgoing IGMP packets will be checked, as well as the the checksums of the IP headers (as a seperate click element)

### Basic Usage
Clients can join/leave multicasts group by calling the following handlers: ``write clientXX/igmp.join [multicast IP]`` and
  ``write clientXX/igmp.leave [multicast IP]``. These lines can be executed in the telnet panel
  ``telnet localhost {portnumber}`` where portnumber is:
- client21: 10003
- client22: 10004
- client31: 10005
- client31: 10006

The router will handle these state changes internally, output will be visible in the console or in the ```.pcap``` files.

### Extra Implementation
- A client can join multiple multicast addresses (tested in worst_case_scenario)
- Checksums of the IGMP header, IP header and UDP header will be checked (tested in worst_case_scenario)
- IGMP Packages with an unknown type (other than Query/Report) will be silently ignored
- Leaving unjoined resources will be silently ignored
- Handler for simulating a client that won't respond

### Scenarios
Our project comes with 2 scenario's: the ```happy_day_scenario.sh``` end the ```worst_case_scenario.sh```. In the first 
one, the given scenario (blackboard) will be executed. The second one will stress-test the router and 
clients by executing the following: 
- Client joins a server that does not exist
- Client leaves a group where he's not joined to  
- Client joins two times consecutively
- Client will be killed, router must stop forwarding after timer has passed
Just run these test anywhere on your PC while the clickfiles are running.

### Extra Handlers
We used the given handlers of the reference implementation to test our created elements. But for some specific reasons
we created our own.
- a Kill Client handler. This handler will can be called to simulate a dying client, which will result in 
not answering to the general queries. Can be undone by calling it again. 
``write clientXX/igmp.kill``

### Nice to know
- All the variables that can be changed are found in ````defaults.hh````. 
- Since our packets were encapped outside of the IGMPRouter, and click doens't natively support to 
add info to the IP header, we needed to come up with our own solution without completely rewriting
the flow. We cannot override Click classes, so we've imported the code of the element ``IPEncap`` 
and modified it to add the Router Alert option. (and recalculation of checksum). We've therefore 
renamed it to ```IPEncapDeluxe```.
- The UDP data and IGMP messages use the same output, but the UDP data doesn't need an extra header. 
To split the flow of these packets, they will be colored using color 1 or 0, and ````PaintSwitch```` 
will distribute them accordingly.

