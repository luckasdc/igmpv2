#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/packet.hh>
#include <clicknet/ip.h>
#include <clicknet/ether.h>
#include <click/timer.hh>
#include "IGMPMessage.hh"
#include "IGMPRouter.hh"
#include "IGMPRouterFilter.hh"
#include <click/packet_anno.hh>




CLICK_DECLS

using namespace router;


int IGMPRouter::initialize(ErrorHandler* errh) {
    this->state = new RouterState();
    if (this->robustness_variable < 2) return -1;
    try {
        this->general_query_timer.initialize(this, true);
        this->general_query_timer.assign(IGMPRouter::send_general_query, this);
        this->general_query_timer.schedule_now();
    } catch (...) {
        return -1;
    }
    return 0;
}

bool IGMPRouter::listening(IPAddress multicast, IPAddress source, int interface) {
//    if (multicast == defaults::all_systems_multicast_address) return true;
//    if (multicast == defaults::report_address) return true;
//
//    auto it = this->state->group_states.find(interface);
//    if (it == this->state->group_states.end()) {
//        click_chatter("Router: interface doesn't exist in state.");
//        return false;
//    }
//    auto group_state = this->state->group_states[interface];
//
//    return group_state->listening(source);
}

void IGMPRouter::push(int port, Packet* p) {

    //this->output(port).push(p);

    switch (port) {
        // packets from server
        case 0: {
            IPAddress source = p->ip_header()->ip_src;
            IPAddress destination = p->ip_header()->ip_dst;
            // Paint the packet with color 0 so it gets forward to the right place
            p->set_anno_u8(PAINT_ANNO_OFFSET, 0);

            // this->filter.is_listening_to(ip_h->ip_dst, ip_h->ip_src)
            // IF the router listens to this stream send via port 1 else 2
            if (this->state->listening(destination, source, 1)) {
                output(1).push(p->clone()->uniqueify());
            } if (this->state->listening(destination, source, 2)) {
                output(2).push(p->clone()->uniqueify());
            } else {
                p->kill();
            }
            break;
        }
            // IGMP port 1
        case 1: {
            this->received_igmp(port, p);
            break;
        }
            // IGMP Port 2
        case 2: {
            this->received_igmp(port, p);
            break;
        }
        default: {
            p->kill();
            return;
        }
    }
}

void IGMPRouter::received_igmp(int port, Packet* p) {
    uint8_t* type = (uint8_t * )(p->data() + p->ip_header_length());
    // TODO VALIDATE

    // Check if report
    switch (*type) {
        case QUERY:
            this->received_igmp_query(port, p);
            return;
        case REPORT:
            this->received_igmp_report(port, p);
            return;
    }
    p->kill();
}

void IGMPRouter::received_igmp_query(int port, Packet* p) {
    click_chatter("Router:\tReceived IGMP Query");
    // Pass effkes
}

void IGMPRouter::received_igmp_report(int port, Packet* p) {

    // TODO try catch zoals client
    IPAddress source = p->ip_header()->ip_src;

    // make report
    MembershipReport* report = (MembershipReport*) (p->data() + p->ip_header_length());
    // get group records
    uint16_t n = ntohs(report->n_group_records);
    auto records = report->get_group_records(p, n);

    for (int i = 0; i < n; i++) {
        // zoek in table naar de group state
        click_chatter(" - A group record: %s on port %d", records[i]->multicast_address.unparse().c_str(), port);
        this->state->RouterState::find_insert_group_state(port, source, records[i]->multicast_address);
    }

    click_chatter("Router:\tHandled Report. Size of records: %d", records.size());
    p->kill();
}

void IGMPRouter::add_handlers() {
}


WritablePacket* generate_general_query(IGMPRouter* router) {
    WritablePacket* packet = Packet::make(sizeof(click_ip) + sizeof(click_ether), nullptr, sizeof(MembershipQuery), 0);
    memset(packet->data(), 0, packet->length());
    packet->set_dst_ip_anno(defaults::all_systems_multicast_address);
    MembershipQuery* membership_query = (MembershipQuery*) (packet->data());
    membership_query->setup();
    membership_query->group_address = IPAddress(0);
    membership_query->qrv = router->robustness_variable;
    membership_query->checksum = 0;
    membership_query->checksum = click_in_cksum(packet->data(), packet->length());
    // Paint the packet with color 1 so it gets forward to the right place
    packet->set_anno_u8(PAINT_ANNO_OFFSET, 1);
    return packet;
}

void IGMPRouter::send_general_query(Timer* timer, void* ptr) {
    click_chatter("Router: Sending General Query");
    IGMPRouter* router = (IGMPRouter*) ptr;
    router->general_query_timer.reschedule_after_sec(router->query_interval);
    for (int i = 0; i < router->noutputs(); i++) {
        router->output(i).push(generate_general_query(router));
    }
}

void IGMPRouter::send_other_query(Timer* timer, void* ptr) {
    auto router = (IGMPRouter*) ptr;
    router->other_querier = false;
    router->general_query_timer.reschedule_after_sec(router->query_interval);
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
ELEMENT_REQUIRES(IGMPRouterFilter)
EXPORT_ELEMENT(IGMPRouter)
