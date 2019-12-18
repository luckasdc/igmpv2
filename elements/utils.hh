//
// Created by Stan Schepers on 02/11/2019.
//

#ifndef IGMP_UTILS_HH
#define IGMP_UTILS_HH

bool checkQuery(Packet *p)
{
    MembershipQuery* query = (MembershipQuery*)(p->data() + p->ip_header_length());

    //if (p->length() - p->ip_header_length() > sizeof(query)) { return false; }

    // One's complement of total payload is done by adding the bitwise operator & 0xFFFF
    // must be equal to 0
    return (click_in_cksum((unsigned char *) query, p->length() - p->ip_header_length()) & 0xFFFF) == 0;

}


#endif //IGMP_UTILS_HH
