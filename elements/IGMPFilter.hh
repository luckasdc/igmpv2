//
// Created by Stan Schepers on 26/11/2019.
//

#ifndef IGMP_IGMPFILTER_HH
#define IGMP_IGMPFILTER_HH


struct IGMPFilterRecord {
    // mode

    // excluded adrresses

    // source_adrresses

    //

};


struct IGMPFilter {
    Hashmap <IPAdress, IGMPFilterRecord> records_map;
    Element* parent;


    bool listening(IPAddress, IPAdress) const;
};


#endif //IGMP_IGMPFILTER_HH
