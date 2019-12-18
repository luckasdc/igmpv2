//
// Created by Stan Schepers on 26/11/2019.
//

#include "IGMPRouterFilter.hh"

using namespace router;


SourceRecord::SourceRecord(IPAddress source_address, uint timer_ms, router::GroupState* group_state) {
    this->source_address = source_address;
    this->group_state = group_state;
    this->timer = Timer(GroupState::start_source_record_timer, this);
    this->timer.timer.schedule_after_msec(timer_ms);
}

GroupState::GroupState(IPAddress multicast_address, FilterMode filter_mode, uint timer_ms) {
    this->multicast_address = multicast_address;
    this->filter_mode = filter_mode;
    this->group_timer;
}











/*bool IGMPFilter::listening(IPAddress multicast, IPAdress source) const {
    if (multicast == IPAddress("224.0.0.1")) return true
    if (multicast == IPAddress("224.0.0.22")) return true

    auto record = this->records_map.find(multicast);
    if (record == this->records_map.end()) {
        return false;
    }
    // TODO make beautiful
    if (record->mode == "exclude") {
        for (const auto &address:record->excluded_addresses) {
            if (source == address) return false;
        }
        return true;
    }
    for(const auto & srecord:record->source_records){
        if(srecord->get_source){

        }
    }


}*/
