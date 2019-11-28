#ifndef CLICK_SIMPLEPULLELEMENT_HH
#define CLICK_SIMPLEPULLELEMENT_HH

#include <click/element.hh>

CLICK_DECLS

class IGMPRouter : public Element {
public:


    const char* class_name() const { return "IGMPRouter"; }

    const char* port_count() const { return "2/3"; }

    const char* processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler*);

    void push(int, Packet*);

private:

    void received_igmp(Packet*);

    void received_igmp_query(Packet*);

    void received_igmp_report(Packet*);

    uint32_t maxSize;
};

CLICK_ENDDECLS
#endif