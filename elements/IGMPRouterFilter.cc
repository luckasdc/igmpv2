//
// Created by Stan Schepers on 26/11/2019.
//


#include "IGMPRouterFilter.hh"
#include "IGMPMessage.hh"
#include "defaults.hh"


using namespace router;

/// Checks if the specified value is equal to any element of the given vector.
template<typename T>
bool in_vector(const T &value, const Vector <T> &vector) {
    for (const auto &item : vector) {
        if (item == value) {
            return true;
        }
    }
    return false;
}

/// Creates a vector whose elements are the intersection of the given
/// vectors.
template<typename T>
Vector <T> intersect_vectors(const Vector <T> &left, const Vector <T> &right) {
    Vector <T> results;
    for (const auto &item : left) {
        if (in_vector<T>(item, right)) {
            results.push_back(item);
        }
    }
    return results;
}

/// Creates a vector whose elements are the union of the given
/// vectors.
template<typename T>
Vector <T> union_vectors(const Vector <T> &left, const Vector <T> &right) {
    Vector <T> results = left;
    for (const auto &item : right) {
        if (!in_vector<T>(item, results)) {
            results.push_back(item);
        }
    }
    return results;
}

/// Creates a vector whose elements are the difference of the given
/// vectors.
template<typename T>
Vector <T> difference_vectors(const Vector <T> &left, const Vector <T> &right) {
    Vector <T> results;
    for (const auto &item : left) {
        if (!in_vector<T>(item, right)) {
            results.push_back(item);
        }
    }
    return results;
}

/// Tests if the lhs vector is a subset of the rhs vector.
template<typename T>
bool is_subset_vectors(const Vector <T> &subset, const Vector <T> &superset) {
    for (const auto &item : subset) {
        if (!in_vector<T>(item, superset)) {
            return false;
        }
    }
    return true;
}

/// Tests if the given vectors contain the same elements.
template<typename T>
bool set_equality_vectors(const Vector <T> &left, const Vector <T> &right) {
    return is_subset_vectors(left, right) && is_subset_vectors(right, left);
}


SourceRecord::SourceRecord(IPAddress source_address, uint timer_ms, router::GroupState* group_state) {
    this->source_address = source_address;
    this->group_state = group_state;
    this->timer = Timer(GroupState::start_source_record_timer, this);
    this->timer.timer.schedule_after_msec(timer_ms);
}

GroupState::GroupState(IPAddress multicast_address, FilterMode filter_mode, uint timer_ms) {
    this->multicast_address = multicast_address;
    this->filter_mode = filter_mode;
    this->group_timer;
}

void GroupState::start_group_timer(Timer* timer, void* data) {

}

void GroupState::end_group_timer() {

}

void SourceRecord::start_source_record_timer(Timer* timer, void* data) {
    this->finished = false;
}

void SourceRecord::end_source_record_timer(router::SourceRecord* source_record) {
    this->finished = true;
}


void GroupState::reception_current_record(GroupRecord* group_record, int GMI) {
    auto original_source_records = this->source_records;
    if (this->filter_mode == FilterMode::INCLUDE) {
        if (group_record->record_type == FilterMode::INCLUDE) {
            // INCLUDE INCLUDE
            for (auto source_address:group_record->get_source_addresses()) {
                auto it = original_source_records.find(source_address);
                if (it != this->source_records.end()) {
                    delete this->source_records[source_address];
                }
                this->source_records[source_address] = new SourceRecord(source_address, GMI, this);
            }
        } else {
            // INCLUDE EXCLUDE
            group_record->record_type == FilterMode::EXCLUDE;

            // B-A
            // (B-A)=0
            for (auto source_address:group_record->get_source_addresses()) {
                auto it = original_source_records.find(source_address);
                if (it != this->source_records.end()) {
                    this->source_records[source_address].timer.clear();
                    this->source_records[source_address].finished = true;
                }
            }

            // A*B
            // Delete (A-B)
            for (const auto &kv:original_source_records) {
                auto source_address = kv.second->source_address;
                if (not group_record->get_source_addresses().find(source_address)) {
                    this->source_records.erase(kv.first);
                }
            }

            // Group Timer=GMI
            this->group_timer.scedule_after_msec(GMI);
        }
    } else {
        if (group_record->record_type == FilterMode::INCLUDE) {
            // EXCLUDE INCLUDE

            // Y-A
            for (auto source_address:group_record->get_source_addresses()) {
                auto it = original_source_records.find(source_address);
                if (it != this->source_records.end()) {
                    this->source_records[source_address].timer.clear();
                    this->source_records[source_address].finished = true;
                }
            }

        } else {
            // EXCLUDE EXCLUDE
        }

    }


}

void RouterState::reception_current_record(GroupRecord* group_record, int GMI) {

}


bool GroupState::listening(IPAddress source) {
    try {

        switch (this->filter_mode) {
            case FilterMode::INCLUDE :
                return this->source_records.find(source) != this->source_records.end();
            case FilterMode::EXCLUDE: {
                return this->addresses.find(source) == this->addresses.end();
            }
            default:
                return false;
        }
    } catch (...) {
        return false;
    }
}


bool RouterState::listening(IPAddress multicast, IPAddress source) {
    if (multicast == defaults::ipAddress) return true;
    if (multicast == defaults::ipAddress2) return true;

    auto it = this->group_states.find(0);
    if (it == this->group_states.end()) {
        click_chatter("Router State: interface doesn't exist in state.")
        return false;
    }
    auto group_state = (GroupState*) (it);

    return group_state->listening(source);
}
