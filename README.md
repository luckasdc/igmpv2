## IGMP - Telecom Project 2019-20

### Handler Usage
telnet localhost {portnumber} where portnumber is:
- router: 10001
- client21: 10003
- client22: 10004
- client31: 10005
- client31: 10006
- server: 10002

list objects: *READ list*

### TODO CLIENT
- [x] GroupMember must receive queries (Luckas)
- [x] implement mode reports (similar to change reports) (Luckas)
- [x] GroupMember can answer queries by sending a reports (Luckas)
- [x] Checksum van query valideren
- [x] QRV Handler
- [x] apply QRV while sending reports
- [x] Do not reply instantly to queries, check 5.2
- [ ] add group-specific reports
- [ ] add expired timer cheking (p 23 en 24)

### TODO CLIENT


### TODO GENERAL
- [x] Proper solution for variables RV, URI, ...
- [ ] TODO IP header  checken 
- [ ] TODO UDP header checken
- [ ] TODO mss in click element steken ip
- [ ] Check + TEST code for max resp time


### BUGS TO FIX






### Things to write in verslagske
- All time ranges are expressed in miliseconds
