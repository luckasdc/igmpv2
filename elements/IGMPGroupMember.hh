#ifndef CLICK_IGMPGROUPMEMBER_HH
#define CLICK_IGMPGROUPMEMBER_HH

#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>
#include "defaults.hh"


CLICK_DECLS


class IGMPGroupMember : public Element {
public:
    IGMPGroupMember();

    ~IGMPGroupMember();

    const char* class_name() const { return "IGMPGroupMember"; }

    const char* port_count() const { return "1/2"; }

    const char* processing() const { return PUSH; }

    int configure(Vector <String> &, ErrorHandler*);

    //int initialize(ErrorHandler* errh);

    void push(int, Packet*);

    // Query Responders
    void handle_query(Packet* p);

    // Handlers
    static int join_group_handler(const String &s, Element* e, void* thunk, ErrorHandler* errh);

    static int leave_group_handler(const String &s, Element* e, void* thunk, ErrorHandler* errh);

    static int kill_self_handler(const String &s, Element* e, void* thunk, ErrorHandler* errh);

    void add_handlers();

    static Packet* generate_report(int type, IPAddress group_address, IGMPGroupMember* self);

    // static send function, will be called by timers
    static void send_change_report(Timer* timer, void* ptr);

    static void send_general_report(Timer* timer, void* ptr);

    static void send_group_specific_report(Timer* timer, void* ptr);


    struct ReportData {
        IGMPGroupMember* self;
        Packet* generated_packet;
        IPAddress group_address;
        int type;
        int retransmit_times;
    };


    bool reply_to_general_query = true;

private:
    HashTable<IPAddress, int> source_set; // Connected to which servers?
    Timer* general_timer = new Timer(); // Queries
    HashTable<IPAddress, Timer*> report_timers; // Reports
    HashTable<IPAddress, Timer*> group_specific_timers; // Group Specific queries

};

CLICK_ENDDECLS
#endif
