
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
        GroupState* group_state; // pointer
        IPAddress source_address = IPAddress(0);
        Timer* timer;
        bool finished = false;

        static void start_source_record_timer(Timer* timer, void* data);

        void end_source_record_timer(SourceRecord* source_record);

        void remove();


        SourceRecord(IPAddress source_address, uint timer_ms, GroupState* group_state);

    };

    struct GroupState {
        RouterState* router_state;
        IPAddress multicast_address = IPAddress(0);
        Timer* group_timer;
        unsigned int interface;
        FilterMode filter_mode = FilterMode::INCLUDE;

        HashTable<IPAddress, SourceRecord*> source_records;

        bool has_changed = false;

        static void start_group_timer(Timer* timer, void* data);

        void end_group_timer();

        bool listening(IPAddress);

//        void reception_current_record(GroupRecord* group_record, int);

//        Vector <IPAddress> get_excluded_addresses() const {
//            Vector <IPAddress> output;
//            for (const auto &kv:this->source_records) {
//                auto source_record = kv.second;
//                if (source_record->finished) {
//                    output.push_back(source_record->source_address);
//                }
//            }
//            return output;
//        }

        GroupState(IPAddress multicast_address, FilterMode filter_mode, uint timer_ms);

    };

    struct RouterState {
        HashTable<int, GroupState*> group_states;


        bool listening(IPAddress, IPAddress);
    };

}

CLICK_ENDDECLS


#endif //IGMP_IGMPFILTER_HH
