Open Source Summit 2018

Improve Linux User-Space Core Libraries with Restartable Sequences

EfficiOS mathieu.desnoyers@efficios.com ✉️



Speaker

* Mathieu Desnoyers
* CEO at EfficiOS Inc.
* Maintainer of: LTTng kernel and user-space tracers, Userspace RCU library, Linux kernel membarrier and rseq system calls,
* Author of the Restartable Sequence patchset merged into Linux 4.18.

{"logo": "EfficiOS", "text_color": {"Effici": "black", "OS": "red"}}

2



Content

* What are restartable sequences (rseq) ?
* Restartable sequences:
    - Use-cases,
    - Algorithm,
    - Upstreaming status,
* Librseq,
* Glibc rseq thread registration,

{"logo": "EfficiOS", "text_color": "black and red", "position": "bottom-left"}
{"page_number": 3, "position": "bottom-right"}



Content

* Restartable Sequences Shortcomings,
* cpu_opv system call,
* Rseq adoption: user-space projects,
* Benchmarks.

EfficiOS
4



What are Restartable Sequences (rseq) ?

* Sequences of user-space instructions with a preparation stage, finalized by a single commit instruction,
* Either executed atomically with respect to preemption, migration, signal delivery, or aborted before the final commit instruction,
* Kernel guarantees “atomic” execution by moving IP to abort handler if needed,
* Use-cases: super-fast update operations on per-cpu data in user-space.

{"image": "A logo located at the bottom left. It consists of the word 'EfficiOS'. The 'Effici' part is in a black, stylized, slightly italicized serif font. The 'OS' part is in a bold, sans-serif red font."}

5



Restartable Sequences Use-Cases

* LTTng-UST (http://lttng.org)
    - User-space tracing in memory buffers shared across processes
* Userspace RCU (http://liburcu.org)
    - Single-process per-cpu grace period tracking,
    - Multi-process per-cpu grace-period tracking,
* jemalloc and glibc per-cpu memory allocator,
* Application-level per-cpu statistics counters,
* ARM64 PMC read from user-space on big.LITTLE without fault on migration.

[Image: A logo in the bottom left corner. The word "Effici" is written in a stylized, black, italicized serif font. The "OS" following it is in a bold, red, sans-serif font. The "i" in Effici is dotted with a small red square.]

6



# Restartable Sequences Algorithm

[IMAGE: A conceptual technical diagram. At the top, a wide red header bar contains the white text "Restartable Sequences Algorithm". Below this, a horizontal flow is depicted. A large orange rectangle labeled "Restartable Sequence Critical Section" is positioned to the left of a smaller yellow rectangle labeled "Abort Handler". An arrow points from the right edge of the orange rectangle to the yellow rectangle. Below the orange rectangle, there are two code blocks. The first block, on the left, is labeled "Thread-Local Storage __rseq_abi:" and defines a C-style struct 'rseq' containing 'int32_t cpu_id' and a pointer 'struct rseq_cs *rseq_cs'. An arrow points from the 'rseq_cs' pointer to a second code block on the right, which defines 'struct rseq_cs' containing pointers 'void *start_ip', 'void *post_commit_ip', and 'void *abort_ip'. Two thin black lines originate from the 'start_ip' and 'abort_ip' fields of the second struct, pointing upwards via arrowheads to the start and end of the "Restartable Sequence Critical Section" orange box respectively.]

Thread-Local Storage __rseq_abi:
struct rseq {
  int32_t cpu_id;
  struct rseq_cs *rseq_cs;
  [...]
};

struct rseq_cs {
  void *start_ip;
  void *post_commit_ip;
  void *abort_ip;
  [...]
};

EfficiOS
7



# Restartable Sequences Algorithm

* Restartable sequence critical section:
    - Preemption or signal delivery interrupting critical section move instruction pointer to abort handler before returning to user-space,
    - Needs to be implemented in assembly,
    - Ends with a single store instruction.

{"type": "graphic", "description": "Two small horizontal rectangular blocks side-by-side. The left block is wider and filled with a solid orange-to-red gradient. The right block is smaller and filled with a solid pale yellow color."}

EfficiOS 8



Restartable Sequences Upstreaming Status

* Linux 4.18:
    - rseq system call merged,
    - rseq wired up for x86 32/64, powerpc 32/64, arm 32, mips 32/64,
* Linux 4.19:
    - rseq wired up for arm 64, s390 32/64,
* Ongoing work:
    - librseq,
    - glibc rseq registration/unregistration at thread start/exit,
    - new cpu_opv system call.

[IMAGE: A company logo located in the bottom left corner. The word "Effici" is written in a black, stylized, italic serif font. The "OS" is written in a bold, red, sans-serif font. The entire logo is set against a white background.]

9



Librseq

* User-space library,
* Handle restartable sequence thread registration with explicit library API call by each thread,
* Provides headers implementing rseq inline assembly code for common use-cases, e.g. per-cpu compare-and-store and per-cpu add.

EfficiOS 10



Glibc Rseq Thread Registration (Ongoing Work)

* Automatically register rseq at thread start and nptl init, unregister rseq at thread exit (ongoing work),
* Introduce a reference counter field in rseq Thread-Local Storage to allow glibc as well as early-adopter applications and libraries to manage rseq registration ownership.

{"image": "A corporate presentation slide. The top features a thick red horizontal banner containing the white text 'Glibc Rseq Thread Registration (Ongoing Work)' centered. The main body is a white background containing two bullet points of black text. In the bottom left corner, there is a logo that reads 'EfficiOS' where 'Effici' is in a black serif font and 'OS' is in a bold red sans-serif font. In the bottom right corner, the number '11' is printed in black."}



Restartable Sequences Shortcomings

* Interaction with debugger single-stepping:
    - Restartable sequences will loop forever (no progress) if single-stepped by a debugger.
* Unable to migrate data between per-cpu data structures without changing the CPU affinity mask, e.g.:
    - Migration of free memory between per-cpu pools,
    - Migration of tasks by per-cpu user-space task schedulers.
* Handling critical sections in signal handlers nested early/late over thread creation/destruction when rseq is not registered is not straightforward.

{"image": "A logo in the bottom left corner. The word 'Effici' is written in a black, serif, italicized font. It is immediately followed by the letters 'OS' written in a bold, red, sans-serif font."}

12



cpu_opv() System Call (Ongoing Work)

* Vector of operations (similar to iovec) to be executed with preemption disabled, on a given CPU,
* Can be used as fallback when rseq fails,
* Kernel temporarily pins all pages touched by operations,
* Limited to 16 operations. Overall sequence of operations limited to 4216 bytes (cache-cold: 4.7µs preemption off latency on x86-64).
* Implements “compare” eq/ne operations that allow checking whether input data provided by user-space has not been modified concurrently.
* Implements memcpy, add, bitwise, shift, and memory barrier operations.

{"image": "A presentation slide. At the top, there is a thick red horizontal banner containing the white text 'cpu_opv() System Call (Ongoing Work)' aligned to the left. Below the banner is a white background containing a bulleted list of six technical points in black sans-serif text. In the bottom left corner, there is a logo that reads 'EfficiOS' where 'Effici' is in a black serif font and 'OS' is in a bold red serif font. In the bottom right corner, the page number '13' is printed in black."}



Rseq Adoption: User-Space Projects

• Library early adopters (likely for: lttng-ust, liburcu, jemalloc)
    – Provide their own weak __rseq_abi TLS symbol (with refcount field),
    – Lazy registration, pthread_setspecific for unregistration,

• Application early adopters
    – Provide their own weak __rseq_abi TLS symbol (with refcount field), or implement their own library for rseq,
    – Explicit registration/unregistration at thread start and before it exits,

• Integration into glibc
    – Provide strong __rseq_abi TLS symbol (with refcount field),
    – Registration at pthread start and nptl init, unregistration at thread exit,
    – Use by glibc memory allocator.

{"image": "A slide footer. On the bottom left, the text 'EfficiOS' is written in a stylized serif font; 'Effici' is in black and 'OS' is in red, followed by a small red dash. On the bottom right, the number '14' is written in a plain black sans-serif font. The background is white."}



Benchmarks

* Test hardware
    - arm32: ARMv7 Processor rev 4 (v7l) "Cubietruck", 2-core,
    - x86-64: Intel E5-2630 v3@2.40GHz, 16-core, hyperthreading enabled.

[Image: A white background slide. At the top, a thick red horizontal banner spans the width of the page containing the word "Benchmarks" in white serif text, left-aligned. In the bottom left corner, the logo "EfficiOS" is displayed; "Effici" is in a black, stylized cursive/serif font, and "OS" is in a bold, red sans-serif font. In the bottom right corner, the page number "15" is written in black sans-serif text.]



# Benchmarks

* Per-CPU statistic counter increment

```jsonl
{"metric": "Per-CPU statistic counter increment", "arch": "arm32", "getcpu+atomic (ns/op)": 344.0, "rseq (ns/op)": 31.4, "speedup": 11.0}
{"metric": "Per-CPU statistic counter increment", "arch": "x86-64", "getcpu+atomic (ns/op)": 15.3, "rseq (ns/op)": 2.0, "speedup": 7.7}
```

* LTTng-UST: write event 32-bit header, 32-bit payload into tracer per-cpu buffer

```jsonl
{"metric": "LTTng-UST", "arch": "arm32", "getcpu+atomic (ns/op)": 2502.0, "rseq (ns/op)": 2250.0, "speedup": 1.1}
{"metric": "LTTng-UST", "arch": "x86-64", "getcpu+atomic (ns/op)": 117.4, "rseq (ns/op)": 98.0, "speedup": 1.2}
```

* liburcu percpu: lock-unlock pair, dereference, read/compare word

```jsonl
{"metric": "liburcu percpu", "arch": "arm32", "getcpu+atomic (ns/op)": 751.0, "rseq (ns/op)": 128.5, "speedup": 5.8}
{"metric": "liburcu percpu", "arch": "x86-64", "getcpu+atomic (ns/op)": 53.4, "rseq (ns/op)": 28.6, "speedup": 1.9}
```

[Image: A logo in the bottom left corner. The word "Effici" is written in a black, stylized sans-serif font. The "OS" is written in a bold, red sans-serif font. The letters are closely spaced.]

16



Benchmark: Prototype Rseq Integration in jemalloc

* Using rseq with per-cpu memory pools in jemalloc at Facebook (based on rseq 2016 implementation).
* The production workload response-time has 1-2% gain avg. latency, and the P99 overall latency drops by 2-3%.

[Image: A logo in the bottom left corner. It consists of the word "EfficiOS". The "Effici" part is in a black, stylized serif font where the 'E' is oversized and flourishes. The "OS" part is in a bold, red, sans-serif font.]

17



Benchmark: Reading the Current CPU Number

ARMv7 Processor rev 4 (v71)
Machine model: Cubietruck

```jsonl
{"metric": "Baseline (empty loop)", "value": "8.4 ns"}
{"metric": "Read CPU from rseq cpu_id", "value": "16.7 ns"}
{"metric": "Read CPU from rseq cpu_id (lazy registration)", "value": "19.8 ns"}
{"metric": "glibc 2.19-0ubuntu6.6 getcpu", "value": "301.8 ns"}
{"metric": "getcpu system call", "value": "234.9 ns"}
```

x86-64 Intel(R) Xeon(R) CPU E5-2630 v3 @ 2.40GHz:

```jsonl
{"metric": "Baseline (empty loop)", "value": "0.8 ns"}
{"metric": "Read CPU from rseq cpu_id", "value": "0.8 ns"}
{"metric": "Read CPU from rseq cpu_id (lazy registration)", "value": "0.8 ns"}
{"metric": "Read using gs segment selector", "value": "0.8 ns"}
{"metric": "\"lsl\" inline assembly", "value": "13.0 ns"}
{"metric": "glibc 2.19-0ubuntu6 getcpu", "value": "16.6 ns"}
{"metric": "getcpu system call", "value": "53.9 ns"}
```

[Image: A logo for "EfficiOS" located in the bottom left corner. The text "Effici" is in a bold, black, sans-serif font, and "OS" is in a slightly thinner, black, sans-serif font. The "O" in OS is stylized as a circle with a small gap at the top.]

18



Links

* linux-rseq development (volatile):
    - https://git.kernel.org/pub/scm/linux/kernel/git/rseq/linux-rseq.git/
* librseq development:
    - https://github.com/compudj/librseq/
* glibc rseq integration development (volatile):
    - https://github.com/compudj/glibc-dev/
* Additional tests/benchmarks branch for rseq (volatile):
    - https://github.com/compudj/rseq-test

[Image: Bottom left corner. A logo consisting of the word "EfficiOS". The "Effici" part is written in a black, elegant, italicized serif font. The "OS" part is written in a bold, sans-serif, red font.]

19



Related Presentations

• “PerCpu Atomics”, Paul Turner, Andrew Hunter, Linux Plumbers Conference 2013
    – https://blog.linuxplumbersconf.org/2013/ocw/system/presentations/1695/original/LPC%20-%20PerCpu%20Atomics.pdf

• “Enabling Fast Per-CPU User-Space Algorithms with Restartable Sequences”, Mathieu Desnoyers, Linux Plumbers Conference 2016
    – https://linuxplumbersconf.org/2016/ocw/proposals/3873.html

• “Restartable Sequences (2017 Edition)”, Mathieu Desnoyers, Kernel Summit 2017
    – https://lwn.net/Articles/KernelSummit2017/

[Image: A logo in the bottom left corner. The word "Effici" is written in a black, serif, italicized font. The "OS" is written in a bold, red, sans-serif font. The "i" in Effici is slightly stylized with a curved stem.]

20



Related Articles

• Restartable sequences
    – https://lwn.net/Articles/650333/
• Restartable sequences restarted
    – https://lwn.net/Articles/697979/
• Restartable sequences and ops vectors
    – https://lwn.net/Articles/737662/

EfficiOS 21



The End

Questions ?

EfficiOS

22
