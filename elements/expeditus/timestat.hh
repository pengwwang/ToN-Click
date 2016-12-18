// -*- mode: c++; c-basic-offset: 4 -*-
#ifndef CLICK_TIMESTAT_HH
#define CLICK_TIMESTAT_HH
#include <click/element.hh>
#include <click/timer.hh>
#include <click/timestamp.hh>
CLICK_DECLS

class TimeStat : public Element {
public:
    TimeStat() CLICK_COLD;
    ~TimeStat() CLICK_COLD;

    const char *class_name() const	{ return "TimeStat"; }
    const char *port_count() const		{ return "1/1"; }

    void *cast(const char *);

    int configure(Vector<String> &, ErrorHandler *) CLICK_COLD;
    int initialize(ErrorHandler *) CLICK_COLD;

    Packet *simple_action(Packet *p);

    void cal_value();
    unsigned int get_value();
    unsigned int get_level();


private:
    unsigned int m_alpha;
    unsigned int m_interval;
    unsigned int m_value;
    Timestamp m_ts;
};

CLICK_ENDDECLS
#endif
