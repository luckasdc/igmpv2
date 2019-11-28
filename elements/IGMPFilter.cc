//
// Created by Stan Schepers on 26/11/2019.
//

#include "IGMPFilter.hh"


bool IGMPFilter::listening(IPAddress multicast, IPAdress source) const {
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


}
