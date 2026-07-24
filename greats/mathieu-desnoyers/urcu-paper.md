Page 1 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 5 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 7 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 9 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 10 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 6 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 8 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 3 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 2 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 4 exception: Cannot connect to host llms.intrinsicapi.com:443 ssl:default [Operation timed out]
Page 2 done.
Page 1 done.
Page 10 done.
Page 5 done.
Page 4 done.
Page 9 done.
Page 8 done.
Page 3 done.
Page 12 done.
Page 13 done.
Page 11 done.
Page 6 done.
Page 7 done.
Page 14 done.
IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS. VOL. X, NO. Y, MONTH 2009
1

User-Level Implementations of Read-Copy Update

Mathieu Desnoyers, Paul E. McKenney, Alan Stern, Michel R. Dagenais and Jonathan Walpole

Abstract—Read-copy update (RCU) is a synchronization primitive that often is used as a replacement for reader-writer locking, due to the fact that it provides extremely lightweight read-side primitives with sharply bounded execution times. RCU updates are typically much heavier weight than RCU reads, especially when updates are computationally expensive.

Although RCU is readily used in a number of kernel-level environments, these implementations make use of interrupt- and preemption-disabling facilities that are often unavailable to user-level applications. The few user-level applications that are available to user applications either rely on primitives commonly available to user applications either rely on primitives commonly available to user-level applications.

This paper describes several classes of efficient RCU implementations for user-level use, based on primitives commonly available to user-level applications.

Finally, performance comparisons of these RCU primitives with each other and to standard locking leads to diverse appropriate locking solutions for diverse workloads. RCU is most effective outside of kernels.

Index Terms—D.4.1 Synchronization < D.4.1 Process Management < D.4 Operating Systems < D Software/Software Engineering < D.4.1 Threads < D.4.1 Process Management < D.4 Operating Systems < D Software/Software Engineering < D.4.1 Concurrency < D.4.1 Process Management < D.4 Operating Systems < D Software/Software Engineering

I. INTRODUCTION

READ-COPY UPDATE (RCU) is a synchronization mechanism that was added to the Linux kernel in October of 2002, RCU achieves scalability improvements by allowing reads to occur concurrently with updates. In contrast with conventional locking primitives that ensure mutual exclusion among concurrent threads regardless of whether they are readers or writers, reader-writers or reader-writer locks that allow concurrent readers but not in the presence of updates, RCU supports concurrency between a single updater and multiple readers. RCU ensures that readers are coherent by maintaining multiple versions of objects and ensuring that they are not freed up until all pre-existing read-side critical sections complete. RCU defines and uses efficient and scalable mechanisms for publishing and reading new versions of an object, as well as one for deferring reclamation of old versions. These mechanisms distribute the work among read and update paths in such a way as to avoid bottlenecks and costly extremes. In some cases, as will be presented in Section IV-B, RCU-read primitives have zero overhead.

Manuscript received July 19, 2008; revised Month Y, 2009.
M. Desnoyers (mathieu.desnoyers@polytechnique.fr) and M. R. Dagenais (michel.dagenais@polytechnique.fr) are with the Computer and Software Engineering department, Ecole Polytechnique de Montreal.
Paul E. McKenney (paulmck@linux.vnet.ibm.com) is with the IBM Linux Technology Center.
Alan Stern (astern@rowland.harvard.edu) is with the Rowland Institute, Harvard University.
Jonathan Walpole (walpole@cs.pdx.edu) is with the Computer Science Department, Portland State University.

Although mechanisms similar to RCU have been used in a number of operating-system kernels (1, 2, 3, 4, 5), and, as shown in Figure 1, is heavily used in the Linux kernel, we are not aware of significant application usage. this lack of application-level use is in part due to the fact that prior user-level RCU implementations imposed global constraints on the application's structure and operation (6), and in some cases of rcu heavy read-side overhead as well as the cost (7). The popularity of RCU in operating-system kernels has been in part due to the fact that these can accommodate the required global constraints imposed by earlier RCU implementations. Kernels therefore permits the use of high-performance quiescent-state based reclamation (QSBR) class RCU implementations. In fact, in server-class (CONFIG_PREEMPT=0) Linux-kernel builds, RCU incurs zero read-side overhead (8).

```jsonl
{"type": "x-y-plot", "title": "Linux Kernel Usage of RCU", "x_axis": "Year", "x_axis_range": ["2002", "2010"], "y_axis": "nRCU API Uses", "y_axis_range": [0, 3000], "data_points": "A line graph starting at (2002, 0), remaining low until 2004, then rising steadily with some fluctuations, peaking near 2009 at approximately 2500, and ending in 2010."}
```

Fig. 1. Linux—Kernel Usage of RCU

Whereas we cannot put forward a single user-level RCU implementation that is ideal for all user-level environments, the three classes of RCU implementations described in this paper should suffice for most applications.

First, Section II provides a brief overview of RCU, including RCU semantics. Then, Section III describes user-level scenarios that could benefit from RCU. This is followed by the presentation of three classes of RCU implementations in Section IV. Finally, Section V presents experimental results, comparing RCU solutions to each other and to standard locks. This leads to recommendations on locking use for diverse workloads presented in Section VI.

II. BRIEF OVERVIEW OF RCU

This section introduces a conceptual view covering most RCU-based algorithms in Section II-A to familiarise the reader



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009                                                                                                                                                  2

[
  {"element": "diagram", "type": "x-y_plot", "description": "Timeline diagram of RCU grace periods. X-axis is 'Time'. Y-axis tracks 5 threads: Reader 1, Reader 2, Reader 3, Reader 4, and Updater. Reader threads have horizontal bars labeled 'reads'. The Updater thread has three distinct phases: 'removal', 'grace period', and 'reclamation'. Vertical dashed lines mark the start and end of the grace period. The grace period begins after the 'removal' phase and ends when all 'reads' sections that started before the removal have completed. A label below the timeline reads 'rcu_assign_pointer() synchronize_rcu() Grace period waits for completion of Pre-existing reads'."}
]

Fig. 2. Schematic of RCU Grace Period and Read-Side Critical Sections

with RCU concepts and recovery. It then presents an informal RCU desiderata section in II-B, which details the goals pursued in this work. Then, Section II-C shows how RCU is used to delete an element from a linked list in the face of concurrent readers. Finally, Section II-D gives an overview of RCU semantics, presenting the synchronization guarantees provided by RCU.

A. Conceptual View of RCU Algorithms

A schematic for the high-level structure of an RCU-based algorithm is shown in the figure 2, which can be thought of as a pictorial view of (1) the present specified in Section II-D.1. The grace period concept, explained thoroughly in section II-D.1, can be defined formally for the needs of this section as a period of time such that all RCU read-side critical sections in existence at the beginning of a given grace period have completed before its end.

Here, each box labeled “Reads” is an RCU read-side critical section that begins with rcu_read_lock() and ends with rcu_read_unlock(). Each row of RCU read-side critical sections denotes a separate thread, for a total of four read-side threads. The two boxes at the bottom left and right of the figure denote a fifth thread, this one performing an RCU update.

This update process is split into two phases. The removal phase denoted by the lower left-hand box and a reclamation phase denoted by the lower right-hand box. These two phases must be separated by a grace period, which is determined by the duration of the synchronize_rcu() execution. During the removal phase, the RCU update removes the elements from the data structure (possibly inserting some as well) by issuing an rcu_assign_pointer() or equivalent pointer-replacement primitive. These removed data elements will not be accessible to RCU read-side critical sections starting after the removal phase ends, but must still be accessed by RCU read-side critical sections initiated during the removal phase. However, by the end of the RCU grace period, all of the RCU read-side critical sections that might be accessing the newly removed data elements are guaranteed to have completed, courtesy of the definition of “grace period”. Therefore, the reclamation phase beginning after the grace period ends can safely free the data elements removed previously.

B. User-Space RCU Desiderata

Extensive use of RCU applications has led to the following user-space RCU desiderata:

1) Read-side primitives (such as rcu_read_lock() and rcu_read_unlock()) bounding RCU read-side critical sections and grace-period primitives (such as synchronize_rcu() and call_rcu()) must have the property that any RCU read-side critical section in existence at the start of a grace period completes by the end of the grace period.
2) RCU read-side primitives should avoid expensive operations such as cache misses, atomic instructions, memory barriers, and conditional branches.
3) RCU read-side primitives should have O(1) computational complexity to enable real-time use. this property guarantees predictability from deadlocks.
4) RCU read-side primitives should be usable in all contexts, including nested within other RCU read-side critical sections. Another important special context is library functions having incomplete knowledge of the user application.
5) RCU read-side primitives should be unconditional, thus eliminating the failure checking that would otherwise complicate testing and validation. This property has the nice side-effect of avoiding livelocks.
6) RCU read-side should not cause write-side starvation: grace periods should always complete, given a event given a steady flow of time-bounded read-side critical sections.
7) Any operation other than a quiescent state (and thus a grace period) should be permitted within an RCU read-side critical section. In particular, non-dependent operations such as I/O and lock acquisition/release should be permitted.
8) It is permissible to mutate an RCU-protected data structure while executing within an RCU read-side critical section. Of course, any grace periods following this mutation must occur after the RCU read-side critical section completes.
9) RCU primitives should be independent of memory allocator design and implementation, so that RCU data structures may be protected regardless of how their data elements are allocated and freed.
10) RCU grace periods should not be blocked by threads that halt outside of RCU read-side critical sections. (But note that most preempt-state-based implementations violate this desideratum).

The RCU implementations described in Section IV are designed to meet the above list of desiderata.

C. RCU Deletion from a Linked List

RCU-protected data structures in the Linux kernel include linked lists, hash tables, radix trees, and a number of custom-built data structures. Figure 3 shows how RCU may be used to delete an element from a linked list that is concurrently being traversed by RCU readers, as long as each reader conducts its traversal within the confines of a single RCU read-side critical section. The first column of the figure presents the data structure view of the updater thread. The second column



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009
Page 3

[
  {"figure": "Fig. 3. RCU Linked-List Deletion", "content": {
    "table_structure": {
      "headers": ["Updater", "Reader Initiated Before Start of Grace Period", "Reader Initiated After Start of Grace Period"],
      "rows": [
        {
          "Updater": "Diagram: Node A points to Node B, Node B points to Node C",
          "Before": "Diagram: Node A points to Node B, Node B points to Node C",
          "After": "Diagram: Node A points to Node B, Node B points to Node C"
        },
        {
          "event": "list_del_rcu(B)",
          "Updater": "Diagram: Node A now points to Node C; Node B still points to Node C",
          "Before": "Diagram: Node A points to Node B, Node B points to Node C",
          "After": "Diagram: Node A points to Node B, Node B points to Node C"
        },
        {
          "event": "synchronize_rcu()",
          "Updater": "Diagram: Node A points to Node C; Node B points to Node C",
          "Before": "Diagram: Node A points to Node C",
          "After": "Diagram: Node A points to Node C"
        },
        {
          "event": "free(B)",
          "Updater": "Diagram: Node A points to Node C; Node B is absent (white space)",
          "Before": "Diagram: Node A points to Node C",
          "After": "Diagram: Node A points to Node C"
        }
      ],
      "vertical_labels": {
        "left_margin": "Grace Period"
      }
    }
  }
}

presents the data structure view of a reader thread starting before the grace period begins. The third column presents a reader thread beginning after the beginning of the grace period.

The first row of the figure shows the lists with elements A, B, and C, to each of which every RCU readers initiated before the beginning of the grace period might both acquire and hold references.

The list_del_rcu() primitive unlinks element B from the list, but leaves the link from B to C intact, as shown on the second row of the figure. This permits any RCU readers currently referring to B to advance in C, as shown on the second and third rows of the figure. The transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition between the second and third rows shows the reader that transition



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS. VOL. X, NO. Y, MONTH 2009

Then the body of a canonical RCU read-side critical section would appear as follows:

q = RCU_dereference(g); R0; R1; R2; R3; ...

where this RCU read-side critical section is enclosed in RCU_read_lock() and RCU_read_unlock(), q is a local pointer, g is the same global pointer updated by the earlier rcu_assign_pointer() (and possibly updated again by some later invocations of rcu_assign_pointer()), and each Ri dereferences q to access one of the fields initialized by one of the statements Ii.

Then we have the following, where A is the rcu_assign_pointer() and D is the RCU_dereference():

A => D => ∀i[Ri(Ii) -> Ri] (2)

In other words, if a given rcu_assign_pointer(), then all accesses the value stored by a given rcu_assign_pointer(), then all statements dereferencing the pointer returned by that rcu_dereference() must see the effects of any initialization statements preceding the rcu_assign_pointer()).

This guarantee allows new data to be initialized and added to an RCU-protected data structure in face of concurrent RCU readers.

Given the broad precise and publication guarantees, these five primitives enable a wide variety of algorithms and data structures providing extremely low read-side overheads for read-mostly data structures (8; 6; 9; 10). Again, note that current updates must be handled by some synchronization mechanism, be it locking, atomic operations, non-blocking synchronization, transactional memory, or a single updater thread.

With this background on RCU, we are ready to consider how it might be used in user-space application levels.

III. USER-SPACE RCU USAGE SCENARIOS

The past year has seen increased interest in applying RCU to user-space applications.

User-level RCU was needed for a user-level infrastructure that provides low-overhead tracing for user-mode applications. RCU is used for tracer control data synchronization in the LTTng tracer implementation (11), which is being ported to a user-space library. This usage scenario poses important constraints to the RCU requirements. This tracing infrastructure cannot be too intrusive in terms of program modification, which makes the QBBR approach presented in Section IV-B inappropriate for such usage scenarios. It also needs to support extensive instrumentation of user-set execution paths, including signal handlers, which require therefore short-sequencing nested RCU critical sections and RCU reader critical sections in signal handlers. This usage scenario is also very performance demanding on workloads involving instrumentation of frequent execution site. Therefore, having a low-overhead and scalable read-side is very important. Therefore, an ideal locking primitive for a tracing library would require no knowledge of the application and could be used to protect data structures used in a library.

User-level RCU has also been proposed for an elliptic-network distributed cloud-based storage project (12). BIND, a major domain name server at the root of Internet domain name resolution, is facing multi-threading scalability issues which are commonly addressed with reader-writer locks (13). Given the domain names are read often but rarely updated, these could benefit from major performance improvement by using user-level RCU. Others have mentioned possibilities in financial applications. One can also argue that RCU has seen long use at user level in the guise of user-mode Linux.

In general, the area of applicability of RCU to user-mode applications appears similar to that in the Linux kernel: read-mostly data structures, especially in cases where scale data contamination be a problem.

IV. CLASSES OF RCU IMPLEMENTATIONS

This section describes several classes of RCU implementations, with Sections IV-B, IV-C, and IV-D presenting user-space RCU implementations that are optimized for different user usage by user-space applications, but first Section IV-A describes some primitives which might help the implementation. The best overall performance is described in Section IV-B offers the best possible read-side performance, but requires that each of the application threads periodically pass through a quiescent state, thus strongly constraining the application's design. The implementation presented in Section IV-C places almost no constraints on the application's design, thus being appropriate for use within a general-purpose library, but thereby having greater read-side overhead. Section IV-D presents an implementation providing low read-side overhead, and requiring only that the application give up one slot of RCU processing to a utility. Section IV-E demonstrates how to create wait-free RCU update primitives.

A. Notation

The examples in this section use a number of primitives that may be unfamiliar, and thus are listed in this section.

Per-CPU variables are defined as DEFINE_PER_THREAD, A thread may access its own instance of a per-THREAD variable using get_thread_var(), or some other thread's instance via per_thread(). The for_each_thread() primitive sequencies through all threads, one at a time.

The pthread_mutex is a type defined by the thread library for mutual exclusion variables. The mutex_lock() primitive acquires the m_bed keyword thus, and mutex_unlock() releases it. The m_bed keyword stands for "memory barrier". A smp_mb() primitive presents a full memory barrier, for example, the sync instruction on the PowerPC architecture, the smp_wmb() and smp_rmb() primitives are, respectively, store and load memory barriers, corresponding, for example, to the fence and lfence instructions on the x86 architecture. The ACCESS, ONCE() primitive prohibits any compiler optimization that might otherwise turn a single store or fetch into multiple fetches, as might happen under heavy register pressure. The barrier() primitive prohibits any compiler code-motion optimization that might otherwise move fetches or stores across the barrier() primitive.



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS. VOL. X, NO. Y, MONTH 2009
Page 5

1 long rcu_gp_ctr = 0;
2 DEFINE_PER_THREAD(long, rcu_reader_qs_gp);
3
4 static inline void rcu_read_lock(void)
5 {
6 }
7
8 static inline void rcu_read_unlock(void)
9 {
10 }
11
12 static inline void rcu_quiescent_state(void)
13 {
14   smp_mb();
15   set_thread_var(rcu_reader_qs_gp, =
16   ACCESS_ONCE(rcu_gp_ctr) + 1);
17   smp_nb();
18 }
19
20 static inline void rcu_thread_offline(void)
21 {
22   smp_mb();
23   get_thread_var(rcu_reader_qs_gp, =
24   ACCESS_ONCE(rcu_gp_ctr));
25 }
26
27 static inline void rcu_thread_online(void)
28 {
29   get_thread_var(rcu_reader_qs_gp, =
30   ACCESS_ONCE(rcu_gp_ctr) + 1);
31   smp_nb();
32 }

1 static inline int rcu_gp_ongoing(int thread)
2 {
3   return per_thread(rcu_reader_qs_gp, thread) & 1;
4 }
5
6 void synchronize_rcu(void)
7 {
8   int t;
9
10  smp_mb();
11  mutex_lock(&rcu_gp_lock);
12  rcu_gp_ctr += 2;
13  for_each_thread(t) {
14    while (rcu_gp_ongoing(t) &&
15      ((per_thread(rcu_reader_qs_gp, t)
16        < rcu_gp_ctr) < 0)) {
17        poll(NULL, 0, 10);
18    }
19    barrier();
20  }
21  mutex_unlock(&rcu_gp_lock);
22  smp_mb();
23 }

Fig. 4. RCU Read Side Using Quiescent States
Fig. 5. RCU Update Side Using Quiescent States

B. Quiescent-State-Based Reclamation RCU

The QSBR RCU implementation provides near zero-overhead read-side, but requires to modify the application, as this section explains.

Figure 4 shows, the read-side primitives used to construct a user-level quiescent-state-based reclamation (QSBR) simple-implementation of RCU based on quiescent states. As can be seen from lines 4-10, the primitive rcu_read_lock() and rcu_read_unlock() primitives do nothing, and can in fact be expected to be inlined and eliminated and optimized away, as they are in server builds of the Linux kernel. This is due to the fact that the quiescent-state-based RCU implementations approximate the extents of the RCU read-side critical sections using the aforementioned quiescent states, which are signaled via rcu_quiescent_state(), from lines 12-18 in the figure. No more overhead is incurred extended quiescent states (for example, when blocking), many instead use the thread_offline() and thread_online() APIs to mark the beginning and the end, respectively, of such an extended quiescent state. As such, thread_offline() is analogous to rcu_read_lock() and thread_online() is analogous to rcu_read_unlock(). These two functions are shown on lines 20-32 in the figure. In either case, it is implied that the RCU read-side to appear within an RCU read-side critical section.

In rcu_quiescent_state(), line 14 executes a memory barrier to prevent any code prior to the quiescent state from being reordered into the quiescent state. Lines 15-16 push up a copy of the global rcu_gp_ctr (RCU grace-period counter), using ACCESS_ONCE() to ensure that the compiler does not employ any optimizations that would result in rcu_gp_ctr being fetched more than once, and then adds one to the value fetched and stores it into the per-thread rcu_reader_qs_gp variable, so that any concurrent instance of synchronize_rcu()

will see an odd-numbered value, thus becoming aware that the new RCU read-side critical section has started. Instances of synchronize_rcu() that are waiting on older RCU read-side critical sections will know to ignore this new one. Finally, line 17 executes a memory barrier to ensure that the update to rcu_reader_qs_gp is seen by all threads before they happen to enter subsequent RCU read-side critical sections.

Some applications might use RCU only occasionally, but use it very heavily when they do use it. Such applications might choose to use rcu_thread_online() when they start using RCU and rcu_thread_offline() when they are no longer using RCU. The time between a call to rcu_thread_online() and a subsequent call to rcu_thread_offline() is an extended quiescent state, so that RCU will not expect explicit quiescent states to be registered during this interval.

The rcu_thread_offline() function simply sets the per-thread rcu_reader_qs_gp variable to the current value of rcu_gp_ctr, which has an even-numbered value. Any instance of synchronize_rcu() will thus know to ignore this thread. A memory barrier is needed at the beginning of the function to ensure all RCU read-side effects are globally visible before the thread is marked offline. No memory barrier is needed at the innermost part of rcu_thread_offline() because it is invalid to perform RCU accesses on this side of the function. Therefore there is no need to prevent reordering.

The rcu_thread_online() function is the counterpart of rcu_thread_offline(), it marks the end of the extended quiescent state. It is similar to rcu_quiescent_state(), except that the only memory barrier is required at the end of the function.

Figure 5 shows the implementation of synchronize_rcu(). It implicitly refers to the variables declared in Lines 1–2 of Figure 4. Lines 1–4 shows the rcu_gp_ongoing() helper function, which returns true if the specified thread’s rcu_reader_qs_gp variable has an odd-numbered value. Lines 6–22 show the implementation of synchronize_rcu() itself. Line 10 is a memory barrier that ensures that the caller’s mutation of the RCU-protected data structure is seen by all CPUs to happen before the grace period identified by this invocation of synchronize_rcu(). Line 11 acquires a pthread_mutex



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009 6

named rcu_gp_lock in order to serialize concurrent calls to synchronize_rcu(), and utilize 12 releases it. Line 12 uses the value "2" to the global variable rcu_gp_ctr to indicate the beginning of a new grace period. The sequence through all three loops in lines 14-16 check to see if the current thread is still in an RCU read-side critical section that began before the counter was incremented back in line 12; if so, it must wait for on line 17. Line 18 ensures that the compiler refetches the rcu_reader_qs per variable. Line 22 executes one last memory barrier to ensure that all CPUs have fully completed their RCU read-side critical sections before the caller of synchronize_rcu() performs any destructive actions (such as freeing of memory).

This implementation has low-cost read-side primitives, as can be seen in Figure 4. Read-side overhead depends on how often rcu_quiescent_state() is called; state-side primitives qualifies as wait-free under the most severe conceivable definition (14). The (synchronize_rcu()) overhead ranges from about 600 nanoseconds on a single-core CPU5 power system to about 1 microsecond or more microseconds on a 64-CPU system with one thread per CPU.

Because it waits for readers to complete, synchronize_rcu() does not qualify as non-blocking. Section IV-E describes how RCU updates can support wait-free algorithms in the same sense as wait-free algorithms are supported by garbage collectors.

However, this implementation requires that each thread either invoke rcu_quiescent_state() periodically or invoke rcu_thread_offline() for extended quiescent states. The need to invoke these functions periodically can make this implementation difficult to use in some situations, such as for certain types of library functions.

In this implementation, decompression does not permit concurrent calls to synchronize_rcu() to share overlapping grace periods. That said, one could easily imagine a production-quality RCU implementation based on this version of RCU.

Finally, on systems where the rcu_gp_ctr is implemented using 32-bit counters, all logic can be half if the RCU is preemplted in line 3 of rcu_read_lock() in Figure 4 enough time to allow the rcu_gp_ctr to advance to through more than half before it is possible to return from rcu_read_lock(). In addition to being 32-bit systems, 32-bit systems can be handled by adapting rcu_read_lock() and rcu_read_unlock() from Figure 6 for use by rcu_quiescent_state() and rcu_offline_thread()), respectively. This would course also require adapting the synchronize_rcu() implementation from Figure 7.

Another point worth discussing is that if read-side critical sections are expected to be extremely long, then the rcu_quiescent_state() primitive must be run with signals disabled, and signals must be kept disabled while threads are quiescent. Effectively, if a memory signal handler nests over rcu_quiescent_state() between the memory barriers, the read-side could be interrupted leave with the reader_rcu_qs per update and therefore spawn across to two grace periods, which could cause synchronize_rcu() to return before the quiescent state is reached and lead to data corruption.

The next section discusses an RCU implementation that is safe for use in libraries, where the library code cannot

```jsonl
{"line": 1, "code": "#define BCPU_GP_CTR_BOTTOM0 0x80000000"}
{"line": 2, "code": "#define RCU_GP_CTR_NEST_MASK (RCU_GP_CTR_BOTTOM_BIT - 1)"}
{"line": 3, "code": "#define rcu_gp_ctr 1"}
{"line": 4, "code": "#define PER_CPU_thread(long), rcu_reader_gp);"}
{"line": 5, "code": ""}
{"line": 6, "code": "static inline void rcu_read_lock(void) {"}
{"line": 7, "code": "    long tmp;"}
{"line": 8, "code": "    long *rrgp;"}
{"line": 9, "code": ""}
{"line": 10, "code": ""}
{"line": 11, "code": "    rrgp = &_get_thread_var(rcu_reader_gp);"}
{"line": 12, "code": "    tmp = *rrgp;"}
{"line": 13, "code": "    if ((tmp & RCU_GP_CTR_NEST_MASK) == 0) {"}
{"line": 14, "code": "        *rrgp = ACCESS_ONCE(rcu_gp_ctr);"}
{"line": 15, "code": "        smp_mb();"}
{"line": 16, "code": "    } else {"}
{"line": 17, "code": "        *rrgp = tmp + 1;"}
{"line": 18, "code": "    }"}
{"line": 19, "code": "}"}
{"line": 20, "code": ""}
{"line": 21, "code": "static inline void rcu_read_unlock(void)"}
{"line": 22, "code": "    long tmp;"}
{"line": 23, "code": "    long *rrgp;"}
{"line": 24, "code": "    smp_mb();"}
{"line": 25, "code": "    _get_thread_var(rcu_reader_gp--);"}
{"line": 26, "code": ""}
{"line": 27, "code": ""}
```

Fig. 6. RCU Read Side Using Memory Barriers

guarantee that all threads of a yet-unwritten application will traverse quiescent states in a timely fashion.

C. General-Purpose RCU

The general-purpose RCU implementation can be theory in used in any software environment, including one in ley benefit functions that are not aware of the design of the enclosing application. However, the price paid for this generality is relatively high read-side overhead, though this overhead is still significantly less than a single compare-and-swap operation on most hardware.

The global variable rcu_gp_ctr is initialized to 1 and a per-thread variable rcu_reader_gp is initialized to zero. The lower-order bits of rcu_reader_gp is a count of the rcu_read_lock() nesting depth, while the upper bit indicates the grace-period phase at the time of the invocation of the outermost rcu_read_lock() (15). The upper bit of the global variable rcu_gp_ctr is the current grace-period phase, while the lower-order field is set to the current grace-period phase, while the lower-order field is set to the current grace-period phase, while this will become apparent shortly.

The reader primitives are shown in Figure 6. Lines 1-4 are declarations, lines 6-19 are rcu_read_lock() and lines 21-27 are rcu_read_unlock().

In rcu_read_lock(), line 11 obtains a reference to the current thread's instance of rcu_reader_gp, and line 12 fetches the its value. Line 13 checks the outermost rcu_read_lock(), and if so, line 14 copies the current value of the global rcu_gp_ctr to this thread's rcu_reader_gp variable, thereby snapping the current grace-period phase and setting the nesting count to 1 in a single operation. Otherwise, line 17 increments the nesting count in this thread's rcu_reader_gp variable.

Line 26 decrements the thread's rcu_reader_gp, which has the effect of decrementing the nesting count.

For outermost read-side rcu_read_lock(), the memory barrier on line 15 ensures that rcu_reader_gp value is globally observable before any of the outermost read-side critical



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS. VOL. X, NO. Y, MONTH 2009
7

line 5 instructs inline int rcu_old_gp_ongoing(int t)
{
    int v = ACCESS_ONCE(per_thread[t].rcu_reader_gp, t);
}
return (v & RCU_GP_CTR_NEST_MASK) & ~RCU_GP_CTR_NEST_MASK;
}

line 9 static void flip_counter_and_wait(void)
{
    int t;
    rcu_gp_ctr = *RCU_GP_CTR_BOTTOM_BIT;
    for_each_thread(t) {
        while (rcu_old_gp_ongoing(t)) {
            poll(NULL, 0, 10);
        }
        barrier();
    }
}

line 22 void synchronize_rcu(void)
{
    smp_mb();
    mutex_lock(&rcu_gp_lock);
    flip_counter_and_wait();
    flip_counter_and_wait();
    mutex_unlock(&rcu_gp_lock);
    smp_mb();
}

[IMAGE_DESCRIPTION: A technical figure labeled "Fig 7." The image contains a detailed architectural flow diagram depicting "RCU Update Side Using Memory Barriers." It shows a sequence of operations involving a "Writer" thread and multiple "Reader" threads. The diagram uses rectangular blocks for processes and arrows to indicate the temporal sequence and memory visibility. Key components include "smp_mb()" barriers, "mutex_lock", and "flip_counter_and_wait" calls. Dotted lines indicate the boundary of the "Grace Period." The layout is linear from top to bottom, illustrating the synchronization lifecycle to ensure all pre-existing readers have completed before memory is reclaimed.]

Fig 7. RCU Update Side Using Memory Barriers

section memory accesses. It ensures that either the compiler not the CPU nor the compiler reader will memorize access across this barrier by adding a compiler barrier and issuing a memory barrier instruction. Only the outermost rcu_read_lock() needs to have such memory barrier because only this outermost lock can change the reader's current grace period grace period.

In other words, line 25 issues a memory barrier to ensure that all globally observable effects of the RCU read-side critical section reach memory before rcu_reader_gp is decremented. The memory barrier on line 25 is only needed for the outermost rcu_read_lock(), but given the outermost and innermost nesting level behave in the exact same way, a branch in the rcu_read_unlock() code is unnecessary, and given the common case is to perform single-level nesting, the memory barrier is executed unconditionally for innermost and outermost nesting levels.

Section IV-D shows one way of getting rid of both memory barriers; however, even with the memory barriers, both rcu_read_lock() and rcu_read_unlock() are wait-free.

The implementation of the rcu_old_gp_ongoing() and rcu_read_unlock() is that a thread given may be ignored by the current grace period phase in either of the following cases:
1) The lower order bits of the thread's rcu_reader_gp variable are all zero in which case, that thread is not currently in an RCU read-side critical section.
2) The upper bit of the thread's rcu_reader_gp variable matches that of the global rcu_gp_ctr, in which case this thread's RCU read-side critical section started after the beginning of the current grace-period phase.

These checks are implemented by the function rcu_old_gp_ongoing(), which is shown in lines 1-7 of Figure 7. These implicitly referring to the declarations and variables in Lines 1-4 of Figure 4. Given a thread t, line 5 fetches t's rcu_reader_gp variable, with the ACCESS_ONCE() primitive ensuring the variable is read with a single memory access. This prevents the compiler from refetching the variable or fetching it in pieces. Line 5 then checks to see if the low-order bits are nonzero, and line 6 then checks to see if the upper bit differs from that of the rcu_gp_ctr global variable. Only if both of these conditions hold does rcu_old_gp_ongoing() report that the current grace-period phase must wait on this thread.

Lines 9-20 of Figure 7 show the flip_counter_and_wait(), which initiates a new grace-period phase and waits for it to elapse. Line 13 completes the update bit of the global variable rcu_gp_ctr, which initiates a new grace-period phase. Line 14 cycles through all threads. The while loop at line 15 repeatedly executes lines 16-17 until rcu_old_gp_ongoing() reports that readers no longer in resides in an RCU read-side critical section that affects the readers in the current grace-period phase. Line 16, which is optional, blocks for a short period of time, and line 17 ensures that the compiler reflects variables when executing rcu_old_gp_ongoing().

Lines 22-30 of Figure 7 shows synchronize_rcu(), which waits for a full memory grace period to elapse. Line 24 executes a memory barrier to ensure that any prior data-structure modification is seen by all threads to precede the grace period. Line 25 acquires rcu_gp_lock to serialize any concurrent invocations of synchronize_rcu(). Lines 26-27 wait for two grace-period phases, lines 28 releases the lock, and line 29 executes a memory barrier to ensure that all the grace period modifications are seen before any subsequent destructive operations (such as free()).

The memory ordering between the rcu_gp_ctr element and testing the current reader's grace period with rcu_old_gp_ongoing() is not strictly needed. The only requirement is that each and every reader that was reading in the exact same read-side critical section before memory barrier on line 24 has completed its reading after the memory barrier on line 29. This two-phase grace period scheme is used to ensure update progress through a grace period even if steady a flow of readers comes. The only requirement is that, when the updater busy-loops waiting for readers, it eventually reaches a point where all new readers are in the new grace period party.

Grace period identification, by either of the two phase-counting counters, ensure that readers that starting during the grace period will not prevent the grace period from completing. In fact, if a simplistic scheme where the updater simply waits for all readers to complete would be used, the updater would reach a point where considered as complete when the updater reaches a point where no reader is active in the system. However, this would no longer allow new readers starting after the beginning of the grace period to impede reaching quiescent state. This would render the grace-period identification in the space of reader threads releasing the read-side critical section for very short periods. Faster cached local data access would therefore provide an unfair advantage to the reader over the updater.

Now that the grace period identification question is settled, this raises the question "why isn't a single grace-period phase sufficient?" To see why, consider the following sequence of events which involves one reader-side critical section and two consecutive grace periods:



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009

1) Thread A invokes rcu_read_lock(), executing lines 11–
13 of Figure 6, and finding that this instance of rcu_
read_lock() is not nested, fetching the value of rcu_gp_
ctr on line 14, but not yet storing it.

2) Thread B invokes synchronize(), executing lines 14–
25 and on Figure 7, then bumping cfg_counter_and_
wait() on line 26, where it complements the grace-period
phase bit on line 13, so that the new value of this bit is
now 1.

3) Because no thread is in an RCU read-side critical sec-
tion (recall that Thread A has not yet executed the
store operation on line 14), Thread B proceeds through
lines 19–40 of Figure 7, returns to synchronize(), rcu_
executing lines 28–30 (recall that line 27 is omitted in
this scenario), and returning to the caller.

4) Thread A now performs the store in line 14 of Figure 6.
Recall that it is using the old value of rcu_gp_ctr where
the value of the grace-period memory barrier phase is 0,

5) Thread A then executes the memory barrier on line 15,
and returns to the caller, which proceeds in turn to the
RCU read-side critical section.

6) Thread B invokes synchronize() rcu_once more, again
implementing the grace-period phase bit on line 13 of
Figure 7, so that the value again is 0.

7) When Thread B examines Thread A’s rcu_reader_gp
variable on lines of Figure 7, it finds that the grace-
period phase bit matches that identified of the global
rcu_gp_ctr Thread A is therefore ignored, and Thread B
therefore exits from synchronize() rcu_.

8) But Thread A is still in its RCU read-side critical sec-
tion in violation of RCU semantics.

Invoking f_counter_and_wait() twice avoids this problem by
making sure the rcu grace period waits for all reader
critical sections for each of the possible two phases.

in a single-phase approach this possible: if the current grace
period is identified by a TX-counter-run counter, as shown in
section IV-B, however, the counter size is important because
this counter is subject overflow. The single-flip problem shown
above, which involves two consecutive grace periods, is ac-
tually a scenario where a single-flip overflow occurs. A similar
scenario is therefore possible given a number of grace periods
sufficient to overflow the grace period counter passing during
an rcu read lock section. This could realistically happen on 32-
bit architectures if read-side critical sections are preempted.

The following section shows one way to eliminate the read-
side memory barriers.

D. Low-Overhead RCU Via Signal Handling

The largest sources of overhead for the QSBR and general-
purpose RCU read-side primitives shown in Figures 4 and 6
are the memory barriers. One way to eliminate this overhead
is to use POSIX signals. The readers signal handlers contain
memory-barrier instructions, which allow an updater to force
readers to execute a memory-barrier instruction only when
needed, rather than stuffing the extra overhead during every
call to a read-side primitive.

```jsonl
{"line": 1, "content": "#define RCU_GP_COUNT (1UL << 0)"}
{"line": 2, "content": "#define RCU_GP_CTR_BIT (1UL << (sizeof(long) * 4))"}
{"line": 3, "content": "#define RCU_GP_CTR_NEST_MASK (RCU_GP_CTR_BIT - 1)"}
{"line": 4, "content": ""}
{"line": 5, "content": "long urcu_gp_ctr = RCU_GP_COUNT;"}
{"line": 6, "content": "long __thread urcu_active_readers = 0L;"}
{"line": 7, "content": ""}
{"line": 8, "content": "static inline void rcu_read_lock(void)"}
{"line": 9, "content": "{"}
{"line": 10, "content": "    long tmp;"}
{"line": 11, "content": ""}
{"line": 12, "content": "    tmp = urcu_active_readers;"}
{"line": 13, "content": "    if (tmp & RCU_GP_CTR_NEST_MASK)"}
{"line": 14, "content": "        urcu_active_readers = ACCESS_ONCE(urcu_gp_ctr);"}
{"line": 15, "content": ""}
{"line": 16, "content": "    urcu_active_readers = tmp + RCU_GP_COUNT;"}
{"line": 17, "content": "    barrier();"}
{"line": 18, "content": "}"}
{"line": 19, "content": ""}
{"line": 20, "content": "static inline void rcu_read_unlock(void)"}
{"line": 21, "content": "{"}
{"line": 22, "content": "    barrier();"}
{"line": 23, "content": "    urcu_active_readers = urcu_active_readers - RCU_GP_COUNT;"}
{"line": 24, "content": "}"}
```

Fig 8. RCU Read Side Using Signals

One unexpected but quite pleasant surprise is that this ap-
proach results in relatively simple read-side primitives. In
contrast, those of preemptible RCU are moderately complex.

The read-side primitives are shown in Figure 8, along with
the definitions and state variables. The urcu_gp_ctr variable
used for grace periods, works like “user-space RCU”. Lines 1–3
show the definitions controlling both the urcu_gp_ctr global
variable (line 5) and the urcu_active_readers per-thread vari-
able (line 6). The low-order bits (those corresponding to
1-bit in RCU_GP_CTR_NEST_MASK) are used to count
the rcu_read_lock() nesting level, while the bit specified by
RCU_GP_CTR_BIT is used to detect grace periods. All other
bits are unused. The urcu_gp_ctr variable is global and can
be changed at any time by any thread, but must be updated only
by the thread holding the lock that guards grace-period detection. The
per-thread urcu_active_readers variable may be modified only by
the corresponding thread, and is otherwise read only by the
thread holding the lock that guards grace-period detection.

The rcu_read_lock() implementation is shown on lines 9–
18. Line 12 picks up the current value of the thread’s
urcu_active_readers, and stores it in a local variable
tmp. Line 13 checks to see if the nesting-level portion of urcu_
active_readers is zero (indicating that this is the outermost
rcu_read_lock()), and if so, line 14 copies the global variable
urcu_gp_ctr this thread’s urcu_active_readers variable. Note
that urcu_gp_ctr has been initialized with its lowest-order bit
set, so that the nesting level is automatically set correctly.
Otherwise, line 16 increments the nesting level portion of the
urcu_active_readers variable. In either case, line 17 executes
a barrier directive in order to prevent the compiler from un-
dertaking any code-motion optimization that might otherwise
cause the contents of the subsequent RCU read-side critical
section to be reordered to precede the rcu_read_lock().

The implementation of rcu_read_unlock() is shown on
lines 20–24. Line 22 executes a barrier directive again, in
order to prevent the compiler from undertaking any code-
motion optimization that might otherwise cause the contents
of the prior RCU read-side critical section to be reordered to



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS. VOL. X, NO. Y, MONTH 2009
9

```jsonl
{"type": "code_block", "content": "thread_registry {\n  thread_t id;\n  long active_readers;\n  char need_mb;\n} registry;\nstatic thread_registry *thread_need_mb;\nstatic int num_readers;\n\nvoid force_mb_all_threads(void) {\n  struct reader_registry *index;\n  if (!registry) return;\n  index = registry;\n  for (index = registry + num_readers; index++; *) {\n    *index->need_mb = 1;\n    thread_kill(index->tid, SIGUSR1);\n  }\n  index = registry;\n  for (index < registry + num_readers; index++; *) {\n    while (*index->need_mb == mb) {\n      thread_kill(index->tid, SIGUSR1);\n      poll(NULL, 0, 1);\n    }\n    smp_mb();\n  }\n}\n\nvoid siguser_handler(int signo, siginfo_t *signinfo, void *context) {\n  smp_mb();\n  need_mb = 0;\n  smp_mb();\n}"}
```

Fig. 9. RCU Signal Handling

follow the rcu_read_unlock(). Line 23 decrements the value of this thread's urcu_active_readers variable, so that if this is the outermost rcu_read_unlock(), the low-order bits indicating the nesting level will now be zero.

Both rcu_read_lock() and rcu_read_unlock() execute a sharply bounded number of instructions, hence, both are wait-free.

The signal-handling primitives are shown in Figure 9, including variable declarations on lines 1-7, force_mb_all_threads() on lines 9-28 and siguser_handler() on lines 30-36.

The structures on lines 1-5 represents a thread, with this thread_t id in order to point to its tid, active_readers variable, and a pointer to its need_mb per-thread variable. Line 6 declares the per-thread need_mb variable, and line 7 defines the global variable num_readers, which contains the number of threads that are represented in the registry array defined on line 5.

The force_mb_all_threads() function ensures a memory barrier is executed on all running threads by sending a POSIX signal to all threads, waiting for each to respond. As we will see, this has the effect of prompting compiler-ordering directives such as barrier() to full memory barriers, while avoiding the need to incur the cost of expensive barriers in read-side primitives in the common case. Lines 13-14 return if there are no readers, and lines 16-19 set each thread's mpb_per_thread variable to the value one, then send that thread a POSIX signal. Note that the system call executed for pthread_kill() implies a full memory barrier before the system call execution at the operating system level. This memory barrier ensures that all memory accesses done prior to the call to pthread_kill() are not reordered after the start of the system call. Lines 20-26 then reada the threads, waiting until one each has responded by setting its need_mb per-thread variable to zero, because some versions of some operating systems can lose signals, line 23 will resend the signal if a response is not received in a timely fashion. Finally, line 27 executes a memory barrier to ensure that the signals have been received and acknowledged before later operations that might otherwise destructively interfere with readers.

Lines 30-36 show the signal handler that runs in response to a given thread receiving the POSIX signal sent by force_mb_all_threads() (siguser_handler) function executes a pair of memory barriers separated by setting its need_mb per-thread variable to zero. This has the effect of placing a full memory barrier at whatever point in the thread's code that was executing at the time that the signal was received, preventing the CPU from reordering accesses prior to that point.

The sender thread has two memory barriers around whole sequence consisting of sending the signal and waiting for the remote threads to acknowledge its reception. The remote thread executes a memory barrier before acknowledging the signal reception. These two conditions ensure that the remote thread's program order and memory accesses passed by a point where they were executing in order between the two memory barriers on the sender thread. Therefore, execution in program order and with ordered memory accesses is ensured on the remote processors at that point. This promotes a full memory barrier on the receiver side to memory barriers, on the only when the matching memory barrier is executed on the sender side.

The update-grade sequence per-stride primitives are shown in Figure 10, including switch_rect_usr_q(opacity) on lines 1-4, rcu_old_gp_ongoing() on lines 5-6, lin_wait_for_quiescent_studios() on lines 17-29, and synchronize_rcu() on lines 31-41.

The synchronize_rcu() function implements a full grace-period phase, where a pair of such phases make up a pair of sequence phase. A single phase is insufficient for the same reasons discussed in Section IV-D. This simple function uses the designated bit in the urcu_gp_ctr global variable.

The rcu_old_gp_ongoing() determines whether or not the thread with the referenced per-thread urcu_active_readers variable is still executing an RCU read-side critical section that started before this grace-period phase started. Lines 10-11 check if there is no thread, and returns zero if there is not, given that a non-existent thread cannot be executing at all, let alone within an RCU read-side critical section. Line 12 holds the lock for the grace-period, because thread registration uses the lock (ACCESS_ONCE()) primitive to prevent a later compiler optimization that might otherwise cause the value to be fetched more than once. Then line 13 checks to see if the responding thread is in an RCU read-side critical section, and if so, line 14 checks to see if that RCU read-side critical section predates the beginning of the current grace-period phase.

The wait_for_quiescent_state() waits for each thread to pass through a quiescent state, thereby completing one phase of the grace period. Lines 21-22 return immediately if there are no threads. Otherwise, the loop spanning lines 23-28 waits



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009 10

```jsonl
{"line": "1 void call_rcu(struct rcu_head *head,", "type": "code"}
{"line": "2 void (*func)(struct rcu_head *head))", "type": "code"}
{"line": "3 {", "type": "code"}
{"line": "4 head->func = func;", "type": "code"}
{"line": "5 head->next = NULL;", "type": "code"}
{"line": "6 enqueue(head, &rcu_data);", "type": "code"}
{"line": "7 }", "type": "code"}
{"line": "8", "type": "code"}
{"line": "9 void call_rcu_cleanup(void)", "type": "code"}
{"line": "10 {", "type": "code"}
{"line": "11 struct rcu_head *next;", "type": "code"}
{"line": "12 struct rcu_head *wait;", "type": "code"}
{"line": "13", "type": "code"}
{"line": "14 for (;;)", "type": "code"}
{"line": "15 wait = dequeue_all(head);", "type": "code"}
{"line": "16 synchronize_rcu();", "type": "code"}
{"line": "17 while (wait) {", "type": "code"}
{"line": "18 next = wait->next;", "type": "code"}
{"line": "19 wait = func(wait);", "type": "code"}
{"line": "20 wait = next;", "type": "code"}
{"line": "21 }", "type": "code"}
{"line": "22 poll(NULL, 0, 1);", "type": "code"}
{"line": "23 }", "type": "code"}
{"line": "24 }", "type": "code"}
```

```jsonl
{"line": "1 void switch_next_urcu_qpriority(void)", "type": "code"}
{"line": "2 {", "type": "code"}
{"line": "3 urcu_gp_ctr = RCU_GP_CTR & RCU_GP_CTR_BIT;", "type": "code"}
{"line": "4 }", "type": "code"}
{"line": "5", "type": "code"}
{"line": "6 static inline int rcu_old_gp_ongoing(long *value)", "type": "code"}
{"line": "7 {", "type": "code"}
{"line": "8 long v;", "type": "code"}
{"line": "9", "type": "code"}
{"line": "10 if (value == NULL)", "type": "code"}
{"line": "11 return 0;", "type": "code"}
{"line": "12 v = ACCESS_ONCE(*value);", "type": "code"}
{"line": "13 return (v & RCU_GP_CTR_MASK) &&", "type": "code"}
{"line": "14 (v ^ urcu_gp_ctr & RCU_GP_CTR_BIT);", "type": "code"}
{"line": "15 }", "type": "code"}
{"line": "16", "type": "code"}
{"line": "17 static void wait_for_quiescent_state(void)", "type": "code"}
{"line": "18 {", "type": "code"}
{"line": "19 struct reader_registry *i;", "type": "code"}
{"line": "20", "type": "code"}
{"line": "21 if (!registry)", "type": "code"}
{"line": "22 return;", "type": "code"}
{"line": "23 i = registry;", "type": "code"}
{"line": "24 for (i = registry; i < num_readers; i++) {", "type": "code"}
{"line": "25 while(rcu_old_gp_ongoing(i->urcu_active_readers))", "type": "code"}
{"line": "26 cpu_relax();", "type": "code"}
{"line": "27 }", "type": "code"}
{"line": "28 }", "type": "code"}
{"line": "29", "type": "code"}
{"line": "30 void synchronize_rcu(void)", "type": "code"}
{"line": "31 {", "type": "code"}
{"line": "32 internal_urcu_lock();", "type": "code"}
{"line": "33 force_mb_all_threads();", "type": "code"}
{"line": "34 switch_next_urcu_qpriority();", "type": "code"}
{"line": "35 wait_for_quiescent_state();", "type": "code"}
{"line": "36 switch_next_urcu_qpriority();", "type": "code"}
{"line": "37 wait_for_quiescent_state();", "type": "code"}
{"line": "38 force_mb_all_threads();", "type": "code"}
{"line": "39 internal_urcu_unlock();", "type": "code"}
{"line": "40 }", "type": "code"}
{"line": "41", "type": "code"}
```

Fig. 10. RCU Update Using Side Signals

Fig. 11. Avoiding Update-Side Blocking by RCU

need not block the accessor algorithm itself, thus as delays built into an automatic garbage collector must not block a white-flag algorithm.

One way of accomplishing this is shown in Figure 11, which implements the asynchronous call_rcu() primitive found in the Linux kernel. Lines 4 and 5 initialize an RCU callback, and line 6 uses a wait-free enqueue algorithm (16) to enqueue the callback on a rcu_data list. This call_rcu() function is then clearly wait-free.

A separate thread would remove and invoke these callbacks after a grace period has elapsed, using synchronize_rcu() for this purpose, as shown on lines 9–24 of Figure 11, with each pass of the loop spanning lines 14–23 wanting for one grace period. Line 15 uses a (possibly blocking) dequeue algorithm to remove the first callback from the rcu_data list, and line 16 waits for a grace period to elapse. Lines 17–21 invoke all the RCU callbacks from the list dequeued by line 15. Finally, line 22 blocks for a short period to allow additional RCU callbacks to be enqueued. Note that the longer line 22 waits, the more RCU callbacks will be accumulated on the rcu_data list. This is a classic memory/CPU trade-off, with longer waits allowing more memory to be occupied by RCU callbacks, but decreasing the per-callback CPU overhead.

Of course, the use of synchronize_rcu() causes call_rcu_cleanup() to be blocking. However, as long as the callback function that was passed to call_rcu() does nothing other than free memory, as long as the synchronization mechanism used to coordinate RCU updates is wait-free, and as long as there is sufficient memory for allocations to succeed without blocking, RCU-based algorithms that use call_rcu() will themselves be wait-free.

E. Wait-Free RCU Updates

Although some algorithms use RCU as a first-class technique, in most situations RCU is instead simply used as an approximation to a garbage collector. In these situations, given sufficient memory, the delays built into synchronize_rcu()

V. EXPERIMENTAL RESULTS

This section presents benchmarks of each RCU mechanism presented in this paper with respect to each other, compared



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009 11

to mutexes, to reader-writer locks, and to per-thread locks¹. It first demonstrates read-side scalability, discusses the impact of read-side critical section length on the respective locking primitive behavior and finally examines under what operation rate impact on read-side performance. The goal of this section is to clearly demonstrate in which situation RCU outperforms classic locking solutions to help identify for which workloads RCU can bring performance improvements compared to classic locks in existing applications.

The machines used to run the benchmarks are an 8-core Intel Core2 Xeon E5405 clocked at 2.0 GHz and a 64-core PowerPC POWER5+ clocked at 1.9 GHz. Each core of the PowerPC machine has 2 hardware threads. To eliminate thread-level contention for processor resources, benchmarks are performed with affinity to the 64 even-numbered CPUs of the 128 logical CPUs presented by the system.

The mutex and reader-writer locker implementations used for comparison are the standard pthread implementations from the GNU C Library 2.7 for 64-bit Intel and GNU C Library 2.5 for 64-bit PowerPC.

STM (Software Transactional Memory) is not included in these comparisons because it is already known to incur high overhead and to scale poorly (17). BTM (Hardware Transactional Memory) (18, 19, 20) is likely to be more scalable than STM. However, BTM hardware is not available to us due to the fact that it is expensive and not very common, preventing us from including it in our performance results.

A. Scalability

Figure 12 presents the read-side scalability comparison of each RCU mechanism with standard locking primitives for the PowerPC. The goal of this test is to determine how each system's synchronization primitive performs under heavy read-side scenarios when the number of CPU increases. This is done by executing from 1 to 64 reader threads for 10 seconds, each taking a read-lock, reading a data unit and releasing the lock in a tight loop. No updater thread is present in this test. As a result, we observe that linear scalability is achieved for RCU and per-thread mutex approaches. This is expected, given readers readers do not need to exchange cache-lines. The QSBR approach is the fastest, followed by the sign-based RCU, general-purpose RCU and per-thread mutex, each having a constant per-CPU overhead. The Intel Xeon behaves similarly.

However, Figure 12 does not show the scalability trend of the pthread mutex and pthread reader-writer lock primitives. This is the purpose of Figure 13, which presents scalability of those two primitives. As we can see, with more than 8 cores, overall performance actually decreases when the number of core increases.

B. Read-Side Critical Section Length

Due to the large performance difference between RCU and other approaches, we notice that linear-scaled graphs are not appropriate for the following comparisons.

¹The per-thread lock approach consists in using one mutex per reader thread. The updater threads must take all the mutexes, always in the same order, to exclude all readers. This approach ensures reader cache locality at the expense of a slower write-side locking.

{"figure": 12, "type": "x-y plot", "title": "Read-Side Scalability of Various Synchronization Primitives, 64-core POWER5+", "x_axis": "Number of cores", "y_axis": "Number of reads / second", "x_range": [0, 70], "y_range": [0, 9e+09], "series": [{"label": "QSBR", "trend": "Linear increase, steepest slope"}, {"label": "Signal-based RCU", "trend": "Linear increase, high slope"}, {"label": "General-purpose RCU", "trend": "Linear increase, moderate slope"}, {"label": "per-thread mutex", "trend": "Linear increase, lower slope"}, {"label": "pthread mutex", "trend": "Increases slightly then plateaus/drops after 8 cores"}, {"label": "pthread reader-writer lock", "trend": "Increases slightly then plateaus/drops after 8 cores"}]}

Therefore, Figure 14 presents the read-side critical section length impact using logarithmic x and y axis. This benchmark is performed with 8 reader threads taking the read lock, reading a data structure, writing to an arbitrary variable, and delaying the releasing the lock, thus simulating any active update internally. On this 8-core machine, we notice that starting at about 1000 cycles per critical section, the difference between RCU and per-thread locks becomes insignificant. At around 20000 cycles per critical section, the reader-writer locks are almost as fast as any other solutions. Only pthread mutex performance always has significantly worse performance for all critical section lengths.

To appropriately present the 64-core read-side critical section length impact on the read-side speed, we must first introduce the effects that alter the reader-writer lock and mutex

{"figure": 13, "type": "x-y plot", "title": "Read-Side Scalability of Mutexes and Reader-Writer Lock, 64-core POWER5+", "x_axis": "Number of cores", "y_axis": "Number of reads / second", "x_range": [0, 70], "y_range": [0, 9e+06], "series": [{"label": "pthread mutex", "trend": "Rises to peak at 8 cores, then declines steadily"}, {"label": "pthread reader-writer lock", "trend": "Rises to peak at 8 cores, then declines steadily, remaining slightly above pthread mutex"}]}

{"figure": 14, "type": "x-y plot", "title": "Impact of Read-Side Critical Section Length, 8-core Intel Xeon, Logarithmic Scale", "x_axis": "Read-side C.S. length (in cycles)", "y_axis": "Number of reads / second", "x_scale": "logarithmic", "y_scale": "logarithmic", "x_range": [10, 1e+08], "y_range": [100, 1e+09], "series": [{"label": "QSBR", "trend": "High flat line, slight decline at very high C.S. length"}, {"label": "Signal-based RCU", "trend": "High flat line, slightly below QSBR"}, {"label": "General-purpose RCU", "trend": "High flat line, slightly below Signal-based RCU"}, {"label": "per-thread mutex", "trend": "Starts lower, converges with RCU series around 1000-10000 cycles"}, {"label": "pthread reader-writer lock", "trend": "Starts mid-range, converges with RCU series around 20000 cycles"}, {"label": "pthread mutex", "trend": "Starts lowest, remains significantly lower than all other series throughout"}]}



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS. VOL. X, NO. Y, MONTH 2009
12

[IMAGE DESCRIPTION: A line graph showing the relationship between "Read-side C.S. length (in cycles)" on the x-axis (logarithmic scale from 0.1 to 1e+06) and "Number of reads / second" on the y-axis (logarithmic scale from 100 to 1e+09). There are four data series: QSBR (top-most, nearly flat line), Signal-based RCU (slight decline), General purpose RCU (steeper decline), and Per-thread mutex (steepest decline). The legend indicates different markers: QSBR uses open circles, Signal-based RCU uses open squares, General purpose RCU uses open triangles, and Per-thread mutex uses asterisks. Additionally, pthread reader-writer lock and pthread mutex are listed in the legend but are overlapping or closely aligned with the mutex trends.]

Fig. 15. Impact of Read-Side Critical Section Length, 8 Reader Threads on POWER5+, Logarithmic Scale

[IMAGE DESCRIPTION: A line graph showing "Updates per second" on the y-axis (logarithmic scale from 1000 to 1e+08) versus "Updates per grace period per core" on the x-axis (logarithmic scale from 1 to 1e+06). A single data line starts at approximately (1, 1000), rises steeply, levels off slightly around 1e+05, and ends at approximately (1e+06, 1e+07). The line is marked with small black dots.]

Fig. 17. Impact of Grace-Period Batch-Size on Number of Update Operations, 8-core Intel Xeon, Logarithmic Scale

[IMAGE DESCRIPTION: A line graph showing "Number of reads / second" on the y-axis (logarithmic scale from 100 to 1e+10) versus "Read-side C.S. length (in cycles)" on the x-axis (logarithmic scale from 0.1 to 1e+06). The curves are similar to Figure 15 but shifted higher on the y-axis. QSBR is the highest, followed by Signal-based RCU, General purpose RCU, and finally Per-thread mutex, which shows the most significant performance drop as length increases.]

Fig. 16. Impact of Read-Side Critical Section Length, 64 Reader Threads on POWER5+, Logarithmic Scale

[IMAGE DESCRIPTION: A line graph showing "Updates per second" on the y-axis (logarithmic scale from 10000 to 1e+07) versus "Updates per grace period per core" on the x-axis (logarithmic scale from 0 to 1e+06). A single line of black dots starts at (10, 10000), rises sharply, and plateaus just below 1e+07 as it reaches 1e+06 on the x-axis.]

Fig. 18. Impact of Grace-Period Batch-Size on Number of Update Operations, 64-core POWER5+, Logarithmic Scale

C. RCU Grace-Period Batch Calibration

After looking at read-only performance, it is appropriate to see how concurrent updates influence the read-side behavior. To approximately represent the RCU update-side performance impact, we must first calibrate the requiem update-side batch size to ensure we amortize the grace-period overhead over multiple updates. Such calibration is presented for Intel and POWER5+ in Figures 17 and 18, respectively for 8 cores and 64 cores. For update operation benchmark, we use half the number of cores for readers and the other half for updates.

Our calibration is based on the signal-based RCU. Because it is likely to provide the highest grace-period overhead due to signal-handler execution. The ideal batch size for both architectures with 8 cores is determined to be 32768 per update thread. Given the test duration is 10 sec, we have to eliminate update batches large enough to ensure a significant portion of updates performed during the test because non-terminated batches are not reclaimed. Thus in this way the largest batch sizes are ignored even if they are slightly better. Figure 18 shows that with 64 cores, the ideal batch size is slightly lower (4096) due to the fact that per-update pointer exchange quickly increases with the number of threads while the grace-period overhead increases linearly. Therefore, smaller batch sizes are required to amortize the grace-period overhead and perform slightly better due to increased cache locality. However, given the performance difference is not very large, we use a 32768 batch size for both 8-core and 64-core tests.

D. Update Overhead

Once batch-size calibration is performed, we can proceed to update rate impact comparison. Figure 19 presents the impact of update frequency on read-side performance for the various



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009
13

{"type": "x-y plot", "title": "Update Overhead, 8-core Intel Xeon, Logarithmic Scale", "x_axis": "Updates/s", "y_axis": "Reads/s", "x_scale": "logarithmic", "y_scale": "logarithmic", "data_series": [{"label": "QSBR", "points": "scattered high values across x-axis"}, {"label": "Signal-based RCU", "points": "linear cluster mid-range"}, {"label": "General-purpose RCU", "points": "linear cluster mid-range"}, {"label": "Per-thread mutex", "points": "lower cluster"}, {"label": "pthread reader-writer lock", "points": "lower cluster"}, {"label": "pthread mutex", "points": "lower cluster"}]}

Fig. 19. Update Overhead, 8-core Intel Xeon, Logarithmic Scale

{"type": "x-y plot", "title": "Update Overhead, 64-core POWER5+, Logarithmic Scale", "x_axis": "Updates/s", "y_axis": "Reads/s", "x_scale": "logarithmic", "y_scale": "logarithmic", "data_series": [{"label": "QSBR", "points": "scattered high values across x-axis"}, {"label": "Signal-based RCU", "points": "linear cluster mid-range"}, {"label": "General-purpose RCU", "points": "linear cluster mid-range"}, {"label": "Per-thread mutex", "points": "lower cluster"}, {"label": "pthread reader-writer lock", "points": "lower cluster"}, {"label": "pthread mutex", "points": "lower cluster"}]}

Fig. 21. Update Overhead, 64-core POWER5+, Logarithmic Scale

{"type": "x-y plot", "title": "Impact of Pointer Exchange on Update Overhead, 8-core Intel Xeon, Logarithmic Scale", "x_axis": "Updates/s", "y_axis": "Reads/s", "x_scale": "logarithmic", "y_scale": "logarithmic", "data_series": [{"label": "Ideal QSBR", "points": "flat line at top"}, {"label": "Ideal Signal-based RCU", "points": "flat line at top"}, {"label": "Ideal General-purpose RCU", "points": "flat line at top"}, {"label": "QSBR", "points": "scattered points"}, {"label": "Signal-based RCU", "points": "cluster"}, {"label": "General-purpose RCU", "points": "cluster"}]}

Fig. 20. Impact of Pointer Exchange on Update Overhead, 8-core Intel Xeon, Logarithmic Scale

{"type": "x-y plot", "title": "Impact of Pointer Exchange on Update Overhead, 64-core POWER5+, Logarithmic Scale", "x_axis": "Updates/s", "y_axis": "Reads/s", "x_scale": "logarithmic", "y_scale": "logarithmic", "data_series": [{"label": "Ideal QSBR", "points": "flat line at top"}, {"label": "Ideal Signal-based RCU", "points": "flat line at top"}, {"label": "Ideal General-purpose RCU", "points": "flat line at top"}, {"label": "QSBR", "points": "scattered points"}, {"label": "Signal-based RCU", "points": "cluster"}, {"label": "General-purpose RCU", "points": "cluster"}]}

Fig. 22. Impact of Pointer Exchange on Update Overhead, 64-core POWER5+, Logarithmic Scale

locking primitives. It is performed by running 4 reader and 4 updater threads and varying the delay between updates. We notice that RCU approaches outperform the per-thread lock approach especially in terms of maximum updates per second. The former can reach 2 million updates per second while per-thread locks can only perform 0.1 million updates per second. Interestingly, on such workload with 4 tight loop readers, mutex outperforms the reader-writer lock primitive in all aspects especially in terms of maximum updates per second. A case of reader starvation with high updates per second rates.

But while Figure 19 presents a fair comparison between the locking primitives, it presents a non ideal scenario for RCU. In our attempt to present a comparison where all locking primitives perform equivalent first work, this figure, we include the RCU pointer exchange overhead. To factor out this overhead, Figure 20 shows both Ideal RCU grace period and the performance and the overheads with added pointer exchange. It is obvious that it is the pointer exchange that becomes the update rate bottleneck, not the grace period bottleneck. Therefore, in an ideal scenario where pointers updates would be local to each thread, RCU could be expected to preserve its read-side scalability characteristics even under frequent updates. Such local updates could be ensured by appropriately designed list or hash table data structures.

Figure 21 shows update overhead on a 64-core POWER5+, with 32 reader and 32 updater threads. We can conclude that RCU QSBR and general purpose approaches reach the highest update rates, even compared to mutexes. This is attributed to the lower performance overhead for exchanging a pointer compared to the multiple atomic operations and memory barriers implied by acquiring and releasing a mutex. Mutex-based benchmark performance schemes were direct to stop drumming at 30,000 updates per second with 32 updater threads. A similar effect is present with only 4 updater threads (graph not presented for brevity). Figure 19 seemed to show that update overhead stayed constant even at higher update frequency for 4 updater threads on the Xeon. Therefore, as the number of concurrent readers and updaters behave similarly regardless of the architecture and on the specific GNU C Library version. Two approaches seem to be very affected by increasing the number of updaters. The reader-writer lock, where updaters clearly seem to be starved by readers, has a maximum update rate of 175 updates per second. Per-thread locks are limited to a maximum update rate of 10000 updates per second with 32 reader threads, as shown in Figure 19.

Finally, Figure 22 presents, as previously done for Xeon, how grace-period detection (ideal RCU) compares to RCU grace period with pointer exchange. Therefore, with appropriately designed data structures, better update locality would ideally lead to constant update overhead as the update frequency increases.

VI. CONCLUSIONS

We have presented a set of RCU implementations covering a wide spectrum of application architectures. QSBR shows



IEEE TRANSACTIONS ON PARALLEL AND DISTRIBUTED SYSTEMS, VOL. X, NO. Y, MONTH 2009                                 14

the best performance characteristics, but severely constrains the application architecture by requiring each reader thread to periodically pass through a quiescent state. Signal-based RCU performs almost as well as does $\text{QSBR}$, but requires devices to signal a wake-up. Unlike the other two, general-purpose RCU incurs signal-based read overhead. However, it minimizes constraints on application architecture, requiring only that each thread invoke an initialization function before entering its first RCU read-side critical section.

Benchmarks demonstrate read-side linear scalability of the RCU and per-thread lock approaches. It also shows that the walmart-scale critical section duration for which the reader-writer locks, RCU and per-thread lock approaches are nearly equivalent in terms of read-side performance impact grows larger as the number of cores increases. These benchmarks also show that, by performing memory reclamation in batch, RCU approaches reach update rates much higher than reader-writer locks, per-thread locks and mutexes on similar workloads where updates are performed on a shared data structure. Furthermore, giving ideal data processing times and perfect cache locality, RCU approaches are shown to have a constant update overhead as update frequency increases. Therefore, the upper-bound for RCU update overhead is demonstrated to be far below lock-based update overhead. Furthermore, it is still possible to decrease RCU update-side overhead even more by designing data structures providing good update cache-locality.

ACKNOWLEDGMENTS

We owe thanks to Magged Al Memari for his illuminating discussions, to Kathy Bennett for her support of this effort, to Etienne Bergeron and Alexandre Desnoyers for reviewing this paper.

This material is based upon work supported by the National Science Foundation under Grant No. CNS-0719851. The work is funded by Google, Natural Sciences and Engineering Research Council of Canada, ERCison and Defence Research and Development Canada.

LEGAL STATEMENT

This work represents the views of the authors and does not necessarily represent the views of Ecole Polytechnique de Montreal, Harvard, IBM, or Portland State University. Linux is a registered trademark of Linus Torvalds.

Other company, product, and service names may be trademarks or service marks of other others.

REFERENCES

[1] B. Gamsa, O. Krieger, J. Appavoo, and M. Stumm, “Toromazo: Maximizing locality and concurrency in a shared memory multiprocessor operating system,” in Proceedings of the 3rd Symposium on Operating System Design and Implementation, New Orleans, LA, February 1999, pp. 87–100.

[2] J. P. Hennessy, D. L. Oliek, and J. W. Seig, “HPS: Passive serialization in a multithreading environment,” US Patent and Trademark Office, Washington, DC, Tech. rep. US Patent 4,809,168 (issued, February) 1989.

[3] V. Jacobson, “Avoid read-side locking via delayed freeing,” September 1993.

[4] A. John, “Dynamic viewpoints – design and implementation,” in USENIX Winter 1995. New Orleans, LA: USENIX Association, January 1995, pp. 11–23.

[5] P. E. McKenney and J. D. Slingcone, “Read-copy update: Using execution history to solve concurrency problems,” in Parallel and Distributed Computing and Systems, Las Vegas, NV, October 1998, pp. 509–518.

[6] T. E. Hart, P. E. McKenney, A. D. Brown, and J. Walpole, “Performance of memory reclamation for lockless synchronization,” J. Parallel Distrib. Comput., vol. 67, no. 12, pp. 1270–1285, 2007.

[7] A. J. Wood, “Practical lock-freedom,” Ph.D. dissertation, King’s College, University of Cambridge, 2003.

[8] D. Ginguletia, P. E. McKenney, J. Triplet, and J. Walpole, “The read-copy-update mechanism for supporting real-time applications on shared-memory multiprocessor systems with Linux,” IBM Journals, System Volume, vol. 47, no. 2, pp. 221–236, May 2008.

[9] P. E. McKenney, “Exploring deferred deletion: An analysis of read-copy-update techniques in operating system kernels,” Ph.D. dissertation, OGI School of Science and Engineering at Oregon Health and Sciences University, December 2004.

[10] ——, “What is RCU? part 2: Usage,” January 2008, available: http://lwn.net/Articles/263130/.

[11] M. Desnoyers and M. R. Dagenais, “Synchronization for fast and reentrant operating system kernel tracking,” 2002.

[12] E. Polyakov, “The elliptics network,” April 2009, available: http://www.ioremap.net/projects/elliptics.

[13] T. Jiminez and P. Vixe, “Implementation and evaluation of moderater parallelism in the Binding DNS server,” in Proceedings of the annual Conference on USENIX/ANNUL Technical Conference, Boston, MA, February 2006, pp. 115–126.

[14] M. Herlihy, “Wait-free synchronization,” ACM TOPLAS, vol. 13, no. 1, pp. 124–149, January 1991.

[15] P. E. McKenney, “Using a maximalism user-level RCU to torture RCU-based algorithms,” in linux.conf.eu 2009, Hobart, Australia, January 2009.

[16] J. M. Crummey and M. L. Scott, “Algorithms for scalable synchronization on shared-memory multiprocessors,” Transactions of Computer Systems, vol. 9, no. 1, pp. 21–65, February 1991.

[17] C. Casalvino, C. Bullmend, M. Michael, H. W. Cain, P. Wu, S. Chitraes, and C. Shatterjee, “Software transactional memory: Why is it only a research toy?” ACM Queue, December 2008.

[18] A. Shvachko, “Non-blocking Approach to Transactional Memory, 2007.

[19] Scalable and reliable communication for hardware transactional memory, 2008.

[20] D. Dice, Y. Lev, M. Moir, and D. Nussbaum, “Early experience with a commercial hardware transactional memory implementation,” in Fourteenth International Conference on Architectural Support for Programming Languages and Operating Systems (ASPLOS ’09), Washington, DC, USA, March 2009, p. 12.
