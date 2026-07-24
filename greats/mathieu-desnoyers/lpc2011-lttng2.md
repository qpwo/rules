Linux Plumbers Conference 2011

LTTng 2.0 : Application, Library and Kernel tracing within your Linux distribution.

E-mail:
mathieu.desnoyers@efficios.com

{"entity": "footer", "content": {"logo": "EfficiOS", "presenter": "Mathieu Desnoyers", "date": "September 9th, 2011", "page_number": 1}}



> Presenter

• Mathieu Desnoyers
• EfficiOS Inc.
    • http://www.efficios.com
• Author/Maintainer of
    • LTTng, LTTng-UST, Babeltrace, LTTV, Userspace RCU

{"footer": {"logo": "EfficiOS", "presenter": "Mathieu Desnoyers", "date": "September 9th, 2011", "page_number": 2}}



> LTTng 2.0 Toolchain Overview

• LTTng 2.0 kernel tracer
• LTTng-UST 2.0 user-space tracer
• LTTng tracing session daemon
• LTTng consumers
• “lttng” CLI / liblttngctl
• Babeltrace
• LTTng-top
• Common Trace Format (CTF)

EffiCOS    Mathieu Desnoyers    September 9th, 2011    3



> LTTng 2.0 Kernel Tracer

• Build against a vanilla or distribution kernel, without need for additional patches,

• Tracepoints, Function tracer, Perf CPU Performance Monitoring Unit (PMU) counters, kprobes, and kretprobes support,

• Supports multiple tracing sessions, flight recorder mode, snapshots, ...

{"footer": {"left": "EfficiOS", "center": "Mathieu Desnoyers", "right": "September 9th, 2011", "page_number": 4}}



> LTTng 2.0 Kernel Tracer

• ABI based on ioctl() returning anonymous file descriptors
– implemented a top-level DebugFS “lttng” file.

• Lib Ring Buffer, initially developed generically for mainline Linux kernel (as a cleanup of the LTTng 0.x ring buffer) has been merged into LTTng 2.0.

• Exports trace data through the Common Trace Format (CTF).

[Image: A footer containing the logo "EfficiOS" in a serif font where "Effici" is black and "OS" is red. To the right of the logo is the text "Mathieu Desnoyers", followed by the date "September 9th, 2011", and the page number "5" at the far right edge.]



> LTTng 2.0 Kernel Tracer

• Supports dynamically selectable “context” information to augment event payload
    – Any Perf PMU counter
    – PID, PPID, TID, executable name (comm), VPID, VTID, ...
    – Dynamic Priority, nice value

{"footer": {"left": "EfficiOS", "center": "Mathieu Desnoyers", "right": "September 9th, 2011", "page_number": 6}}



> LTTng-UST 2.0
User-space Tracer

• TRACEPOINT_EVENT() API for application/library static instrumentation.
• libust linked with applications, listening for LTTng session daemon commands.
• Supports per-user and system-wide tracing.
• “tracing” group: no need to be root to perform system-wide tracing.

EfficiOS       Mathieu Desnoyers       September 9th, 2011       7



> TRACEPOINT_EVENT

In header:

TRACEPOINT_EVENT(ust_tests_hello_tptest,
    TP_PROTO(int anint, long *values,
        char *text, size_t textlen,
        double doublearg, float floatarg),
    TP_ARGS(anint, values, text, textlen,
        doublearg, floatarg),
    TP_FIELDS(
        ctf_integer(int, intfield, anint)
        ctf_integer_hex(int, intfield2, anint)
        ctf_array(long, arrfield1, values, 3)
        ctf_sequence(char, seqfield1, text,
            size_t, textlen)
        ctf_string(stringfield, text)
        ctf_float(float, floatfield, floatarg)
        ctf_float(double, doublefield, doublearg)
    )
)

[Image Description: A presentation slide with a dark blue header section and a white body. In the blue header, the text "> TRACEPOINT_EVENT" is written in large, yellow, sans-serif capital letters. In the white body, there is a code snippet in a monospaced font illustrating a C-style macro definition for TRACEPOINT_EVENT. A black arrow points from the text "Tracepoint name convention" (located on the right side of the slide) to the first argument of the macro, "ust_tests_hello_tptest". At the bottom left, there is a logo "EfficiOS" where "Effici" is black and "OS" is red. In the center bottom, the name "Mathieu Desnoyers" is written. In the bottom right, the date "September 9th, 2011" is written, followed by the slide number "8".]



> User-level Tracepoint

Name convention

< [com_company_]project_[component_]event >

Where "company" is the name of the company,
"project" is the name of the project,
"component" is the name of the project component (which may include several levels of sub-components, e.g.
...component_subcomponent_...) where the tracepoint is located (optional),
"event" is the name of the tracepoint event.

Tracepoint invocation within the code:

void fct(void)
{
    tracepoint(ust_tests_hello_tptest, i, values,
               text, strlen(text), dbl, flt);
}

[Image: A slide footer. On the left, a logo for "EfficiOS" where "Effici" is in black sans-serif font and "OS" is in bold red sans-serif font. In the center, the text "Mathieu Desnoyers". On the right, the text "September 9th, 2011". In the far bottom right corner, the page number "9".]



> Extended Tracepoint Declaration

• API planned, feature not implemented yet.
• TRACEPOINT_LOGLEVEL_ENUM()
    – Loglevels defined by the application implementor.
• Following a TRACEPOINT_EVENT():
    – TRACEPOINT_LOGLEVEL(name, level)
        • Optional selection of tracepoint activation on a per-loglevel basis.
    – TRACEPOINT_FORMAT(name, “format”)
        • Optional pretty-printing.

{"footer": {"left": "EfficiOS", "center": "Mathieu Desnoyers", "right": "September 9th, 2011", "page_number": 10}}



> tracepoint_printf()

• Feature planned
• tracepoint_printf(name, “fmt”, ...);
• Augment Common Trace Format to store format strings
• Export only binary data through buffers.
• Pretty-printing performed at post-processing.

{"footer": {"left": "EfficiOS", "center": "Mathieu Desnoyers", "right": "September 9th, 2011", "page": 11}}



> LTTng-UST 2.0 Buffering

• Port of the lib ring buffer to user-space.
• Supports buffering between processes through POSIX shared memory maps.
• Wake-up though pipes.
• Buffers per process (for security), shared with consumer. Faster/lower memory consumption insecure global buffers feature planned too.
• Takes care of security concerns involved with sharing data structures between processes.

{"footer": {"logo": "EfficiOS", "author": "Mathieu Desnoyers", "date": "September 9th, 2011", "page": 12}}



> LTTng Tracing Session Daemon

• Both centralized (system-wide) and per-user.
• Controls
    – LTTng kernel tracer (domain)
    – LTTng-UST application/library tracer (domain)
    – Right management by UNIX socket file access rights (tracing group).
    – File descriptor credentials passed through UNIX sockets
• Presents a unified notion of system-wide tracing session, with multiple “domains”.

[Image: A logo consisting of the word "Effici" in a black serif font, followed by "OS" in a bold, red sans-serif font.]

Mathieu Desnoyers                                September 9th, 2011                                13



> LTTng Consumers

• Spawned by the tracing sessions daemon
• Design guide-lines:
    – Minimal access, aiming at a design where sessiond opens all files, consumers just copy data between memory maps and file descriptors (received though UNIX socket credentials).
• Disk output (splice, mmap).
• In-place mmap buffer consumption (lttngtop).
• Planned network transport.

[Image: A presentation slide. The top contains a dark blue horizontal header bar spanning the width of the page. Inside the bar, left-aligned, is the text "> LTTng Consumers" in a gold/yellow sans-serif font. The main body of the slide has a white background with black sans-serif text. The content is a bulleted list. The first level bullets are large black dots; the second level bullet is a small horizontal dash. At the bottom left, there is a logo that reads "EfficiOS" where "Effici" is in a black serif font and "OS" is in a bold red sans-serif font. To the right of the logo, centered toward the bottom, is the name "Mathieu Desnoyers". Further to the right is the date "September 9th, 2011". In the bottom right corner is the page number "14".]



> LTTng CLI / liblttngctl

• Unified control interface for kernel and user-space tracing
    – “lttng” git-alike command line interface
    – All tracing control commands available through an API: liblttngctl and lttng.h

{"footer": {"logo": "EfficiOS", "author": "Mathieu Desnoyers", "date": "September 9th, 2011", "page": 15}}



> LTTng UI examples

lttng list -k # list available kernel tracpoints
lttng create mysession # create session “mysession”
lttng enable-event -k -a # enable all available tracepoints
lttng enable-event sched_switch,sys_enter -k
lttng enable-event aname -k --probe symbol+0xffff760695
lttng enable-event aname -k --function <symbol_name>
lttng add-context -k -e sched_switch -t pid # add PID context
lttng add-context -k -e sched_switch -t perf:cpu-cycles
lttng start # start tracing

...

lttng stop # stop tracing
lttng destroy # teardown session

# text output
babeltrace -n $HOME/lttng-traces/mysession-<date>-<time>

[Image: A slide footer. On the far left is the "EfficiOS" logo, where "Effici" is in black serif font and "OS" is in a bold red sans-serif font. In the center is the name "Mathieu Desnoyers". To the right of the name is the date "September 9th, 2011". On the far right is the slide number "16".]



> LTTng 2.0 kernel tracer demo

[Image: A presentation slide. The top section is a thick navy blue horizontal banner containing the text "> LTTng 2.0 kernel tracer demo" in a yellow, sans-serif font. The middle section is a large, empty white space. At the bottom, on a white background, is a footer containing the logo "EfficiOS" on the left (with "Effici" in black serif font and "OS" in red serif font), followed by the name "Mathieu Desnoyers" in the center-left, the date "September 9th, 2011" in the center-right, and the page number "17" on the far right. All footer text is in a black serif font.]

EfficiOS Mathieu Desnoyers September 9th, 2011 17



> Common Trace Format

• Trace format specification
  – Funded by
    • Linux Foundation CE Linux Forum and Ericsson
  – In collaboration with Multi-Core Association Tool Infrastructure Workgroup
    • Freescale, Mentor Graphics, IBM, IMEC, National Instruments, Nokia Siemens Networks, Samsung, Texas Instruments, Tilera, Wind River, University of Houston, Polytechnique Montréal, University of Utah.
  – Gathered feedback from Linux kernel developers and SystemTAP communities.

{"logo": "EfficiOS", "text_color": "black and red"}
Mathieu Desnoyers
September 9th, 2011
18



> Common Trace Format

• Targets system-wide and multi-system trace representation in a common format, for integrated analysis:
    – Software traces
        • Across multiple CPUs
        • Across the software stack (Hypervisor, kernel, library, applications)
    – Hardware traces
        • DSPs, device-specific tracing components.
        • GPUs.

{"footer": {"left": "EfficiOS", "center": "Mathieu Desnoyers", "right_date": "September 9th, 2011", "page_number": 19}}



> Common Trace Format

• Babeltrace
  – Reference implementation trace conversion tool and read/seek API for trace collections.
  – Initially converts
    • From CTF to text
    • From dmesg text log to CTF
• LTTng kernel 2.0 and LTTng-UST 2.0
  – Native CTF producer reference implementation.
• Available at: http://www.efficios.com/ctf

{"footer": {"logo": "EfficiOS", "author": "Mathieu Desnoyers", "date": "September 9th, 2011", "page": 20}}



> Distributions

• Distributions shipping LTTng 0.x
    – Wind River Linux, Montavista, STLinux, Linaro, Yocto, Mentor Embedded Linux, ELinOS, Novell SuSE Enterprise RT Linux.

• Packages
    – Debian and Ubuntu
        • UST, Userspace RCU, LTTV

• Working closely with Ubuntu and Debian to have LTTng 2.0 toolchain ready for the next Ubuntu LTS.

{"logo": "EfficiOS", "presenter": "Mathieu Desnoyers", "date": "September 9th, 2011", "page": 21}



> Distributions

* Fedora
    - Fedora packages available for LTTng 0.x user-space tracing and trace analysis, LTTng 2.0 packages planned,
    - Actively looking for a sponsor.

* RHEL 6
    - Interested in discussing backport of Steven's Tracepoint patches from 2.6.35:
        - “tracing: Let tracepoints have data passed to tracepoint callbacks”

EfficiOS        Mathieu Desnoyers        September 9th, 2011        22



> Questions ?

LTTng 2.0 prereleases available at
http://lttng.org/lttng2.0

{"image": "A large, bold, black question mark symbol positioned on the left side of the slide."}

{"image": "The word 'EfficiOS' in a stylized logo. 'Effici' is written in a black, elegant, cursive serif font with a large decorative 'E'. 'OS' is written in a bold, clean, red sans-serif font."}
- http://www.efficios.com

• LTTng Information
- http://lttng.org
- ltt-dev@lists.casi.polymtl.ca

{"image": "A small version of the EfficiOS logo (black cursive 'Effici' and red sans-serif 'OS') located in the bottom left corner."}

Mathieu Desnoyers
September 9th, 2011
23
