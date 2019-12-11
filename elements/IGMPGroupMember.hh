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

    void push(int, Packet*);

    // Query Responders
    static int handle_query(Packet* p);

    // Handlers
    static int join_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh);
    static int leave_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh);
    void add_handlers();

    Packet* generate_report(IPAddress group_address, int type);

private:
    uint32_t maxSize;
};

CLICK_ENDDECLS
#endif
