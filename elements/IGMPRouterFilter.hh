#pragma once

#ifndef IGMP_IGMPFILTER_HH
#define IGMP_IGMPFILTER_HH

#include <click/string.hh>
#include <click/vector.hh>

#include "IGMPMessage.hh"

enum class FilterMode : String {
    INCLUDE = "INCLUDE",
    EXCLUDE = "EXCLUDE"
};

namespace router {

    struct SourceRecord;

    struct GroupState {
        IPAddress multicast_address;
        Timer group_timer;
        FilterMode filter_mode = FilterMode::INCLUDE;
        HashMap<IPAddress, SourceRecord*> source_records;

        static void start_group_timer(Timer* timer, void* data);

        static void start_source_record_timer(Timer* timer, void* data);

        void end_group_timer();

        void end_source_record_timer(SourceRecord* source_record);

        void record(GroupRecord* group_record);

        GroupState(IPAddress multicast_address, FilterMode filter_mode, uint timer_ms);

    };

    struct SourceRecord {
        GroupState* group_state;
        IPAddress source_address;
        Timer timer;

        SourceRecord(IPAddress source_address, uint timer_ms, GroupState* group_state);

    };

    struct RouterState {
        HashMap<int, GroupState*> group_states;


        bool listening(IPAddress, IPAddress) const;
    };

}


#endif //IGMP_IGMPFILTER_HH
