The Nouveau community at FOSDEM
o

History
oooo

Architecture
ooooooooooooooooooo

Conclusion
o

Demos
o

[Image: A centered, rectangular blue box with rounded corners and a subtle drop shadow. Inside the box, white sans-serif text reads "Nouveau" in a larger font on the top line, and "Recap, on-going and future work" in a slightly smaller font on the second line.]

Martin Peres, Lucas Stach & the Nouveau community

Ph.D. student at LaBRI, B.Eng. student at HfTL

February 4, 2012



The Nouveau community at FOSDEM | History ○○○○ | Architecture ○○○○○○○○○○○○○○○○ | Conclusion ○ | Demos ○

# Summary

{"item": 1, "text": "The Nouveau community at FOSDEM", "status": "active"}
{"item": 2, "text": "History", "status": "inactive"}
{"item": 3, "text": "Architecture", "status": "inactive"}
{"item": 4, "text": "Conclusion", "status": "inactive"}
{"item": 5, "text": "Demos", "status": "inactive"}



The Nouveau community at FOSDEM    History ○○○○    Architecture ○○○○○○○○○○○○○○○○ ○    Conclusion ○    Demos ○
Hello world!

{"section": "Your hosts for the next hour", "members": [{"name": "Martin Peres", "handle": "mupuf"}, {"name": "Maarten Lankhorst", "handle": "mlankhorst"}, {"name": "Lucas Stach", "handle": "lynxeye"}]}

{"section": "Also attending", "members": [{"name": "Emil Velikov", "handle": "xexaxo"}, {"name": "Francisco Jerez", "handle": "curro"}, {"name": "Roy Spliet", "handle": "rspliet"}]}

{"section": "Not attending", "members": [{"name": "Ben Skeggs", "handle": "darktama"}, {"name": "Marcin Koscielnicki", "handle": "mwk"}, {"name": "Christoph Bumiller", "handle": "calim"}, {"name": "... and many more!", "handle": null}]}



The Nouveau community at FOSDEM    History (selected)    Architecture    Conclusion    Demos

Summary

1 The Nouveau community at FOSDEM
2 History
3 Architecture
4 Conclusion
5 Demos



The Nouveau community at FOSDEM    History    Architecture    Conclusion    Demos
                                 ● ○ ○ ○    ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○ ○    ○    ○
A brief recap of Nouveau & Linux

History: NVIDIA – a new hope
* 1998(?): NVIDIA releases “nv”, a Linux open-source 2D-only driver
* 1998: Obfuscation commit, release only pre-processed source



The Nouveau community at FOSDEM
History (active)
Architecture
Conclusion
Demos

A brief recap of Nouveau & Linux

After we already finalized XFree86-3.3.3 NVIDIA forced the XFree86 Project to replace the sources we had with sources that were partly run through the C preprocessor in order to remove some of the names that NVIDIA thought might give away IP from NVIDIA. This resulted in unreadable and unmaintainable code.

The XFree86 Project is strongly opposed to such obfuscated code. We do not regard this as free software according to our standards. Due to the extremely late date of this decision from NVIDIA we decided to include the code as offered by NVIDIA. We are considering to remove support for the later NVIDIA chips in a future release, though.

Xfree86, commit message from 11/18/98



The Nouveau community at FOSDEM
History
Architecture
Conclusion
Demos
A brief recap of Nouveau & Linux

History: The open-source strikes back

* 2005: Stephane Marchesin improves nv and works on 3D
    * Project named Nouveau after an unfortunate automatic spelling correction
    * Back-port nv updates to Nouveau
* 2008: Open Arena runs on nv40
* 2009: KMS driver based on TTM for memory management
* 2010: Merged in Linux 2.6.33
* 2010: Nv is deprecated by NVIDIA, “use VESA”.



The Nouveau community at FOSDEM
History
Architecture
Conclusion
Demos

A brief recap of Nouveau & Linux

History: The return of the Jedi

“It’s so hard to write a graphics driver that open-sourcing it would not help [...] In addition, customers aren’t asking for opensource drivers.”

Andrew Fear, NVIDIA software product manager, April 2006



The Nouveau community at FOSDEM
History ○○○○
Architecture ○○○○○○○○○○○○○○○○
Conclusion ○
Demos ○

Summary

{"list": [{"index": 1, "text": "The Nouveau community at FOSDEM", "status": "inactive"}, {"index": 2, "text": "History", "status": "inactive"}, {"index": 3, "text": "Architecture", "status": "active", "sub-items": ["Short hardware introduction", "Components"]}, {"index": 4, "text": "Conclusion", "status": "inactive"}, {"index": 5, "text": "Demos", "status": "inactive"}]}



The Nouveau community at FOSDEM
History ○○○○
Architecture ●○○○○○○○○○○○○○○○
Conclusion ○
Demos ○
Short hardware introduction

Chipset families
* NV03: RIVA 128
* NV04: RIVA TNT/TNT2
* NV10: Geforce2/4 MX
* NV20: Geforce 3, Geforce 4 Ti
* NV30: Geforce FX
* NV40: Geforce 6/7 (the first real family)
* NV50: Geforce 8/9/100/200/300 (the biggest family)
* NVC0: Geforce 400/500, AKA Fermi
* NVD9: Strange Fermi



The Nouveau community at FOSDEM
History oooo
Architecture o○○○○○○○○○○○○○○○ o
Conclusion o
Demos o
Short hardware introduction

Hardware introduction
* NVIDIA GPUs are controlled by "objects"
* Command submission uses DMA buffers
* GPU can access the host's memory through the aperture
* NV50 features a full-blown virtual memory system for its channels (GPU process)

Important engines
* PFIFO: main command fetching engine
* PCRTC/PDISPLAY: display scanout engines
* PGRAPH: main graphics object AKA OpenGL cast into silicon
* PMPEG: video acceleration found on older cards (MPEG1/2)
* PBSP/PVP/PPPP: modern video decoding engines



The Nouveau community at FOSDEM    History    Architecture    Conclusion    Demos
                                    oooo    oo○○○○○○○○○○○○○    o    o
Components

The 4 parts of Nouveau
* Linux kernel module
* libdrm-nouveau
* DDX: xf86-video-nouveau
* Mesa3D drivers:
    * nouveau-vieux
    * nvfx
    * nv50
    * nvc0



The Nouveau community at FOSDEM
History oooo
Architecture oo●oooooooooo
Conclusion o
Demos o
Components

{"element": "header", "text": "Linux kernel module: Nouveau"}

* Resources management
    * GPU Channels
    * Memory Management
* Command-submission
* Kernel Mode Setting
* Power Management



The Nouveau community at FOSDEM History Architecture Conclusion Demos
oooo ooo●oooooooooo o o

Components

Power management
* Parsing power-management-related vbios table
* Temperature management: calibration + fan
* Setting the clocks: changes the performance level
* Other: clock gating, PCIE downclocking, DAC reclocking

Current state
* nvc0: Setting engine clocks (mostly)
* nv30-c0: Preliminary support to reclock (unreliable)
* nv50-a3: Reliable clock changes (almost)
* nv40-d9: RAM configuration (mostly functional)
* Fan management: toggle (todo), pwm (done), i2c (WIP)
* Performance counters: initial work on nv40-c0



The Nouveau community community at FOSDEM    History ○○○○    Architecture ○○○○○○○○○○○○○○○○    Conclusion ○    Demos ○
Components

WIP
* nva3-d9: Setting memory clocks (using PDAEMON)
* nv50-a3: Ironing out stability bugs
* AGP/PCIE, Clock gating: reverse engineering
* Fan: Temperature-based management (toggle, pwm and i2c)
* Perfmon infrastructure, dynamic reclocking

{"type": "bar_chart", "title": "OpenArena v0.8.5", "subtitle": "Resolution: 1920 x 1080", "source": "OpenBenchmarking.org", "unit": "Frames Per Second, More Is Better", "data": [{"label": "GT 220 - Nouveau Stock SE +/- 0.00", "value": 66.80}, {"label": "GT 220 - Nouveau Reclocked SE +/- 0.03", "value": 86.73}, {"label": "GT 220 - NVIDIA 290.10 SE +/- 0.31", "value": 97.60}], "x_axis": [20, 40, 60, 80, 100], "footer": "Powered By Phoronix Test Suite 3.8.0m1"}

http://www.phorix.com/scan.php?page=article&item=nouveau_reclocking_one&num=5



The Nouveau community at FOSDEM
History (4 empty circles)
Architecture (1 filled circle, 13 empty circles)
Conclusion (1 empty circle)
Demos (1 empty circle)
Components

libdrm-nouveau
* Buffer management
    * everything is a buffer!
* Wraps around the IOCTL interface

Work in progress
* A new implementation is being written
* Designed with nv40 class hardware in mind
* Pushbuffer replay



The Nouveau community at FOSDEM
History oooo
Architecture oo○○○○○○○○○○○○○
Conclusion o
Demos o
Components

DDX – xf86-video-nouveau
* EXA (2D acceleration)
* X-Video
* supports full range of GPUs (NV04 - NVD9)
* makes use of all engines for acceleration (including 3D engine)



The Nouveau community at FOSDEM    History    Architecture    Conclusion    Demos
                                    oooo    oo○○○○○○○○○○○○○○○    o    o
Components

Mesa3D: drivers for 3D and more
* two different approaches here: Classic and Gallium3D
* nouveau-vieux: classic Mesa3D driver for NV04,NV1x,NV2x
    * only supports fixed function OpenGL
* Gallium pipe-driver
    * nvfx
    * nv50
    * nvc0



The Nouveau community at FOSDEM    History    Architecture    Conclusion    Demos
                                    oooo    ooooooooooOooooo    o    o
Components

[IMAGE DESCRIPTION: A technical architecture diagram titled "Components" showing the software stack for graphics drivers. The diagram is organized as a series of nested boxes with arrows indicating data flow and dependencies.

At the top is a box labeled "Applications" containing five sub-boxes: "Weston", "x-server", "wine", "nexuiz", and "Qt".

Below this is a large box labeled "Mesa". Inside Mesa, on the right, is a box labeled "libgl". In the center is a section labeled "Gallium" which contains a box titled "State trackers" with four sub-boxes: "egl", "xorg", "D3d1x", and "OpenGL". To the right of this is a box labeled "Mesa Classics (simplified)" containing five sub-boxes: "intel", "radeon", "nouveau vieux", and "swrast".

Below the State trackers is another box labeled "Gallium" which leads into a box labeled "drivers (compilers)". This contains six sub-boxes: "softpipe", "llvmpipe", "r600g", "r300g", "nvc0", "nv50", and "nvfx".

At the bottom of the diagram are three final components: "LLVM", "GPU (through libdrm)", and "CPU".

Arrows connect the layers: Applications point down to libgl and the Gallium state trackers. State trackers point down to the Gallium drivers. The drivers point down to the GPU and LLVM. LLVM points to the CPU. The Mesa Classics boxes point directly to the GPU. There is a dashed line from the "Applications" area to "libgl" with the handwritten-style text "Wouldn't it be great ?" written vertically.]



The Nouveau community at FOSDEM
History [o o o o]
Architecture [o o o o o o o o o o o o o]
Conclusion [o]
Demos [o]
Components

nvfx
* Gallium driver for NV3x, 4x class GPUs
* One of the first gallium drivers
* Created by merging separate nv30 and nv40 drivers
* Accumulated much old cruft
* No maintainer for over one year

Work in progress
* Will hopefully be soon replaced by rewritten driver



The Nouveau community at FOSDEM History Architecture Conclusion Demos
Components

nv50
* Gallium driver for the NV50 family
* Current codebase was formed by adapting nvc0
* First open driver to run OpenCL on the hardware?

Work in progress
* Merge the reworked shader compiler
* Implement remaining OpenGL 3.x features
* Merge the OpenCL work



[Image Description: A presentation slide with a dark blue header and white background. The header contains a navigation menu with the following items: "The Nouveau community at FOSDEM", "History" (with four small circles underneath), "Architecture" (with a series of eleven small circles, the fifth one being larger/highlighted), "Conclusion" (with one circle), and "Demos" (with one circle). Below the menu, a thin dark blue bar contains the word "Components" aligned to the left. The main body contains two rounded-corner boxes with blue title bars. The first box is titled "nvc0" and contains two bullet points: "Gallium driver for the NVC0 family" and "One of the first drivers to support OpenGL 3". The second box is titled "Work in progress" and contains two bullet points: "DX11" and "OpenGL 3.1+". At the bottom of the slide is a black terminal window containing white monospaced text listing OpenGL vendor and renderer strings and extensions.]

nvc0
* Gallium driver for the NVC0 family
* One of the first drivers to support OpenGL 3

Work in progress
* DX11
* OpenGL 3.1+

```jsonl
{"type": "terminal_output", "content": "GLX_SGIX_fbconfig, GLX_SGIX_pbuffer, GLX_SGIX_visual_select_group, GLX_EXT_texture_from_pixmap\nOpenGL vendor string: nouveau\nOpenGL renderer string: Gallium 0.4 on NVC4\nOpenGL version string: 3.0 Mesa 8.0-d0-devel (git-bce699)\nOpenGL shading language version string: 1.30\nOpenGL extensions:\nGL_ARB_multisample, GL_EXT_bgra, GL_EXT_bgra, GL_EXT_blend_color, GL_EXT_blend_minmax, GL_EXT_blend_subtract, GL_EXT_copy_texture, GL_EXT_polygon_offset, GL_EXT_subtexture, GL_EXT_texture_object, GL_EXT_vertex_array, GL_EXT_compiled_vertex_array, GL_EXT_texture, GL_EXT_texture3D, GL_IBM_rasterops_clip, GL_ARB_point_parameters, GL_EXT_draw_range_elements, GL_EXT_packed_pixels, GL_EXT_point_parameters"}
```



The Nouveau community at FOSDEM    History ○○○○    Architecture ○○○○○○○○○○○○○○○○ ○○    Conclusion ○    Demos ○
Components

OpenCL
- Developed by Francisco Jerez
- Sponsored by the X.org EVOC
- Very experimental
- In-depth presentation here at 18:00

d3d1x
- Direct 3D 10/11 on Gallium
- “Works” only on nouveau, still a bit hackish
- Started by Luca Barbieri
- Currently developed by nv50/c0’s maintainer: Christoph Bumiller
- Unigine Heaven runs on it when you replace the wine d3d10/11 dlls



The Nouveau community at FOSDEM
History
Architecture
Conclusion
Demos
Components

Video decoding
* HD video decoding on atoms
* Lower power consumption

PMPEG
* Blob doesn’t use this engine
* Decodes MPEG1/2

PBSP/PVP/PPPP
* Used by NVIDIA
* Decodes most video streams



The Nouveau community at FOSDEM History Architecture Conclusion Demos
Components

[Image: A high-resolution screenshot of a Linux desktop environment showing a technical demonstration of VDPAU. The screen is divided into three main sections. On the left, a vertical file manager window is open, showing a directory of files including "vp3.mbing.txt", "h264.c", and "nvd_video_c". In the center-left, a terminal window with a black background displays a dense stream of white and red system logs, hexadecimal memory addresses, and kernel debug output. On the right, a large media player window titled "MPlayer" is active, playing a colorful, stylized 2D animation. The animation features a vivid rainbow arc stretching across a landscape with stylized red and orange clouds, purple mountains, and green rolling hills under a light blue sky. At the bottom of the screen, a smaller terminal window shows the command line and output for "mplayer". The entire desktop is set against a neutral grey background.]

VDPAU running on Nouveau with the proprietary microcodes



The Nouveau community at FOSDEM    History oooo    Architecture ooooooooooooooo    Conclusion o    Demos o

Summary

1 The Nouveau community at FOSDEM
2 History
3 Architecture
4 Conclusion
5 Demos



The Nouveau community at FOSDEM
History
Architecture
Conclusion
Demos

Why Nouveau?

Why Nouveau?
* Support old cards unsupported by the blob (< nv40)
* Develop new features (KMS, Xrandr, wayland, d3d1x)
* Plug & play support, no need to install and maintain the blob
* For the fun!



The Nouveau community at FOSDEM History Architecture Conclusion Demos

Summary

{"list": [{"item": 1, "text": "The Nouveau community at FOSDEM"}, {"item": 2, "text": "History"}, {"item": 3, "text": "Architecture"}, {"item": 4, "text": "Conclusion"}, {"item": 5, "text": "Demos"}]}



The Nouveau community at FOSDEM
History oooo
Architecture oooooooooooooooo
Conclusion o
Demos ●

Some fun and experimentation

MPEG1/2 video decoding
* Using MPlayer
* Kernel-side: done
* Mesa: merged in 8.0

reclocking and opengl
* Performance improvements in OpenArena/Nexuiz
* Performance improvements in xvmc
* Dynamic reclocking (load-based reclocking)!
