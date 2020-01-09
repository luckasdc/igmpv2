#ifndef CLICK_IPENCAPDELUXE_HH
#define CLICK_IPENCAPDELUXE_HH

#include <click/element.hh>
#include <click/glue.hh>
#include <click/atomic.hh>
#include <clicknet/ip.h>

CLICK_DECLS

class IPEncapDeluxe : public Element { public:

    IPEncapDeluxe() CLICK_COLD;
    ~IPEncapDeluxe() CLICK_COLD;

    const char *class_name() const		{ return "IPEncapDeluxe"; }
    const char *port_count() const		{ return PORTS_1_1; }

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    bool can_live_reconfigure() const		{ return true; }
    int initialize(ErrorHandler *) CLICK_COLD;
    void add_handlers() CLICK_COLD;

    Packet *simple_action(Packet *);

private:

    click_ip _iph;
    atomic_uint32_t _id;

    inline void update_cksum(click_ip *, int) const;
    static String read_handler(Element *, void *) CLICK_COLD;

};

CLICK_ENDDECLS


#endif //IGMP_IPENCAPDELUXE_HH
