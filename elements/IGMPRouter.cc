#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include "IGMPRouter.hh"
#include "IGMPMessage.hh"


CLICK_DECLS

class MembershipReportExtended;


int IGMPRouter::initialize(ErrorHandler* errh) {
    try {
        this->general_query_timer.initialize(this, true);
        this->general_query_timer.assign(IGMPRouter::send_general_query, this);
        this->general_query_timer.schedule_now();
    } catch (...) {
        return -1;
    }
    return 0;
}

int IGMPRouter::configure(Vector <String> &conf, ErrorHandler* errh) {
//    if (Args(conf, this, errh).read_m("MAXPACKETSIZE", maxSize).complete() < 0) return -1;
//    if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
    return 0;
}

void IGMPRouter::push(int port, Packet* p) {
    click_chatter("Got a packet of size %d", p->length());
    if (p->length() > 10000000000) p->kill();

    this->output(port).push(p);

//    switch (port) {
//        // packets from server
//        case 0: {
//            auto ip_h = (click_ip*) p->data();
//            // this->filter.is_listening_to(ip_h->ip_dst, ip_h->ip_src)
//            // IF the router listens to this stream send via port 1 else 2
//            if (1 == 1) {
//                output(1).push(p);
//            } else {
//                output(2).push(p);
//            }
//            break;
//        }
//            // all IGMP
//        case 1: {
//            this->received_igmp(p);
//            break;
//        }
//        default: {
//            p->kill();
//            return;
//        }
//    }
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

void IGMPRouter::add_handlers() {
}


void IGMPRouter::send_general_query(Timer* timer, void* pVoid) {
    IGMPRouter* router = (IGMPRouter*) pVoid;

    WritablePacket* packet = Packet::make(sizeof(click_ip) + sizeof(click_ether), nullptr, sizeof(MembershipQuery), 0);
    memset(packet->data(), 0, packet->length());

    packet->set_dst_ip_anno(IPAddress("224.0.0.1"));
    MembershipQuery* membership_query = new(packet->data()) MembershipQuery;
    membership_query->setup();
    membership_query->group_address = IPAddress(0);
    membership_query->qrv = router->robustness_variable;


    membership_query->checksum = 0;
    membership_query->checksum = click_in_cksum(packet->data(), packet->length());

    click_chatter("hello");

    router->general_query_timer.reschedule_after_sec(10);
    for (int i = 0; i < router->noutputs(); i++) {
        router->output(i).push(packet);
    }
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouter)
