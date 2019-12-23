#ifndef IGMP_IGMPMESSAGE_HH
#define IGMP_IGMPMESSAGE_HH

#define REPORT 0x22
#define QUERY 0x11
#define IN 1
#define EX 2
#define EX_TO_IN 3
#define IN_TO_EX 4
#define RESPONSE_TO_QUERY 5 // internal usage only

// TODO Fixed variables: a decent solution for these
#define URI 2
#define RV 2

enum class report_record_type : uint8_t {
    m_include = 1,
    m_exclude = 2,
    c_include = 3,
    c_exlude = 4
};

struct MembershipQuery {
    uint8_t type; // 0x11
    uint8_t max_resp_code;
    uint16_t checksum;
    IPAddress group_address;
    uint8_t resv : 4;
    uint8_t s_flag : 1;
    uint8_t qrv : 3;
    uint8_t qqic;
    uint8_t n_sources;
    // Vector<IPAddress> source_addresses;

    void setup() {
        this->type = QUERY;
        this->n_sources = htons(0);
        this->qrv = 0;
        this->s_flag = 1;
        this->max_resp_code = 1;
    }
};

struct GroupRecord {
    uint8_t record_type;
    uint8_t aux_data_len;
    uint16_t n_sources = 0;
    IPAddress multicast_address;
    // Vector<IPAddress> source_addresses;
};

struct GroupRecordExtended {
    GroupRecord header;
    Vector <IPAddress> source_addresses;
};


struct MembershipReport {
    uint8_t type; // 0x22
    uint8_t reserved_1;
    uint16_t checksum;
    uint16_t reserved_2;
    uint16_t n_group_records;
    // Vector<GroupRecord> group_records;

//    static Vector<GroupRecord*> get_group_records(Packet* packet, int n_groups) {
//        Vector<GroupRecord*> output;
//        for (int j = 0; j < n_groups; j++) {
//            // Add every grouprecord using pointer arithmetics (i is for calculating the amount of space)
//            GroupRecord* group = (GroupRecord*) (packet->data() + sizeof(MembershipReport) + (j * sizeof(GroupRecord)));
//            output.push_back(group);
//        }
//        return output;
//    }

//    inline unsigned int size() { return 8 + (this->n_group_records * sizeof(group_record)) }
};

struct MembershipReportExtended {
    MembershipReport header;
    Vector <GroupRecordExtended> group_records;


};

#endif //IGMP_IGMPMESSAGE_HH
