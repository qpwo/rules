History
Architecture
Past & current work
The Nouveau community
Demos

[Image: A presentation slide with a white background. At the top, there is a dark navy blue horizontal navigation bar containing the text "History", "Architecture", "Past & current work", "The Nouveau community", and "Demos" in a white sans-serif font. Below the bar, centered on the slide, is a large rounded rectangle with a deep blue gradient fill and a soft drop shadow. Inside this rectangle, the word "Nouveau" is written in a large white sans-serif font, and below it, the phrase "The community & past, current and future developments" is written in a slightly smaller white sans-serif font. Below the blue box, centered on the page, is the text "Martin Peres, Ben Skeggs & the Nouveau community", followed by "Ph.D. student at LaBRI, Red Hat", and finally "September 13, 2011". All three lines of bottom text are in a black sans-serif font.]



History
Architecture
Past & current work
The Nouveau community
Demos

Summary

1 History
2 Architecture
3 Past & current work
4 The Nouveau community
5 Demos



History Architecture Past & current work The Nouveau community Demos

History: NVIDIA, a new hope

* 1998(?): NVIDIA releases a Linux open-source 2D-only driver(nv)
* 1998: Obfuscation commit (release only pre-processed source)



History Architecture Past & current work The Nouveau community Demos

After we already finalized XFree86-3.3.3 NVIDIA forced The XFree86 Project to replace the sources we had with sources that were partly run through the C preprocessor in order to remove some of the names that NVIDIA thought might give away IP from NVIDIA. This resulted in unreadable and unmaintainable code.

The XFree86 Project is strongly opposed to such obfuscated code. We do not regard this as free software according to our standards. Due to the extremely late date of this decision from NVIDIA we decided to include the code as offered by NVIDIA. We are considering to remove support for the later NVIDIA chips in a future release, though.

Xfree86, commit message from 11/18/98



History Architecture Past & current work The Nouveau community Demos

History: The open-source strikes back

* 2005: Stephane Marchesin improves nv and works on 3D
    * Project named Nouveau after an unfortunate automatic spelling correction
    * Back-port nv updates to Nouveau
* 2008: Open Arena runs on nv40
* 2009: KMS driver based on TTM
* 2010: Merged in Linux 2.6.33
* 2010: Nv is deprecated by NVIDIA, say “use VESA”.



History Architecture Past & current work The Nouveau community Demos

History: The return of the Jedi

“It’s so hard to write a graphics driver that open-sourcing it would not help [...] In addition, customers aren’t asking for opensource drivers.”
Andrew Fear, NVIDIA software product manager, April 2006



History
Architecture
ooo
Past & current work
oooo
The Nouveau community
oooooo
Demos

Summary

1 History

2 Architecture
• Chipset families
• Components

3 Past & current work

4 The Nouveau community

5 Demos



History Architecture (●○) Past & current work (○○○○) The Nouveau community (○○○○○○) Demos
Chipset families

Chipset families
{"type": "list", "items": [
  {"id": "NV03", "name": "RIVA 128"},
  {"id": "NV04", "name": "RIVA TNT/TNT2"},
  {"id": "NV10", "name": "Geforce2/4 MX"},
  {"id": "NV20", "name": "Geforce 3, Geforce 4 Ti"},
  {"id": "NV30", "name": "Geforce 5/FX"},
  {"id": "NV40", "name": "Geforce 6/7 (the first real family)"},
  {"id": "NV50", "name": "Geforce 8/9/100/200/300 (the biggest family)"},
  {"id": "NVC0", "name": "Geforce 400/500, AKA Fermi"},
  {"id": "NVD9", "name": "Half-Kepler chipset"}
]}



History Architecture (selected) Past & current work The Nouveau community Demos
Components

{"module": "Linux module Nouveau", "features": ["Kernel Mode Setting", "Command-submission", "Resource allocation"]}

{"module": "Nouveau DDX", "features": ["EXA (2D acceleration)", "X-Video"]}

{"module": "Mesa: 3D acceleration", "features": ["Nouveau_vieux: 3D for NV04,NV10,NV20 (mesa classic)", "NVFX: 3D for the NV30,40 families (gallium)", "Nouveau: 3D for NV50,C0 families (gallium)"]}



History Architecture Past & current work The Nouveau community Demos
Components

NV30/40 microcodes
* HWSQ: very limited use (LVDS), no flow control
* CtxProgs on nv40: Context switching

NV50 microcodes
* CtxProgs: Context-switching microcode
* HWSQ v2 (formerly PMS): memory reclocking
* nv98+ Flexible MicroCode(Fμc): a general-purpose microcode. Broad usage (PCrypt, PDaemon, Vdec, ...)

NVC0-D9
* nvc0: PGRAPH (the rendering engine) is converted to Fμc
* nvd9: some remaining engines are converted to Fμc



History
Architecture
Past & current work
The Nouveau community
Demos

Summary

1 History
2 Architecture
3 Past & current work
* Modesetting, 2D
* 3D support
* Power management
* HW video decoding
4 The Nouveau community
5 Demos



History Architecture Past & current work The Nouveau community Demos
Modesetting, 2D

```jsonl
{"section": "ModeSetting - Done", "items": ["NV04: TNT2", "NV10: Geforce2/4 MX", "NV20: Geforce 3, Geforce 4 Ti", "NV30: Geforce 5/FX", "NV40: Geforce 6/7", "NV50: Geforce 8/9/100/200/300 (the biggest family)", "NVC0: Geforce 400/500, AKA Fermi"]}
```

```jsonl
{"section": "ModeSetting - WIP", "items": ["NVD9: Partially Kepler"]}
```



History Architecture Past & current work The Nouveau community Demos
3D support

3D Drivers and support
* nouveau_vieux (NV04,10,20): classic mesa driver. Unsupported.
* nvfx (NV30,40): gallium driver. Works but no maintainer.
* nouveau (NV50,C0): gallium driver. Supported!

WIP
* add nvc1 support
* improve performance
* crashes with Unigine Tropics & Heaven on some chipsets



History Architecture Past & current work The Nouveau community Demos
Power management

Power management
* Readings: Temperature & clocks
* vbios parsing: mostly
* setting clocks: unreliable, potentially dangerous

WIP
* nvc0: setting clocks
* nv40-d9: memory timings (almost ready)
* nv30-c0: reliable clock changes (almost ready)
* AGP/PCIE, clock gating: reverse engineering
* performance counters, dynamic reclocking: WIP



History
Architecture
ooo
Past & current work
ooo●o
The Nouveau community
oooooo
Demos

HW video decoding

HW video decoding
* MPEG1/2: nv40-98

Why is MPEG4 hw decoding so hard?
* involves at least 4 engines
* different ISAs (Fμc, VPx)
* codecs needs to be implemented



History
Architecture
Past & current work
The Nouveau community
Demos

Summary

1 History
2 Architecture
3 Past & current work
4 The Nouveau community
    * Composition
5 Demos



History Architecture ○○○ Past & current work ○○○○ The Nouveau community ●○○○○○ Demos
Composition

The nouveau community
* The largest xorg-related IRC channel
* Composed of:
    * One paid developer
    * Former developers
    * Student developers
    * Enthusiasts

The nouveau maintainer
Nouveau is maintained by Ben Skeggs(darktama):
* Hired by Red Hat in 2009
* Located in Brisbane, Australia (GMT+10)
* Works on almost everything



History    Architecture    Past & current work    The Nouveau community    Demos
Composition

{"box": "Gallium’s nouveau maintainer", "content": "Christoph Bumiller (calim) maintains the nv50-c0 gallium driver:\n- Physics master student at the University of Vienna (Austria, GMT+1)\n- Main Nouveau Gallium contributor\n- nv50/c0 3D support\n- performance improvements"}

{"box": "Marcin Kościelnicki (mwk)", "content": "Polish master student at the university of Warsaw (GMT+1)\n- Implemented most of the GPGPU-oriented PSCNV nouveau fork\n- Reversed most of the Fermi’s architecture and video decoding"}



History Architecture (ooo) Past & current work (oooo) The Nouveau community (oo ooo) Demos
Composition

```jsonl
{"name": "Pekka Paalanen", "handle": "pq", "details": ["Finnish (GMT+2)", "Worked on mmiotraces, a register DB", "Does some communication-related work"]}
{"name": "Francisco Jerez", "handle": "curro", "details": ["Spanish Physics student (GMT+1)", "Worked on page-flipping and nouveau_vieux"]}
{"name": "Marcin Slusarz", "handle": "joi", "details": ["Polish SQL/C++/Java developer (GMT+1)", "Fixes software bugs (mesa + kernel), maintains Valgrind-MMT"]}
```



History Architecture Past & current work The Nouveau community Demos
Composition

{"name": "Emil Velikov", "handle": "xexaxo", "details": ["Bulgarian student at the University of Nottingham (England, GMT+0)", "Reverse engineering of some PM-related vbios table", "Other PM-related implementation work", "Debugging/Testing"]}

{"name": "Roy Spliet", "handle": "RSpliet", "details": ["Dutch master student at the Delft University (GMT+1)", "Memory timings reverse engineering"]}



History Architecture Past & current work The Nouveau community Demos
Composition

Martin Peres (mupuf)
* Engineer, Ph.D. student at LaBRI (France, GMT+1)
* Reclocking process
* Thermal-zones & thermal management
* Minor reverse engineering

Maxim Levitsky (MaximLevitsky)
* Student at the Technion University of Haifa (GMT+2)
* Important reverse engineering work on reducing power consumption
* Stability-related reverse engineering
* New-comer



History Architecture (ooo) Past & current work (oooo) The Nouveau community (ooooo●) Demos

Composition

{"name": "Maarten Lankhorst", "username": "mlankhorst", "details": ["Netherlands (GMT+1)", "Implemented XVMC support", "Work towards an open VDPAU", "New-comer"]}



History
Architecture
Past & current work
The Nouveau community
Demos

Summary

1 History
2 Architecture
3 Past & current work
4 The Nouveau community
5 Demos



History Architecture Past & current work The Nouveau community Demos

{"item": "MPEG1/2 video decoding", "details": ["using MPlayer", "Kernel-side: done", "Mesa: merged 3 days ago"]}



History
Architecture
Past & current work
The Nouveau community
Demos

Dynamic reclocking
* see clocks changing according to the load
* performance improvements in OpenArena
* performance improvements in xvmc
