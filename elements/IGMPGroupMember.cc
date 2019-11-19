#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IGMPGroupMember.hh"


CLICK_DECLS
IGMPGroupMember::IGMPGroupMember()
{}

IGMPGroupMember::~ IGMPGroupMember()
{}

int IGMPGroupMember::configure(Vector<String> &conf, ErrorHandler *errh) {
    if (Args(conf, this, errh).read_m("MAXPACKETSIZE", maxSize).complete() < 0) return -1;
    if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
    return 0;
}

void IGMPGroupMember::push(int, Packet *p){
    click_chatter("Got a packet of size %d",p->length());
    if (p->length() > maxSize)  p->kill();
    else output(0).push(p);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPGroupMember)
