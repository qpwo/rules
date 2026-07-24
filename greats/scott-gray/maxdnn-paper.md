maxDNN: An Efficient Convolution Kernel for Deep Learning with Maxwell GPUs

Andrew Lavin alavin@acm.org

eBay Research Labs Machine Learning

February 3, 2015

Abstract

This paper describes maxDNN [1] a computationally efficient convolution kernel for deep learning with the NVIDIA Maxwell GPU. maxDNN reaches 96.35% computational efficiency on typical deep learning network architectures. The design combines ideas from cuda-convnet2 with the Maxas SGEMM assembly code. We only address forward propagation (FPROP) operation of the network, but we believe that the same techniques used here will be effective for backward propagation (BPROP) as well.

1 Introduction

The central algorithm of convolutional neural networks is the 2D convolution of a bank of multi-channel filters against a mismatch of multi-channel 2D maps [2]. Modern GPUs have demonstrated the ability to compute convolutions significantly faster than CPUs [3], cuda-convnet2 and cuDNN are singing implementations of spatial domain convolution [2]. ftcuDNN is a GPU implementation of frequency domain convolution maps [7].

When comparing convolution kernels, it is customary to report execution time or throughput. The problem with these measurements is that they conflate the separate issue of algorithm complexity, computational efficiency, and device peak throughput.

Computational efficiency is an interesting problem to solve, because it is in practice very difficult to write highly efficient GPU kernels. Anecdotal evidence suggests that for many real world problems such as SGEMM and convolution, it is not possible to create a GPU kernel with greater than 80% computational efficiency using the CUDA Toolkit. The fact that the cuBLAS SGEMM kernel reaches 91% computational efficiency on NVIDIA Maxwell GPUs using assembly optimization suggests that efficient kernels are possible on Maxwell, but that the CUDA Toolkit does not provide the necessary tools to create them.

1The maxDNN source code can be downloaded from https://github.com/ebay/maxDNN



The Maxas project is an open source assembler for NVIDIA Maxwell GPUs, created by Scott Gray [3]. It includes as a programming example an assembly implementation of SGEMM that reaches 96% computational efficiency.

We created maxDNN to demonstrate that the same techniques used by Maxas for generating efficient SGEMM machine code are also effective for convolution.

2 Convolution

The convolution used in deep learning takes a minibatch of $N_b$ 2D multi-channel maps of size $W_i \times H_i \times N_{ci}$ and a bank of $N_f$ 2D multi-channel filters of size $S_k \times S_k \times N_{ci}$. Convolution of $N_b$ maps against $N_f$ filters is performed separately in corresponding channels and the result is summed over all channels. A stride, possibly equal to 1, is chosen that yields the desired output map size, $W_o \times H_o \times N_f$. The origin of the convolution is offset by an optional padding parameter, and the image is assumed to be wrapped in an apron of zeros to handle boundary spills. We also scale the results by a scalar, $\alpha$.

The algorithmic complexity of convolution is:

{"type": "formula", "content": "C(N_b, W_{co}, H_{co}, N_{ci}, N_{co}, S_k) = 2N_bW_{co}H_{co}(N_{ci}S_k^2 + 1)N_{co} FLOPs"}

where a single multiply-accumulate operation counts as 2 FLOPs.

3 cuda-convnet2

cuda-convnet2 [4] is perhaps the most efficient convolution across a wide variety of popular network shapes [2]. It uses an interesting strategy for implementing direct convolution: the map and filter data is ordered in memory so that the inner dimension is the number of batches or filters. The outer dimensions are the width, height, and channels of the maps.

The calculation of $N_{co}$ by $N_{ci}$ outputs for a single output map coordinate is just a matrix multiply between an unrolled input map patch, of size $N_b \times S_k^2 \times N_{ci}$, and the filter matrix, of size $S_k^2 N_{ci} \times N_{co}$.

Each of the $N_{co}$ columns of the filter matrix contains the $S_k^2 N_{ci}$ weights for a single filter.

The $S_k^2 N_{ci}$ columns of the input matrix must be gathered from non-contiguous segments of the input map. Column offset calculations used in the general operation require extra integer instructions compared with the code used in general matrix multiply, which operates on contiguous blocks of matrix data.

cuda-convnet2 also employs effective GPU programming techniques, including:

* Textures to load global memory, reducing indexing arithmetic.
* Loads and processes a tile of 8 columns of data per iteration, amortizing global load latency.

It is worth noting that cuda-convnet2 targeted the Kepler GPU, which has half (48K) the shared memory of the Maxwell GPU (96K). Extra shared memory makes possible additional latency hiding strategies.

2



3.1 Maxas: The Maxwell Assembler

Maxas is an open source assembler for the NVIDIA Maxwell architecture, created by Scott Gray. It gives the developer complete control over the scheduling of instructions and allocation of registers. The project includes an SGEMM implementation that reaches 96% computational efficiency on Maxwell GM204 GPUs.

The project is interesting not just for the assembler itself, but also for the SGEMM implementation and accompanying documentation, which is the best sample program published so far for creating high efficiency kernels. Several advanced techniques are used, including:

* Use of 128 bit texture load instructions to reduce number of global loads, to increase size of maximum array to 4GB (due to max texture index 2^25), and to reduce indexing calculations
* Double buffering of global memory loads to hide global memory latency.
* Double buffering of shared memory loads to hide shared memory latency and reduce number of warp synchronizations per iteration
* Coalesced storing to global memory by reorganizing output values through shared memory.
* Zero initialization of registers using 128 bit shared memory loads.

The inner loop of Maxas SGEMM64 is 98.8% floating point point instructions (texture and shared memory load instructions are dual issued with arithmetic instructions and therefore are not counted). Occupancy is just 25%, limited by the use of 127 registers per warp, so the high computational efficiency demonstrates the latency hiding power of instruction level parallelism.

The project contains SGEMM variants that perform 64 × 64 and 128 × 128 shared memory blocking. Both use 8 × 8 register blocking and load 8 columns of data per iteration.

3.2 maxDNN

The strategy behind our maxDNN kernel combines the style of cuda-convnet2 convolution with the matrix multiply assembly code of Maxas SGEMM. Maxas SGEMM64 was modified so that each block traverses a patch of the input map to compute a 64 × 64 filter-image block for a single output map coordinate. The z-coordinate of the block index is used to enumerate the filter-image blocks. Basically this just required adjusting the indexing calculations in the existing SGEMM64 code. To reduce the number of indexing calculations required to traverse the input map patch, we lifted the calculation of pixel/channel offset locations into constant memory. The pixel offset is just added to the patch offset to compute the input map offset. This replaces the 3 nested loops over channels, rows, and columns of a patch with a single loop over all the precomputed offset locations. The result is an inner loop that is 98.3% floating point point instructions.

We also physically zero padded the input map to handle boundary overruns. We believe this restriction could be removed with a modest decrease in the percentage of floating point instructions.

3



3.3 Experiments

We compare the performance of maxDNN to cuDNN v.2 RC1 on a GEFORCE GTX980 graphries card which uses the NVIDIA Maxwell GM204 GPU. cudaconvnet2 was not used because it has not been optimized for the Maxwell architecture.

cuDNN v.2 RC2 was also available, but showed significantly worse performance, so we reverted to RC1.

We measure performance using computational efficiency, which is the ratio of the actual throughput of the program to the peak throughput of the device. The GM204 consists of 16 processors each with 128 cores. Each core is capable of executing 1 multiply-accumulate per clock cycle. So one can calculate the device peak throughput by

Peak Throughput = 2FLOPs · 128 · 16 · GPU Clock Speed

The factor of 2 is due to the custom of counting a single multiply accumulate operation as 2 FLOPs.

Another way to measure computational efficiency is to divide the number of executed floating point instructions by 128, and then divide again by the number of processor clock cycles:

{"formula": "CE = (1 fp instructions) / (128 processor clocks)"}

This appears to be the formula used by the flop_zp efficiency metric in the nvprof profiling command in the CUDA Toolkit. It has the advantage of being independent of the clock speed, which can vary during kernel execution.

Using the above measure of efficiency, a kernel can get credit for unnecessary work by performing more floating point instructions than are strictly necessary. We see this arise as a modest effect in maxDNN when the filter size ($Sk^2N_c$) is not a multiple of the tile size (8). It has a more pronounced effect when the number of filters or mini-batch size is not a multiple of shared memory blocking size (64 x 64).

Therefore we modify the computational efficiency function to only give credit for the number of FLOPs actually required by the direct convolution algorithm:

{"formula": "CE = (C(N_b, W_o, H_o, N_c, S_k, N_f, S_k)) / (2 · 128 processor clocks)"}

We report efficiency for two recent Imagenet contest winners, Alexnet (v.2) and Overfeat. The minibatch size for both networks is 128.

3.4 Results

Table 1 compares the computational efficiency of cuDNN and maxDNN for FPROP convolution on the layers of Alexnet and Overfeat.

maxDNN efficiency for Alexnet v.2 ranges between 93.4% and 95.5%. The worst performance is on the input layer, where a patch only has 11 x 11 x 3 elements. This reduces the size of the main loop, where almost all of the FLOPs are performed, compared with the initialization and storage code sections, which can be thought of as fixed overhead.

4



{"table": [{"Network": "Alexnet v.2", "Layer": "conv1", "Input": "224x224x3", "Output": "55x55x64", "Kernel": "11x11", "cuDNN Efficiency": "32.5%", "maxDNN Efficiency": "93.4%"}, {"Network": "Alexnet v.2", "Layer": "conv2", "Input": "27x27x64", "Output": "27x27x192", "Kernel": "5x5", "cuDNN Efficiency": "46.7%", "maxDNN Efficiency": "95.5%"}, {"Network": "Alexnet v.2", "Layer": "conv3", "Input": "13x13x192", "Output": "13x13x384", "Kernel": "3x3", "cuDNN Efficiency": "51.5%", "maxDNN Efficiency": "95.1%"}, {"Network": "Alexnet v.2", "Layer": "conv4", "Input": "13x13x384", "Output": "13x13x256", "Kernel": "3x3", "cuDNN Efficiency": "57.0%", "maxDNN Efficiency": "95.0%"}, {"Network": "Alexnet v.2", "Layer": "conv5", "Input": "13x13x256", "Output": "13x13x256", "Kernel": "3x3", "cuDNN Efficiency": "55.9%", "maxDNN Efficiency": "94.6%"}, {"Network": "Overfeat", "Layer": "L1", "Input": "231x231x3", "Output": "56x56x96", "Kernel": "11x11", "cuDNN Efficiency": "39.6%", "maxDNN Efficiency": "70.3%"}, {"Network": "Overfeat", "Layer": "L2", "Input": "24x24x96", "Output": "20x20x256", "Kernel": "5x5", "cuDNN Efficiency": "74.0%", "maxDNN Efficiency": "95.6%"}, {"Network": "Overfeat", "Layer": "L3", "Input": "12x12x256", "Output": "12x12x512", "Kernel": "3x3", "cuDNN Efficiency": "54.2%", "maxDNN Efficiency": "94.4%"}, {"Network": "Overfeat", "Layer": "L4", "Input": "12x12x512", "Output": "12x12x1024", "Kernel": "3x3", "cuDNN Efficiency": "62.9%", "maxDNN Efficiency": "96.2%"}, {"Network": "Overfeat", "Layer": "L5", "Input": "12x12x1024", "Output": "12x12x1024", "Kernel": "3x3", "cuDNN Efficiency": "63.2%", "maxDNN Efficiency": "96.3%"}]}

Table 1: maxDNN convolution has consistently high efficiency for a variety of convolution shapes, providing the number of filters and minibatch size are both multiples of 64. cuDNN efficiency varies a lot between layers. These results are for FPROP operation with minibatch size 128.

maxDNN efficiency for Overfeat reaches 96.3%, and is over 94.4% for all layers but the first, which scores just 70.3%. This is due to the fact that the number of filters in this layer, 96, is not a multiple of the block size, 64 x 64.

We believe this could be addressed with a kernel that uses a block size of 64 x 32. This would reduce the computational intensity with respect to global memory loads, but the high L2 cache hit rate of the kernel suggests there is a surplus of device memory bandwidth. Additional block sizes could be developed to accommodate small minibatch sizes. At a certain block size the computational intensity would be too low and the kernel would become the device memory bandwidth limited, experiments are required to determine this threshold.

cuDNN efficiency on Alexnet v.2 varies between 32.5% and 57.6%. Not only are these numbers significantly lower, but the variance is much higher. For Overfeat the cuDNN efficiency varies from 39.6% to 74.0%. The first layer in each network is the least efficient, apparently due to a larger number of integer instructions used in indexing calculations.

One of the stated design goals of cuDNN was to achieve consistently high efficiency on a variety of convolution shapes using a single kernel [1]. Although cuDNN reports flexibility with respect to minibatch size [1], we can see that in practice the performance varies a lot with respect to layer parameters.

3.5 Conclusion

We developed an efficient convolution kernel for Maxwell GPUs using the Maxas assembler, Maxas SGEMM64 source code, and the cuda-convnet2 approach to convolution. We believe the same approach could be applied to the BPROP operation of convolutional neural networks.

The efficiency of maxDNN convolution rivals that of the best SGEMM implementations. Therefore maxDNN represents an existence proof that high efficiency GPU convolution is possible.

5



[Image Description: Two vertically stacked bar charts.
Top chart title: "Computational Efficiency for Alexnet v.2".
Bottom chart title: "Computational Efficiency for Overfeat".

Both charts share the same axis layout:
- Y-axis: "Computational Efficiency" ranging from 0 to 100 in increments of 10.
- X-axis: "Network Layer".
- Legend: Dark blue bars represent "maxDNN", light green bars represent "cuDNN".

Top Chart Data (Approximate):
- conv1: maxDNN ~92, cuDNN ~34
- conv2: maxDNN ~93, cuDNN ~46
- conv3: maxDNN ~94, cuDNN ~52
- conv4: maxDNN ~93, cuDNN ~58
- conv5: maxDNN ~93, cuDNN ~56

Bottom Chart Data (Approximate):
- L1: maxDNN ~71, cuDNN ~41
- L2: maxDNN ~96, cuDNN ~74
- L3: maxDNN ~97, cuDNN ~56
- L4: maxDNN ~97, cuDNN ~65
- L5: maxDNN ~95, cuDNN ~66]

Figure 1: FPROP convolution with minibatch size 128 for Alexnet v.2.

Figure 2: FPROP convolution with minibatch size 128 for Overfeat. maxDNN efficiency suffers when the number of filters is not a multiple of 64, but is otherwise consistently high. maxDNN variants with other shared memory blocking sizes would likely address this shortcoming.

6



Acknowledgement

The author would like to thank eBay Research Labs Machine Learning Director Dennis DeCoste for his guidance during the course of this project.

References

[1] Sharan Chetlur, Cliff Wooley, Philippe Vandermersch, Jonathan Cohen, John Tran, Bryan Catanzaro, and Evan Shellman. cummer, efficient primi- ties. CoRR, abs/1410.0759, 2014.

[2] Soumith Chintala. convnet-benchmarks.
https://github.com/soumith/convnet-benchmarks, 2015. [Online; accessed 23-January-2015].

[3] Soumith Chintala. Maxas: NervanaSystems/maxas, 2014. [Online; architecture
https://github.com/NervanaSystems/maxas, accessed 23-January-2015].

[4] Alex Krizhevsky. cuda-convnet2. https://code.google.com/p/cuda-convnet2/, 2014. [Online; accessed 23-January-2015].

[5] Alex Krizhevsky, Ilya Sutskever, and Geoffrey E Hinton. Imagenet clas- sification with deep convolutional neural networks. In Advances in neural information processing systems, pages 1097–1105, 2012.

[6] Yann LeCun, Fa Jie Huang, and Leon Botton. Learning methods for generic object recognition with invariance to pose and lighting. In Computer Vision and Pattern Recognition, 2004, CVPR 2004, Proceedings of the 2004 IEEE Computer Society Conference on, volume 2, pages II–97. IEEE, 2004.

[7] Nicolas Vasilache, Jeff Johnson, Michaël Mathieu, Soumith Chintala, Serkan Piantino, and Yann LeCun. Fast convolutional nets with ffbtn: A GPU performance evaluation. CoRR, abs/1412.7580, 2014.

7
