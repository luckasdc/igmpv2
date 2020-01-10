#ifndef IGMP_DEFAULTS_HH
#define IGMP_DEFAULTS_HH

namespace defaults {
    const int robustness_variable = 2; // not 1 or 0
    const int query_interval = 20; // s default: 125
    const int query_response = 50; // ds
    const int last_member_query_count = robustness_variable; // default : r_v
    const int last_member_query_interval = 10; //ds
    const int startup_query_count = robustness_variable; // default: r_v
    const int startup_query_interval = query_interval/4;
    const bool startup = false; // do startup queries

    const IPAddress all_systems_multicast_address = IPAddress("224.0.0.1"); // ipAddress
    const IPAddress report_address = IPAddress("224.0.0.22"); // ipAddress2

    const int unsolicited_report_interval = 1; // s
}

#endif //IGMP_DEFAULTS_HH