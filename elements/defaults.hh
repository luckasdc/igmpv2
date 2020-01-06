//
// Created by Stan Schepers on 27/12/2019.
//

#ifndef IGMP_DEFAULTS_HH
#define IGMP_DEFAULTS_HH

namespace defaults {
    const int robustness_variable = 2; // r_v - 1 = # pakketten (not 0 or 1)
    const int query_interval = 10; // s default: 125
    const int query_response = 100; // ds
    const int last_member_query_count = 2; // default : r_v
    const int last_member_query_interval = 10; //ds
    const int unsolicited_report_interval = 1; // s
    const int startup_query_count = 2; // default: r_v

    const IPAddress ipAddress = IPAddress("224.0.0.1");
    const IPAddress ipAddress2 = IPAddress("224.0.0.22");
}

#endif //IGMP_DEFAULTS_HH