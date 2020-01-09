#ifndef CLICK_SIMPLEPULLELEMENT_HH
#define CLICK_SIMPLEPULLELEMENT_HH

#include <click/element.hh>
#include <click/timer.hh>
#include <click/hashmap.hh>
#include "defaults.hh"
#include "IGMPRouterFilter.hh"


CLICK_DECLS

using namespace router;


class IGMPRouter : public Element {
public:


    const char* class_name() const { return "IGMPRouter"; }

    const char* port_count() const { return "-/="; }

    const char* processing() const { return PUSH; }

    int initialize(ErrorHandler* errh);

    void add_handlers();

    void push(int, Packet*);

    static void send_general_query(Timer* timer, void* ptr);

    static void send_other_query(Timer* timer, void* ptr);

    static void send_group_specific_query(Timer* timer, void* ptr);

    int robustness_variable = defaults::robustness_variable;
    int query_interval = defaults::query_interval;
    int query_response = defaults::query_response;
    int last_member_query_count = defaults::last_member_query_count;
    int last_member_query_interval = defaults::last_member_query_interval;
    int unsolicited_report_interval = defaults::unsolicited_report_interval;

    bool other_querier = false;

    IPAddress _group_address;

    inline int group_member_interval() { return robustness_variable * query_interval + query_response; }

    inline int last_member_query_timer() { return last_member_query_count * last_member_query_interval; }

    RouterState* state;

    bool listening(IPAddress, IPAddress, int);

    void received_igmp(int port, Packet* p);

    void received_igmp_query(int port, Packet* p);

    void received_igmp_report(int port, Packet* p);

    void set_leave_timers(int, IPAddress, IPAddress);

    void set_general_timer();

    static void send_specific_query(Timer*, void*);

    static void delete_group(Timer*, void*);

    static void general_delete(Timer*, void*);

    bool checkQuery(Packet* p);

    uint32_t maxSize;

    Timer general_query_timer;
    Timer other_querier_timer;


};

CLICK_ENDDECLS
#endif