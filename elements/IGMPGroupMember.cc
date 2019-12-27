#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
#include <click/timestamp.hh>
#include <clicknet/ether.h>
#include <clicknet/ip.h>
#include <clicknet/udp.h>
#include "IGMPGroupMember.hh"
#include "IGMPMessage.hh"



CLICK_DECLS
IGMPGroupMember::IGMPGroupMember()
{}

IGMPGroupMember::~ IGMPGroupMember()
{}


bool checkQuery(Packet* p) {

    // TODO IP header  checken
    // TODO UDP header checken
    // TODO mss in click element steken ip
    MembershipQuery* query = (MembershipQuery*) (p->data() + p->ip_header_length());

    //if (p->length() - p->ip_header_length() > sizeof(query)) { return false; }

    // One's complement of total payload is done by adding the bitwise operator & 0xFFFF
    // must be equal to 0
    return (click_in_cksum((unsigned char*) query, p->length() - p->ip_header_length()) & 0xFFFF) == 0;
}


int IGMPGroupMember::configure(Vector<String> &conf, ErrorHandler *errh) {

    return 0;
}

void IGMPGroupMember::push(int port, Packet *p){
    click_chatter("Got a packet of size %d on input %d", p->length(), port);
    if (port == 0) { handle_query(p); }
    else { p->kill(); }

}

void IGMPGroupMember::handle_query(Packet *p) {

    try {
        const MembershipQuery* query = (MembershipQuery*) (p->data() + p->ip_header_length());

        if (query->type == QUERY) {

            if (checkQuery(p)) {
                //click_chatter("client igmp checksum is valid!");
                Packet* packet = this->generate_report(RESPONSE_TO_QUERY, query->group_address, this);
                if (packet != nullptr) { // Check if it is connected to at least one source
                    click_chatter("packet genereated");
                    ReportData* report = new ReportData();
                    report->self = this;
                    report->generated_packet = packet;
                    uint32_t delay = rand() % query->get_max_response_time() * 10^3; // ms -> sec

                    // Send report based on the following rules:

                    // 1. Already a response scheduled, which departs before calculated delay? -> dont create new one
                    if (this->general_timer->scheduled()) {
                        click_chatter("Already a query scheduled. Calculating difference.");
                        auto difference = (this->general_timer->expiry() - Timestamp::now());
                        if ( delay < difference.msecval()) {
                            return;
                        }
                    }
                    // 2. If type is General query: cancel previous responses
                    if (query->group_address.empty() && this->general_timer != nullptr) {
                        click_chatter("Got a General Query, and will cancel previous ones");
                        if (this->general_timer->scheduled()) { // If theres a previous one, remove timer
                            this->general_timer->clear();
                        }
                        this->general_timer->assign(&IGMPGroupMember::send_general_report, report);
                        this->general_timer->initialize(this);
                        this->general_timer->schedule_after_msec(delay);
                    }

                    // 3. If Group specific AND no pending previous ones, use group timers
                    // TODO
                    click_chatter("Todo: respond to group specific query");

                    // 4. TODO

                }
            }
        }
    }
    catch (...) {
        p->kill();
        return;
    }
}

int IGMPGroupMember::join_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh) {
    Vector<String> args;
    IPAddress group;
    IGMPGroupMember* self = (IGMPGroupMember*) e;
    cp_argvec(s, args);
    if(Args(args, self, errh).read_mp("GROUP", group).complete() < 0) return -1;

    // Check if not already in table
    if (self->source_set.find(group) != self->source_set.end()) {
        return 0;
    }

    // Add current group to source_set
    self->source_set.find_insert(group, 1);
    // Generate Membership report (Sent while joining or when responding to membership query)
    Packet* packet = self->generate_report(IN_TO_EX, group, self);

    ReportData* report = new ReportData();
    report->self = self;
    report->group_address = group;
    report->type = IN_TO_EX; // TODO klopt dit?
    report->generated_packet = packet;
    report->retransmit_times = RV;

    self->output(1).push(packet->clone()->uniqueify());

    self->report_timers[group] = new Timer(&IGMPGroupMember::send_change_report, report);
    self->report_timers[group]->initialize(self);
    uint32_t delay = rand() % URI * 10^3; // sec -> ms
    click_chatter("delay added (1st send): %d", delay);
    self->report_timers[report->group_address]->schedule_after_msec(delay);

    return 0;
}

int IGMPGroupMember::leave_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh) {
    Vector<String> args;
    IPAddress group;
    IGMPGroupMember* self = (IGMPGroupMember*) e;
    cp_argvec(s, args);
    if(Args(args, self, errh).read_mp("GROUP", group).complete() < 0) return -1;

    // Check if it exists in table
    if (self->source_set.find(group) == self->source_set.end()) {
        return 0;
    }

    // Generate Membership report (Sent while joining or when responding to membership query)
    Packet* packet = self->generate_report(EX_TO_IN, group, self);
    // remove current group from source_set
    self->source_set.erase(group);

    ReportData* report = new ReportData();
    report->self = self;
    report->group_address = group;
    report->type = EX_TO_IN; // TODO klopt dit?
    report->generated_packet = packet;
    report->retransmit_times = RV;

    self->output(1).push(packet->clone()->uniqueify());

    self->report_timers[group] = new Timer(&IGMPGroupMember::send_change_report, report);
    self->report_timers[group]->initialize(self);
    uint32_t delay = rand() % URI * 10^3; // ms -> sec
    click_chatter("delay added (1st send): %d", delay);
    self->report_timers[report->group_address]->schedule_after_msec(delay);
    return 0;
}

void IGMPGroupMember::add_handlers() {
    add_write_handler("join", join_group_handler);
    add_write_handler("leave", leave_group_handler);

}

Packet* IGMPGroupMember::generate_report(int type, IPAddress group_address, IGMPGroupMember* self) {

    int n_records = self->source_set.size();
    click_chatter("Generate report! n_records = %d", n_records);
    if (n_records == 0) {
        return nullptr;
    }

    // View example in clickfaq.pdf
    int tailroom = 0;
    int packetsize = sizeof(struct MembershipReport) + n_records * sizeof(struct GroupRecord);
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, tailroom);

    if (packet == 0 ) {
        click_chatter("Cannot generate packet!");
        return packet;
    }
    memset(packet->data(), 0, packet->length()); //

    MembershipReport* report = (MembershipReport*)(packet->data());
    report->type = REPORT;
    if (type == RESPONSE_TO_QUERY) {
        report->n_group_records = htons(n_records);
        int i = 0;
        for (HashTable<IPAddress, int>::iterator it = self->source_set.begin(); it; ++it) {
            // Add every grouprecord using pointer arithmetics (i is for calculating the amount of space)
            GroupRecord* group = (GroupRecord*)(packet->data() + sizeof(report) + i * sizeof(GroupRecord));
            group->record_type = EX;
            group->multicast_address = it.key();
            i++;
        }
    }
    else {
        report->n_group_records = htons(1);
        GroupRecord* group = (GroupRecord*)(packet->data() + sizeof(report));
        group->record_type = type;
        group->multicast_address = group_address;
    }

    report->checksum = click_in_cksum(packet->data(), packet->length());

    return packet;

}

void IGMPGroupMember::send_change_report(Timer* timer, void* ptr) {
    // Convert pointer back to report data struct
    ReportData* report = (ReportData*) ptr;

    // Send report
    report->self->output(1).push(report->generated_packet->clone()->uniqueify());

    // Update retransmit times
    report->retransmit_times--;

    // Reschedule if it needs to be transmitted again
    if (report->retransmit_times > 1) {

        uint32_t delay = rand() % URI * 10^3; // ms -> sec
        click_chatter("delay added (consecutive send): %d", delay);

        report->self->report_timers[report->group_address]->schedule_after_msec(delay);
    }
    else {
        // Delete timer
        report->self->report_timers[report->group_address]->clear();
    }
}

void IGMPGroupMember::send_general_report(Timer *timer, void *ptr) {
    // Convert pointer back to report data struct
    ReportData* report = (ReportData*) ptr;
    // Send report
    report->self->output(1).push(report->generated_packet->clone()->uniqueify());
    // Clear timer
    timer->clear();
}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPGroupMember)
