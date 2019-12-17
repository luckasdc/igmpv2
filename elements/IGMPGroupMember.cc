#include <click/config.h>
#include <click/args.hh>
#include <click/error.hh>
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

int IGMPGroupMember::configure(Vector<String> &conf, ErrorHandler *errh) {

    return 0;
}

void IGMPGroupMember::push(int, Packet *p){
    click_chatter("Got a packet of size %d",p->length());
    handle_query(p);
    output(0).push(p);
}

int IGMPGroupMember::handle_query(Packet *p) {
    const MembershipQuery* query = (MembershipQuery*) (p->data() + p->ip_header_length());

    if (query->type == QUERY) {
        click_chatter("client igmp header temptype is %d", query->type);
        click_chatter("client igmp checksum is %d", query->checksum);
        click_chatter("client igmp group address is %s", query->group_address.unparse().c_str());

        Packet* packet = this->generate_report(RESPONSE_TO_QUERY, query->group_address);
        this->output(0).push(packet);
    }
}

int IGMPGroupMember::join_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh) {
    Vector<String> args;
    IPAddress group;
    IGMPGroupMember* self = (IGMPGroupMember*) e;
    cp_argvec(s, args);
    if(Args(args, self, errh).read_mp("GROUP", group).complete() < 0) return -1;
    click_chatter("Entered group ip %s", group.unparse().c_str());


    // Add current group to source_set
    self->source_set.find_insert(group, 1);
    // Generate Membership report (Sent while joining or when responding to membership query)
    Packet* packet = self->generate_report(IN_TO_EX, group);
    self->output(0).push(packet);
    return 0;
}

int IGMPGroupMember::leave_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh) {
    Vector<String> args;
    IPAddress group;
    IGMPGroupMember* self = (IGMPGroupMember*) e;
    cp_argvec(s, args);
    if(Args(args, self, errh).read_mp("GROUP", group).complete() < 0) return -1;
    click_chatter("Leave group ip %s", group.unparse().c_str());

    // remove current group from source_set
    self->source_set.erase(group);
    // Generate Membership report (Sent while joining or when responding to membership query)
    Packet* packet = self->generate_report(EX_TO_IN, group);
    self->output(0).push(packet);
    return 0;
}

void IGMPGroupMember::add_handlers() {
    add_write_handler("join", join_group_handler);
    add_write_handler("leave", leave_group_handler);

}

Packet* IGMPGroupMember::generate_report(int type, IPAddress group_address) {

    int n_records = this->source_set.size();
    click_chatter("Generate report! n_records = %d", n_records);

    // View example in clickfaq.pdf
    int tailroom = 0;
    int packetsize = sizeof(struct MembershipReport) + n_records * sizeof(struct GroupRecord);
    int headroom = sizeof(click_ether) + sizeof(click_ip);
    WritablePacket* packet = Packet::make(headroom, 0, packetsize, tailroom);
    click_chatter("Size of packet generated: %d", packetsize);
    click_chatter("Size of click_ether: %d, click_ip: %d", sizeof(click_ether), sizeof(click_ip));

    if (packet == 0 ) {
        click_chatter("Cannot generate packet!");
        return packet;
    }
    memset(packet->data(), 0, packet->length()); //

    MembershipReport* report = (MembershipReport*)(packet->data());
    report->type = REPORT;
    report->n_group_records = htons(n_records);
    if (type == RESPONSE_TO_QUERY) {
        int i = 0;
        for (HashTable<IPAddress, int>::iterator it = this->source_set.begin(); it; ++it) {
            // Add every grouprecord using pointer arithmetics (i is for calculating the amount of space)
            GroupRecord* group = (GroupRecord*)(packet->data() + sizeof(report) + i * sizeof(GroupRecord));
            group->record_type = EX;
            group->multicast_address = it.key();
            i++;
        }
    }
    else {
        GroupRecord* group = (GroupRecord*)(packet->data() + sizeof(report));
        group->record_type = type;
        group->multicast_address = group_address;
    }

    report->checksum = click_in_cksum(packet->data(), packet->length());

    return packet;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPGroupMember)
