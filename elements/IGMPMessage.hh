#ifndef IGMP_IGMPMESSAGE_HH
#define IGMP_IGMPMESSAGE_HH

#include <click/config.h>
#include <click/packet.hh>
#include <memory>

#define REPORT 0x22
#define QUERY 0x11

#define IN 1
#define EX 2
#define EX_TO_IN 3
#define IN_TO_EX 4
#define RESPONSE_TO_QUERY 5 // internal usage only

enum class report_record_type : uint8_t {
    m_include = 1,
    m_exclude = 2,
    c_include = 3,
    c_exlude = 4
};

typedef union {
    float f;
    struct {
        unsigned int sign : 1;
        unsigned int exp : 3;
        unsigned int mant : 4;
    } parts;
} float_cast;


struct MembershipQuery {
    uint8_t type; // 0x11
    uint8_t max_resp_code;
    uint16_t checksum;
    IPAddress group_address;
    uint8_t qrv : 3;
    uint8_t s_flag : 1;
    uint8_t resv : 4;
    uint8_t qqic;
    uint8_t n_sources;
    // Vector<IPAddress> source_addresses;

    unsigned int code2value(uint8_t c) const {
        if (c < 128) return c;
        float_cast mrc = {.f = c};
        return ((mrc.parts.mant) | 0x10 << (mrc.parts.exp + 3) * 100);
    }

    uint8_t value2code(unsigned int v) const {
        if (v < 128) return (uint8_t) v;
        uint8_t c = 128;
        while (c < 255) {
            uint8_t value_of_c = code2value(c);
            if (v == value_of_c) return c;
            if (v < value_of_c) return c - 1;
            c += 1;
        }
        return 255;
    }

    unsigned int get_max_response_time() const {
        return code2value(this->max_resp_code);
    }

    void set_max_response_code(unsigned int max_reponse_time) {
        this->max_resp_code = value2code(max_reponse_time);
    }

    unsigned int get_query_interval_time() const {
        return code2value(this->qqic);
    }

    void set_query_interval_code(unsigned int time) {
        this->qqic = value2code(time);
    }

    Vector <IPAddress> get_source_addresses() {
        Vector <IPAddress> output;
        auto start = this + sizeof(MembershipQuery);
        for (int j = 0; j < this->n_sources; j++) {
            uint32_t data = *(uint32_t * )(start + (j * sizeof(IPAddress)));
            IPAddress ip = IPAddress(data);
            output.push_back(ip);
        }
        return output;
    }

    void setup() {
        this->type = QUERY;
        this->n_sources = htons(0);
        this->s_flag = 0;
        this->resv = 0;
    }
};

struct GroupRecord {
    uint8_t record_type;
    uint8_t aux_data_len;
    uint16_t n_sources = 0;
    IPAddress multicast_address;
    // Vector<IPAddress> source_addresses;

    Vector <IPAddress> get_source_addresses() {
        Vector <IPAddress> output;
        auto start = this + sizeof(GroupRecord);
        for (int j = 0; j < this->n_sources; j++) {
            uint32_t data = *(uint32_t * )(start + (j * sizeof(IPAddress)));
            IPAddress ip = IPAddress(data);
            output.push_back(ip);
        }
        return output;
    }
};


struct MembershipReport {
    uint8_t type; // 0x22
    uint8_t reserved_1;
    uint16_t checksum;
    uint16_t reserved_2;
    uint16_t n_group_records;

    static Vector<GroupRecord*> get_group_records(Packet* p, int n_records) {
        Vector < GroupRecord * > output;
        for (int j = 0; j < n_records; j++) {
            // Add every grouprecord using pointer arithmetics (i is for calculating the amount of space)
            GroupRecord* group_record = (GroupRecord*) (p->data() + p->ip_header_length() + sizeof(MembershipReport) + j * sizeof(GroupRecord));
            output.push_back(group_record);
        }
        return output;
    }

};

struct RouterAlert {
    uint8_t type;
    uint8_t length;
    uint16_t value;
};


#endif //IGMP_IGMPMESSAGE_HH
