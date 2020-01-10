//
// Created by Stan Schepers on 26/11/2019.
//

#include <click/config.h>
#include <algorithm>
#include "IGMPRouterFilter.hh"
#include "IGMPMessage.hh"
#include "defaults.hh"


CLICK_DECLS

using namespace router;

/// Checks if the specified value is equal to any element of the given vector.
//template<typename T>
//bool in_vector(const T &value, const Vector <T> &vector) {
//    for (const auto &item : vector) {
//        if (item == value) {
//            return true;
//        }
//    }
//    return false;
//}
//
///// Creates a vector whose elements are the intersection of the given
///// vectors.
//template<typename T>
//Vector <T> intersect_vectors(const Vector <T> &left, const Vector <T> &right) {
//    Vector <T> results;
//    for (const auto &item : left) {
//        if (in_vector<T>(item, right)) {
//            results.push_back(item);
//        }
//    }
//    return results;
//}
//
///// Creates a vector whose elements are the union of the given
///// vectors.
//template<typename T>
//Vector <T> union_vectors(const Vector <T> &left, const Vector <T> &right) {
//    Vector <T> results = left;
//    for (const auto &item : right) {
//        if (!in_vector<T>(item, results)) {
//            results.push_back(item);
//        }
//    }
//    return results;
//}
//
///// Creates a vector whose elements are the difference of the given
///// vectors.
//template<typename T>
//Vector <T> difference_vectors(const Vector <T> &left, const Vector <T> &right) {
//    Vector <T> results;
//    for (const auto &item : left) {
//        if (!in_vector<T>(item, right)) {
//            results.push_back(item);
//        }
//    }
//    return results;
//}
//
///// Tests if the lhs vector is a subset of the rhs vector.
//template<typename T>
//bool is_subset_vectors(const Vector <T> &subset, const Vector <T> &superset) {
//    for (const auto &item : subset) {
//        if (!in_vector<T>(item, superset)) {
//            return false;
//        }
//    }
//    return true;
//}
//
///// Tests if the given vectors contain the same elements.
//template<typename T>
//bool set_equality_vectors(const Vector <T> &left, const Vector <T> &right) {
//    return is_subset_vectors(left, right) && is_subset_vectors(right, left);
//}


SourceRecord::SourceRecord(IPAddress source_address, router::GroupState* group_state) {
    this->source_address = source_address;
    this->group_state = group_state;

//    delete this->timer;
//    this->timer = new Timer(GroupState::start_source_record_timer, this);
//    this->timer->schedule_after_msec(timer_ms);
}

GroupState::GroupState(int interface, IPAddress multicast_address, RouterState* routerState) {
    this->multicast_address = multicast_address;
    this->interface = interface;
    this->router_state = routerState;
}

void GroupState::start_group_timer(Timer* timer, void* data) {

}

void GroupState::end_group_timer() {

}

void SourceRecord::start_source_record_timer(Timer* timer, void* data) {
//    this->finished = false;
}

void SourceRecord::end_source_record_timer(router::SourceRecord* source_record) {
//    this->finished = true;
}


void GroupState::start_listening(IPAddress client) {
    // Mark that group must keep on serving

    auto it = this->source_records.find(client);
    if (it == this->source_records.end()) {
        // Source Record doesn't exist yet. Creating new record.
        this->source_records.find_insert(client, new SourceRecord(client, this));
        return;
    }
    this->has_replied = true;
    this->has_replied_to_general = true;
}


bool RouterState::listening(IPAddress multicast, IPAddress source, int interface) {
    //if (multicast == defaults::all_systems_multicast_address) return true;
    //if (multicast == defaults::report_address) return true;
    click_chatter("listening to group on interface %d, mc %s ", interface, multicast.unparse().c_str());


    if (this->get_group(interface, multicast) != nullptr) {
        return true;
    }
    click_chatter("Router State: interface doesn't exist in state.");
    return false;
}


GroupState* RouterState::get_group(int interface, IPAddress server) {
    for (GroupState* group : this->group_states) {
        if (group->multicast_address == server and group->interface == interface and group->router_state == this) {
            return group;
        }
    }
    return nullptr;
}

bool RouterState::find_insert_group_state(int port, IPAddress client, IPAddress server) {

    if (this->get_group(port, server) == nullptr) {
        // Group doesn't exist yet. Creating new Group.
        this->group_states.push_back(new GroupState(port, server, this));
        this->group_states.back()->start_listening(client);

        return true;
    }
    GroupState* group_state = this->get_group(port, server);
    group_state->start_listening(client);

    return true;
}

void RouterState::delete_not_replied_general_query() {
    for (GroupState* group : this->group_states) {
        if (not group->has_replied_to_general and group->router_state == this){
            this->delete_group(group);
        }
    }
}

void RouterState::reset_not_replied_general() {
    for (GroupState* group : this->group_states) {
        group->has_replied_to_general = false;
    }
}

void RouterState::delete_group(GroupState* group_state) {
    group_state->router_state = nullptr;
}

void SourceRecord::remove() {
    if (this->group_state == nullptr) delete this;
    if (this->group_state->router_state == nullptr) delete this;
    this->group_state->source_records.erase(
        this->source_address);
    delete this;
}

CLICK_ENDDECLS
ELEMENT_PROVIDES(IGMPRouterFilter)

