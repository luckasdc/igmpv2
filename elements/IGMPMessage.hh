#ifndef IGMP_IGMPMESSAGE_HH
#define IGMP_IGMPMESSAGE_HH

struct MessageType {

};

struct RecordType {

};

struct IPHeader {

};

struct IGMPMessage {
    uint8_t type;
    uin
};


struct report_query {
    uint8_t type = 0x22;
    uint8_t reserved;
    uint16_t checksum;
    uint16_t reserved;
    uint16_t n_group_records;
    Vector<group_record> group_records;

    inline unsigned int size() { return 8 + (this->n_group_records * sizeof(group_record)) }
};


#endif //IGMP_IGMPMESSAGE_HH
