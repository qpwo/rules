Introduction
o

Kepler support
o

Kernel
ooo

Userspace
oooo

Tools
oooo

Community
ooo

{"type": "graphic", "content": "A large, centered rectangular button with rounded corners. The button is a deep royal blue with a subtle drop shadow creating a 3D effect against a white background. Inside the button, the text is centered and white. The top line reads 'Nouveau' in a larger font, and the bottom line reads 'Recap, on-going and future work' in a slightly smaller font."}

Martin Peres & the Nouveau community

Ph.D. student at LaBRI

September 25, 2013



Introduction
o
Kepler support
o
Kernel
ooo
Userspace
oooo
Tools
oooo
Community
ooo

Summary

{"item": 1, "text": "Introduction", "status": "active"}
{"item": 2, "text": "Kepler support", "status": "inactive"}
{"item": 3, "text": "Kernel", "status": "inactive"}
{"item": 4, "text": "Userspace", "status": "inactive"}
{"item": 5, "text": "Tools", "status": "inactive"}
{"item": 6, "text": "Community", "status": "inactive"}



Introduction
Kepler support
o
Kernel
ooo
Userspace
oooo
Tools
oooo
Community
ooo

Introduction

Introduction

Introduction
- Last nouveau update was at FOSDEM 2012;
- Many improvements since then :).



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Summary

{"item": 1, "label": "Introduction", "status": "inactive"}
{"item": 2, "label": "Kepler support", "status": "active"}
{"item": 3, "label": "Kernel", "status": "inactive"}
{"item": 4, "label": "Userspace", "status": "inactive"}
{"item": 5, "label": "Tools", "status": "inactive"}
{"item": 6, "label": "Community", "status": "inactive"}



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Kepler support

# Kepler support

[Image: A rectangular information box with a dark blue header bar containing the white text "Kepler". Below the header is a light grey body with rounded bottom corners and a subtle drop shadow. Inside the body is a bulleted list with dark blue circular bullets.]

* New NVIDIA card family released in March 2012;
* Modesetting support released on the same day;
* Un-released 3D support happened a few days later;
* 2D/3D accel support released less than a month after (after libdrm2).



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Summary

1 Introduction
2 Kepler support
3 Kernel
  * Optimus/prime
  * Power Management
4 Userspace
5 Tools
6 Community



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Kernel updates

# Kernel updates
* Nouveau left staging (Linux 3.4);
* Major internal re-architecturing, called core (Linux 3.7);

# The core architecture
* Separate code per-chipset;
* Kind of object-oriented (ctor, dtor, init & fini);
* Should limit regressions when adding support to new cards;
* Contribution by Ben Skeggs.



Introduction
Kepler support
Kernel
Userspace
Tools
Community
Optimus/prime

Optimus/prime

Optimus/Prime support
* Offloading support added by Dave Airlie in Linux 3.9;
* Synchronisation between drivers, worked on by mlankhorst;

More information + how to
http://nouveau.freedesktop.org/wiki/Optimus/



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Power Management

Power Management

Thermal management
- Temperature monitoring support added for most cards;
- Except for the i2c-only temperature probes.

Fan management
- Static fan management added in Linux 3.7;
- Experimental automatic fan management added in Linux 3.9;
- Enabled by default in Linux 3.??.

Contact Martin Peres (mupuf) if you have problems!



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Summary

1 Introduction
2 Kepler support
3 Kernel
4 Userspace
* Libdrm_nouveau2
* Video decoding
* Direct 3D
* Performance counters
5 Tools
6 Community



Introduction
Kepler support
○
Kernel
○○○
Userspace
●○○○
Tools
○○○○
Community
○○○

Libdrm_nouveau2

Userspace updates - Libdrm_nouveau2

{"title": "Libdrm_nouveau2", "items": ["Expose BOs' VM addresses;", "Support multiple threads per channel;", "Rework the rellocation mechanism;", "Reduce the occurences of -ENOSPC;", "Released in April 2012 by Ben Skeggs."]}

{"title": "Libdrm_nouveau2 : Mesa updates", "items": ["Mesa drivers updated to use Libdrm_nouveau2;", "Nvfx rewritten and renamed nv30;", "Various fixes to the other drivers."]}



Introduction
Kepler support
○
Kernel
○○○
Userspace
○○○
Tools
○○○○
Community
○○○

Video decoding

Userspace updates - Video decoding

{"section": "Video decoding : Maarten Lankhorst", "items": ["Fermi+ support added by Maarten Lankhorst;", "Rely on user-extracted firmwares (mmiotrace)."]}

{"section": "Video decoding : Ilia Mirkin", "items": ["VP2/3/4 support added by Ilia Mirkin;", "Wrote a script to extract firmwares from the blob;", "Work on video planes on older generations."]}

{"section": "More information", "content": "http://nouveau.freedesktop.org/wiki/VideoAcceleration"}



Introduction  Kepler support  Kernel  Userspace  Tools  Community
Direct 3D

Nine: a d3d9 state tracker

Nine: a d3d9 state tracker
* Started by Joakim Sindholt;
* Completed by Christoph Bumiller
* Runs Skyrim, Civilization 5, Anno 1404 and StarCraft 2;
* Up to 2 times faster than Wine's d3d implementation.

Announcement
http://lists.freedesktop.org/archives/mesa-dev/2013-July/041900.html

Source tree
https://github.com/chrisbmr/Mesa-3D/tree/gallium-nine



Introduction
Kepler support
○
Kernel
○○○
Userspace
○○○●
Tools
○○○○
Community
○○○

Performance counters

# Performance counters

Performance counters
* MP-counters support for Fermi+;
* Exposed through Gallium-HUD;
* Kepler support by Christoph Bumiller;
* Fermi support by Samuel Pitoise (GSOC 2013).



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Summary

1 Introduction
2 Kepler support
3 Kernel
4 Userspace
5 Tools
- Envytools repo moved
- RESTification of the documentation
- Falcon C Compiler
- Falcon & other NVIDIA ISAs Decompiler!
6 Community



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Envytools repo moved

Envytools

Envytools
* is a collection of nvidia-related tools and docs;
* was primarily hosted by Pathscale;
* but was also hosted by mwk & sourceforge;
* moved to one repo with every dev as admins.

More information
http://lists.freedesktop.org/archives/nouveau/2013-July/013089.html



Introduction
Kepler support
ooo
Kernel
ooo
Userspace
oooo
Tools
o○○
Community
ooo

RESTification of the documentation

Envytools : documentation

[Box 1]
Title: hwdocs before
- text-based documentation of NVIDIA hw;
- links written as plain text.

[Box 2]
Title: hwdocs after
- text-based documentation of NVIDIA hw;
- can generate pretty html documentations.
- Example: http://envytools.github.io/envytools/.



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Falcon C Compiler

# Falcon C compiler

Falcon C compiler
* Started by Shinpei Kato;
* work for PGRAPH firmwares;
* can be extended to support PDAEMON.

Links
* Source: https://github.com/CS005/guc
* Paper: http://hgpu.org/?p=10251



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Falcon & other NVIDIA ISAs Decompiler!

NVIDIA ISAs decompiler

{"element": "header_box", "title": "decompiler", "content": [
  {"bullet": "Decompiler project started by Marcin Kościelnicki;"},
  {"bullet": "works on falcon & vp2macro;"},
  {"bullet": "will support xtensa & possibly vuc;"},
  {"bullet": "will be released after Marcin’s master thesis (soon);"},
  {"bullet": "Example: http://ng.0x04.net/~mwk/deco.txt."}
]}



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Summary

1 Introduction
2 Kepler support
3 Kernel
4 Userspace
5 Tools
6 Community
* Bugzilla cleaning
* Wiki portage & rewrite
* New member!



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Bugzilla cleaning

Community

Bugzilla cleaning
* Started by Ilia Mirkin;
* closed all bugs not updated since 2011;
* asking people to reproduce on current Nouveau;
* Reduced bug reports from 410 to 167;
* Helped fixing some actual bugs along the way.



Introduction
Kepler support
Kernel
Userspace
Tools
Community

Wiki portage & rewrite

Community

Wiki portage
* Freedesktop moved to ikiwiki;
* killed a lot of spam along the way;
* but it is now harder to add content.

Wiki clean up & rewrite
* Started by Ilia Mirkin & Martin Peres;
* Rewrote all the main pages to make them helpful;
* deleted the old cruft.



Introduction
Kepler support
○
Kernel
○○○
Userspace
○○○○
Tools
○○○○
Community
●○

New member!

Community - Welcome NVIDIA!

Flash news
* NVIDIA released NDA-free documentation today;
* documentation on the DCB-related vbios tables;
* offered us a contact email to answer questions;
* are willing to improve the out-of-the-box experience of users;
* Welcome to the Nouveau community, NVIDIA!
