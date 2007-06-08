#ifndef TAMER_EVENT_HH
#define TAMER_EVENT_HH 1
#include <tamer/xevent.hh>
#include <functional>
namespace tamer {

/** @file <tamer/event.hh>
 *  @brief  The event template classes and helper functions.
 */

/** @class event tamer/event.hh <tamer/tamer.hh>
 *  @brief  A future occurrence.
 *
 *  Each event object represents a future occurrence, such as the completion
 *  of a network read.  When the expected occurrence actually happens---for
 *  instance, a packet arrives---the event is triggered via its trigger()
 *  method.  A function can wait for the event using @c twait special forms,
 *  which allow event-driven code to block.
 *
 *  Events have from zero to four trigger slots of arbitrary type.  For
 *  example, an event of type <tt>event<int, char*, bool></tt> has three
 *  trigger slots with types <tt>int</tt>, <tt>char*</tt>, and <tt>bool</tt>.
 *  The main constructors for <tt>event<int, char*, bool></tt> take three
 *  reference arguments of types <tt>int&</tt>, <tt>char*&</tt>, and
 *  <tt>bool&</tt>, and its trigger() method takes value arguments of types
 *  <tt>int</tt>, <tt>char*</tt>, and <tt>bool</tt>.  Calling <tt>e.trigger(1,
 *  "Hi", false)</tt> will set the trigger slots to the corresponding values.
 *  This can be used to pass information back to the function waiting for the
 *  event.
 *
 *  Each event is in one of two states, active or empty.  An active event is
 *  ready to be triggered, while an empty event has already been triggered.
 *  Events can be triggered at most once; triggering an empty event has no
 *  additional effect.  The empty() and operator unspecified_bool_type()
 *  member functions test whether an event is empty or active.
 *
 *  <pre>
 *      Constructors               Default constructor
 *           |                             |
 *           v                             v
 *         ACTIVE   ==== trigger ====>   EMPTY   =====+
 *                                         ^       trigger
 *                                         |          |
 *                                         +==========+
 *  </pre>
 *
 *  Multiple event objects may refer to the same underlying occurrence.
 *  Triggering an event can thus affect several event objects.  For instance,
 *  after an assignment <tt>e1 = e2</tt>, @c e1 and @c e2 refer to the same
 *  occurrence.  Either <tt>e1.trigger()</tt> or <tt>e2.trigger()</tt> would
 *  trigger the underlying occurrence, making both @c e1 and @c e2 empty.
 *
 *  The bind_all() method produces a version of the event that has no slots.
 *  Triggering this event triggers the underlying occurrence, but does not
 *  change the values stored in the trigger slots.  For instance:
 *
 *  @code
 *     tvars { event<int> e; int x = 0; }
 *
 *     twait {
 *        e = make_event(x);
 *        e.trigger(1);
 *     }
 *     printf("%d\n", x);           // will print 1
 *
 *     twait {
 *        e = make_event(x);
 *        e.bind_all().trigger();
 *        e.trigger(2);                  // ignored
 *     }
 *     printf("%d\n", x);           // will print 1
 *  @endcode
 *
 *  An event is automatically triggered when the last reference to its
 *  underlying occurrence goes out of scope.  As with bind_all(), no trigger
 *  slots are assigned in this case.
 *
 *  Events can have associated trigger notifiers, which are triggered when the
 *  event itself is triggered.  A trigger notifier is simply an
 *  <tt>event<></tt> object.
 *
 *  Events are usually created with the make_event() helper function, which
 *  automatically detects the right type of event to return.
 */
template <typename T0, typename T1, typename T2, typename T3>
class event { public:

    /** @brief  Default constructor creates an empty event. */
    event()
	: _e(tamerpriv::simple_event::make_dead()) {
    }

    /** @brief  Construct a two-ID, four-slot event on rendezvous @a r.
     *  @param  r   Rendezvous.
     *  @param  i0  First event ID.
     *  @param  i1  Second event ID.
     *  @param  s0  First trigger slot.
     *  @param  s1  Second trigger slot.
     *  @param  s2  Third trigger slot.
     *  @param  s3  Fourth trigger slot.
     */
    template <typename R, typename I0, typename I1>
    event(R &r, const I0 &i0, const I1 &i1, T0 &s0, T1 &s1, T2 &s2, T3 &s3)
	: _e(new tamerpriv::simple_event(r, i0, i1)),
	  _s0(&s0), _s1(&s1), _s2(&s2), _s3(&s3) {
    }

    /** @brief  Construct a one-ID, four-slot event on rendezvous @a r.
     *  @param  r   Rendezvous.
     *  @param  i0  First event ID.
     *  @param  s0  First trigger slot.
     *  @param  s1  Second trigger slot.
     *  @param  s2  Third trigger slot.
     *  @param  s3  Fourth trigger slot.
     */
    template <typename R, typename I0>
    event(R &r, const I0 &i0, T0 &s0, T1 &s1, T2 &s2, T3 &s3)
	: _e(new tamerpriv::simple_event(r, i0)),
	  _s0(&s0), _s1(&s1), _s2(&s2), _s3(&s3) {
    }

    /** @brief  Construct a no-ID, four-slot event on rendezvous @a r.
     *  @param  r   Rendezvous.
     *  @param  s0  First trigger slot.
     *  @param  s1  Second trigger slot.
     *  @param  s2  Third trigger slot.
     *  @param  s3  Fourth trigger slot.
     */
    template <typename R>
    event(R &r, T0 &s0, T1 &s1, T2 &s2, T3 &s3)
	: _e(new tamerpriv::simple_event(r)),
	  _s0(&s0), _s1(&s1), _s2(&s2), _s3(&s3) {
    }

    /** @brief  Construct event for the same occurrence as @a e.
     *  @param  e  Source event.
     */
    event(const event<T0, T1, T2, T3> &e)
	: _e(e._e), _s0(e._s0), _s1(e._s1), _s2(e._s2), _s3(e._s3) {
	_e->use();
    }

    /** @brief  Destroy the event instance.
     *  @note   The underlying occurrence is canceled if this event was the
     *          last remaining reference.
     */
    ~event() {
	_e->unuse();
    }

    typedef tamerpriv::simple_event::unspecified_bool_type unspecified_bool_type;

    /** @brief  Test if event is active.
     *  @return  True if event is active, false if it is empty. */
    operator unspecified_bool_type() const {
	return *_e;
    }

    /** @brief  Test if event is empty.
     *  @return  True if event is empty, false if it is active. */
    bool operator!() const {
	return _e->empty();
    }

    /** @brief  Test if event is empty.
     *  @return  True if event is empty, false if it is active. */
    bool empty() const {
	return _e->empty();
    }

    /** @brief  Trigger event.
     *  @param  v0  First trigger value.
     *  @param  v1  Second trigger value.
     *  @param  v2  Third trigger value.
     *  @param  v3  Fourth trigger value.
     *
     *  Does nothing if event is empty.
     */
    void trigger(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3) {
	if (_e->trigger()) {
	    if (_s0) *_s0 = v0;
	    if (_s1) *_s1 = v1;
	    if (_s2) *_s2 = v2;
	    if (_s3) *_s3 = v3;
	}
    }

    /** @brief  Trigger event.
     *  @param  v0  First trigger value.
     *  @param  v1  Second trigger value.
     *  @param  v2  Third trigger value.
     *  @param  v3  Fourth trigger value.
     *
     *  Does nothing if event is empty.
     *
     *  @note   This is a synonym for trigger().
     */
    void operator()(const T0 &v0, const T1 &v1, const T2 &v2, const T3 &v3) {
	trigger(v0, v1, v2, v3);
    }

    typedef void result_type;
    typedef const T0 &first_argument_type;
    typedef const T1 &second_argument_type;
    typedef const T2 &third_argument_type;
    typedef const T3 &fourth_argument_type;

    /** @brief  Register a trigger notifier.
     *  @param  e  Trigger notifier.
     *
     *  If this event is empty, @a e is triggered immediately.  Otherwise,
     *  when this event is triggered, triggers @a e.
     */
    void at_trigger(const event<> &e) {
	_e->at_trigger(e);
    }

    /** @brief  Return a no-slot event for the same occurrence as @a e.
     *  @return  New event.
     *
     *  The returned event refers to the same occurrence as this event, so
     *  triggering either event makes both events empty, but the returned
     *  event has no trigger slots.
     */
    inline event<> bind_all() const;
    
    /** @brief  Assign this event to the same occurrence as @a e.
     *  @param  e  Source event.
     */
    event<T0, T1, T2, T3> &operator=(const event<T0, T1, T2, T3> &e) {
	e._e->use();
	_e->unuse();
	_e = e._e;
	_s0 = e._s0;
	_s1 = e._s1;
	_s2 = e._s2;
	_s3 = e._s3;
	return *this;
    }

    /** @internal
     *  @brief  Fetch underlying occurrence.
     *  @return  Underlying occurrence.
     */
    tamerpriv::simple_event *__get_simple() const {
	return _e;
    }
    
  private:

    tamerpriv::simple_event *_e;
    T0 *_s0;
    T1 *_s1;
    T2 *_s2;
    T3 *_s3;

};


/** @cond specialized_events
 *
 *  Events may be declared with three, two, one, or zero template arguments,
 *  as in <tt>event<T0, T1, T2></tt>, <tt>event<T0, T1></tt>,
 *  <tt>event<T0></tt>, and <tt>event<></tt>.  Each specialized event class
 *  has functions similar to the full-featured event, but with parameters to
 *  constructors and @c trigger methods appropriate to the template arguments.
 */

template <typename T0, typename T1, typename T2>
class event<T0, T1, T2, void> { public:

    event()
	: _e(tamerpriv::simple_event::make_dead()) {
    }

    template <typename R, typename I0, typename I1>
    event(R &r, const I0 &i0, const I1 &i1, T0 &s0, T1 &s1, T2 &s2)
	: _e(new tamerpriv::simple_event(r, i0, i1)),
	  _s0(&s0), _s1(&s1), _s2(&s2) {
    }

    template <typename R, typename I0>
    event(R &r, const I0 &i0, T0 &s0, T1 &s1, T2 &s2)
	: _e(new tamerpriv::simple_event(r, i0)),
	  _s0(&s0), _s1(&s1), _s2(&s2) {
    }

    template <typename R>
    event(R &r, T0 &s0, T1 &s1, T2 &s2)
	: _e(new tamerpriv::simple_event(r)),
	  _s0(&s0), _s1(&s1), _s2(&s2) {
    }

    event(const event<T0, T1, T2> &e)
	: _e(e._e), _s0(e._s0), _s1(e._s1), _s2(e._s2) {
	_e->use();
    }
    
    ~event() {
	_e->unuse();
    }

    typedef tamerpriv::simple_event::unspecified_bool_type unspecified_bool_type;
    
    operator unspecified_bool_type() const {
	return *_e;
    }

    bool operator!() const {
	return _e->empty();
    }
    
    bool empty() const {
	return _e->empty();
    }

    void trigger(const T0 &v0, const T1 &v1, const T2 &v2) {
	if (_e->trigger()) {
	    if (_s0) *_s0 = v0;
	    if (_s1) *_s1 = v1;
	    if (_s2) *_s2 = v2;
	}
    }

    void operator()(const T0 &v0, const T1 &v1, const T2 &v2) {
	trigger(v0, v1, v2);
    }

    typedef void result_type;
    typedef const T0 &first_argument_type;
    typedef const T1 &second_argument_type;
    typedef const T2 &third_argument_type;

    void at_trigger(const event<> &e) {
	_e->at_trigger(e);
    }

    inline event<> bind_all() const;
    
    event<T0, T1, T2> &operator=(const event<T0, T1, T2> &e) {
	e._e->use();
	_e->unuse();
	_e = e._e;
	_s0 = e._s0;
	_s1 = e._s1;
	_s2 = e._s2;
	return *this;
    }
    
    tamerpriv::simple_event *__get_simple() const {
	return _e;
    }
    
  private:

    tamerpriv::simple_event *_e;
    T0 *_s0;
    T1 *_s1;
    T2 *_s2;
    
};


template <typename T0, typename T1>
class event<T0, T1, void, void>
    : public std::binary_function<const T0 &, const T1 &, void> { public:

    event()
	: _e(tamerpriv::simple_event::make_dead()) {
    }

    template <typename R, typename I0, typename I1>
    event(R &r, const I0 &i0, const I1 &i1, T0 &s0, T1 &s1)
	: _e(new tamerpriv::simple_event(r, i0, i1)), _s0(&s0), _s1(&s1) {
    }

    template <typename R, typename I0>
    event(R &r, const I0 &i0, T0 &s0, T1 &s1)
	: _e(new tamerpriv::simple_event(r, i0)), _s0(&s0), _s1(&s1) {
    }

    template <typename R>
    event(R &r, T0 &s0, T1 &s1)
	: _e(new tamerpriv::simple_event(r)), _s0(&s0), _s1(&s1) {
    }

    event(const event<T0, T1> &e)
	: _e(e._e), _s0(e._s0), _s1(e._s1) {
	_e->use();
    }
    
    ~event() {
	_e->unuse();
    }

    typedef tamerpriv::simple_event::unspecified_bool_type unspecified_bool_type;
    
    operator unspecified_bool_type() const {
	return *_e;
    }

    bool operator!() const {
	return _e->empty();
    }
    
    bool empty() const {
	return _e->empty();
    }

    void trigger(const T0 &v0, const T1 &v1) {
	if (_e->trigger()) {
	    if (_s0) *_s0 = v0;
	    if (_s1) *_s1 = v1;
	}
    }

    void operator()(const T0 &v0, const T1 &v1) {
	trigger(v0, v1);
    }

    void at_trigger(const event<> &e) {
	_e->at_trigger(e);
    }

    inline event<> bind_all() const;
    
    event<T0, T1> &operator=(const event<T0, T1> &e) {
	e._e->use();
	_e->unuse();
	_e = e._e;
	_s0 = e._s0;
	_s1 = e._s1;
	return *this;
    }
    
    tamerpriv::simple_event *__get_simple() const {
	return _e;
    }
    
  private:

    tamerpriv::simple_event *_e;
    T0 *_s0;
    T1 *_s1;

};


template <typename T0>
class event<T0, void, void, void>
    : public std::unary_function<const T0 &, void> { public:

    event()
	: _e(tamerpriv::simple_event::make_dead()) {
    }

    template <typename R, typename I0, typename I1>
    event(R &r, const I0 &i0, const I1 &i1, T0 &s0)
	: _e(new tamerpriv::simple_event(r, i0, i1)), _s0(&s0) {
    }

    template <typename R, typename I0>
    event(R &r, const I0 &i0, T0 &s0)
	: _e(new tamerpriv::simple_event(r, i0)), _s0(&s0) {
    }

    template <typename R>
    event(R &r, T0 &s0)
	: _e(new tamerpriv::simple_event(r)), _s0(&s0) {
    }

    inline event(const event<> &e, const no_slot &marker);

    event(const event<T0> &e)
	: _e(e._e), _s0(e._s0) {
	_e->use();
    }
    
    ~event() {
	_e->unuse();
    }

    typedef tamerpriv::simple_event::unspecified_bool_type unspecified_bool_type;
    
    operator unspecified_bool_type() const {
	return *_e;
    }

    bool operator!() const {
	return _e->empty();
    }
    
    bool empty() const {
	return _e->empty();
    }

    void trigger(const T0 &v0) {
	if (_e->trigger()) {
	    if (_s0) *_s0 = v0;
	}
    }

    void operator()(const T0 &v0) {
	trigger(v0);
    }

    void at_trigger(const event<> &e) {
	_e->at_trigger(e);
    }

    inline event<> bind_all() const;
    
    event<T0> &operator=(const event<T0> &e) {
	e._e->use();
	_e->unuse();
	_e = e._e;
	_s0 = e._s0;
	return *this;
    }
    
    tamerpriv::simple_event *__get_simple() const {
	return _e;
    }
    
  private:

    tamerpriv::simple_event *_e;
    T0 *_s0;

};


template <>
class event<void, void, void, void> { public:

    event()
	: _e(tamerpriv::simple_event::make_dead()) {
    }

    template <typename R, typename I0, typename I1>
    event(R &r, const I0 &i0, const I1 &i1)
	: _e(new tamerpriv::simple_event(r, i0, i1)) {
    }

    template <typename R, typename I0>
    event(R &r, const I0 &i0)
	: _e(new tamerpriv::simple_event(r, i0)) {
    }

    template <typename R>
    explicit event(R &r)
	: _e(new tamerpriv::simple_event(r)) {
    }

    event(const event<> &e)
	: _e(e._e) {
	_e->use();
    }
    
    event(event<> &e)
	: _e(e._e) {
	_e->use();
    }
    
    ~event() {
	_e->unuse();
    }

    typedef tamerpriv::simple_event::unspecified_bool_type unspecified_bool_type;
    
    operator unspecified_bool_type() const {
	return *_e;
    }

    bool operator!() const {
	return _e->empty();
    }
    
    bool empty() const {
	return _e->empty();
    }

    void trigger() {
	_e->trigger();
    }

    void operator()() {
	trigger();
    }

    typedef void result_type;

    void at_trigger(const event<> &e) {
	_e->at_trigger(e);
    }

    event<> &bind_all() {
	return *this;
    }

    event<> bind_all() const {
	return *this;
    }

    event<> &operator=(const event<> &e) {
	e._e->use();
	_e->unuse();
	_e = e._e;
	return *this;
    }

    tamerpriv::simple_event *__get_simple() const {
	return _e;
    }

    static inline event<> __take(tamerpriv::simple_event *e) {
	return event<>(take_marker(), e);
    }

  private:

    tamerpriv::simple_event *_e;

    struct take_marker { };
    inline event(const take_marker &, tamerpriv::simple_event *e)
	: _e(e) {
    }
    
    friend class tamerpriv::simple_event;
    
};

/** @endcond */


/** @defgroup make_event Helper functions for making events
 *
 *  The @c make_event() helper function simplifies event creation.  @c
 *  make_event() automatically selects the right type of event for its
 *  arguments.  There are 3*5 = 15 versions, one for each combination of event
 *  IDs and trigger slots.
 *
 *  @{ */
 
/** @brief  Construct a two-ID, four-slot event on rendezvous @a r.
 *  @param  r   Rendezvous.
 *  @param  i0  First event ID.
 *  @param  i1  Second event ID.
 *  @param  t0  First trigger slot.
 *  @param  t1  Second trigger slot.
 *  @param  t2  Third trigger slot.
 *  @param  t3  Fourth trigger slot.
 *
 *  @note Versions of this function exist for any combination of two, one, or
 *  zero event IDs and four, three, two, one, or zero trigger slots.  For
 *  example, <tt>make_event(r)</tt> creates a zero-ID, zero-slot event on
 *  <tt>rendezvous<> r</tt>, while <tt>make_event(r, 1, i, j)</tt> might
 *  create a one-ID, two-slot event on <tt>rendezvous<int> r</tt>.
 */
template <typename I0, typename I1, typename J0, typename J1, typename T0, typename T1, typename T2, typename T3>
inline event<T0, T1, T2, T3> make_event(rendezvous<I0, I1> &r, const J0 &i0, const J1 &i1, T0 &s0, T1 &s1, T2 &s2, T3 &s3)
{
    return event<T0, T1, T2, T3>(r, i0, i1, s0, s1, s2, s3);
}

template <typename I0, typename J0, typename T0, typename T1, typename T2, typename T3>
inline event<T0, T1, T2, T3> make_event(rendezvous<I0> &r, const J0 &i0, T0 &s0, T1 &s1, T2 &s2, T3 &s3)
{
    return event<T0, T1, T2, T3>(r, i0, s0, s1, s2, s3);
}

template <typename T0, typename T1, typename T2, typename T3>
inline event<T0, T1, T2, T3> make_event(rendezvous<> &r, T0 &s0, T1 &s1, T2 &s2, T3 &s3)
{
    return event<T0, T1, T2, T3>(r, s0, s1, s2, s3);
}

template <typename I0, typename I1, typename J0, typename J1, typename T0, typename T1, typename T2>
inline event<T0, T1, T2> make_event(rendezvous<I0, I1> &r, const J0 &i0, const J1 &i1, T0 &s0, T1 &s1, T2 &s2)
{
    return event<T0, T1, T2>(r, i0, i1, s0, s1, s2);
}

template <typename I0, typename J0, typename T0, typename T1, typename T2>
inline event<T0, T1, T2> make_event(rendezvous<I0> &r, const J0 &i0, T0 &s0, T1 &s1, T2 &s2)
{
    return event<T0, T1, T2>(r, i0, s0, s1, s2);
}

template <typename T0, typename T1, typename T2>
inline event<T0, T1, T2> make_event(rendezvous<> &r, T0 &s0, T1 &s1, T2 &s2)
{
    return event<T0, T1, T2>(r, s0, s1, s2);
}

template <typename I0, typename I1, typename J0, typename J1, typename T0, typename T1>
inline event<T0, T1> make_event(rendezvous<I0, I1> &r, const J0 &i0, const J1 &i1, T0 &s0, T1 &s1)
{
    return event<T0, T1>(r, i0, i1, s0, s1);
}

template <typename I0, typename J0, typename T0, typename T1>
inline event<T0, T1> make_event(rendezvous<I0> &r, const J0 &i0, T0 &s0, T1 &s1)
{
    return event<T0, T1>(r, i0, s0, s1);
}

template <typename T0, typename T1>
inline event<T0, T1> make_event(rendezvous<> &r, T0 &s0, T1 &s1)
{
    return event<T0, T1>(r, s0, s1);
}

template <typename I0, typename I1, typename J0, typename J1, typename T0>
inline event<T0> make_event(rendezvous<I0, I1> &r, const J0 &i0, const J1 &i1, T0 &s0)
{
    return event<T0>(r, i0, i1, s0);
}

template <typename I0, typename J0, typename T0>
inline event<T0> make_event(rendezvous<I0> &r, const J0 &i0, T0 &s0)
{
    return event<T0>(r, i0, s0);
}

template <typename T0>
inline event<T0> make_event(rendezvous<> &r, T0 &s0)
{
    return event<T0>(r, s0);
}

template <typename I0, typename I1, typename J0, typename J1>
inline event<> make_event(rendezvous<I0, I1> &r, const J0 &i0, const J1 &i1)
{
    return event<>(r, i0, i1);
}

template <typename I0, typename J0>
inline event<> make_event(rendezvous<I0> &r, const J0 &i0)
{
    return event<>(r, i0);
}

inline event<> make_event(rendezvous<> &r)
{
    return event<>(r);
}

/** @} */

template <typename T0, typename T1, typename T2, typename T3>
inline event<> event<T0, T1, T2, T3>::bind_all() const {
    _e->use();
    return event<>::__take(_e);
}

template <typename T0, typename T1, typename T2>
inline event<> event<T0, T1, T2>::bind_all() const {
    _e->use();
    return event<>::__take(_e);
}

template <typename T0, typename T1>
inline event<> event<T0, T1>::bind_all() const {
    _e->use();
    return event<>::__take(_e);
}

template <typename T0>
inline event<T0>::event(const event<> &e, const no_slot &)
    : _e(e.__get_simple()), _s0(0) {
    _e->use();
}

template <typename T0>
inline event<> event<T0>::bind_all() const {
    _e->use();
    return event<>::__take(_e);
}

}
#endif /* TAMER__EVENT_HH */
