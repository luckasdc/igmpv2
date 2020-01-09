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

struct GeneralQueryBlob {
    IGMPRouter* router;
    int last_member_query_count; // hoeveel nog versturen

    GeneralQueryBlob(IGMPRouter* router, int lmqc) : router(router), last_member_query_count(lmqc) {}
};

int IGMPRouter::initialize(ErrorHandler* errh) {
    this->state = new RouterState();
    if (this->robustness_variable < 2) return -1;
    try {
        this->general_query_timer.initialize(this, true);
        this->general_query_timer.assign(IGMPRouter::send_general_query,
                                         new GeneralQueryBlob(this, this->last_member_query_count));
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
            }
            if (this->state->listening(destination, source, 2)) {
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
    // not finished
    try {
        if (not checkQuery(p)) {
            p->kill();
            return;
        }
        IPAddress source = p->ip_header()->ip_src;
        auto query = (MembershipQuery*) (p->data() + p->ip_header_length());

        if(query->qrv > 2){
            this->robustness_variable = query->qrv;
        }

        click_chatter("Router:\tHandled Query. Size of records: %d");
        p->kill();
    }
    catch (...) {
        p->kill();
    }
}


WritablePacket* generate_leave_query(IGMPRouter* router, int interface, IPAddress multicast) {
    WritablePacket* packet = Packet::make(sizeof(click_ip) + sizeof(click_ether), nullptr, sizeof(MembershipQuery), 0);
    memset(packet->data(), 0, packet->length());
    packet->set_dst_ip_anno(multicast);
    MembershipQuery* membership_query = (MembershipQuery*) (packet->data());
    membership_query->setup();
    membership_query->group_address = multicast;
    membership_query->qrv = router->robustness_variable;
    membership_query->set_query_interval_code(router->last_member_query_interval / 10);
    membership_query->set_max_response_code(router->last_member_query_interval);
    membership_query->checksum = 0;
    membership_query->checksum = click_in_cksum(packet->data(), packet->length());
    packet->set_anno_u8(PAINT_ANNO_OFFSET, 1);
    return packet;
}

struct LeaveBlob {
    IGMPRouter* router;
    IPAddress multicast;
    int last_member_query_count; // hoeveel nog versturen
    int interface;
    unsigned int time_schedule; // na hoeveel tijd terug versturen (in ms)

    LeaveBlob(IGMPRouter* router, IPAddress multicast, int last_member_query_count, int interface,
              unsigned int time) : router(
        router), multicast(multicast), last_member_query_count(last_member_query_count), interface(interface),
                                   time_schedule(time) {}
};


void IGMPRouter::send_specific_query(Timer* timer, void* pVoid) {
    auto blob = (LeaveBlob*) pVoid;
    blob->last_member_query_count -= 1;
    if (blob->last_member_query_count >= 0) {
        auto packet = generate_leave_query(blob->router, blob->interface, blob->multicast);
        blob->router->output(blob->interface).push(packet);
        timer->reschedule_after_ms(blob->time_schedule);
    } else {
        delete timer;
    }
}

void IGMPRouter::delete_group(Timer* timer, void* pVoid) {
    auto group_state = (GroupState*) pVoid;
    group_state->group_timer = nullptr;
    delete timer;
    if (not group_state->has_replied) {
        // Didn't replied
        group_state->router_state->delete_group(group_state);
        delete group_state;
    }
}


void IGMPRouter::set_leave_timers(int interface, IPAddress source, IPAddress multicast) {
    GroupState* group_state = this->state->get_group(interface, multicast);

    // Check if timer is already set
    if (group_state->group_timer != nullptr) return;
    group_state->has_replied = false;

    // Group Timer
    group_state->group_timer = new Timer(IGMPRouter::delete_group, group_state);
    group_state->group_timer->initialize(this);
    click_chatter("Will delete after %d", this->last_member_query_timer());
    group_state->group_timer->schedule_after_sec(this->last_member_query_timer() / 10); //ds

    unsigned int time_schedule_query = 100 * last_member_query_interval;
    auto blob = new LeaveBlob(this, multicast, this->last_member_query_count, interface, time_schedule_query);
    Timer* query_timer = new Timer(IGMPRouter::send_specific_query, blob);
    query_timer->initialize(this);
    query_timer->schedule_now();
    return;
}

void IGMPRouter::received_igmp_report(int port, Packet* p) {

    try {
        if (not checkQuery(p)) {
            p->kill();
            return;
        }
        IPAddress source = p->ip_header()->ip_src;
        // parse report
        MembershipReport* report = (MembershipReport*) (p->data() + p->ip_header_length());
        // get group records
        uint16_t n = ntohs(report->n_group_records);
        auto records = report->get_group_records(p, n);

        for (int i = 0; i < n; i++) {
            // zoek in table naar de group state

            click_chatter(" - A group record: %s", records[i]->multicast_address.unparse().c_str());
            switch (records[i]->record_type) {
                case IN_TO_EX: {
                    this->state->RouterState::find_insert_group_state(port, source, records[i]->multicast_address);
                    break;
                }
                case EX_TO_IN: {
                    this->set_leave_timers(port, source, records[i]->multicast_address);
                    break;
                }
                case IN: {
                    break;
                    // NADA
                }
                case EX: {
                    this->state->RouterState::find_insert_group_state(port, source, records[i]->multicast_address);
                    break;
                }
            }
        }

        click_chatter("Router:\tHandled Report. Size of records: %d", records.size());
        p->kill();
    }
    catch (...) {
        p->kill();
    }
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
    membership_query->qrv = (uint8_t) router->robustness_variable;
    membership_query->set_max_response_code(router->query_response);
    membership_query->set_query_interval_code(router->query_interval);
    membership_query->checksum = 0;
    membership_query->checksum = click_in_cksum(packet->data(), packet->length());
    // Paint the packet with color 1 so it gets forward to the right place
    packet->set_anno_u8(PAINT_ANNO_OFFSET, 1);
    return packet;
}

void IGMPRouter::set_general_timer() {

}


void IGMPRouter::general_delete(Timer* timer, void* pVoid) {
    delete timer;
    auto router = (IGMPRouter*) pVoid;
    router->state->delete_not_replied_general_query();
}

void IGMPRouter::send_general_query(Timer* timer, void* pVoid) {
    click_chatter("Router: Sending General Query");
    auto blob = (GeneralQueryBlob*) pVoid;
    for (int i = 0; i < blob->router->noutputs(); i++) {
        blob->router->output(i).push(generate_general_query(blob->router));
    }
    blob->router->general_query_timer.reschedule_after_sec(blob->router->query_interval);
    // trigger delete timer if count is zero
    if (blob->last_member_query_count == 0) {
        click_chatter("Kill all didn't replied.");
        auto general_timer = new Timer(IGMPRouter::general_delete, blob->router);
        general_timer->initialize(blob->router);
        general_timer->schedule_after_sec(blob->router->group_member_interval());
        blob->last_member_query_count = blob->router->robustness_variable;
    }
    blob->last_member_query_count -= 1;
}

void IGMPRouter::send_other_query(Timer* timer, void* ptr) {
    auto router = (IGMPRouter*) ptr;
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

bool IGMPRouter::checkQuery(Packet* p) {
    // TODO IP header  checken
    MembershipReport* report = (MembershipReport*) (p->data() + p->ip_header_length());

    // One's complement of total payload is done by adding the bitwise operator & 0xFFFF
    // must be equal to 0
    return (click_in_cksum((unsigned char*) report, p->length() - p->ip_header_length()) & 0xFFFF) == 0;

}


CLICK_ENDDECLS
ELEMENT_REQUIRES(IGMPRouterFilter)

EXPORT_ELEMENT(IGMPRouter)
