[Image: A digital illustration of a stylized mole. The mole is colored in shades of purple and grey with a rounded body and a pointed snout. It is wearing thick, round, black-rimmed glasses. It has small, pink, clawed paws visible beneath its body. The illustration is centered on a white background.]

LinuxCon North America 2016

Latency Outliers Root Cause Analysis in the Field by Combining Aggregation and Tracing Tools

EfficiOS

mathieu.desnoyers@efficios.com [email icon]
julien.desfossez@efficios.com [email icon]



Presenters

* Mathieu Desnoyers
    - CEO at EfficiOS
    - LTTng, Linux, Userspace RCU, Babeltrace maintainer.
* Julien Desfossez
    - Software Developer at EfficiOS
    - Latency Tracker, LTTng-Analyses, LTTngTop maintainer.

{"image_description": "Company logo located at the bottom left. The text reads 'EfficiOS'. The 'Effici' portion is written in a black, elegant serif font with a stylized, flowing cursive 'E'. The 'OS' portion is written in a bold, clean, red sans-serif font."}



Content

* Trace buffering vs in-place aggregation
* Automate problem analysis by combining aggregation and post-processing tools
* Periodic use-case demo
    - Jack audio server
* Aperiodic use-cases demos
    - Memcached
* Benchmarks
* Future Work

EfficiOS
3



Trace Buffering vs In-Place Aggregation

* Trace buffering:
    - Store events into a buffer,
    - Analysis performed at post-processing,
    - Multiple analyses can be performed on the same recorded trace,
    - E.g. Ftrace, Perf, LTTng.
* In-place aggregation:
    - Run-time analysis directly using event input,
    - Aggregation performed in the traced execution context,
    - E.g. eBPF, DTrace, SystemTAP.

{"image": "A logo located in the bottom left corner. It consists of the word 'EfficiOS'. The 'Effici' part is written in a black, stylized, slightly cursive serif font. The 'OS' part is written in a bold, sans-serif red font."}

4



Trace Buffering vs In-Place Aggregation

• Often presented as competing tracing solutions,
• In reality, can be combined to create powerful analysis tools.

EfficiOS
5



Combining Trace Buffering with Aggregation

[IMAGE: A technical flow diagram illustrating a system architecture for trace analysis. The diagram consists of five light-grey rectangular boxes connected by black arrows.
1. Top Right: A box labeled "LTTng flight recorder" with subtext "tracing Linux kernel and user-space (always on)".
2. Center Left: A box labeled "Latency tracker" with subtext "Tracking long response time".
3. Bottom Left: A box labeled "Trigger script".
4. Center Right: A box labeled "LTTng Analyses" with subtext "Summarize trace, statistical breakdown, identify outliers."
5. Bottom Row: Two boxes; the left one is "Trace Compass" with subtext "Graphical trace analyses", and the right one is "Babeltrace" with subtext "View trace as text log".

Flow: An arrow points from "Latency tracker" down to "Trigger script". A line exits "Trigger script", goes right, then up to point into "LTTng flight recorder". Another arrow points from "LTTng flight recorder" down into "LTTng Analyses". Finally, two arrows branch out from "LTTng Analyses", one pointing down to "Trace Compass" and one pointing down to "Babeltrace".
Text annotations are placed between the boxes: to the left of the trigger path, it says "Wake-up triggered by detected long response time", and above the path to the flight recorder, it says "Gather snapshot of detailed activity during the long response-time."]

EfficiOS
6



Latency Tracker

* Kernel module to track down latency problems at run-time,
* Simple API that can be called from anywhere in the kernel (tracepoints, kprobes, netfilter hooks, hardcoded in other module or the kernel tree source code),
* Keep track of entry/exit events and calls a callback if the delay between the two events is higher than a threshold.

{"image": "A logo in the bottom left corner. It consists of the word 'EfficiOS'. The 'Effici' part is in a black, stylized, slightly italicized serif font. The 'OS' part is in a bold, red, sans-serif font."}

7



Latency Tracker Usage

tracker = latency_tracker_create(threshold, timeout, callback);

latency_tracker_event_in(tracker, key);
.....
latency_tracker_event_out(tracker, key);

If the delay between the event_in and event_out for the same key is higher than “threshold”, the callback function is called.

The timeout parameter allows to launch the callback if the event_out takes too long to arrive (off-CPU profiling).

{"image": "The logo for 'EfficiOS'. The word 'Effici' is written in a black, elegant, italicized serif font. The 'OS' is written in a bold, sans-serif red font. The 'i' in Effici has a stylized dot that blends into the overall flow of the calligraphy."}

8



Latency Tracker: Low-Impact, Low-Overhead

* Memory allocation:
    - Custom memory allocator implemented with lock-free per-CPU RCU free-lists and pre-allocated NUMA pools,
    - Out-of-context worker thread can expand the memory pools as needed up to a user-configurable limit,
    - Prior to 3.17, custom call_rcu thread to avoid wake-up deadlock. Starting from 3.17, use call_rcu_sched().

* State tracking:
    - Userspace-rcu hashtable ported to the Linux kernel:
        - Lock-free insertion and removal, wait-free lookups

{"logo": "EfficiOS", "style": "The word 'Effici' is in a black, elegant serif italic font, followed by 'OS' in a bold, red sans-serif font. The 'E' in Effici is stylized with a large flourish."}

9



Implemented Latency Trackers

* Block layer: from block request issue to completion,
* Network: from socket buffer receive to consumption by user-space,
* Wake-up: from each thread wake-up to next scheduling of that thread,
* Off-cpu: from each thread preemption/blocking to next execution of that thread,
* IRQ handler: from irq handler entry to exit,
* System call: from system call entry to exit.

{"image": "Logo for 'EfficiOS'. The 'Effici' part is in a black, stylized cursive/serif font. The 'OS' part is in a bold, red sans-serif font."}

10



# Response Time: Interrupt to Thread Execution

{"type": "flowchart", "nodes": [
  {"id": "n1", "text": "do_IRQ_entry", "color": "red"},
  {"id": "n2", "text": "irq_handler_entry", "color": "blue"},
  {"id": "n3", "text": "softirq_raise", "color": "green"},
  {"id": "n4", "text": "irq_handler_exit", "color": "blue"},
  {"id": "n5", "text": "do_IRQ_exit", "color": "red"},
  {"id": "n6", "text": "softirq_entry", "color": "green"},
  {"id": "n7", "text": "sched_waking", "color": "yellow"},
  {"id": "n8", "text": "softirq_exit", "color": "green"},
  {"id": "n9", "text": "sched_switch", "color": "yellow"}
], "edges": [
  {"from": "n1", "to": "n2"},
  {"from": "n2", "to": "n3"},
  {"from": "n3", "to": "n4"},
  {"from": "n4", "to": "n5"},
  {"from": "n3", "to": "n6"},
  {"from": "n6", "to": "n7"},
  {"from": "n7", "to": "n8"},
  {"from": "n7", "to": "n9"}
]}

Mainline kernel hardware interrupts critical path

{"type": "logo", "description": "The word 'EfficiOS' located in the bottom left corner. 'Effici' is written in a black, stylized serif italic font, while 'OS' is written in a bold, red, sans-serif block font."}

11



Latency Tracker: Online Critical Path Analysis

* Measure response time,
* Execution contexts and wakeup chains tracking in kernel module
    - For both mainline kernel and preempt-rt,
    - NMI, IRQ, SoftIRQ, wakeup/scheduling chains.
* Follow critical path from interrupt servicing to completion of task,
* Can perform user-defined action when latencies are higher than a specified threshold,

{"image": "Logo for 'EfficiOS'. The word 'Effici' is written in a black, elegant, serif font with a slight italic lean. The 'OS' is written in a bold, sans-serif red font. The logo is positioned in the bottom left corner of the slide."}

12



Online Critical Path Analysis Configuration

• Passing parameters to latency tracker kernel module
    – Latency threshold,
    – Chain filters:
        • User-space task, pid, process name, RT task, Interrupt source (timer or IRQ/SoftIRQ number),
    – Chain stops when target task starts to run,
    – Chain stops when target task blocks,
• Track work begin/end with identifiers from instrumented user-space
    – Complex asynchronous use-cases.

[Image Description: A professional presentation slide. The top features a thick red horizontal banner containing the text "Online Critical Path Analysis Configuration" in a white, serif font. The main body of the slide is white with black text. The content consists of a bulleted list with two primary points and several sub-bullets. In the bottom left corner, there is a logo that reads "EfficiOS", where "Effici" is in a black stylized serif font and "OS" is in a bold red sans-serif font. In the bottom right corner, the page number "13" is written in a simple black sans-serif font.]



LTTng Kernel and User-Space Tracers

• Low-overhead, correlated kernel and user-space tracing,
    – Ring buffers in shared memory.
• User-defined filtering on event arguments,
• System-wide or tracking of specific processes,
• Optionally gather performance counters and extra fields as contexts.
• Support disk I/O output, in-memory flight recorder, network streaming, live reading.

[IMAGE: A presentation slide. At the top, there is a thick red horizontal banner spanning the width of the page containing the white text "LTTng Kernel and User-Space Tracers". The main body of the slide is a white background with a bulleted list of five items in black sans-serif text. The first bullet has a nested sub-bullet. In the bottom left corner, there is a logo consisting of the word "EfficiOS", where "Effici" is in a black stylized serif font and "OS" is in a bold red sans-serif font. In the bottom right corner, the number "14" is printed in black.]

EfficiOS 14



LTTng Kernel Tracer (LTTng-modules)

* Load kernel tracer modules (no kernel patching required!), or build into the Linux kernel image,
* LTTng kernel tracer hooks on:
    - Tracepoints,
    - System call entry/exit with detailed argument content,
    - Kprobes,
    - Kretprobes.

{"logo": "The word 'EfficiOS' located in the bottom left corner. 'Effici' is written in a black, stylized serif font where the 'E' is large and decorative. 'OS' is written in a bold, red, sans-serif font."}

15



LTTng User-Space Tracer (LTTng-UST)

• Dynamically loaded shared library,
• Fast user-space tracing, fast-path entirely in user-space,
• Instruments:
    – Application and libraries with lttng-ust tracepoints, tracef, tracelog,
    – Java JUL and Log4j loggers, Python logger,
    – Malloc, pthread mutex with symbol override,
    – Function entry/exit by compiling with -finstrument-functions.
• Dumps base address information required to map process addresses to executable and library functions/source code using ELF and DWARF.

[Image: A logo located at the bottom left consisting of the text "EfficiOS". The "Effici" is written in a black, serif, italicized font. The "OS" is written in a bold, red, sans-serif font. The letters are closely spaced and aligned on a single baseline.]

16



LTTng Analyses

* Offline analysis based on LTTng traces,
* Analyze CPU, memory, I/O, interrupts, scheduling, system calls,
* Distribution, top, log over threshold:
    - I/O latency,
    - IRQ handler duration, SoftIRQ raise latency, handler duration,
    - Thread wakeup latency (sched_waking to sched_switch in),
    - User-defined periods based on kernel and user-space events.
* Integrated with Trace Compass graphical user interface.

{"image": "A horizontal logo located at the bottom left. The word 'Effici' is written in a black, stylized, cursive-like serif font, followed by 'OS' in a bold, red, sans-serif uppercase font."}

17



Trace Compass

* Graphical user interface,
* Useful for correlating trace analysis results with detailed graphical representation,
* Implements its own analyses,
* Implements LAMI JSON interface to interact with external analysis scripts.

EfficiOS
18



Trace Compass

[Toolbar: File, Help]
[Navigation: kernel, Control Flow]
[Tool Icons: Search, Filter, Zoom, View Options, Settings]

| Process | TID | 16:23:08.2040100 | 16:23:08.2040200 | 16:23:08.2040300 | 16:23:08.2040400 | 16:23:08.2040500 |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| kernel | | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] |
| systemd | 1 | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] |
| gnome-terminal- | 2958 | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] |
| bash | 32649 | [Green Bar/Blue Gaps] | [Green Bar/Blue Gaps] | [Green Bar/Pink Line] | [Green Bar/Blue Gaps] | [Green Bar/Red End] |
| gdm3 | 1127 | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar/Red End] |
| gdm-session-wor | 2647 | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] |
| gnome-session | 2662 | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] |
| gnome-shell | 2822 | [Green Bar] | [Green Bar] | [Green Bar] | [Orange Bar] | [Green Bar] |
| firefox-esr | 29690 | [Green Bar] | [Blue: re write] | [Green Bar] | [Blue: fsr f f] | [Green Bar/Blue: f] |
| Timer | 29711 | [Blue Bar] | [Blue Bar] | [Blue Bar] | [Green Bar/Blue Gaps] | [Green Bar/Blue] |
| Compositor | 29717 | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] |
| Xorg | 1158 | [Green Bar] | [Blue: select] | [Blue: ioctl sele] | [Green Bar] | [Green Bar] |
| swapper/2 | 0 | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] | [Green Bar] |
| kthread | 2 | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] |
| kworker/u16:2 | 31802 | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] | [Yellow Bar] |

[Toolbar: Statistics, Resources]
[Date: 2016 Aug 21]
[Timeline Scale: 16:23:08.2040100 to 16:23:08.2040500]

{"cpu_usage": [{"cpu": "CPU 0", "events": ["bash", "gnome-shell", "Compositor", "gnome-shell", "bash"]}, {"cpu": "CPU 1", "events": ["firefox-esr", "writev", "select", "Xorg", "ioctl", "fsr", "fu", "fu", "Compositor", "f", "f", "gnome-shell", "f"]}, {"cpu": "CPU 2", "events": ["gnome-terminal-", "kw", "gnome-terminal-"]}, {"cpu": "CPU 3", "events": []}, {"irq": "IRQ 1", "events": []}, {"irq": "IRQ 12", "events": []}, {"irq": "IRQ 45", "events": []}, {"irq": "IRQ 48", "events": []}]}

[Footer: Histogram, Properties, Bookmarks, System Call Density]
[Histogram Graph: X-axis time 16:23:08.204013051 to 16:23:08.204673556. Y-axis 0 to 3. Features several thin blue vertical spikes of varying height.]
[Selection Box: Selection Start 16:23:08.204 434 007 | Selection End 16:23:08.204 434 007]

T: 2016-08-21 16:23:08.2040434007



Interrupt handlers duration frequency distributions

```jsonl
{"type": "bar_chart", "title": "Interrupt handlers duration frequency distributions", "x_axis": "Duration (upper bound) (ns)", "y_axis": "Value (interrupts)", "scale": "logarithmic", "data": [{"label": "i8042 (1)", "values": {"3145": 400, "6199": 150, "9253": 30, "12308": 1, "15362": 1}}, {"label": "i8042 (12)", "values": {"3145": 320, "6199": 280, "9253": 20, "12308": 3, "15362": 5}}, {"label": "0000:00:1f.2 (45)", "values": {"3145": 15, "6199": 120, "9253": 600, "12308": 30, "15362": 120, "18416": 110, "21471": 60, "24525": 25, "27579": 18, "30634": 5, "33688": 2}}, {"label": "i815 (48)", "values": {"3145": 2, "6199": 10, "9253": 20, "12308": 40, "15362": 30, "18416": 10, "21471": 2, "24525": 3, "27579": 0, "30634": 0, "33688": 3}}, {"label": "eth4 (49)", "values": {"3145": 0, "6199": 0, "9253": 0, "12308": 0, "15362": 1, "18416": 0, "21471": 0, "24525": 0, "27579": 0, "30634": 0, "33688": 0, "36742": 3, "39796": 4, "42851": 10}}, {"label": "iwlwifi (50)", "values": {"3145": 0, "6199": 0, "9253": 0, "12308": 0, "15362": 0, "18416": 0, "21471": 0, "24525": 0, "27579": 0, "30634": 0, "33688": 0, "36742": 0, "39796": 0, "42851": 0, "45905": 2}}]}
```



Babeltrace

• Common Trace Format (CTF) trace reader/converter,
• Performs time-based trace correlation/merge,
• Expose APIs (C, C++, Python) for reading CTF traces,
• Pretty-print traces into text log.

{"logo": "EffiCiOS", "position": "bottom-left", "style": "Serif font, 'Effici' in black, 'OS' in red"}
{"page_number": 21, "position": "bottom-right"}



Periodic Use-Case Demo

* Jack
    - Infrastructure for communication between audio applications and with audio hardware
    - http://www.jackaudio.org
    - Scheduling latency caused by unsuitable priorities.

[Image: A presentation slide. The top features a thick, solid red horizontal header bar containing the text "Periodic Use-Case Demo" in a white, serif font. The main body of the slide is white with a black bulleted list. The bottom left corner contains the logo "EfficiOS", where "Effici" is in a black, stylized serif font and "OS" is in a bold, red sans-serif font. The bottom right corner contains the page number "22" in a small black sans-serif font.]



Aperiodic Use-Cases Demos

* Memcached
    - Distributed in-memory object caching system
    - http://memcached.org
    - Response-time to start handling client query
        - Interrupt servicing latency caused by long driver interrupt handler
    - Response-time to complete client query handling
        - I/O latency caused by logging

{"logo": "EfficiOS", "position": "bottom-left", "style": "The word 'Effici' is in a black, stylized serif font, followed by 'OS' in a bold, red sans-serif font."}

23



Benchmarks

• Latency tracker online critical path
    – Memcached, through gigabit interface,
    – 10k requests,
    {"metric": "Baseline", "value": "491 ms"}
    {"metric": "With tracker", "value": "520 ms"}
    {"metric": "Overhead", "value": "5.9 %"}

[Image: The bottom left corner contains the logo "EfficiOS". The "Effici" is written in a stylized, black, serif font with a slight italic lean. The "OS" is in a bold, sans-serif, bright red font. The bottom right corner contains the page number "24" in a simple black sans-serif font.]



Latency Tracker Critical Path Bechmarks

```jsonl
{"Test": "CPU", "Baseline": "19.20s", "Tracker": "19.20s", "Overhead": "0.00%"}
{"Test": "Memory", "Baseline": "32.33s", "Tracker": "32.37s", "Overhead": "0.30%"}
{"Test": "File Read/Write", "Baseline": "9.04 s", "Tracker": "9.50 s", "Overhead": "5.10%"}
{"Test": "Network 1Gbps", "Baseline": "942Mbps/s", "Tracker": "942Mbps/s", "Overhead": "0.00%"}
{"Test": "Network 10Gbps", "Baseline": "8.02Gbps/s", "Tracker": "7.70Gbps/s", "Overhead": "3.89%"}
{"Test": "OLTP (MySQL)", "Baseline": "2.27s", "Tracker": "2.38s", "Overhead": "4.84%"}
```

EfficiOS

25



Latency Tracker Critical Path Benchmarks

{"Metric": "Ratio of requests", "Transition": "0.6%", "No transition": "99.4 %"}
{"Metric": "Average latency", "Transition": "1136.93 ns", "No transition": "259.13 ns"}
{"Metric": "Standard deviation", "Transition": "278.71 ns", "No transition": "28.42 ns"}
{"Metric": "Minimum latency", "Transition": "565 ns", "No transition": "237 ns"}
{"Metric": "Maximum latency", "Transition": "3028 ns", "No transition": "1938 ns"}
{"Metric": "Average instruction count", "Transition": "2024", "No transition": "756"}
{"Metric": "Average L1 misses", "Transition": "38.78", "No transition": "3.04"}
{"Metric": "Average LLC misses", "Transition": "3.66", "No transition": "0.003"}
{"Metric": "Average TLB misses", "Transition": "0.12", "No transition": "0.002"}
{"Metric": "Average branch misses", "Transition": "3.08", "No transition": "0.15"}

[Image: A logo in the bottom left corner. It consists of the word "EfficiOS". "Effici" is written in a black, serif, italicized font. "OS" is written in a bold, red, sans-serif font.]

26



Future Work

• Expose API to lock-free memory allocator, hash table, and latency tracker for use in eBPF scripts. Would provide:
    – NMI-safe lock-free memory allocator vs per-freelist spin lock with interrupts off,
    – NMI-safe lock-free hash table vs per-bucket locking with interrupts off,
    – Would allow hooking eBPF scripts to perf NMIs triggered on performance counter overflows.
• Re-implement latency tracker online critical path module state-machine as eBPF high-level code (bcc).

[Image Description: A logo located at the bottom left. It consists of the word "EfficiOS". The "Effici" part is in a black, serif, italicized font. The "OS" part is in a bold, red, sans-serif font. The "O" in OS is slightly larger than the "S".]

27



Links

LTTng:
http://lttng.org

Latency tracker:
https://github.com/efficios/latency-tracker

LTTng analyses scripts:
https://github.com/lttng/lttng-analyses

TraceCompass:
http://tracecompass.org/

Babeltrace
http://diamon.org/babeltrace

Common Trace Format
http://diamon.org/ctf

{"logo": "EffiCiOS logo in bottom left corner. 'Effici' is in a black serif font with the 'E' being larger and stylized, 'OS' is in a bold red sans-serif font."}

28



Questions ?

{"logo": "EfficiOS", "url": "www.efficios.com"}

{"image": "A stylized cartoon illustration of a mole. The mole is colored in shades of purple and grey with a rounded, plump body. It is facing right and wearing thick, black-rimmed circular glasses. It has small, pinkish-white claws on its front paws and a small pink nose. The mole is depicted from the side, slightly angled towards the viewer."}

{"link": "lttng.org"}
{"email": "lttng-dev@lists.lttng.org"}
{"twitter": "@lttng_project"}

EfficiOS

29
