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

//static unsigned long long  schedCnt = 0;

TimeStat::TimeStat() : m_value(0),
                       m_tvalue(0),
                       m_agg(0),
                       m_tagg(0),
                       m_timer(this),
                       m_alpha(0),
                       m_interval(0),
                       m_quan(0),
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
        //click_chatter("fuck");
        return NULL;
    }
}

int
TimeStat::configure(Vector<String> &conf, ErrorHandler *errh)
{
    unsigned int alpha, interval, quan;
    if (Args(conf, this, errh)
                .read("ALPHA", alpha)
                .read("INTERVAL", interval)
                .read("QUAN", quan)
                .complete() < 0) {
        click_chatter("A TimeStat is failed to create");
        return -1;
    }
    m_alpha = alpha;
    m_interval = interval;
    m_quan = quan;
    click_chatter("A TimeStat is created | ALPHA= %d percent, INTERVAL = %d nanosecond, QUAN = %d ", m_alpha, m_interval, m_quan);
    return 0;
}

int
TimeStat::initialize(ErrorHandler *errh) {
    m_timer.initialize(this);
    m_timer.schedule_now();    
//Timestamp ti(0, m_interval);
    //m_timer.reschedule_after(ti);
    //click_chatter("init");
    startTimestamp = Timestamp::now();
    return 0;
}


Packet *TimeStat::simple_action(Packet *p) {
    /* Add Packet Size to Value */
    //const click_ip *ip_hdr = p->ip_header();
    //uint16_t p_size = ntohs(ip_hdr->ip_len);
    //m_value += p_size;
    //m_agg += 1;
    //click_chatter("action");
    return p;
}

void
TimeStat::run_timer(Timer *)
{
    //click_chatter("run_timer");
    m_tvalue = m_value;
    m_tagg = m_agg;
    m_agg = 0;
    m_value = m_value * m_alpha / 100;

  //  schedCnt ++;


    // click_chatter("%lld   %s: %{timestamp}\n", schedCnt,
     //              declaration().c_str(), &now ); 
   if (isCnt && startTimestamp + Timestamp::make_sec(1) <= Timestamp::now()) {
       click_chatter("cnt = %lld\n", schedCnt);
       isCnt = false;
   }

   if(isCnt) {
      schedCnt++;
    }

   // click_chatter("cnt = %lld\n", schedCnt);

    Timestamp ti(0, m_interval);
    m_timer.reschedule_after(ti);
}

unsigned int TimeStat::get_value() {
    return m_tvalue;
}

unsigned int TimeStat::get_level() {
    return m_tvalue / m_quan;
}

void
TimeStat::add_handlers() {
    add_read_handler("value", read_handler, 0);
    //click_chatter("add handler");
}

String
TimeStat::read_handler(Element *e, void *thunk) {
    String s = "";
    TimeStat *ts = (TimeStat *) e;
     s +="agg=" + String(ts->m_agg) +  "tagg=" + String(ts->m_tagg) + " m_value=" + String(ts->m_value) + " m_tvalue=" + String(ts->m_tvalue) +  " m_tlevel=" + String(ts->get_level());
    return s;
}
   

CLICK_ENDDECLS
EXPORT_ELEMENT(TimeStat)


