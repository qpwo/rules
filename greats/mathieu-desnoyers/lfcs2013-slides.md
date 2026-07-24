Collaboration Summit 2013

[IMAGE: A digital illustration of a stylized, purple-grey mole facing right. The mole has a rounded, plump body with a slightly lighter purple underbelly. It wears round, black-rimmed eyeglasses over its small eyes. Its snout is pinkish with small whiskers. It has several pale pink, clawed digging paws extending from its underside. The art style is a clean vector illustration with bold black outlines and simple cell shading.]

LTTng-UST: Efficient System-Wide User-Space Tracing

EfficiOS   christian.babeux@efficios.com [Email Icon]
@c_bab [Twitter Icon]



whoami

Christian Babeux, Software Developer, EfficiOS,

Background in embedded and ASIC tools,

Active contributor to the LTTng projects:
• lttng-tools & lttng-ust,
• CI infra, Website, Twitter.

AUR package maintainer for Arch Linux.

2



Content

* Overview of LTTng 2.x and UST,
* User-space instrumentation sources,
* Trace format standardisation efforts,
* Tales from a user-space tracer,
* Recent features & future work.

3



Overview of LTTng 2.x

{"image": "A red minimalist vector icon consisting of two overlapping document shapes. The rear document is a solid red rectangle with a small white horizontal cutout at the top center. The front document is a red outline of a page with the top-right corner folded down (dog-eared), creating a white interior space."}

4



Overview of LTTng 2.x

{"checkbox": "Unified user interface, API, kernel and user space tracers", "status": "checked"}

Trace output in a unified format,

Low overhead,

Shipped in distros: Ubuntu, Debian, Suse, Fedora, Linaro, Wind River, etc.

5



Project overview

[
  {"element": "box", "content": {"image": "A stylized illustration of a mole. The mole is dark grey/purple with a rounded body and small, powerful pink claws protruding from its front paws. It is wearing round, black-rimmed glasses over its eyes. The mole is facing right.", "text": "Tracers"}}
]

[
  {"element": "box", "content": {"image": "A simple red icon of a crescent wrench, tilted diagonally.", "text": "Utilities"}}
]

[
  {"element": "box", "content": {"image": "A simple red icon of a bar chart consisting of four vertical bars of increasing height, enclosed within a rounded square border.", "text": "Viewers"}}
]

6



Tracers

{"diagram": "A conceptual layout consisting of a large red-bordered rectangle containing two smaller white boxes. In the center of the large rectangle is the text 'Tracers'. To the left is a white box containing the text 'lttng-modules'. To the right is a white box containing the text 'lttng-ust'."}

• lttng-modules: kernel tracer module, compatible with kernels from 2.6.38* to 3.9,
• lttng-ust: user-space tracer, in-process library.

* Kernel tracing is now possible on 2.6.32 to 2.6.37 by backport of 3 Linux Kernel patches [1].

7



Utilities

[IMAGE: A red-outlined rectangular box with rounded corners. Inside the box, the word "Utilities" is centered at the top. Below that, a smaller red-outlined rectangular box with rounded corners contains the text "lttng-tools". To the left of the large box is a red icon of a wrench tilted at a 45-degree angle.]

• lttng-tools: cli utilities and daemons for trace control,
    – lttng: cli utility for tracing control,
    – lttng-sessiond: tracing registry daemon,
    – lttng-consumerd: consume trace data,
    – lttng-relayd: network streaming daemon.

8



Viewers

{"image": "A red rounded-rectangle container labeled 'Viewers' at the top center. Inside the container are three smaller white rounded-rectangles arranged horizontally. The first contains the text 'babeltrace', the second contains 'lttngtop', and the third contains 'Eclipse lttng plugin'."}

{"icon": "A red rounded-square icon containing a white stylized bar chart with four vertical bars of increasing height."}

• babeltrace: cli text viewer, trace converter, plugin system,
• lttngtop: ncurse top-like viewer,
• Eclipse lttng plugin: front-end for lttng, collect, visualize and analyze traces, highly extensible.

9



Overview of LTTng-UST

{"image_description": "A minimalist graphic centered on a white background. It consists of a solid red silhouette of a person's head and shoulders (a profile icon). The head is a perfect circle and the shoulders are a rounded, semi-oval shape. At the very top of the page, there is a thick, solid red horizontal bar spanning the entire width."}

10



LTTng-UST – Features

Pure userspace implementation,
– Shared memory map between apps and trace consumers,
– Portable to other OS: BSDs, Cygwin (experimental).

Optimized for low-overhead, high-throughput [2],
– Generic kernel ringbuffer ported to userspace,
– Efficient concurrent data structures for trace control.

{"image_description": "A slide with a red header bar containing the white text 'LTTng-UST – Features'. The body is white. There are two main bullet points. The first is preceded by a red silhouette icon of a person's head and shoulders. The second is preceded by a red silhouette icon of a stylized gear or circular mechanical component. The number '11' is in the bottom right corner."}

11



LTTng-UST – Features (cont.)

{"icon": "two interlocking red gears", "text": "Dynamically enabled, statically defined instrumentation,"}

{"icon": "three red stylized human silhouettes", "text": "Per user tracing and system-wide tracing, \n– Tracing group for system-wide tracing."}

{"icon": "red lightning bolt", "text": "Traces recoverable even after application crash."}

12



LTTng-UST – How does it work?

{"icon": "person_silhouette", "text": "Users instrument their applications with static tracepoints,"}
{"icon": "link_chain", "text": "liblttng-ust, in-process library, dynamically linked with application,"}
{"icon": "numbered_list", "text": "Session setup, etc.,"}
{"icon": "gears", "text": "Run app, collect traces,"}
{"icon": "bar_chart", "text": "Post analysis with viewers."}

13



Tracing session - Setup

{"image": "A technical architectural diagram showing the interaction between three components: 'lttng', 'sessiond', and 'consumerd'. Each component name is enclosed in a red-bordered rectangle. A red double-headed horizontal arrow connects 'lttng' and 'sessiond', with the text 'UNIX Socket' centered beneath the arrow. A thick red curved arrow points from 'sessiond' to 'consumerd', with the word 'Spawn' written above the apex of the curve."}

Session setup $ lttng create
User-space event enabling $ lttng enable-event -u -a
Start tracing $ lttng start

14



Tracing session - A wild app appears

{"image": "A technical architectural diagram with red outlines and black text on a white background. At the top, a large red rounded rectangle contains the text 'Instrumented application'. Attached to the bottom edge of this rectangle is a smaller rounded rectangle labeled 'UST listener thread'. Below the main rectangle, there are three bullet points: 'Listener thread spawned via constructor (GCC extension),', 'App registration,', and 'Send SHM and wait fd.'. At the bottom of the diagram are two rounded rectangles: 'sessiond' on the left and 'consumerd' on the right. A vertical double-headed red arrow connects 'UST listener thread' and 'sessiond', labeled 'UNIX Socket'. A horizontal double-headed red arrow connects 'sessiond' and 'consumerd', labeled 'UNIX Socket'. To the right of 'sessiond' is a dashed red rectangular area containing two concentric red circles, labeled 'SHM'. A vertical green line connects the top right of the 'Instrumented application' box down to the 'consumerd' box, labeled 'Pipe'."}

* Listener thread spawned via constructor (GCC extension),
* App registration,
* Send SHM and wait fd.

15



Tracing session – App. execution & teardown

[IMAGE: A technical architecture diagram. At the top, a large red-outlined rounded rectangle contains the text "Instrumented application". Inside this box, on the left, is a thick red circular arrow pointing clockwise. On the right, there is a small red-outlined table with three rows; the top row says "Tracepoint 1", the middle row contains three dots "...", and the bottom row says "Tracepoint N". A thick red arrow points downwards from this box into a dashed red-outlined rectangle. Inside this dashed rectangle is a large red circle consisting of two concentric rings. To the left of this dashed area is a smaller red-outlined box containing the text "UST listener thread". A double-headed red arrow connects the "UST listener thread" box to a box below it labeled "sessiond". A double-headed red arrow connects the "UST listener thread" box to the dashed red rectangle via a label "SHM". To the right of the dashed rectangle, a green vertical double-headed arrow is labeled "Pipe". Below the dashed rectangle is a red-outlined box labeled "consumerd". A double-headed red arrow connects "sessiond" and "consumerd" with the label "UNIX Socket". A vertical double-headed red arrow connects the "UST listener thread" and "sessiond" with the label "UNIX Socket". From "consumerd", a thick red arrow points down to a red-outlined cylinder representing a database. To the left of the cylinder, a thick red arrow points left toward a red-outlined box containing the text "$viewer".]

• App running,
• Events written to ringbuffer,
• Notification of data availability via pipe,
• App unregistered via destructor.

16



User-space instrumentation sources

{"image": "A large, bold, red magnifying glass icon centered on the page. The icon consists of a thick red circular ring representing the lens and a thick red diagonal line extending from the bottom right of the circle acting as the handle."}

17



# Tracepoints - Declaration

TRACEPOINT_EVENT(
    /* Provider name */
    ust_tests_hello,

    /* Tracepoint name */
    tptest,

    /* Type, variable name */
    TP_ARGS(int, anint,
            long *, values,
            float, floatarg),

    /* Type, field name, expression */
    TP_FIELDS(ctf_integer(int, intfield, anint),
              ctf_array(long, arrfield1, values, 3),
              ctf_float(float, floatfield, floatarg))
)

18



Tracepoints - Invocation

void function(void)
{
    int i = 0;
    long vals[3] = { 0x42, 0xCC, 0xC001CAFE };
    float flt = M_PI;

    [...]
    tracepoint(ust_tests_hello,
                tptest,
                i,
                &vals,
                flt);

    [...]
}

19



SystemTAP SDT Providers

Integration result of Collaboration Summit 2011 discussions,

Compatibility with SystemTAP SDT,
- Users can use SystemTAP with tracepoint() instrumented code.

20



# Uprobes

Kernel patchset merged in 3.5,

LTTng integration:
- Initial lttng-modules patchset proposed [4],
- Need usability improvement
- Interface not exported by kernel

21



Trace format standardisation efforts

22



Trace format standardisation efforts

[
  {
    "image_description": "A three-panel comic strip in the style of xkcd. The drawing consists of simple black line art on a white background. Panel 1: A box containing the text 'SITUATION: THERE ARE 14 COMPETING STANDARDS.' Panel 2: Two stick figures standing and talking. The first figure is a simple circle head with a line body and two legs. The second figure is a similar stick figure but with a small ponytail. A speech bubble from the first figure says '14?! RIDICULOUS! WE NEED TO DEVELOP ONE UNIVERSAL STANDARD THAT COVERS EVERYONE'S USE CASES.' A smaller speech bubble from the second figure says 'YEAH!'. Panel 3: A box. At the top is a smaller boxed header that says 'SOON:'. Below that is the text 'SITUATION: THERE ARE 15 COMPETING STANDARDS.'",
    "text_content": "HOW STANDARDS PROLIFERATE: ((handwriting1))\n(SEE: A/C CHARGERS, CHARACTER ENCODINGS, INSTANT MESSAGING, ETC) ((handwriting1))\n\nSITUATION:\nTHERE ARE\n14 COMPETING\nSTANDARDS. ((handwriting1))\n\n14?! RIDICULOUS!\nWE NEED TO DEVELOP\nONE UNIVERSAL STANDARD\nTHAT COVERS EVERYONE'S\nUSE CASES. ((handwriting1))\n\nYEAH! ((handwriting2))\n\nSOON: ((handwriting1))\n\nSITUATION:\nTHERE ARE\n15 COMPETING\nSTANDARDS. ((handwriting1))"
  }
]

Source: xkcd.com/927

23



Trace format standardisation efforts

Joking aside: We need a common open format,

Collaboration: Multicore Association, Ericsson,

Goals of the Common Trace Format (CTF):
- Common format for SW and HW traces,
- Portable,
- Compact,

Tools based on CTF:
- LTTng 2.x, Babeltrace, Eclipse LTTng plugin
- GDB (save trace to CTF) [3],
- Javeltrace

24



Common Trace Format

{"image": "A technical diagram consisting of three rounded red rectangular containers arranged horizontally. The left container is labeled 'Metadata' and contains a block of code-like text: 'typealias ... ;', 'trace { ... };', 'clock { ... };', 'stream { ... };', and 'event { ... };'. The middle container is labeled 'Stream 0' and contains a vertical stack of nested rounded red rectangles representing packets. The top packet is labeled 'Packet 0' and contains sub-sections labeled 'Header', 'Context', 'Event 0', and an ellipsis '...'. Below this is another identical packet structure, followed by an ellipsis, and ending with a packet labeled 'Packet m'. The right container is labeled 'Stream n' and mirrors the exact structure of 'Stream 0'. Between the 'Stream 0' and 'Stream n' containers are three horizontal dots indicating a sequence. To the left of the bottom text is a small red icon of two overlapping documents."}

Self-described, packet-based format.

25



Common Trace Format – More info.

{"description": "A red speech bubble icon facing right."} “Interoperability Between Tracing Tools with the Common Trace Format”,
– Mathieu Desnoyers at Linux Plumbers 2012 [5]

{"description": "A red document icon with a folded top-right corner and three horizontal lines representing text."} Common Trace Format (CTF) Specification [6],

{"description": "A red rounded square icon containing the word 'git' in white lowercase sans-serif font."} Common Trace Format compliance testsuite [7].

26



Tales from a user-space tracer

{"image": "A stylized, minimalist icon of a closed book tilted slightly to the right. The icon is solid red with white negative space forming the page edges and a horizontal bookmark or page divider. The book has rounded corners and a thick red cover outline."}

27



# Non-intrusive handling of SIGPIPE

[Image: A red circular arrow icon representing a refresh or cycle symbol.] Ringbuffer delivery notification use a pipe,
- Traced applications can receive SIGPIPE if consumer end dies abruptly.

[Image: A red circle containing a white X symbol.] Suppress SIGPIPE only in our lib without affecting signal handling for the rest of the process [8].

28



TLS & constructors

• Thread Local Storage (TLS) variable storage in dynamically libs. allocated when first used [9],

• Rely on internal glibc mutex to protect against dynamic linker,

• Same mutex is held while running ctor/dtor,

29



TLS & constructors (cont.)

[Image: A technical diagram illustrating a concurrency or locking issue during thread spawning. On the left side, under the label "liblttng-ust constructor", there are two rounded rectangles. The top rectangle contains the text "dl_load_lock (ok)" and has a red unlocked padlock icon to its left. Below it is a red jagged explosion-like burst symbol. Below the burst is another rounded rectangle containing "ust_lock (fail)". A red arrow starts from the explosion burst, labeled "Spawn", and points to the right into a large red dashed-line bounding box labeled "UST listener thread". Inside this dashed box, there are two more rounded rectangles. The top one contains "ust_lock (ok)" and has a red unlocked padlock icon to its right. Below this is the text "TLS lookup", and below that is a rounded rectangle containing "dl_load_lock (fail)".]

30



TLS & constructors (cont.)

• Take mutex within constructors while TLS fixup performed,

{"image": "A red triangle with a white exclamation mark inside, serving as a warning icon."} Deadlock!

• Workaround: Force TLS fixup within lib ctor.

31



Tracing of apps closing all fds

32



Close all the things

• When daemonizing, some apps close all available fds,

{"text": "CLOSE"}

[Image: A digital meme illustration. A stylized, wide-eyed character with a huge open mouth and a frantic expression is screaming. The character has a simple yellow head, a pink shirt, and thin black limbs. They are holding a yellow broomstick in their right hand, thrusting it upward. The character is centered against a bright yellow, jagged, starburst-style background. Large, bold, black-outlined yellow text at the bottom reads "ALL THE FDS". In the bottom left corner, there is a small watermark that reads "imgflip.com".]

33



Tracing of apps closing all fds

• When daemonizing, some apps close all available fds,

[{"type": "icon", "description": "Red triangle warning sign with an exclamation mark in the center"}] No communication == No tracing.

• Fix: None for the moment.

34



Recent features & future work

[IMAGE: A red stylized icon depicting a road receding into a horizon. The image consists of a red trapezoid representing the road surface, with a vertical white rectangular strip running up the center to represent a lane divider. The white strip is broken into three distinct segments, creating the appearance of a dashed line. The entire graphic is centered on a white background.]

35



Recent features

{"checkbox": "2.1 (Basse Messe)", "status": "checked"}

{"icon": "globe", "text": "Network streaming over TCP,"}
• Introduce lttng-relayd, receive traces from remote consumers.

{"icon": "filter", "text": "Filtering before data collection,"}
• C-like syntax, bytecode interpreter.
• UST only for the moment.

{"icon": "medical_kit", "text": "Session daemon health monitoring API."}

36



# Network streaming over TCP

[Image: A high-resolution screenshot of a Linux desktop environment showing three separate terminal windows layered over a light gray background.

Window 1 (Top Left): A terminal window with the title "test4@thinkos. ~". The window has a standard GUI frame with minimize, maximize, and close buttons. The terminal content is a black background with white text. It shows a single command line: `test4@thinkos:~$ lttng-relayd`.

Window 2 (Center Right): A terminal window with the title "compudj@squeeze-i386: ~". It is layered partially over Window 1. The terminal content is a black background with white text. It shows a series of root shell commands and outputs:
`root@squeeze-i386:~# lttng create -U net://thinkos`
`Session auto-20120827-141834 created.`
`Traces will be written in net://thinkos`
`root@squeeze-i386:~# lttng enable-event -k -a`
`All kernel events are enabled in channel channel0`
`root@squeeze-i386:~# lttng start`
`Tracing started for session auto-20120827-141834`
`root@squeeze-i386:~# lttng destroy`
`Session auto-20120827-141834 destroyed`
`root@squeeze-i386:~#` followed by a white block cursor.

Window 3 (Bottom Left): A terminal window with the title "test4@thinkos: ~/lttng-". The terminal content is a black background with white text. It shows a command and the resulting file directory listing:
`test4@thinkos:~/lttng-traces/squeeze-i386$ find .`
`.`
`./auto-20120827-141834`
`./auto-20120827-141834/kernel`
`./auto-20120827-141834/kernel/metadata`
`./auto-20120827-141834/kernel/channel0_1`
`./auto-20120827-141834/kernel/channel0_0`
`test4@thinkos:~/lttng-traces/squeeze-i386$` followed by a white block cursor.]

37



Filtering (1)

[Image: A screenshot of a Linux terminal window titled "test4@thinkos: ~". The window has a dark gray/black background with white monospaced text. The terminal displays a sequence of LTTng commands and outputs. The text reads:
"test4@thinkos:~$ lttng create
Session auto-20120827-142450 created.
Traces will be written in /home/test4/lttng-traces/auto-20120827-142450
test4@thinkos:~$ lttng enable-event -u -a --filter "(intfield > 42 && intfield <= 44 ) || longfield == 1"
All UST events are enabled in channel0
test4@thinkos:~$ lttng start
Tracing started for session auto-20120827-142450
test4@thinkos:~$ lttng destroy
Session auto-20120827-142450 destroyed
test4@thinkos:~$"]

Filter:

“(intfield > 42 && intfield <= 44) || longfield == 1”

38



Filtering (2)

[Image: A screenshot of a computer terminal window. The window has a grey title bar with the text "test4@thinkos: ~/lttng-traces" and standard window control buttons (minimize, maximize, close) on the right. Below the title bar is a menu bar with options: File, Edit, View, Search, Terminal, Help. The main body of the window is a black command-line interface with white and light-grey text. Several lines of log data are displayed, showing timestamps and structured data fields like 'cpu_id', 'intfield', and 'longfield'. Three specific sections of the text are circled in red: the value '0, 1' for longfield in the first block, '{ intfield = 43,' in the second block, and '{ intfield = 44,' in the third block. The terminal prompt is 'test4@thinkos:~/lttng-traces$'.]

test4@thinkos:~/lttng-traces$ babeltrace auto-20120827-142450/
[14:25:09.3948326990] (+?.??????????) thinkos:lt-hello:22206 ust_tests_hello:tp
test: { cpu_id = 0 }, intfield = 1, intfield2 = 0, 1, longfield = 1, netintfield = 1, netintfieldhex = 0x1l, arrfield1 = [ [0] = 1, [1] = 2, [2] = 3 ], arrfield2 = "test", __seqfield1_length = 4, seqfield1 = [ [0] = 116, [1] = 101, [2] = 115, [3] = 116 ], __seqfield2_length = 4, seqfield2 = "test", stringfield = "test", floatfield = 2222, doublefield = 2, boolfield = 1 }
[14:25:09.3948343662] (+0.0000016672) thinkos:lt-hello:22206 ust_tests_hello:tp
test: { cpu_id = 0 } { intfield = 43, intfield2 = 0x2B, longfield = 43, netintfield = 43, netintfieldhex = 0x2b, arrfield1 = [ [0] = 1, [1] = 2, [2] = 3 ], arrfield2 = "test", __seqfield1_length = 4, seqfield1 = [ [0] = 116, [1] = 101, [2] = 115, [3] = 116 ], __seqfield2_length = 4, seqfield2 = "test", stringfield = "test", floatfield = 2222, doublefield = 2, boolfield = 1 }
[14:25:09.3847126] (+0.000000484) thinkos:lt-hello:22206 ust_tests_hello:tp
test: { cpu_id = 0 } { intfield = 44, intfield2 = 0x2c, longfield = 44, netintfield = 44, netintfieldhex = 0x2c, arrfield1 = [ [0] = 1, [1] = 2, [2] = 3 ], arrfield2 = "test", __seqfield1_length = 4, seqfield1 = [ [0] = 116, [1] = 101, [2] = 115, [3] = 116 ], __seqfield2_length = 4, seqfield2 = "test", stringfield = "test", floatfield = 2222, doublefield = 2, boolfield = 1 }
test4@thinkos:~/lttng-traces$

“(intfield > 42 && intfield <= 44) || longfield == 1”

39



Recent features (cont.)

✓ 2.2 (Cuda, Currently in RC)
  👤 Per-uid buffers in UST,

  ⏳ Context filtering,
  • '$ctx.procname == “demo*”',
  • '$ctx.vpid > 9000'.

  📌 Trace file size limits,

40



Future work

{"icon": "airplane", "text": "Flight recorder mode tracing (2.3),"}

{"icon": "lightning bolt", "text": "Trace data extracted on core dump (2.3),"}

{"icon": "coffee cup", "text": "Java tracing."}

41



Future work (cont.)

Tracer triggers actions on specific events & filters

Compressed, encrypted streaming and storage,

LTTng accepted in Google Summer of Code [10].
– Dynamic instrumentation support in UST,
– Android port.

42



Conclusion

{"checkbox": "checked", "text": "Usability of user space tracing in production"}

43



Questions ?

EfficiOS
www.efficios.com

?

[
  {
    "image_description": "A stylized cartoon illustration of a grey, rounded animal resembling a mole or a small mammal. The creature is facing right and is slightly hunched. It is wearing round, black-rimmed glasses. Its paws are small and pale pink, clutching the ground. The art style is clean with bold outlines and soft shading on the grey fur."
  }
]

lttng.org
lttng-dev@lists.lttng.org
@lttng_project

44



# References

* [1] – Userspace tracing in small footprint devices – Jason Wessel
* [2] – lttng-modules README -
* [3] – [lttng-dev] [lttng-modules PATCH] Add uprobes support – Yannick Brosseau
* [4] – [PATCH v3 00/15] CTF Support – Yao Qi
* [5] - “Interoperability Between Tracing Tools with the Common Trace Format” - Mathieu Desnoyers , Linux Plumbers 2012
* [6] - Common Trace Format (CTF) Specification
* [7] - Common Trace Format compliance testsuite
* [8] – LTTng-UST – 2C44F5B9 - Fix UST SIGPIPE handling
* [9] – ELF Handling for Thread-Local Storage – Ulreich Drepper (page 8)
* [10] – LTTng GSoC 2013 Ideas list

45
