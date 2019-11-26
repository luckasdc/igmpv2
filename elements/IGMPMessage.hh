#ifndef IGMP_IGMPMESSAGE_HH
#define IGMP_IGMPMESSAGE_HH

struct MessageType {

};

struct RecordType {

};

struct IPHeader {

};

struct MembershipQuery {
    uint8_t type =  17; // 0x11
    uint8_t max_resp_code;
    uint16_t checksum;
    IPAddress group_address;
    uint8_t resv : 4;
    uint8_t s_flag : 1;
    uint8_t qrv : 3;
    uint8_t qqic;
    uint8_t n_sources;
    Vector<IPAddress> source_addresses;
};

struct GroupRecord {
    uint8_t record_type;
    uint8_t aux_data_len = 0;
    uint16_t n_sources = 0;
    IPAddress multicast_address;
    //Vector<IPAddress> source_addresses;
};

struct MembershipReport {
    uint8_t type = 0x22; // 0x22
    uint8_t reserved_1 = 0;
    uint16_t checksum;
    uint16_t reserved_2 = 0;
    uint16_t n_group_records;
    //Vector<GroupRecord> group_records;

    //inline unsigned int size() { return 8 + (this->n_group_records * sizeof(group_record)) }
};


#endif //IGMP_IGMPMESSAGE_HH
