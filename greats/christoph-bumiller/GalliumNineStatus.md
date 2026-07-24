Native D3D9 on Mesa Gallium Nine : the status

Native D3D9 on Mesa
Gallium Nine : the status

Axel Davy

FOSDEM 2015



Native D3D9 on Mesa Gallium Nine : the status

1 Introduction
2 Wine integration
3 Presenting to the screen
    * D3D9 queue
    * multi-gpu
    * Misc
4 Gallium Nine internals
5 Performance
    * Test configuration 1
    * Test configuration 2
    * Conclusion
6 Plans for the future



Native D3D9 on Mesa Gallium Nine : the status
Introduction

What is this talk about ?

* 2002: d3d9 release
* 2004: OpenGL 2.0 release
* 2004: d3d9 gets improved with shader model 3 support
* 2006: OpenGL 2.1 release
* 2006: d3d10 release
* 2008: OpenGL 3.0 release
* 2009: d3d11 release
* 2010: OpenGL 3.3 and 4.0 release
* 2012: first game with d3d11 support but no d3d9 support
* 2014: most new d3d games still released with d3d9 support



Name D3D9 on Mesa Gallium Nine : the status
Introduction

What is this talk about ?

Why do we want d3d9 ?
⇒ If you want play all d3d9 games released. There’s a lot of them

But we have Steam on Linux ?
⇒ That’s cool, but what about this game Put your game here which is not ported ?

Recent games are enough for me !
⇒ Cool for you

But wine already supports d3d9 ?
⇒ Yes, but we can get better support with Gallium Nine.



Native D3D9 on Mesa Gallium Nine : the status
Introduction

What is Gallium Nine

Project started in 2010 by Joakim Sindholt.

Boosted in 2013 by Christoph Bumiller

Project slowly improves over 2014 and get merged in Mesa.



What is Gallium Nine

{"type": "flowchart", "nodes": [{"id": "1", "text": "State trackers", "children": [{"id": "1a", "text": "Video acceleration (vaapi, vdpau, etc)"}, {"id": "1b", "text": "Gallium Nine"}, {"id": "1c", "text": "Mesa"}]}, {"id": "2", "text": "Gallium Helpers"}, {"id": "3", "text": "Drivers", "children": [{"id": "3a", "text": "R600"}, {"id": "3b", "text": "Radeonsi"}, {"id": "3c", "text": "nouveau"}, {"id": "3d", "text": "llo"}]}], "edges": [{"from": "1", "to": "2"}, {"from": "1b", "to": "3", "label": "Gallium Api TGSI"}, {"from": "2", "to": "3"}]}

[Image description: A presentation slide with a dark blue header bar containing the text "Native D3D9 on Mesa Gallium Nine : the status" and "Introduction" in a small white sans-serif font. Below the header is a large gradient blue banner with the centered white text "What is Gallium Nine". The main body of the slide is white. In the center is a technical block diagram consisting of three main rectangular containers. The first container on the left is labeled "State trackers" at the top and contains three nested rectangles: "Video acceleration (vaapi, vdpau, etc)", "Gallium Nine", and "Mesa". An arrow points from the "State trackers" box to a middle box labeled "Gallium Helpers". From "Gallium Helpers", an arrow points to a third container on the right labeled "Drivers". The "Drivers" container contains four nested rectangles: "R600", "Radeonsi", "nouveau", and "llo". Additionally, a long arrow originates from the "Gallium Nine" box in the first container, passes under the "Gallium Helpers" box, and points to the "Drivers" container; this arrow is labeled "Gallium Api TGSI". All boxes have black borders and black sans-serif text.]



Native D3D9 on Mesa Gallium Nine : the status
Introduction

What is Mesa

{"type": "flowchart", "nodes": [{"id": 1, "text": "GLX\nEGL\n\nGLSL parser\nand optimiser\n\nGL API"}, {"id": 2, "text": "Dri drivers\ni965\nr200\n..."}, {"id": 3, "text": "Gallium\nMesa\nstate tracker"}], "edges": [{"from": 1, "to": 2}, {"from": 1, "to": 3}]}



Native D3D9 on Mesa Gallium Nine : the status
Introduction

How Gallium Nine and Wine are linked

{"type": "diagram", "structure": "stacked_boxes", "content": [{"level": 1, "text": "Wine"}, {"level": 2, "text": "Wine dlls"}, {"level": 3, "text": "Wine d3d9 gl backend: talks to GL and uses Window System API"}, {"level": 4, "text": "Wine nine backend: talks to X directly and links to gallium nine"}]}



D3D9 on Mesa Gallium Nine : the status
Wine integration

Plan

1 Introduction
2 Wine integration
3 Presenting to the screen
  - D3D9 queue
  - multi-gpu
  - Misc
4 Gallium Nine internals
5 Performance
  - Test configuration 1
  - Test configuration 2
  - Conclusion
6 Plans for the future



Native D3D9 on Mesa Gallium Nine : the status
Wine integration

Gallium Nine

Gallium Nine is:
* Mesa only. No proprietary drivers support !
* Gallium only. Poor intel support !

It is composed of:
* Gallium state tracker
* Wine d3d9.dll integration



Native D3D9 on Mesa Gallium Nine : the status
Wine integration

How integration works

Wine - Gallium
d3d9.dll → Direct3DCreate9 → IDirect3D9.
IDirect3D9 → IDirect3DDevice9.

IDirect3D9: Used to get supported formats, resolutions, multisampling modes and device info.
IDirect3D9: Uses D3DAdapter9 for the implementation.
IDirect3DDevice9: Used for everything related to rendering.
IDirect3DDevice9: Uses ID3DPresent to get window size and send buffers to the screen.



Native D3D9 on Mesa Gallium Nine : the status
Wine integration

How integration works

* Wine connects to Gallium Nine and implements all the Window system bits

* Gallium Nine does everything else

⇒ It is possible to use Gallium Nine without Wine (Xnine).



Wine integration
Window system integration

Implementation goals:
* Client side buffer allocation
* Good multi-gpus laptop support
* Behaviour close to expected behaviour

Answer:
* X DRI3 is about client side buffer allocation (≠ DRI2)
* X PRESENT enables control with precision the buffer presentation

For better compatibility, we implemented DRI2/PRESENT fallback relying on EGL_EXT_image_dma_buf_import extension



Native D3D9 on Mesa Gallium Nine : the status
Presenting to the screen

Plan

1 Introduction
2 Wine integration
3 Presenting to the screen
    * D3D9 queue
    * multi-gpu
    * Misc
4 Gallium Nine internals
5 Performance
    * Test configuration 1
    * Test configuration 2
    * Conclusion
6 Plans for the future



D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
D3D9 queue

Present extension

D3D9 expects Render-ahead queue.

OpenGL: As Fast as possible OR synchronized with screen refresh. synchronized with screen refresh: if at vblank n, two frames are presented, only last one will be shown (at vblank n + 1).
⇒ Tripple buffering possible.

D3D9: As Fast as possible OR synchronized with screen refresh. synchronized with screen refresh: new presentation is a last vblank scheduled + 1.
All frames are presented. NO Tripple buffering.



D3D9 on Gallium Nine : the status
Presenting to the screen
D3D9 queue

# D3D9 Render-ahead queue

Apps define the number of back buffers and vblank synchronization.

At every presentation you get a free back buffer from the back buffer pool (order/behaviour defined by parameter). Wait is done when no back buffer is free.
$\Rightarrow$ In practice apps use 2 back buffers, so OpenGL behaviour is ok. However some apps use 3 back buffers.

{"image": "A presentation slide. The top contains a dark blue header bar with white and light blue text. The main body is a white background with black sans-serif text. At the very bottom right, there is a small row of grey navigation icons (arrows, squares, and refresh symbols)."}



Name D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Some laptops have integrated gpu + dedicated gpu.

Under Mesa OpenGL you can use DRI_PRIME or device_id to choose the gpu.



Native D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

How GPU offloading works

Reminder on how DRI_PRIME works:

Get granted access to the device:
* DRI2: Special Flag for it
* DRI3: Use Render-nodes!

How devices talk to each other.
* Render to a tiled buffer in VRAM
* DRI2: Send it to X server, which will copy to linear buffer
* DRI3: Copy to a linear buffer and present it



Native D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Sorry !



D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Sorry !

DRI_PRIME under DRI3 sucks. It wasn’t intended to !
* dma-buf fences still not implemented for all gpus
* radeon driver doesn’t use dma copy anymore for the presentation copy

⇒ GPU will sometimes display whole frames older than the previous one, or display one partially updated (triangle shaped tearing)



D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

# multi-gpu

DRI_PRIME sucks because of synchronization.

DRI2: No synchronization expected. dgpu copies to one buffer, igpu reads from it.
DRI3: Synchronization expected one day. dgpu copies to several buffers, igpu reads from them.
DRI2 always tears, DRI3 has more potential but will show frames in wrong order or not rendered yet because of missing synchronization.

Note: we could workaround Mesa to have DRI3 do the same than DRI2 for now.



Native D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Wait !



D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Wait !
You expect synchronization done in the kernel.



Device D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Wait !
You expect synchronization done in the kernel.

Why not Mesa side ?



D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Wait !
You expect synchronization done in the kernel.

Why not Mesa side ?
$\Rightarrow$ That’s the solution taken for Gallium Nine



D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
multi-gpu

multi-gpu

Gallium Nine thread_submit=true parameter

Uses an additional thread to do the presentations.
Wait the buffer is rendered before presenting.

Result: Excellent. Same performance, but NO DRI_PRIME bugs.
Tear-free possible !



Native D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
Misc

Presentation of multisampled buffers

Apps can ask for a multisampled backbuffer/depth buffer.
But you want to present a single-sampled buffer.

Similar to the multi-gpu case, do a copy.
Rendering is done to multisampled buffer, and copied to non-multisampled buffer.



Native D3D9 on Mesa Gallium Nine : the status
Presenting to the screen
Misc

Throttling

Throttling: Wait done when cpu submits too fast new frames and gpu cannot keep up.

$\Rightarrow$ Extremely important for lag control.

Throttling queue: Usually 2 buffers max for Mesa.

Controlled in Gallium Nine by throttle_value (default 2).
0 means "always wait" (equivalent to glFinish. Bad for performance. No lag).
-1 means "do not wait": Have fun.



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

Plan

1 Introduction
2 Wine integration
3 Presenting to the screen
    - D3D9 queue
    - multi-gpu
    - Misc
4 Gallium Nine internals
5 Performance
    - Test configuration 1
    - Test configuration 2
    - Conclusion
6 Plans for the future



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

# How apps do render

Usually an app does every frame hundreds of:

- Change some Render states
- Change textures bound
- Update vertex buffer
- Switch to another Vertex/Pixel shader
- Update shader constants
- Draw
- Repeat until Presentation

Apps minimize the changes done at every draw call for better performance



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

State changes

Render states in gallium are changed in groups

```jsonl
{"pipe_state": "pipe_depth_stencil_alpha_state"}
{"pipe_state": "pipe_rasterizer_state"}
{"pipe_state": "pipe_blend_state"}
{"pipe_state": "pipe_sampler_state"}
{"pipe_state": "..."}
```

D3D9 states are changed individually

```jsonl
{"d3d9_state": "D3DRS_SHADEMODE"}
{"d3d9_state": "D3DRS_CULLMODE"}
{"d3d9_state": "D3DRS_FILLMODE"}
{"d3d9_state": "..."}
```



D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

State changes

States changes are commited before every new draw call.



Nvidia D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

# Vertex/Pixel shaders

Wine and Mesa state tracker both delay shader compilation at draw time.

$\Rightarrow$ there are stuttering during the first minutes of play.

This is because a vertex shader can be used with several pixel shaders and vice versa $\Rightarrow$ need to link the gl shader again for all new combinations, and because of coordinate handling (rendering to framebuffer or backbuffer doesn't have same coordinates).



D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

Vertex/Pixel shaders

Gallium Nine doesn’t have to cope with coordinate system changes: Doesn’t change !

Gallium Nine compiles shaders at the time they are expected to be compiled. Are compiled once for all (except for very special cases, but very few shaders will be concerned).



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

Mapping Vertex shader outputs to pixel shader inputs

[BOX: Vertex shader code]
VS3.0
DEF c87 { 306.5 1.000000 0.000000 0.000000 }
DCL v0 POSITION0
DCL v1 TEXCOORD0
DCL v2 COLOR0
DCL v3 BLENDWEIGHT0
DCL v4 BLENDINDICES0
DCL o0 POSITION0
DCL o1.xy__ TEXCOORD0
DCL o2.xyz_ TEXCOORD1
DCL o3 COLOR0
DCL o4 COLOR1
...



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

# Mapping Vertex shader outputs to pixel shader inputs

**Vertex shader code**
```
VERT
DCL IN[0]
DCL IN[1]
DCL IN[2]
DCL IN[3]
DCL IN[4]
DCL IN[5]
DCL IN[6]
DCL OUT[0], POSITION
DCL OUT[1].xy, GENERIC[0]
DCL OUT[2].xyz, GENERIC[1]
DCL OUT[3], COLOR
DCL OUT[4], COLOR[1]
...
```



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

Mapping Vertex shader outputs to pixel shader inputs

{"element": "code_block", "title": "Pixel shader code", "content": "PS3.0\nDEF c15 { 2.000000 -1.000000 0.000000 0.000000 }\nDEF c16 { -0.000000 -1.000000 -2.000000 1.000000 }\nDEFI iconst[0] { 3 0 0 0 }\nDCL v0.xy__ TEXCOORD0\nDCL v2.xyz_ TEXCOORD1\nDCL v6 COLOR0\nDCL v7 COLOR1\n..."}



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

# Mapping Vertex shader outputs to pixel shader inputs

We associate a index to every usage/index possible, and fills the data into GENERIC[index].

Index bijection fixed. GENERIC[index] can be sparse.
No need to recompile when using different pixel or vertex shader !



D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

Translate shader code

Code in binary format, already optimised.

Translation easy:
MUL r0._yzw r0.yyyy c10.xyz
becomes
MUL TEMP[0].yzw, TEMP[0].yyyy, CONST[10].xyz



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

Translate shader code

But special cases to handle around 0, Inf and NaN

```jsonl
{"instruction": "RSQ r0.x___ r0.xxxx", "transformation": "becomes", "result": "RSQ TEMP[0].x, TEMP[0].xxxx\nMIN TEMP[0].x, IMM[0].wwww, TEMP[0].xxxx\nWith IMM[0].wwww = FLT_MAX"}
```



Mapping formats

```jsonl
{"D3DFMT_A8R8G8B8": "PIPE_FORMAT_B8G8R8A8_UNORM"}
{"D3DFMT_D24S8": "PIPE_FORMAT_S8_UINT_Z24_UNORM"}
{"D3DFMT_D24X8": "PIPE_FORMAT_X8Z24_UNORM"}
{"D3DFMT_D16": "PIPE_FORMAT_Z16_UNORM"}
```

We map to the equivalent gallium format.



if something is handwritten put like ((handwriting1)) at the end of the line.

if half is filled by one handwriting and half by another put ((handwriting1)) at the first guys and ((handwriting2)) on second guys

Gallium Nine internals
D3D9 on Mesa Gallium Nine : the status

Conclusion

* State handling is easy
* Draw call mapping are easy
* Shader code to TGSI is easy
* Format conversion is easy

Great, but why are there still bugs ?

* fixed function code special undocumented behaviours
* How to handle cases supposed to be forbidden by the spec, but that apps do anyway ?
* Undocumented special behaviours
* Stateblocks are hard to implement right



Native D3D9 on Mesa Gallium Nine : the status
Gallium Nine internals

# CPU overhead

* Gallium Nine has low CPU overhead because the conversion from d3d9 call to gallium API is easy.
* With Gallium API, we can assume API call succeed. No need to check driver error. Checks are done by Gallium Nine before submission.
* State change: Could do better
    * What we do: put flags on which gallium state groups need being updated. Update them at draw call.
    * What we could do: update the state groups structure right away, and put flag to submit it at draw call.



Native D3D9 on Mesa Gallium Nine : the status
Performance

# Plan

1 Introduction
2 Wine integration
3 Presenting to the screen
    * D3D9 queue
    * multi-gpu
    * Misc
4 Gallium Nine internals
5 Performance
    * Test configuration 1
    * Test configuration 2
    * Conclusion
6 Plans for the future



Native D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 1

laptop
Intel(R) Core(TM) i5-3210M CPU @ 2.50GHz
Amd HD 7730M (Slowest GCN card !)

OS:
Win 7
Ubuntu 14.10
Arch Linux, Mesa Ixit git + llvm SI scheduler + dma copy enabling patch

This is a GPU limited scenario.
Under Win, the AMD card is maximum 2x better than the Intel card, but it is only reached for heavy games (Skyrim, etc)



D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 1

```jsonl
{"OS": "Win", "Intel card": "83", "Amd card": "85"}
{"OS": "Ubuntu Native", "Intel card": "55", "Amd card": "60"}
{"OS": "Arch nine with SI scheduler", "Intel card": "NA", "Amd card": "89"}
{"OS": "Arch nine without SI scheduler", "Intel card": "NA", "Amd card": "80"}
{"OS": "Arch wine with SI scheduler", "Intel card": "50", "Amd card": "63"}
{"OS": "Arch wine without SI scheduler", "Intel card": "50", "Amd card": "56"}
```

Frames per Second (fps) on Portal on the same scene with same settings (Mid)

Sorry, couldn't test more on this machine. But as additional info, Skyrim looks like 75% of win perf under Arch nine. (And more like 50% for Wine)



D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

Intel i5 3330
Amd HD 7790

OS:
opensuze factory, Mesa Ixit git

Note:
tests with WINEDEBUG=--all, cpu on performance governor
This is a more CPU limited scenario.



Direct3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "image_comparison", "content": [
  {
    "panel": "left",
    "description": "A screenshot of a 3D game environment featuring a brown, rocky terrain with circular stone structures. Overlaid on the screen are two performance graphs. The top graph shows a green line fluctuating mostly at the top of the scale (values ranging from 0 to 1.61) with a sharp dip in the center. The bottom graph shows multiple jagged lines of different colors (red, green, blue, yellow) fluctuating between 0 and 100. Below the graphs is a legend: a green square labeled 'Fps: 241', and five colored squares (green, red, blue, magenta, yellow) labeled 'cpu: 25', 'cpu0: 15', 'cpu1: 38', 'cpu2: 18', and 'cpu3: 31' respectively.",
    "caption": "Figure: Gallium Hud under nine. Harvest Massive Encounter"
  },
  {
    "panel": "right",
    "description": "A screenshot of the same 3D game environment as the left image. Overlaid are two performance graphs. The top graph shows a green line that stays high but has a much wider and deeper dip in the center compared to the first image. The bottom graph shows multiple jagged colored lines (red, green, blue, yellow) with higher peaks on the right side of the graph. Below the graphs is a legend: a green square labeled 'Fps: 72', and five colored squares (green, red, blue, magenta, yellow) labeled 'cpu: 28', 'cpu0: 31', 'cpu1: 16', 'cpu2: 5', and 'cpu3: 66' respectively.",
    "caption": "Figure: Gallium Hud under wine. Harvest Massive Encounter"
  }
]}



D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "x-y_plot", "title": "Frame Time/Performance Top Left", "y_axis": [0, 213, 427, 641, 855, 1070], "data": "Green line showing high stability around 800-1000, followed by a sharp drop and volatile spikes between 200 and 600, ending with a return to a lower stable baseline."}
{"type": "x-y_plot", "title": "Frame Time/Performance Top Right", "y_axis": [0, 367, 734, 1100, 1470, 1840], "data": "Green line showing high stability around 1400, followed by a sharp drop and highly volatile spikes between 300 and 1100, ending with a return to a lower stable baseline."}

{"type": "x-y_plot", "title": "CPU Usage Bottom Left", "y_axis": [0, 20, 40, 60, 80, 100], "series": {"cpu": {"color": "red", "value": 25}, "cpu0": {"color": "green", "value": 15}, "cpu1": {"color": "cyan", "value": 38}, "cpu2": {"color": "blue", "value": 18}, "cpu3": {"color": "magenta", "value": 31}}, "data": "Multiple overlapping lines jittering mostly between 20 and 40 on the y-axis."}
{"type": "x-y_plot", "title": "CPU Usage Bottom Right", "y_axis": [0, 20, 40, 60, 80, 100], "series": {"cpu": {"color": "red", "value": 46}, "cpu0": {"color": "green", "value": 36}, "cpu1": {"color": "cyan", "value": 35}, "cpu2": {"color": "blue", "value": 76}, "cpu3": {"color": "magenta", "value": 37}}, "data": "Multiple overlapping lines with significantly higher volatility and higher peaks, reaching up to 80 on the y-axis."}

Figure: Gallium Hud under nine. Harvest Massive Encounter
Figure: Gallium Hud under wine csmt. Harvest Massive Encounter



Native D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "x-y plot", "title": "Performance Graph Left Top", "y_axis": {"min": 0, "max": 887, "ticks": [0, 177, 354, 532, 709, 887]}, "series": [{"color": "green", "label": "fps", "value": 33, "trend": "spiking"}]}
{"type": "x-y plot", "title": "Performance Graph Left Bottom", "y_axis": {"min": 0, "max": 100, "ticks": [0, 20, 40, 60, 80, 100]}, "series": [{"color": "green", "label": "cpu", "value": 16}, {"color": "red", "label": "cpu0", "value": 15}, {"color": "cyan", "label": "cpu1", "value": 20}, {"color": "purple", "label": "cpu2", "value": 11}, {"color": "yellow", "label": "cpu3", "value": 13}]}

{"type": "x-y plot", "title": "Performance Graph Right Top", "y_axis": {"min": 0, "max": 1.33, "ticks": [0, 266, 532, 798, 1.06, 1.33]}, "series": [{"color": "green", "label": "fps", "value": 35, "trend": "spiking"}]}
{"type": "x-y plot", "title": "Performance Graph Right Bottom", "y_axis": {"min": 0, "max": 100, "ticks": [0, 20, 40, 60, 80, 100]}, "series": [{"color": "green", "label": "cpu", "value": 29}, {"color": "red", "label": "cpu0", "value": 32}, {"color": "cyan", "label": "cpu1", "value": 30}, {"color": "purple", "label": "cpu2", "value": 30}, {"color": "yellow", "label": "cpu3", "value": 24}]}

Figure: Gallium Hud under nine.
Kingdoms of Amalur Reckoning

Figure: Gallium Hud under wine.
Kingdoms of Amalur Reckoning



Native D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "x-y plot", "title": "Performance Graph 1", "y_axis": {"min": 0, "max": 887, "steps": [0, 177, 354, 532, 709, 887]}, "series": [{"color": "green", "label": "Fps", "value": 33, "data": "variable peak reaching 887"}]}
{"type": "x-y plot", "title": "CPU Usage Graph 1", "y_axis": {"min": 0, "max": 100, "steps": [0, 20, 40, 60, 80, 100]}, "series": [{"color": "green", "label": "cpu", "value": 16}, {"color": "red", "label": "cpu0", "value": 15}, {"color": "blue", "label": "cpu1", "value": 20}, {"color": "magenta", "label": "cpu2", "value": 11}, {"color": "yellow", "label": "cpu3", "value": 13}]}

{"type": "x-y plot", "title": "Performance Graph 2", "y_axis": {"min": 0, "max": 1.70, "steps": [0, 339, 678, 1.02, 1.36, 1.70]}, "series": [{"color": "green", "label": "Fps", "value": 37, "data": "variable peak reaching 1.70"}]}
{"type": "x-y plot", "title": "CPU Usage Graph 2", "y_axis": {"min": 0, "max": 100, "steps": [0, 20, 40, 60, 80, 100]}, "series": [{"color": "green", "label": "cpu", "value": 47}, {"color": "red", "label": "cpu0", "value": 62}, {"color": "blue", "label": "cpu1", "value": 49}, {"color": "magenta", "label": "cpu2", "value": 32}, {"color": "yellow", "label": "cpu3", "value": 48}]}

Figure: Gallium Hud under nine.
Kingdoms of Amalur Reckoning

Figure: Gallium Hud under wine
csmt.
Kingdoms of Amalur Reckoning



[Image Description: A side-by-side comparison of two screenshots from the game "Legend of Grimrock 2", each featuring an overlay of the Gallium HUD performance monitors. The background of both images shows a dark, forested environment with gnarled, leafless tree branches against a pale blue/grey sky.

The left image is labeled "Gallium Hud under nine". It contains two main graph areas. The top graph is a vertical bar chart showing frame times; a bright green line peaks sharply around the 1.31ms mark. A text label reads "fps: 56" next to a green square. The bottom graph is a multi-colored line chart (red, blue, cyan, magenta, yellow) showing CPU usage across four cores; the lines fluctuate mostly between 20 and 40 units on a scale of 0 to 100.

The right image is labeled "Gallium Hud under wine". It mirrors the layout of the left. The top graph's green line peaks higher, near the 2.10ms mark. The text label reads "fps: 25" next to a green square. The bottom graph shows significantly more erratic and higher CPU spikes, with the cyan line hitting the 100 mark.

Both images have the same legend in the bottom left of their respective graph areas:
Green square: cpu
Red square: cpu0
Blue square: cpu1
Cyan square: cpu2
Magenta square: cpu3
Yellow square: (unlabeled in legend but present in graph)]

D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "x-y-plot", "label": "Frame Times (Nine)", "y_axis": ["1.31", "1.05", "786", "524", "262", "0"], "data": {"fps": 56}}
{"type": "x-y-plot", "label": "CPU Usage (Nine)", "y_axis": ["100", "80", "60", "40", "20", "0"], "data": {"cpu": 24, "cpu0": 12, "cpu1": 25, "cpu2": 23, "cpu3": 33}}

{"type": "x-y-plot", "label": "Frame Times (Wine)", "y_axis": ["2.10", "1.68", "1.26", "842", "421", "0"], "data": {"fps": 25}}
{"type": "x-y-plot", "label": "CPU Usage (Wine)", "y_axis": ["100", "80", "60", "40", "20", "0"], "data": {"cpu": 27, "cpu0": 4, "cpu1": 100, "cpu2": 0, "cpu3": 3}}

Figure: Gallium Hud under nine.
Legend Of Grimrock 2

Figure: Gallium Hud under wine.
Legend Of Grimrock 2



D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

[
  {"type": "plot", "title": "FPS Graph Left", "y_axis": "FPS", "values": [0, 262, 524, 786, 1.05, 1.31], "data_points": "jagged line peaking at 1.31", "current_value": "fps: 56"},
  {"type": "plot", "title": "FPS Graph Right", "y_axis": "FPS", "values": [0, 403, 806, 1.21, 1.61, 2.02], "data_points": "jagged line peaking at 2.02", "current_value": "fps: 43"},
  {"type": "plot", "title": "CPU Graph Left", "y_axis": "Percentage", "values": [0, 20, 40, 60, 80, 100], "data_points": "multi-colored jagged lines (green, red, cyan, magenta, yellow) oscillating between 20 and 100", "current_values": {"cpu": 24, "cpu0": 12, "cpu1": 25, "cpu2": 23, "cpu3": 33}},
  {"type": "plot", "title": "CPU Graph Right", "y_axis": "Percentage", "values": [0, 20, 40, 60, 80, 100], "data_points": "multi-colored jagged lines (green, red, cyan, magenta, yellow) oscillating heavily between 40 and 100", "current_values": {"cpu": 51, "cpu0": 39, "cpu1": 25, "cpu2": 100, "cpu3": 38}}
]

Figure: Gallium Hud under nine.
Legend Of Grimrock 2

Figure: Gallium Hud under wine
csmt.
Legend Of Grimrock 2



Direct3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "x-y_plot", "title": "FPS Graph (Left)", "y_axis": {"min": 0, "max": 253, "steps": [0, 50, 101, 151, 202, 253]}, "series": [{"label": "FPS", "value": 120, "trend": "erratic_oscillating_between_50_and_200"}]}

{"type": "x-y_plot", "title": "FPS Graph (Right)", "y_axis": {"min": 0, "max": 271, "steps": [0, 54, 108, 162, 216, 271]}, "series": [{"label": "FPS", "value": 52, "trend": "stable_low_with_single_spike_to_150"}]}

{"type": "x-y_plot", "title": "Performance Metrics (Left)", "y_axis": {"min": 0, "max": 100, "steps": [0, 20, 40, 60, 80, 100]}, "series": [{"label": "cpu", "value": 26}, {"label": "cpu0", "value": 80}, {"label": "cpu1", "value": 11}, {"label": "cpu2", "value": 8}, {"label": "cpu3", "value": 10}]}

{"type": "x-y_plot", "title": "Performance Metrics (Right)", "y_axis": {"min": 0, "max": 100, "steps": [0, 20, 40, 60, 80, 100]}, "series": [{"label": "cpu", "value": 29}, {"label": "cpu0", "value": 98}, {"label": "cpu1", "value": 5}, {"label": "cpu2", "value": 7}, {"label": "cpu3", "value": 5}]}

[Image Description: A side-by-side comparison of two software windows showing technical performance overlays (HUDs) over a video game. The background of the game depicts a dimly lit, ornate interior room with classical architecture, including a large table and warm ambient lighting. The left window shows high, volatile FPS fluctuations and a multi-colored CPU usage graph with a significant red peak. The right window shows a much flatter, lower FPS line and a more stable CPU usage graph. Both windows have standard macOS-style window controls (red, yellow, green buttons) in the top left corner.]

Figure: Gallium Hud under nine. Poker Night 2
Figure: Gallium Hud under wine. Poker Night 2



Native D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

[
  {"type": "x-y plot", "title": "FPS Graph Left", "x_axis": "Time", "y_axis": "FPS", "y_range": [0, 253], "current_value": 120, "trend": "High volatility, spiking between 50 and 200"},
  {"type": "x-y plot", "title": "CPU Usage Graph Left", "x_axis": "Time", "y_axis": "Percentage", "y_range": [0, 100], "data": {"cpu": 26, "cpu0": 80, "cpu1": 11, "cpu2": 8, "cpu3": 10}, "trend": "CPU0 showing high spikes up to 80%"}
]

[
  {"type": "x-y plot", "title": "FPS Graph Right", "x_axis": "Time", "y_axis": "FPS", "y_range": [0, 195], "current_value": 101, "trend": "Moderate volatility, staying mostly between 78 and 156"},
  {"type": "x-y plot", "title": "CPU Usage Graph Right", "x_axis": "Time", "y_axis": "Percentage", "y_range": [0, 100], "data": {"cpu": 54, "cpu0": 98, "cpu1": 98, "cpu2": 12, "cpu3": 8}, "trend": "CPU0 and CPU1 showing extreme saturation near 100%"}
]

Figure: Gallium Hud under nine. Poker Night 2
Figure: Gallium Hud under wine csmt. Poker Night 2



D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "x-y plot", "title": "FPS Graph (Left)", "x_axis": "Time", "y_axis": "FPS", "y_range": [0, 687], "data_trend": "Baseline near 0 with a sharp, narrow spike reaching approximately 600", "current_value": "Fps: 40"}

{"type": "x-y plot", "title": "FPS Graph (Right)", "x_axis": "Time", "y_axis": "FPS", "y_range": [0, 560], "data_trend": "Gradual decline from 400 down to 0", "current_value": "Fps: 16"}

{"type": "x-y plot", "title": "CPU Usage Graph (Left)", "x_axis": "Time", "y_axis": "Percentage", "y_range": [0, 100], "series": {"cpu": "green", "cpu0": "red", "cpu1": "blue", "cpu2": "magenta", "cpu3": "yellow"}, "data_trend": "Erratic spikes across all cores, peaking between 40% and 80%", "current_values": {"cpu": 33, "cpu0": 39, "cpu1": 37, "cpu2": 23, "cpu3": 35}}

{"type": "x-y plot", "title": "CPU Usage Graph (Right)", "x_axis": "Time", "y_axis": "Percentage", "y_range": [0, 100], "series": {"cpu": "green", "cpu0": "red", "cpu1": "blue", "cpu2": "magenta", "cpu3": "yellow"}, "data_trend": "Erratic spikes, with the red line (cpu0) showing the highest peaks reaching nearly 90%", "current_values": {"cpu": 29, "cpu0": 19, "cpu1": 45, "cpu2": 36, "cpu3": 18}}

Figure: Gallium Hud under nine. Skyrim
Figure: Gallium Hud under wine. Skyrim



D3D9 on Mesa Gallium Nine : the status
Performance
Test configuration 2

{"type": "chart", "description": "Line graph showing frame time/performance", "x_axis": "time", "y_axis": "value", "y_scale": [0, 137, 274, 412, 549, 687], "data": "single green spike peaking near 600, baseline near 137"}
{"type": "metric", "label": "Fps", "value": 40}

{"type": "chart", "description": "Line graph showing frame time/performance", "x_axis": "time", "y_axis": "value", "y_scale": [0, 191, 383, 575, 767, 959], "data": "single green spike peaking near 800, baseline near 191"}
{"type": "metric", "label": "Fps", "value": 28}

{"type": "chart", "description": "Multi-line graph showing CPU core utilization", "x_axis": "time", "y_axis": "percentage", "y_scale": [0, 20, 40, 60, 80, 100], "data": {"cpu_total": "green", "cpu0": "red", "cpu1": "blue", "cpu2": "magenta", "cpu3": "cyan"}}
{"type": "metrics", "data": {"cpu": 33, "cpu0": 39, "cpu1": 37, "cpu2": 23, "cpu3": 35}}

{"type": "chart", "description": "Multi-line graph showing CPU core utilization", "x_axis": "time", "y_axis": "percentage", "y_scale": [0, 20, 40, 60, 80, 100], "data": {"cpu_total": "green", "cpu0": "red", "cpu1": "blue", "cpu2": "magenta", "cpu3": "cyan"}}
{"type": "metrics", "data": {"cpu": 56, "cpu0": 34, "cpu1": 76, "cpu2": 75, "cpu3": 38}}

[Image Description: Two side-by-side screenshots of a 3D game environment (Skyrim), depicting a forest scene with pine trees and a greyish-white sky. Overlaid on each screenshot is a semi-transparent black HUD containing performance telemetry. The left HUD contains a performance line graph (top) and a CPU utilization graph (bottom) with associated numerical values. The right HUD mirrors this layout but shows different data values and a higher Y-axis scale on the top graph. Both HUDs have a small window-style title bar at the top with a red close button and a grey minimize/maximize button.]

Figure: Gallium Hud under nine. Skyrim
Figure: Gallium Hud under wine csmt. Skyrim



D3D9 on Mesa Gallium Nine : the status
Performance
Conclusion

* When Nine works, it’s usually faster than Wine.
* Lower cpu usage



Native D3D9 on Mesa Gallium Nine : the status
Plans for the future

Plan

1 Introduction
2 Wine integration
3 Presenting to the screen
    * D3D9 queue
    * multi-gpu
    * Misc
4 Gallium Nine internals
5 Performance
    * Test configuration 1
    * Test configuration 2
    * Conclusion
6 Plans for the future



Native D3D9 on Mesa Gallium Nine : the status
Plans for the future

Wine vs Nine

Both Wine and Nine have bugs on some games (graphical bugs, games not launching, etc)
Currently Wine gets more games to work (but Nine manages to run games wine cannot run properly)

A fast, well-working Wine is better than everything else. But hard!
Better than working on d3d1x state trackers, it would be better help wine with GL extensions.
$\Rightarrow$ But in the next few years, we expect Gallium Nine to still beat Wine.



D3D9 on Mesa Gallium Nine : the status
Plans for the future

Merging Nine support into Wine

Currently Mesa >= 10.4 have Gallium Nine support. But it needs special code Wine side.

One needs to compile a special branch of Wine → not easy for users!

We have now PlayOnLinux support, and we could be integrated to wine staging in the near future.



D3D9 on Mesa Gallium Nine : the status
Plans for the future

This is the end...

Thanks for your attention.

Questions ?
