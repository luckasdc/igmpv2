
#ifndef CLICK_IGMPFILTER_HH
#define CLICK_IGMPFILTER_HH

#include <click/string.hh>
#include <click/timer.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>

CLICK_DECLS

class GroupRecord;


enum class FilterMode : unsigned int {
    INCLUDE = 1,
    EXCLUDE = 0
};

namespace router {

    struct GroupState;
    struct RouterState;


    struct SourceRecord {

        SourceRecord(IPAddress source_address, GroupState* group_state);

        GroupState* group_state; // pointer
        IPAddress source_address = IPAddress(0);
        Timer* timer;
        bool finished = false;

        static void start_source_record_timer(Timer* timer, void* data);
        void end_source_record_timer(SourceRecord* source_record);
        void remove();
    };

    struct GroupState {
        //Constructor
        GroupState(int interface, IPAddress multicast_address, RouterState* router);

        RouterState* router_state;
        IPAddress multicast_address = IPAddress(0);
        Timer* group_timer = nullptr;
        unsigned int interface;
        FilterMode filter_mode = FilterMode::INCLUDE;
        bool has_replied = false;
        bool has_replied_to_general = false;

        HashTable<IPAddress, SourceRecord*> source_records;

        bool has_changed = false;

        static void start_group_timer(Timer* timer, void* data);

        void end_group_timer();

        void start_listening(IPAddress client);

//        void reception_current_record(GroupRecord* group_record, int);

        Vector <IPAddress> get_excluded_addresses() const {
            Vector <IPAddress> output;
            for (const auto &kv:this->source_records) {
                auto source_record = kv.second;
                if (source_record->finished) {
                    output.push_back(source_record->source_address);
                }
            }
            return output;
        }

        GroupState(IPAddress multicast_address, FilterMode filter_mode, uint timer_ms);

    };

    struct RouterState {
        Vector<GroupState*> group_states{};

        GroupState* get_group(int port, IPAddress mul);
        bool find_insert_group_state(int port, IPAddress client, IPAddress server);

        void delete_group(GroupState* groupState);

        void delete_not_replied_general_query();

        bool listening(IPAddress server, IPAddress source, int);
    };


}

CLICK_ENDDECLS


#endif //IGMP_IGMPFILTER_HH
