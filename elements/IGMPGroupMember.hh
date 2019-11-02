#ifndef CLICK_IGMPGROUPMEMBER_HH
#define CLICK_IGMPGROUPMEMBER_HH
#include <click/element.hh>
CLICK_DECLS

class IGMPGroupMember : public Element {
public:
    IGMPGroupMember();
    ~IGMPGroupMember();

    const char *class_name() const	{ return "IGMPGroupMember"; }
    const char *port_count() const	{ return "1/1"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    Packet* pull(int);
private:
    uint32_t maxSize;
};

CLICK_ENDDECLS
#endif