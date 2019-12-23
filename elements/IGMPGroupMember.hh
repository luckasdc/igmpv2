#ifndef CLICK_IGMPGROUPMEMBER_HH
#define CLICK_IGMPGROUPMEMBER_HH
#include <click/element.hh>
#include <click/hashtable.hh>
#include <click/timer.hh>


CLICK_DECLS


class IGMPGroupMember : public Element {
public:
    IGMPGroupMember();
    ~IGMPGroupMember();

    const char *class_name() const	{ return "IGMPGroupMember"; }
    const char *port_count() const	{ return "1/1"; }
    const char *processing() const	{ return PUSH; }
    int configure(Vector<String>&, ErrorHandler*);

    //int initialize(ErrorHandler* errh);

    void push(int, Packet*);

    // Query Responders
    void handle_query(Packet* p);

    // Handlers
    static int join_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh);
    static int leave_group_handler(const String& s, Element* e, void* thunk, ErrorHandler* errh);
    void add_handlers();

    static Packet* generate_report(int type, IPAddress group_address, IGMPGroupMember* self);

    static void send_report(Timer* timer, void* ptr);

    struct ReportData {
        IGMPGroupMember* self;
        Packet* generated_packet;
        IPAddress group_address;
        int type;
        int retransmit_times;
    };



private:
    HashTable<IPAddress, int> source_set;
    Timer general_query_timer;
    HashTable<IPAddress, Timer*> report_timers;


};

CLICK_ENDDECLS
#endif
