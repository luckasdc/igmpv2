#ifndef IGMP_IGMPMESSAGE_HH
#define IGMP_IGMPMESSAGE_HH

struct MessageType {

};

struct RecordType {

};

struct IPHeader {

};

struct IGMPQueryMessage {
    uint8_t type;
    uint8_t max_resp_code;
    uint16_t checksum;
    IPAddress group_address;
    uint8_t resv : 4;
    uint8_t s_flag : 1;
    uint8_t qrv : 3;
    uint8_t qqic;
    uint8_t n_sources;
    std::vector<IPAddress> source_addresses;
};

struct group_record {
    uint8_t record_type;
    uint8_t aux_data_len;
    uint16_t n_sources;
    IPAddress multicast_address
    std::vector<IPAddress> source_addresses;
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
