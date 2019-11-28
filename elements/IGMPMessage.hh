#ifndef IGMP_IGMPMESSAGE_HH
#define IGMP_IGMPMESSAGE_HH

#define REPORT 0x22
#define QUERY 0x11
#define IN 1
#define EX 2
#define EX_TO_IN 3
#define IN_TO_EX 4

enum class report_record_type : uint8_t {
    m_include = 1,
    m_exclude = 2,
    c_include = 3,
    c_exlude = 4
};

struct MembershipQuery {
    uint8_t type = 17; // 0x11
    uint8_t max_resp_code;
    uint16_t checksum;
    IPAddress group_address;
    uint8_t resv : 4;
    uint8_t s_flag : 1;
    uint8_t qrv : 3;
    uint8_t qqic;
    uint8_t n_sources;
    // Vector<IPAddress> source_addresses;
};

struct GroupRecord {
    uint8_t record_type;
    uint8_t aux_data_len ;
    uint16_t n_sources = 0;
    IPAddress multicast_address;
    // Vector<IPAddress> source_addresses;
};

struct GroupRecordVector {};


struct MembershipReport {
    uint8_t type; // 0x22
    uint8_t reserved_1;
    uint16_t checksum;
    uint16_t reserved_2;
    uint16_t n_group_records;
    // Vector<GroupRecord> group_records;

//    inline unsigned int size() { return 8 + (this->n_group_records * sizeof(group_record)) }
};


#endif //IGMP_IGMPMESSAGE_HH
