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
    if (Args(conf, this, errh).read_m("MAXPACKETSIZE", maxSize).complete() < 0) return -1;
    if (maxSize <= 0) return errh->error("maxsize should be larger than 0");
    return 0;
}

void IGMPGroupMember::push(int, Packet *p){
    click_chatter("Got a packet of size %d",p->length());
    if (p->length() > maxSize)  p->kill();
    else output(0).push(p);
}

int IGMPGroupMember::join_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh) {
    Vector<String> args;
    IPAddress group;
    IGMPGroupMember* self = (IGMPGroupMember*) e;
    cp_argvec(s, args);
    if(Args(args, self, errh).read_mp("GROUP", group).complete() < 0) return -1;
    click_chatter("Entered group ip %s", group.unparse().c_str());

    // Generate Membership report (Sent while joining or when responding to membership query)
    Packet* packet = self->generate_report(group, IN_TO_EX);
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

    // Generate Membership report (Sent while joining or when responding to membership query)
    Packet* packet = self->generate_report(group, EX_TO_IN);
    self->output(0).push(packet);
    return 0;
}

void IGMPGroupMember::add_handlers() {
    add_write_handler("join", join_group_handler);
    add_write_handler("leave", leave_group_handler);

}

Packet* IGMPGroupMember::generate_report(IPAddress group_address, int type) {

    // View example in clickfaq.pdf
    int tailroom = 0;
    int packetsize = sizeof(struct MembershipReport) + sizeof(struct GroupRecord); // TODO: 1 is for join report, hardcode
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
    report->n_group_records = htons(1); // TODO HARDCODE

    GroupRecord* group = (GroupRecord*)(packet->data() + sizeof(report));
    group->record_type = type;
    group->multicast_address = group_address;

    report->checksum = click_in_cksum(packet->data(), packet->length());

    return packet;

}

CLICK_ENDDECLS
EXPORT_ELEMENT(IGMPGroupMember)
