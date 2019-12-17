#include "IGMPMessage.hh"



MembershipReportExtended MembershipReportExtended::parse(Packet* p) {
    MembershipReportExtended output;

    // parse first half of packet (MembershipReport)
    MembershipReport* membershipReport = (MembershipReport*) p->data();
    click_chatter("type: %s", membershipReport->type);
    click_chatter("n group records: %d", membershipReport->n_group_records);

    return output;
}