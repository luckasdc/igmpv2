#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include "IGMPMessage.hh"
#include "IGMPRouter.hh"




CLICK_DECLS


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
    click_chatter("Router:\tReceived IGMP Query");
    auto data = p->data();
    // Pass effkes
}

void IGMPRouter::received_igmp_report(Packet* p) {
    click_chatter("Router:\tReceived IGMP Report");
    auto data = p->data();

    // make report
    auto report = (MembershipReport*) data;

    // loop over group_records
    uint16_t n_group_records = ntohs(report->n_group_records);
    auto group_record = (GroupRecord*) (data + sizeof(MembershipReport));

    for (int i = 0; i < n_group_records; i++) {
        // zoek in table naar de group state


        // check state

        // maak aanpassingen

    }
    click_chatter("Router:\tHandled Report");
    p->kill();
}

void IGMPRouter::add_handlers() {
}


WritablePacket* generate_general_query(IGMPRouter* router) {
    WritablePacket* packet = Packet::make(sizeof(click_ip) + sizeof(click_ether), nullptr, sizeof(MembershipQuery), 0);
    memset(packet->data(), 0, packet->length());

    packet->set_dst_ip_anno(IPAddress("224.0.0.1"));
    MembershipQuery* membership_query = (MembershipQuery*) (packet->data());
    membership_query->setup();
    membership_query->group_address = IPAddress(0);
    membership_query->qrv = router->robustness_variable;
    membership_query->checksum = 0;
    membership_query->checksum = click_in_cksum(packet->data(), packet->length());
    return packet;
}

void IGMPRouter::send_general_query(Timer* timer, void* ptr) {
    IGMPRouter* router = (IGMPRouter*) ptr;
    click_chatter("hello");
    router->general_query_timer.reschedule_after_sec(10);
    for (int i = 0; i < router->noutputs(); i++) {
        router->output(i).push(generate_general_query(router));
    }
}

void IGMPRouter::send_other_query(Timer* timer, void* ptr) {
    auto router = (IGMPRouter*) ptr;
    router->other_querier = false;
    router->general_query_timer.reschedule_after_sec(10);
}

void IGMPRouter::send_group_specific_query(Timer* timer, void* ptr) {
    auto router = (IGMPRouter*) ptr;

    MembershipQuery query;
    query.setup();
    query.set_max_response_code(router->last_member_query_interval);
    query.group_address = router->_group_address;
    router->_group_address = IPAddress("0.0.0.0");

}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPRouter)
