Userspace RCU Library:

What Linear Multiprocessor
Scalability Means for Your
Application

Linux Plumbers Conference 2009

Mathieu Desnoyers
École Polytechnique de Montréal

{"image": "A cartoon illustration of a mole located in the bottom right corner. The mole is dark grey with a lighter grey belly. It is wearing small, round, black-rimmed glasses. It has a large, pink, fleshy nose with several small whiskers protruding from the sides. The mole is facing left and is depicted from the chest up, with a slight smile."}



> Mathieu Desnoyers

* Author/maintainer of :
    - LTTV (Linux Trace Toolkit Viewer)
        - 2003...
    - LTTng (Linux Trace Toolkit Next Generation)
        - 2005...
    - Immediate Values
        - 2007...
    - Tracepoints
        - 2008...
    - Userspace RCU Library
        - 2009...

2

[Image: A cartoon illustration of a stylized, plump, purple-grey mole. The mole is viewed from the side, facing right. It has a rounded body, a pointed snout, and is wearing small, round, black-rimmed glasses. It has small, pink, clawed paws resting on a surface. The art style is digital with clean lines and soft shading.]



> Contributions by

* Paul E. McKenney
    - IBM Linux Technology Center
* Alan Stern
    - Rowland Institute, Harvard University
* Jonathan Walpole
    - Computer Science Department, Portland State University
* Michel Dagenais
    - Computer and Software Engineering Dpt., École Polytechnique de Montréal

{"image": "A cartoon illustration of a stylized, grey-purple mole located in the bottom right corner. The mole is depicted from the side, facing left, with a rounded body, small paws, and a pinkish snout. It is wearing small, round, black-rimmed glasses on its face. The art style is clean with bold outlines and simple shading."}

3



> Summary

* RCU Overview
* Kernel vs Userspace RCU
* Userspace RCU Library
* Benchmarks
* RCU-Friendly Applications

4

[Image: A cartoon illustration of a mole located in the bottom right corner. The mole is depicted from the side, facing right. It has dark grey/purple fur, a pinkish snout, and small pink claws. It is wearing round, black-rimmed glasses. The art style is clean with bold outlines and simple shading.]



> Linux Kernel RCU Usage

```jsonl
{"type": "line_plot", "title": "Linux Kernel RCU Usage", "x_axis": "Year", "x_ticks": [2002, 2003, 2004, 2005, 2006, 2007, 2008, 2009, 2010], "y_axis": "# RCU API Uses", "y_ticks": [0, 500, 1000, 1500, 2000, 2500, 3000], "data_points": [{"x": 2002, "y": 0}, {"x": 2003, "y": 150}, {"x": 2004, "y": 300}, {"x": 2005, "y": 450}, {"x": 2006, "y": 800}, {"x": 2007, "y": 1500}, {"x": 2008, "y": 1800}, {"x": 2009, "y": 2400}, {"x": 2010, "y": 2600}]}
```

5

[Image description: A cartoon illustration of a mole's head and shoulders appearing from the bottom right corner. The mole is a muted purple-grey color with a light grey snout and chin. It has small, dark eyes partially obscured by round, black-rimmed spectacles. The mole has small, pinkish, clawed paws resting on the bottom edge of the frame. The style is clean 2D vector art with bold black outlines and simple shading.]



> RCU Overview

* Relativistic programming
    - Updates seen in different orders by CPUs
    - Tolerates conflicts
* Linear scalability
* Wait-free read-side
* Efficient updates
    - Only a single pointer exchange needs exclusive access

6

[Image: A cartoon illustration of a purple-grey mole positioned in the bottom right corner. The mole is wearing round, black-rimmed glasses and has a small, pinkish nose. Its large, pink, clawed paws are visible at the bottom, resting on the edge of the frame. The mole has a slightly grumpy or focused expression, with a few short whiskers protruding from its snout.]



> Schematic of RCU Update and Read-Side C.S.

[IMAGE: A technical sequence diagram illustrating Read-Copy Update (RCU) mechanisms. The y-axis is labeled "Threads" with a curly bracket encompassing four reader threads and one updater thread. The x-axis is labeled "Time" with an arrow pointing right.
- Reader 1 has three rectangular blocks labeled "reads", separated by gaps.
- Reader 2 has two rectangular blocks labeled "reads".
- Reader 3 has two rectangular blocks labeled "reads".
- Reader 4 has two rectangular blocks labeled "reads".
- The Updater thread has a single long horizontal bar divided into three sections: "removal", "grace period", and "reclamation".
- A vertical line drops down from the end of the "removal" section.
- An arrow labeled "rcu_read_lock()" points to the start of the second "reads" block of Reader 1.
- An arrow labeled "rcu_read_unlock()" points to the end of the second "reads" block of Reader 1.
- A label "Pre-existing reads" with arrows points to the "reads" blocks of Reader 1, 2, and 3 that overlap with the "removal" phase.
- An arrow labeled "rcu_assign_pointer()" points to the transition between "removal" and "grace period".
- An arrow labeled "synchronize_rcu()" points to the end of the "grace period" section.
- A double-headed horizontal arrow spans the "grace period" section with a caption: "Grace period waits for completion of pre-existing reads".
- In the bottom right corner, there is a cartoon illustration of a purple platypus wearing round glasses, looking towards the left.]

7



> RCU Linked-List Deletion

{"column": "Updater", "steps": [
  {"state": "A -> B -> C"},
  {"action": "list_del_rcu(B)"},
  {"state": "A -> B -> C (A now points to C, B still pointed to by some)"},
  {"action": "synchronize_rcu()"},
  {"state": "A -> B -> C (Waiting for grace period)"},
  {"action": "free(B)"},
  {"state": "A -> C"}
]}

{"column": "Reader Initiated Before Start of Grace Period", "steps": [
  {"state": "A -> B -> C"},
  {"state": "A -> B -> C (Reader still sees B)"},
  {"state": "A -> B -> C (Reader finishes access to B)"}
]}

{"column": "Reader Initiated After Start of Grace Period", "steps": [
  {"state": "null"},
  {"state": "null"},
  {"state": "A -> C (Reader only sees updated list)"},
  {"state": "A -> C"}
]}

[Image description: A cartoon illustration of a purple mole wearing round black glasses. The mole is positioned in the bottom right corner, facing left. It has a light pink snout and small pink claws visible beneath its chin. The style is a clean vector drawing with bold outlines and soft shading.]

8



> Kernel vs Userspace RCU

* Quiescent state
    - Kernel threads
        * Wait for kernel pre-existing RCU read-side C.S. to complete
    - User threads
        * Wait for process pre-existing RCU read-side C.S. to complete

9

[Image: A stylized cartoon illustration of a mole's head facing right, positioned in the bottom right corner. The mole has dark grey/purplish fur, a large pinkish-beige snout with visible nostrils, and small, pointed claws visible under its chin. It is wearing small, round, black-rimmed glasses over its eyes, giving it an academic or studious appearance. The art style is clean with bold outlines and simple shading.]



> Userspace RCU Library

* QSBR
    - liburcu-qsbr.so
* Generic RCU
    - liburcu-mb.so
* Signal-based RCU
    - liburcu.so
* call_rcu()
    - liburcu-defer.so

10

{"image_description": "A digital illustration of a stylized, purple-grey mole located in the bottom right corner. The mole is wearing round, black-rimmed glasses and is facing right. It has a pinkish, segmented nose/snout with visible whiskers and small, claw-like paws visible underneath its chin. The art style is a clean, cartoonish vector illustration with a white outline separating the character from the white background."}



> QSBR

* Detection of quiescent state:
    - Each reader thread calls rcu_quiescent_state() periodically.
* Require application modification
* Read-side with very low overhead

{"image_description": "A cartoon illustration of a purple-grey mole located in the bottom right corner. The mole is facing left, wearing small, round black-rimmed spectacles. It has small, pink, clawed paws and a pinkish nose. The art style is clean with bold outlines and simple shading."}

11



> Generic RCU

• Detection of quiescent state:
  – rcu_read_lock()/rcu_read_unlock() mark the beginning/end of the critical sections
  – Counts nesting level
• Suitable for library use
• Higher read-side overhead than QSBR due to added memory barriers

12

[Image description: A digital illustration of a cartoon mole's head appearing from the bottom right corner of the frame. The mole is dark grey/purple with a rounded snout, small ears, and large round glasses with black frames. It has several small, pinkish-white claws visible beneath its chin, as if it is digging upwards. The art style is clean with soft shading and bold outlines.]



> Signal-based RCU

* Same quiescent state detection as Generic RCU
* Suitable for library use, but reserves a signal
* Read-side close to QSBR performance
    - Remove memory barriers from rcu_read_lock()/rcu_read_unlock().
    - Replaced by memory barriers in signal handler, executed at each update-side memory barrier.

13

{"image_description": "A cartoon illustration of a grey mole located in the bottom right corner. The mole is wearing round, black-rimmed glasses and is partially cropped by the edge of the slide. It has a pinkish nose and small pink claws visible at the bottom."}



> call_rcu()

* Eliminates the need to call synchronize_rcu() after each removal
* Queues RCU callbacks for deferred batched execution
* Wait-free unless per-thread queue is full
* “Worker thread” executes callbacks periodically
* Energy-efficient, uses sys_futex()

[Image description: A digital illustration of a stylized, cartoonish mole located in the bottom right corner. The mole is grayish-purple with a rounded body, small ears, and a large pinkish-tan snout. It is wearing small, round, black-rimmed spectacles. The mole is depicted from the chest up, appearing to peek into the frame from the bottom right edge.]

14



> Example: RCU Read-Side

struct mystruct *rcudata = &somedata;

/* register thread with rcu_register_thread()/rcu_unregister_thread() */
void fct(void)
{
    struct mystruct *ptr;

    rcu_read_lock();
    ptr = rcu_dereference(rcudata);
    /* use ptr */
    rcu_read_unlock();
}

15

{"image_description": "A cartoon illustration of a grey, plump, mole-like creature positioned in the bottom right corner. The creature is facing left with a slightly grumpy or focused expression. It has small, beady eyes behind round, black-rimmed glasses and a pinkish, fleshy snout with several small whiskers. The art style is clean with bold black outlines and simple shading."}



> Example: exchange pointer

struct mystruct *rcudata = &somedata;

void replace_data(struct mystruct data)
{
    struct mystruct *new, *old;

    new = malloc(sizeof(*new));
    memcpy(new, &data, sizeof(*new));
    old = rcu_xchg_pointer(&rcudata, new);
    call_rcu(free, old);
}

16

{"image_description": "A stylized, cartoon illustration of a purple-grey gopher's head facing right. The gopher has a large, round nose, a small mouth with two visible front teeth, and is wearing round black-rimmed glasses. Its fur is textured with short, dark strokes. The image is positioned in the bottom right corner of the slide."}



> Example:
compare-and-exchange pointer

struct mystruct *rcudata = &somedata;

/* register thread with rcu_register_thread()/rcu_unregister_thread() */
void modify_data(int increment_a, int increment_b)
{
    struct mystruct *new, *old;

    new = malloc(sizeof(*new));
    rcu_read_lock();    /* Ensure pointer is not re-used */
    do {
        old = rcu_dereference(rcudata);
        memcpy(new, old, sizeof(*new));
        new->field_a += increment_a;
        new->field_b += increment_b;
    } while (rcu_cmpxchg_pointer(&rcudata, old, new) != old);
    rcu_read_unlock();
    call_rcu(free, old);
}

17

[Image Description: A cartoon illustration of a grey, stylized animal character, possibly a gopher or similar rodent, located in the bottom right corner. The character is wearing round black-rimmed glasses and has a mischievous or focused expression. It is depicted from the shoulders up, facing left, with its small pink paws visible at the bottom, appearing to be interacting with or resting on the edge of the slide. The art style is clean with bold black outlines and flat grey coloring with subtle shading.]



> Benchmarks

* Read-side Scalability
* Read-side C.S. length impact
* Update Overhead

[Image description: A digital illustration of a cartoon mole positioned in the bottom right corner. The mole has grey-purple fur, a large pinkish-beige nose, and small pink claws. It is wearing round, black-rimmed spectacles. The mole is facing left, looking slightly upwards with a focused expression.]

18



# Read-Side Scalability

```jsonl
{"type": "x-y plot", "title": "Read-Side Scalability", "x_axis": "Number of cores", "y_axis": "Number of reads / second", "series": [{"label": "QSBR", "trend": "linear increase", "start_point": [0, 0], "end_point": [64, 8e+09]}, {"label": "Signal-based RCU", "trend": "linear increase", "start_point": [0, 0], "end_point": [64, 6.5e+09]}, {"label": "General-purpose RCU", "trend": "linear increase", "start_point": [0, 0], "end_point": [64, 1e+09]}, {"label": "Per-thread mutex", "trend": "flat/near-zero", "start_point": [0, 0], "end_point": [64, 0.2e+09]}, {"label": "pthread mutex", "trend": "flat/near-zero", "start_point": [0, 0], "end_point": [64, 0.1e+09]}, {"label": "pthread reader-writer lock", "trend": "flat/near-zero", "start_point": [0, 0], "end_point": [64, 0.1e+09]}]}
```

64-cores POWER5+

19

[Image description: A cartoon illustration of a GNU (wildebeest) head in profile, facing right. The GNU is depicted in a muted purple-grey color with a lighter grey underbelly and muzzle. It has a prominent, curved horn on its head and is wearing small, round, black-rimmed glasses. Its mouth is slightly open, revealing a pink tongue and lower teeth. The art style is clean vector art with bold black outlines.]



> Read-Side C.S. Length Impact

```jsonl
{"type": "x-y plot", "title": "Read-Side C.S. Length Impact", "x_axis": "Read-side C.S. length (in cycles)", "x_scale": "logarithmic", "x_range": ["0.1", "1e+06"], "y_axis": "Number of reads / second", "y_scale": "logarithmic", "y_range": ["100", "1e+10"], "series": [{"label": "QSBR", "marker": "plus", "trend": "Starts near 1e+10 at x=0.1, stays flat until x=1, then declines linearly on log scale to approx 1e+04 at x=1e+06"}, {"label": "Signal-based RCU", "marker": "cross", "trend": "Starts near 1e+10 at x=0.1, stays flat until x=1, then declines linearly on log scale to approx 1e+04 at x=1e+06"}, {"label": "General-purpose RCU", "marker": "asterisk", "trend": "Starts near 1e+09 at x=0.1, stays flat until x=1, then declines linearly on log scale to approx 1e+04 at x=1e+06"}, {"label": "Per-thread mutex", "marker": "open square", "trend": "Starts near 1e+09 at x=0.1, stays flat until x=10, then declines linearly on log scale to approx 1e+04 at x=1e+06"}, {"label": "pthread reader-writer lock", "marker": "filled square", "trend": "Starts near 3e+06 at x=0.1, stays flat until x=100, then declines linearly on log scale to approx 1e+04 at x=1e+06"}, {"label": "pthread mutex", "marker": "open circle", "trend": "Starts near 3e+06 at x=0.1, stays flat until x=10, then declines linearly on log scale to approx 300 at x=1e+06"}]}
```

64-cores POWER5+, logarithmic scale (x, y)

20

[Image description: A cartoon illustration of a mole's head appearing from the bottom right corner. The mole is purple-grey with a pinkish snout and small pink claws visible. It is wearing round, black-rimmed glasses. The style is a clean vector illustration with bold black outlines.]



> Update Overhead

```jsonl
{"type": "x-y plot", "title": "Update Overhead", "x_axis": "Updates/s", "x_scale": "logarithmic", "y_axis": "Reads/s", "y_scale": "logarithmic", "series": [{"label": "QSBR", "marker": "+", "data": "high plateau across x-axis, slight dip at 1e+06"}, {"label": "Signal-based RCU", "marker": "x", "data": "no visible data points"}, {"label": "General-purpose RCU", "marker": "*", "data": "plateau around 1e+09, sharp decline after 1e+05"}, {"label": "Per-thread mutex", "marker": "open square", "data": "plateau around 2e+08, ends at 1e+04"}, {"label": "pthread reader-writer lock", "marker": "filled square", "data": "low plateau around 1e+06, stable until 1e+02"}, {"label": "pthread mutex", "marker": "open circle", "data": "low plateau around 1e+06, slight rise then decline after 1e+05"}]}
```

64-cores POWER5+, logarithmic scale (x, y)

{"description": "A cartoon illustration of a grey, stylized animal (possibly a mole or shrew) viewed from the side. It has a rounded body, small ears, and is wearing black-rimmed circular glasses. The creature's mouth is open, and it appears to be shouting or screaming, with stylized pinkish-white sound waves or breath emanating from its mouth. The art style is a clean vector drawing with bold outlines and simple shading."}

21



> RCU-Friendly Applications

* Multithreaded applications with read-often shared data
    - Cache
        * Name servers
        * Proxy
        * Web servers with static pages
    - Configuration
        * Low synchronization overhead
        * Dynamically modified without restart

22

[Image description: A cartoon illustration of a grey, plump, mole-like creature positioned in the bottom right corner. The creature is facing left, wearing small, round, black-rimmed spectacles. It has a pinkish, fleshy snout and small, clawed paws visible at the bottom. The art style is a clean digital vector with a thick dark outline and soft shading.]



> RCU-Friendly Applications

* Libraries supporting multithreaded applications
    - Tracing library, e.g. lib UST (LTTng port for userspace tracing)
        * http://git.dorsal.polymtl.ca/?p=ust.git

{"image_description": "A cartoon illustration of a purple-grey mole located in the bottom right corner. The mole is facing left, wearing small round spectacles on its nose. It has small pink claws digging into the bottom edge of the frame. The art style is clean digital 2D vector art with a white highlight on the top of the head."}

23



> RCU-Friendly Applications

* Libraries supporting multithreaded applications (cont.)
    - Typing/data structure support
        * Typing system
            - Creation of a class is a rare event
            - Reading class structure happens at object creation/destruction (_very_ often)
            - Applies to gobject
                * Used by: gtk/gdk/glib/gstreamer...
        * Efficient hash tables
        * Glib “quarks”

{"image_description": "A cartoon illustration of a purple gopher located in the bottom right corner. The gopher is wearing round glasses and has a focused or slightly stressed expression. It is depicted from the chest up, with its small paws held up near its face."}

24



> RCU-Friendly Applications

* Routing tables in userspace
* Userspace network stacks
* Userspace signal-handling
    - Signal-safe read-side
    - Could implement an inter-thread signal multiplexer
* Your own ?

[Image Description: A cartoon illustration of a purple-grey mole located in the bottom right corner. The mole is wearing round, black-rimmed spectacles. It has a large, pinkish, fleshy nose with visible skin folds and small, clawed paws resting beneath its chin. The art style is a clean, digital 2D vector illustration with soft shading.]

25



> Info / Download / Contact

* Mathieu Desnoyers
    - Computer and Software Engineering Dpt., École Polytechnique de Montréal
* Web site:
    - http://www.lttng.org/urcu
* Git tree
    - git://lttng.org/userspace-rcu.git
* Email
    - mathieu.desnoyers@polymtl.ca

26

[Image: A stylized, cartoon illustration of a purple-grey mole. The mole is depicted from the side/front, wearing small, round, black-rimmed glasses. It has a large, bulbous pink nose and small, pale pink claws protruding from its front paws. The mole has a slight, neutral expression and is shaded to give a 3D rounded effect, set against a plain white background.]
