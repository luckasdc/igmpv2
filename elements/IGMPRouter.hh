#ifndef CLICK_SIMPLEPULLELEMENT_HH
#define CLICK_SIMPLEPULLELEMENT_HH
#include <click/element.hh>
CLICK_DECLS

class IGMPRouter : public Element {
public:
    SimplePullElement();
    ~SimplePullElement();

    const char *class_name() const	{ return "IGMPRouter"; }
    const char *port_count() const	{ return "1/1"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    Packet* push(int, Packet*);
private:
    uint32_t maxSize;
};

CLICK_ENDDECLS
#endif