#include <click/element.hh>
#include <click/timer.hh>

 class PeriodicPrinter : public Element { public:
     PeriodicPrinter();
     const char *class_name() const { return "PeriodicPrinter"; }
     int initialize(ErrorHandler *errh);
     void run_timer(Timer *timer);
   private:
     Timer _timer;
 };

 PeriodicPrinter::PeriodicPrinter()
     : _timer(this)    // Sets _timer to call this->run_timer(&_timer)
 {                     // when it fires.
 }

 int PeriodicPrinter::initialize(ErrorHandler *) {
     _timer.initialize(this);   // Initialize timer object (mandatory).
     _timer.schedule_now();     // Set the timer to fire as soon as the
                                // router runs.
     return 0;
 }

 void PeriodicPrinter::run_timer(Timer *timer) {
     // This function is called when the timer fires.
     assert(timer == &_timer);
     Timestamp now = Timestamp::now_steady();
     click_chatter("%s: %p{timestamp}: timer fired with expiry %p{timestamp}!\n",
                   declaration().c_str(), &now, &_timer.expiry_steady());
		   // _timer.expiry_steady() is the steady-clock Timestamp
		   // at which the timer was set to fire.
     _timer.reschedule_after_sec(5);  // Fire again 5 seconds later.
 }
