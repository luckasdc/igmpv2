#ifndef CLICK_SIMPLEPULLELEMENT_HH
#define CLICK_SIMPLEPULLELEMENT_HH

#include <click/element.hh>
#include <click/timer.hh>

CLICK_DECLS

class IGMPRouter : public Element {
public:


    const char* class_name() const { return "IGMPRouter"; }

    const char* port_count() const { return "3/3"; }

    const char* processing() const { return PUSH; }

    int initialize(ErrorHandler * errh);


    void add_handlers();

    int configure(Vector <String> &, ErrorHandler*);

    void push(int, Packet*);

    static void send_general_query(Timer* timer, void* ptr);


    int robustness_variable = 2;

private:

    void received_igmp(Packet*);

    void received_igmp_query(Packet*);

    void received_igmp_report(Packet*);

    uint32_t maxSize;

    Timer general_query_timer;

};

CLICK_ENDDECLS
#endif