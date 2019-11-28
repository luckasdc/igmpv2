#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include "IGMPRouter.hh"
#include "IGMPMessage.hh"


CLICK_DECLS


int IGMPRouter::configure(Vector <String> &conf, ErrorHandler* errh) {
    if (Args(conf, this, errh).read_m("MAXPACKETSIZE", maxSize).complete() < 0) return -1;
    if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
    return 0;
}

void IGMPRouter::push(int port, Packet* p) {
    click_chatter("Got a packet of size %d", p->length());
    if (p->length() > maxSize) p->kill();

    switch (port) {
        case 0: {
            auto ip_h = (click_ip*) p->data();
            // this->filter.is_listening_to(ip_h->ip_dst, ip_h->ip_src)
            if (1==1) {
                output(1).push(p);
            } else {
                output(2).push(p);
            }
            break;
        }
        case 1: {
            this->received_igmp(p);
            break;
        }
        default: {
            p->kill();
            return;
        }
    }
}

void IGMPRouter::received_igmp(Packet* p) {
    click_chatter("Received IGMP Packet");

    auto data = p->data();

    auto igmp_member_header = reinterpret_cast<MembershipReport*>(p);
    // Check if membership query
    // Check if report
    switch (igmp_member_header->type) {
        case 17:
            this->received_igmp_query(p);
        case 34:
            this->received_igmp_report(p);
    }
    p->kill();
}

void IGMPRouter::received_igmp_query(Packet* p) {
    click_chatter("Received IGMP Query");
    auto data = p->data();
    // Pass effkes
}

void IGMPRouter::received_igmp_report(Packet* p) {
    click_chatter("Received IGMP Report");
    auto data = p->data();

}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouter)
