// -*- mode: c++; c-basic-offset: 4 -*-
/*
 * red.{cc,hh} -- element implements Random Early Detection dropping policy
 * Eddie Kohler
 *
 * Copyright (c) 1999-2000 Massachusetts Institute of Technology
 * Copyright (c) 2001 International Computer Science Institute
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and associated documentation files (the "Software"),
 * to deal in the Software without restriction, subject to the conditions
 * listed in the Click LICENSE file. These conditions include: you must
 * preserve this copyright notice, and you cannot mention the copyright
 * holders in advertising related to the Software without their permission.
 * The Software is provided WITHOUT ANY WARRANTY, EXPRESS OR IMPLIED. This
 * notice is a summary of the Click LICENSE file; the license in that file is
 * legally binding.
 */

#include <click/config.h>
#include "timestat.hh"
#include <click/args.hh>
#include <click/error.hh>
#include <clicknet/ip.h>
CLICK_DECLS

TimeStat::TimeStat() : m_alpha(0),
                       m_interval(0),
                       m_value(0),
                       m_ts() {
}

TimeStat::~TimeStat()
{
}

void *
TimeStat::cast(const char *n)
{
    if (strcmp(n, "TimeStat") == 0) {
        return (Element *) this;
    } else {
        return NULL;
    }
}

int
TimeStat::configure(Vector<String> &conf, ErrorHandler *errh)
{
    unsigned int alpha, interval;
    if (Args(conf, this, errh)
                .read("ALPHA", alpha)
                .read("INTERVAL", interval)
                .complete() < 0) {
        click_chatter("A TimeStat is failed to create");
        return -1;
    }
    m_alpha = alpha;
    m_interval = interval;
    click_chatter("A TimeStat is created | ALPHA= %d percent, INTERVAL = %d ms", m_alpha, m_interval);
    return 0;
}

int
TimeStat::initialize(ErrorHandler *errh)
{
    m_ts.assign_now_steady();
    return 0;
}


Packet *TimeStat::simple_action(Packet *p) {
    /* Add Packet Size to Value */
    const click_ip *ip_hdr = p->ip_header();
    uint16_t p_size = ntohs(ip_hdr->ip_len);
    m_value += p_size;
    /* Adjust Value by TimeStamp */
    cal_value();
    return p;
}

void TimeStat::cal_value() {
    unsigned int ro;
    Timestamp ts_now = Timestamp::now_steady();
    if (ts_now.sec() - m_ts.sec() > 0) {
        m_value = 0;
        goto end;
    }

    ro = (ts_now.usec() - m_ts.usec()) / m_interval;
    if (ro >= 50) {
        m_value = 0;
        goto end;
    }

    for (int i=0; i <= ro; i++) {
        m_value = m_value * m_alpha / 100;
    }
end:
    m_ts = ts_now;
    return;
}


unsigned int TimeStat::get_value() {
    cal_value();
    return m_value;
}

unsigned int TimeStat::get_level() {
    return m_value / 1857;
}









CLICK_ENDDECLS
EXPORT_ELEMENT(TimeStat)
