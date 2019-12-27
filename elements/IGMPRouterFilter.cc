//
// Created by Stan Schepers on 26/11/2019.
//

#include "IGMPMessage.hh"
#include "IGMPRouterFilter.hh"


using namespace router;

/// Checks if the specified value is equal to any element of the given vector.
template <typename T>
bool in_vector(const T &value, const Vector<T> &vector)
{
    for (const auto &item : vector)
    {
        if (item == value)
        {
            return true;
        }
    }
    return false;
}

/// Creates a vector whose elements are the intersection of the given
/// vectors.
template <typename T>
Vector<T> intersect_vectors(const Vector<T> &left, const Vector<T> &right)
{
    Vector<T> results;
    for (const auto &item : left)
    {
        if (in_vector<T>(item, right))
        {
            results.push_back(item);
        }
    }
    return results;
}

/// Creates a vector whose elements are the union of the given
/// vectors.
template <typename T>
Vector<T> union_vectors(const Vector<T> &left, const Vector<T> &right)
{
    Vector<T> results = left;
    for (const auto &item : right)
    {
        if (!in_vector<T>(item, results))
        {
            results.push_back(item);
        }
    }
    return results;
}

/// Creates a vector whose elements are the difference of the given
/// vectors.
template <typename T>
Vector<T> difference_vectors(const Vector<T> &left, const Vector<T> &right)
{
    Vector<T> results;
    for (const auto &item : left)
    {
        if (!in_vector<T>(item, right))
        {
            results.push_back(item);
        }
    }
    return results;
}

/// Tests if the lhs vector is a subset of the rhs vector.
template <typename T>
bool is_subset_vectors(const Vector<T> &subset, const Vector<T> &superset)
{
    for (const auto& item : subset)
    {
        if (!in_vector<T>(item, superset))
        {
            return false;
        }
    }
    return true;
}

/// Tests if the given vectors contain the same elements.
template <typename T>
bool set_equality_vectors(const Vector<T> &left, const Vector<T> &right)
{
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

void GroupState::start_source_record_timer(Timer* timer, void* data) {

}

void GroupState::end_source_record_timer(router::SourceRecord* source_record) {

}


void GroupState::reception_current_record(GroupRecord* group_record, int GMI) {

    if (group_record->record_type == FilterMode::INCLUDE) {
        if (this->filter_mode == FilterMode::INCLUDE) {
            for (auto source_address:group_record->get_source_addresses()) {
                auto it = this->source_records.find(source_address);
                if (it != this->source_records.end()) {
                    this->source_records[source_address].timer.schedule_after_msec(GMI);
                } else {
                    this->source_records[source_address] = SourceRecord(source_address, GMI, this);
                }
            }
        } else {
            // EXCLUDE
            group_record->record_type == FilterMode::EXCLUDE;




        }
    } else {

    }


}




bool IGMPFilter::listening(IPAddress multicast, IPAdress source) const {
    if (multicast == IPAddress("224.0.0.1")) return true
    if (multicast == IPAddress("224.0.0.22")) return true

    auto record = this->records_map.find(multicast);
    if (record == this->records_map.end()) {
        return false;
    }
    // TODO make beautiful
    if (record->mode == "exclude") {
        for (const auto &address:record->excluded_addresses) {
            if (source == address) return false;
        }
        return true;
    }

    return false;
}
