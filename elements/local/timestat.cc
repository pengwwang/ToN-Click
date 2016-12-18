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
#include <click/timestamp.hh>

CLICK_DECLS

TimeStat::TimeStat() 
  : m_timer(this),
  m_interval(100),
  schedCnt(0),
  isCnt(true)
{
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
    unsigned int interval;
    if (Args(conf, this, errh)
                .read("INTERVAL", interval)
                .complete() < 0) {
        click_chatter("A TimeStat is failed to create");
        return -1;
    }
    m_interval = interval;
    click_chatter("A TimeStat is created | INTERVAL = %d nanosecond", m_interval);
    return 0;
}

int
TimeStat::initialize(ErrorHandler *errh) {
    m_timer.initialize(this);
    m_timer.schedule_now();    
    startTimestamp = Timestamp::now();
    return 0;
}


Packet *TimeStat::simple_action(Packet *p) {
    return p;
}

void
TimeStat::run_timer(Timer *)
{
    if (isCnt && startTimestamp + Timestamp::make_sec(10) <= Timestamp::now()) {
       click_chatter("cnt = %lld\n", schedCnt);
       isCnt = false;
   }

   if(isCnt) {
      schedCnt++;
    }

    Timestamp ti(0, m_interval);
    m_timer.reschedule_after(ti);
}

CLICK_ENDDECLS
EXPORT_ELEMENT(TimeStat)


