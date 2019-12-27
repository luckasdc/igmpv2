#pragma once

#ifndef IGMP_IGMPFILTER_HH
#define IGMP_IGMPFILTER_HH

#include <click/string.hh>
#include <click/hashtable.hh>
#include <click/vector.hh>

class GroupRecord;


enum class FilterMode : unsigned int {
    INCLUDE = 1,
    EXCLUDE = 0
};

namespace router {

    struct SourceRecord;

    struct GroupState {
        IPAddress multicast_address;
        Timer group_timer;
        FilterMode filter_mode = FilterMode::INCLUDE;
        HashTable<IPAddress, SourceRecord*> source_records;
        bool has_changed = false;

        static void start_group_timer(Timer* timer, void* data);

        void end_group_timer();

        void reception_current_record(GroupRecord* group_record);

        GroupState(IPAddress multicast_address, FilterMode filter_mode, uint timer_ms);


    };

    struct SourceRecord {
        GroupState* group_state; // pointer
        IPAddress source_address;
        Timer timer;

        static void start_source_record_timer(Timer* timer, void* data);

        void end_source_record_timer(SourceRecord* source_record);

        SourceRecord(IPAddress source_address, uint timer_ms, GroupState* group_state);

    };

    struct RouterState {
        HashTable<int, GroupState*> group_states;


        bool listening(IPAddress, IPAddress) const;
    };

}


#endif //IGMP_IGMPFILTER_HH
