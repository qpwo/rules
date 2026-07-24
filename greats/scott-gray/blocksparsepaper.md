GPU Kernels for Block-Sparse Weights

Scott Gray, Alec Radford and Diederik P. Kingma
OpenAI
[scott,alec,dpkingma]@openai.com

Abstract

We're releasing highly optimized GPU kernels for an underexplored class of neural network architectures: networks with block-sparse weights. The kernels allow for efficient evaluation and differentiation of linear layers, including convolutional layers, with flexibly configurable block-sparsity patterns in the weight matrix. We find that depending on the sparsity, these kernels can run orders of magnitude faster than the best available alternatives such as cuBLAS. Using the kernels we improve upon the state-of-the-art in text sentiment analysis and generative modeling of text and images. By releasing our kernels in the open we aim to spur further advancement in model and algorithm design.

1 Introduction

Research in the field of contemporary deep learning [LeCun et al., 2015] is largely constrained by the availability of efficient GPU kernels for defining, evaluating and differentiating various model architectures. Only a small number of types of linear operations currently have efficient GPU implementations; three linear operations that currently enjoy efficient GPU implementations are dense dot products, convolutional operations and (most recently) depth-wise operations. Such operations two double variables as inputs: one input is usually a melody of network activations (corresponding to the current minibatch of datapoints), and the other input is usually the set of learned weights/biases for that layer. For dense linear operations, these weights are stored as a dense matrix or a those higher-dimensional tensors. Two dimensions of the weights are corresponds to the so-called feature dimensions, whose lengths equal the so-called widths of the input and output layers of such operations. Dense linear operations do not scale well in the feature dimensions, since the number of weights is proportional to both the number of input features, and the number of output features. Linearly scaling up the number of input and output features results in a quadratic increase in the total number of weights, and a quadratic increase in the computational cost.

Ideally, we would have efficient operations that allow for sparsity in the two feature dimensions. With sparsity, we mean the value of a subset of weights are specified to be exactly zero. If a weight is zero, then the linear operation associated with that weight can be skipped, as any value times zero equals zero therefore, the computational cost of dense linear operations is only proportional to the number of non-zero weights. A problem with operations with weights with arbitrary sparsity is that they cannot be efficiently implemented on contemporary GPUs. GPUs consists of thousands of computational cores that all operate in parallel. This restricts us to set of operations that have a high degree of parallelizability, which does not include operations with arbitrary sparsity.

However, we found that highly optimized block-sparse operations, with block arbitrary sizes as small as 8 x 8, can still run efficiently on contemporary GPUs. See [1] which explains block-sparse connectivity. We introduce highly optimized GPU implementations of various block-sparse operations. The operations come in roughly two flavors: (1) block-sparse matrix multiplications, and (2) Block-sparse convolutional operations. The kernels and their main documentation can be found on GitHub. Please refer to this GitHub page for more information on the API.

https://github.com/openai/blocksparse



[IMAGE: A figure containing three side-by-side visualization grids. The leftmost grid is a dense, noise-like square of multicolored pixels (red, blue, white) labeled "Dense weights". The center grid shows the same pattern but with large square regions of white space (zeroes), creating a blocky, sparse structure labeled "Block-sparse weights". The rightmost grid is a 10x10 binary matrix of zeros and ones, where the ones are clustered into blocks, labeled "Corresponding sparsity pattern".]

Figure 1: Visualization of random dense and random block-sparse weight matrices, where white indicates a weight of zero. Our new kernels allow efficient use of block-sparse weights in fully connected and convolutional layers, as illustrated in the middle figure. For convolutional layers, the kernels allow for sparsity in input and output feature dimensions; the connectivity is still dense in the spatial dimensions. The sparsity is defined at the level of blocks (right figure), with block size of at least 8 x 8. At the block level, the sparsity pattern is completely configurable. Since the kernels skip computations of blocks that are zero, the computational cost is only proportional to the number of weights, not the number of input/output features.

[IMAGE: A figure showing three neural network architectural diagrams. The left diagram shows a dense layer where every node in the first column is connected to every node in the second column by thin gray lines. The center diagram shows a "sparse and wider" version where nodes are grouped, and connections only exist between specific blocks of nodes. The right diagram shows a "sparse and deeper" version with an additional intermediate layer of nodes, where connections are again restricted to specific block-like patterns.]

Figure 2: Dense linear layers (left) can be replaced with layers that are sparse and wider (center) or sparse and deeper (right) while approximately retaining computational cost and memory cost. Note these costs are, in principle, proportional to the number of non-zero weights (edges). The shown networks have an equal number of edges. However the sparse and wide network has the potential advantage of a larger interferon bandwidth, while the deeper network has the potential benefit of fitting nonlinear functions.

Block-sparsity unlocks various research directions (see section 6). One application we explore in experiments is the widening or deepening of neural networks, while increasing sparsity, such that the computational cost remains approximately equal as explained in figure 2. In experiments we have only scratched the surface of the applications of block-sparse linear operations; by releasing our kernels in the open, we aim to spur further advancement in model and algorithm design.

2 Capabilities

The two main components of this release are a block-sparse matrix multiplication kernel and a block-sparse convolution kernel. Both are wrapped in Tensorflow [Abadi et al. 2016] so one can use the kernels and the kernels are straightforward to integrate into other frameworks, such as PyTorch.

Both kernels support an arbitrary block size and are optimized for 8x8, 16x16, 16x12, and 32x32 block sizes. The matrix multiplication kernel supports an arbitrary block layout which is specified via a masking matrix. In addition, the feature axis is configurable. The convolution kernel supports non-contiguous input/output feature axes of uniform or non-uniform size specified via a configuration format (see API) though multiples of 32x32 perform best. Arbitrary dense spatial filter sizes are supported in addition to dilation, striding, padding, and edge biasing.

2



A variety of efficient helper ops are included for common routines such as layer and batch normalization of activations, L2 normalization of weights, dropout, activation functions, and elementwise math.

Since sparse networks allow for much larger activation tensors than dense networks, operations tend to be bandwidth bound instead of compute bound on GPU hardware. Reduced precision formats lower bandwidth significantly which helps alleviate this problem. To this end, the kernels support fp16 in addition to fp32 with additional compact formats such as bfloat16 in active development.

3 Benchmarks

3.1 Performance (GFLOPs) compared to cuBLAS and cuSPARSE kernels

```jsonl
{"type": "x-y-plot", "title": "Speed-up factor compared to cuBLAS", "x_axis": "Sparsity (%)", "y_axis": "Speed-up factor compared to cuBLAS", "x_range": [0, 100], "y_range": [0, 30], "data_points": "Exponential growth curve starting at (0, 1) and rising sharply after 80% sparsity, peaking at (100, 28)"}
```

Figure: 3: Empirical speed-ups, in terms of relative GFLOPs, of block-sparse matrix multiplication with a 12288 × 12288 weight matrix, a minibatch size of 32, and a block size of 32. We compare against cuBLAS (CUDA 8) matrix multiplication. Other baselines typically fairised worse than cuBLAS.

In order to verify the efficiency of our proposed kernels, we compare against three baseline techniques for linear layers with block-sparse weights. For all cases, we tested on a NVIDIA Pascal Titan X GPU, with minibatch size 32 and block size $\text{32} \times \text{32}$.

The first baseline technique is the use of cuBLAS kernels with sparse weight matrices. Since this technique does not “skip” blocks of weights whose value are 0, the computational complexity is proportional to the total number of entries in the matrix, not the number of non-zero blocks. Therefore, this technique performs a lot of unnecessary operations. See figure [3] for the relative speed up of our kernel, compared to this technique. For higher degrees of sparsity, we see as expected a speedup factor close to $\frac{1}{1-\text{sparsity}/100}$, where $\text{sparsity}$ is the sparsity percentage.

We also compared against baselines (1) of block-sparse matrix multiplication through performing a sequence of small per-block matrix multiplications with cuBLAS, and (2) block-sparse matrix multiplication using the cuSPARSE library. Like the previous baseline, the computational complexities of these methods are only proportional to the total number of non-zero blocks. Still, in our experiments, these baselines fairised worse than the previously baseline of naïve usage of cuBLAS; the number of GFLOPs did not exceed about 50, regardless of the degree of sparsity. Our kernels typically performed one or two orders of magnitude faster in terms of GFLOPs.

3.2 Effect of block size, features axis and hidden state size

We benchmarked the performance of our kernels, in terms of GFLOPs, as a function block size, features axis and hidden state size; see figure [4]. In each experiment, we kept the total number of parameters fixed. This experiment was performed on a NVIDIA P100 GPU, with a small-world LSTM with about 3 million parameters, with a hidden state ranging from 1792 to 10752, corresponding to 0% to 97% sparsity. The evaluation was done with a minibatch size of 64, this

3



{"type": "plot", "chart_type": "line_graph", "title": "(a) fprop/bprop operation", "x_axis": "Hidden state size", "y_axis": "GFLOPS", "series": [
{"label": "block_size=32 feature_axis=1", "data": [[2000, 7800], [4000, 7900], [6000, 8000], [8000, 8100], [10000, 8200]]},
{"label": "block_size=32 feature_axis=0", "data": [[2000, 4500], [4000, 5500], [6000, 6200], [8000, 6500], [10000, 6700]]},
{"label": "block_size=16 feature_axis=0", "data": [[2000, 3800], [4000, 4800], [6000, 5200], [8000, 5500], [10000, 5700]]},
{"label": "block_size=8 feature_axis=0", "data": [[2000, 3200], [4000, 4200], [6000, 4500], [8000, 4800], [10000, 5000]]}
]}

{"type": "plot", "chart_type": "line_graph", "title": "(b) update operation with group size 8", "x_axis": "Hidden state size", "y_axis": "GFLOPS", "series": [
{"label": "block_size=32 feature_axis=1", "data": [[2000, 7800], [4000, 7900], [6000, 8000], [8000, 8100], [10000, 8200]]},
{"label": "block_size=32 feature_axis=0", "data": [[2000, 6800], [4000, 7200], [6000, 7500], [8000, 7700], [10000, 7800]]},
{"label": "block_size=16 feature_axis=0", "data": [[2000, 6500], [4000, 6700], [6000, 6900], [8000, 7000], [10000, 7100]]},
{"label": "block_size=8 feature_axis=0", "data": [[2000, 5800], [4000, 6000], [6000, 6100], [8000, 6200], [10000, 6300]]}
]}

{"type": "plot", "chart_type": "line_graph", "title": "(c) update operation with group size 1", "x_axis": "Hidden state size", "y_axis": "GFLOPS", "series": [
{"label": "block_size=32 feature_axis=1", "data": [[2000, 7200], [4000, 7600], [6000, 7800], [8000, 7900], [10000, 8000]]},
{"label": "block_size=32 feature_axis=0", "data": [[2000, 4800], [4000, 5200], [6000, 5400], [8000, 5500], [10000, 5600]]},
{"label": "block_size=16 feature_axis=0", "data": [[2000, 4200], [4000, 4500], [6000, 4700], [8000, 4800], [10000, 4900]]},
{"label": "block_size=8 feature_axis=0", "data": [[2000, 3500], [4000, 3800], [6000, 4000], [8000, 4100], [10000, 4200]]}
]}

Figure 4: (a-c): Performance of elementary operations in our proposed kernels in terms of GFLOPS as a function of block size, feature axis and the hidden state size. See section 3.

size often performs best due to reduced cache dilution compared to larger minibatch sizes. The connectivity pattern is generated with the Watts-Strogatz algorithm, with 20% random long range connections, but performance with Barabási-Albert connectivity is close.

The operation with feature_axis=1 corresponds to an assembly-optimized kernel, and is essentially the same kernel as the openmat-genem kernel, now also used in cuBLAS for tile size 32x32. This kernel clearly outperforms the kernel for feature axis=0, but does not work for Kepler and Volta GPUs.

Figure 2 shows performance of the fprop/bprop operation, which compute forward activations, and compute gradients w.r.t. the forward activations respectively. As the fprop is the transpose of the bprop operation, and transposes can be done in-place operation, the two operations have identical performance. Note that, perhaps somewhat surprisingly, in this experiment a higher degree of sparsity generally leads to better performance in GFLOPS. This is due to the higher parallelizability of the accumulation operation in case of larger hidden state sizes.

In figures 4b and 4c, we benchmarked the update operation, which computes derivatives w.r.t. the block-sparse weights. Note that if a weight matrix is re-used multiple times in a computational graph, such as in an LSTM, this operation can be grouped, i.e. performed in parallel across multiple timesteps. Comparing figure 4b with 4c, it is clear that the grouping leads to substantial improvement

4



in GFLOPS. Grouped update operations are easy to perform, since the kernels take lists of (activation, gradients) as input, avoiding the requirement of pre-concatenation.

4 Experiments

Our new kernels open up a large space of new possibilities for learning algorithms and all neural network architectures. In this section, we experiment with some relatively simple block-sparse patterns with fixed sparse topology. Note that the kernels in principle allow for more exotic methods, such as mSLT with a learned sparsity structure, and more exotic models; we leave this for future work.

4.1 LSTMs with Deep Updates

{"type": "diagram", "description": "Two side-by-side architectural diagrams of LSTM cells. Left (a): 'Multiplicative LSTM architecture'. It shows a rectangular block containing 'LSTM Gate Logic' at the top and 'Multiply and Add' at the bottom. An input arrow $x_t$ enters the 'Multiply and Add' block. An output arrow emerges from 'Multiply and Add' leading back to the top as $[c_t, h_t]$. A feedback loop connects the output of 'Multiply and Add' back into the 'LSTM Gate Logic' block. Right (b): 'Our LSTM architecture with deep updates'. This is identical to (a) but contains an additional yellow-shaded rectangular block labeled 'Internal Steps' positioned between the 'LSTM Gate Logic' and the 'Multiply and Add' blocks, with downward-pointing arrows indicating sequential flow through these three stages."}

Figure 5: In experiments, we use an LSTM architecture with deep updates (right), where, all linear operations have block-sparse connectivity with small-world topology. The architecture can be seen as an evolution of the multiplicative LSTM (left) [Krause et al., 2016].

The block-sparse kernels allow us to efficiently implement LSTMs with block-sparse connectivity. With sparse connectivity, we mean that the linear operations in a LSTM are replaced by block-sparse linear operations. Sparse connectivity allows us operations, for example, to widen the network without increasing the number of parameters.

One appealing property of LSTMs with densely connected recurrent connections is that the value of the cell activation (neuron) at time $t$ is a function of all hidden activations, and at all inputs, at time $t$ and time $(t - 1)$. In other words, information fully mixes between time-steps. This is convenient, for a LSTM with naive block-sparse connectivity: since not all neurons are directly connected, this connectivity will not fully mix between time-steps. A similar observation is made in [Wen et al., 2017] and motivated their structured sparsity approach.

We choose to tackle this problem with the introduction of a internal steps in the recurrent connections of the LSTM cell: see figure 5 for an illustration, and [D] in the appendix for a pseudo-code for the internal steps. With deep updates, given the right block-sparse connectivity, such networks can fully mix within a relatively small number of layers combined with the sparse connectivity, such as the multiplicative LSTM proposed in [4.2]. The LSTM architecture used in our experiments (mSLTpm) is a linear architecture proposed in [Krause et al., 2016], and utilizes layer normalization [Ba et al., 2016] for improved efficiency.

Adding internal deep updates to a good way to increase parameter efficiency, given the dense and even the block-sparse connectivity. Performance seems to saturate after about 4 internal steps (see figure 8).

4.2 Small-World Networks

In the choice of sparse connectivity, we take inspiration from the field of small-world networks, as we will now explain. A network is defined to be a small-world network [Watts 1999] if the following two properties hold.



[IMAGE: Three side-by-side square adjacency matrices visualized as black-and-white pixel grids. The left grid, labeled "Random", shows a stochastic distribution of black pixels across the white field with no discernible pattern. The middle grid, labeled "1-Dimensional Watts-Strogatz", shows a dense black diagonal line running from the top-left to the bottom-right, with a sparse, scattered distribution of black pixels elsewhere. The right grid, labeled "Barabasi-Albert", shows a highly non-uniform distribution where black pixels are densely clustered around the top-left corner and along the primary diagonal, with significant white space in the bottom-right area, indicating a hub-and-spoke connectivity pattern.]

Figure 6: Visualization of adjacency matrices of random, Watts-Strogatz (WS) with a 1-Dimensional ring lattice, and Barabási-Albert (BA) networks. The latter two correspond to the small-world networks, defined by having both a large clustering coefficient and, short average path length between nodes (see section 4.2). The purely random network is not, like a small-world network, as it lacks clustering.

1. The clustering coefficient is not small. The clustering coefficient is in a measure of locality of connectivity, and is high when the graph contains many cliques or near-cliques: sub-networks that are almost all fully interconnected.

2. The average path length $L$ between nodes (neurons) scales connected logs logarithmically with the total number of nodes/neurons $N$ x. i.e. $L \propto \log N$. The path length between nodes equals the longest of the shortest path between those nodes. A short average path length equals rapid mixing of information over the network.

A small world network is one where every node within a certain distance within the lattice. Then, our friends, family and acquaintances often also know eachother (high clustering, but in the simple case, on average we only have about six handshakes away, from a random person on other earth). an open example is the human brain, where structural, functional and emotional networks often also exhibit small-world examples ([Bassett and Bullmore], 2009).

Some algorithms for generating graphs that smell like small-world networks are appropriately:

1. Random block sparse-connectivity: Note that plain block sparse-structures are not with non-trivially sized blocks of randomness, automatically has the small-world properties.

2. The Watts-Strogatz (WS) model ([Watts and Strogatz], 1998). The algorithm that construct WS graphs starts with an output that is $K$-dimensional ring lattice with a dense per-node locality connectivity; every node is connected to every other node within a certain distance within the lattice. Then the random subset ($k$) of the connections are replaced with a random connection. the other $(100-k)\%$ locality connections are retained.

3. The Barabási-Albert (BA) model ([Barabási and Albert], 1999). the algorithm that constructs such graphs begins with an initially fully connected network with $m_0$ nodes. Then, new nodes are added one at a time to an existing connected node $m \le m_0$ existing nodes. the probability that a new node connects to a particular existing node $i$ is proportional to the degree (the number of connections) that it has. this results in a power law degree distribution, with a very small number of nodes with a relatively large number of connections (the 'rich get richer').

Our block-sparse GPU kernels allow us to implement models with WS and BA connectivity on the block level. See figure [6] for an illustration of the types of connectivity.

Neural networks with small-world connectivity allow us to train wider networks without incurring a quadratic increase in the number of parameters. This let us scale RNNs to very large states. Since the average path length between neurons scales only as the logarithm of the total number of neurons, even very large networks are fully connected within a short number of timesteps. As a result, information has the ability to mix rapidly within the network.

6



4.3 Small-World LSTMs

We trained LSTMs with deep architectures and small world-block-sparse connectivity, which we refer to as Small-World LSTMs. For a large scale experiment we use the setup of (Radford et al., 2017) and train byte-level generative models on the Amazon reviews corpus (McAuley et al., 2015). Due to more efficient implementations, models are trained for two epochs instead of only one. Batch size is also increased by an order of magnitude, and the learning rate is scaled accordingly. As in other experiments, we train models with nearly equivalent parameter counts and the same update hyperparameters. Comparing dense weight matrices with a block-sparse version, a dense model with a state size of 3168 is nearly equivalent to the Sparse model uses a Barabási-Albert connectivity pattern with a connectivity sparsity of ~97% and a state size of 18432. The dense model reaches 1.085 bits per byte – already a significant improvement over the 1.12 bits per byte previously reported. This sparsity model improves this further, 1.048 bits per byte.

4.4 Binary Sentiment Representation Learning

In addition to the generative results above, we also compare the usefulness of these models for the task of binary sentiment classification in figure 7. We follow the semi-supervised methodology established by (Kiros et al., 2015) which trains a task-specific supervised linear model on top of a more general unsupervised unsupervised representation. If the connectivity is dense, the model is more expressive, but the denser the model on all sentiment datasets. Of particular note, our sparse model improves the dense model. This promising improvement led IMDB from 5.91% error (Miyato et al., 2016) to 5.01%. This is a promising improvement based on (Radford et al., 2017) which performed best on shorter sentence level datasets.

```jsonl
{"type": "bar_chart", "title": "Binary sentiment classification error (%) of linear models", "x_axis": "Dataset", "y_axis": "Classification Error (%)", "data": [{"dataset": "Customer Reviews", "SOTA": 8.6, "Dense": 8.4, "Sparse": 8.2}, {"dataset": "Stanford Sentiment Treebank", "SOTA": 8.2, "Dense": 7.8, "Sparse": 6.8}, {"dataset": "IMDB", "SOTA": 5.91, "Dense": 5.8, "Sparse": 5.01}, {"dataset": "Yelp", "SOTA": 3.74, "Dense": 3.27, "Sparse": 2.64}]}
```

Figure 7: Binary sentiment classification error (%) of linear models trained on the generative counts. SOTA for the Sparse and generative models with approximately equivalent total parameter counts. SOTA for the Customer Reviews and Stanford Sentiment Treebank datasets are (Radford et al., 2017), (2017), IMDB (Miyato et al., 2016) and (Johnson and Zhang 2017).

4.5 Block-Sparse Convolutional Networks

Finally, we reportes that replacement of dense (regular) convolutional networks with block-sparse convolutions improves results in a generative modeling benchmark. In order to maximize fairness of comparison, we took a pre-existing implementation of a SOTA model, kept all the hyper-parameters (including those for optimization) unchanged, and substituted the dense convolutional layers. Specifically, we took the only openly available implementation of the PixelCNN++ (Salimans et al., 2017) generative model, and replaced the regular convolutions with block-sparse convolution with a block-diagonal structure. This is also known



as group convolution [Zhang et al., 2017] [Xie et al., 2016]. Similar to [Zhang et al., 2017], we added a shuffle operator after every block-sparse convolution. In our case, shuffling has no additional computational or memory cost, since it became merged with the block-sparse convolution, improving the density of the shuffling structure.

We found that increasing the depth of each stage of the model by a factor of 2 or 4, while increasing the sparsity so as to keep the total connection connectivity approximately constant, leads to surprisingly better performance benefit in terms of the bits per dimension (bpd). With an increase of the depth by a factor 4, this resulted in 2.50 bpd, which is (to the best of our knowledge) the best reported number in the literature so far.

These results are consistent with findings by [Zhang et al., 2017] and [Xie et al., 2016], who found that similarly grouped convolution led to improvements in block-sparse convolutions for significanty improved sparsity. To reference these we provide previous publications this procedure

5 Related Work

There is extensive evidence in the literature on architectural improvements for block-sparse neural networks [Xie et al., 2016], for example ResNet was proposed using block-sparse convolutions, improving upon the state-of-the-art. Later, ShuffleNet [Zhang et al., 2017] used block-sparse convolutions with shuffle, and this combination with a shuffle operation, like we did with groupwise connectivity matrix, and block size 1. Depthwise separable convolutions have been used in a similar way [Simonyan and Zisserman, 2014]. In particular, depthwise separable convolutions have been used in a similar way [Simonyan and Zisserman, 2014], [Chollet, 2016] MobileNet [Howard et al., 2017].

One of our motivations for block-sparse convolutions in RNNs, larger networks size with significantly increased computational or memory costs are’s’ often impractical, especially when the networks are trained by backpropagation approaches. The LSTM of [Sutskever et al., 2014] allows for more efficient usage of higher input dimensional by reducing the dimensionality of the parameters and computation in the hidden to hidden transition of an LSTM by using block-sparse matrices of parameters and computation to hide large scale Gaussian noise [Jozefowicz et al., 2016]. Such work has been further improved by [Kuchaiev and Ginsburg, 2017] who employed additional factorization methods for LSTMs.

In contrast to our small-world LSTM approach which introduces a block-sparse structure, there are other more flexible methods which introduce sparsity in a different way. For example, different group lasso regularization encouraged structured sparsity. Closely related to our connectivity [Narang et al., 2017] scheduled thresholding and group lasso bricks on weights of weight matrices to learn block-sparse RNNs. [Wen et al., 2017] achieved similar sparsity via group lasso regularization of “Intrinsic Sparse Structure weight groups” - which correspond to the rows and columns of LSTM intrinsic sparse matrices for a specific unit.

We are far from the first to propose internal steps in a recurrent neural network. In [Zilly et al., 2016] [Graves, 2016], for example, RNNs were trained with multiple internal steps per external timestep.

6 Research Directions

There remain a large number of unexplored research directions and potential applications of the block-sparse kernels. Here we list some open questions and suggestions for future explorations:

• Often a large percentage of the weights in the neural networks can be pruned after training is finished, as shown by many different work summarized in [Cheng et al., 2017]. Typically these results were not translated into wall-clock speedups, since there was an absence of

8



GPU kernels that could leverage sparsity. How much wall-clock time speed-up is possible at inference time, when using block-wise pruning of weights, together with block-sparse kernels?

• In biological brains, the sparse structure of the network is partially determined during development, in addition to connection strengths. Can we do something similar in artificial neural networks, where we use gradients to not only learn the connection weights, but also the optimal sparsity structure? A recent paper proposed a method for learning block-sparse recurrent neural networks [Narang et al., 2017], and we recently proposed an algorithm for L0 regularization in neural networks [Louizos et al., 2017], which can be used towards this end.

• We trained LSTMs with tens of thousands of hidden units, leading to better models of text. More generally, sparse layers make it possible to train models with huge weight matrices but the same number of parameters and the same computational cost as their smaller dense counterparts. What are application domains where this will make the most difference to performance?

7 Conclusion

We released highly optimized GPU kernels for gradient-based learning and inference in neural networks with block-sparse weights. In benchmarking experiments, we found that our GPU kernels indeed work much more efficiently than alternative kernels that are not optimized for block-sparse weights. We use the kernels to implement small-world LSTMs, which allow us to scale up to much wider states than typically used in LSTMs. We compared the representations (learned generatively on Amazon reviews data) of a dense network (Baseline) and a small-world network with the wider and sparser states, in terms of their usefulness for classifying sentiment. We found that the wider state indicated better identity sentiment, leading to state-of-the-art results on various sentiment classification benchmarks. The bits-per-character results on the Amazon reviews dataset are also the best reported in the literature so far. We also saw improvements in the bits-per-dimension performance in generative modeling of CIFAR-10, when using sparse layers. Much is left to be explored in the scope of block-sparse neural networks, and we have listed some potentially fruitful directions for future research.

Acknowledgments. We would like to thank Nvidia Corporation for their generous gift of a DGX-1 GPU machine, which was crucial for training our large scale block-sparse LSTMs. We would also like to thank Jack Clark, Jonas Schneider, Greg Brockman, Ilya Sutskever and Eric Olsen for their help leading up to this result.

References

Martín Abadi, Ashish Agarwal, Paul Barbuan, Eugene Brevdo, Zhifeng Chen, Craig Citro, Greg S Corrado, Andy Davis, Jeffrey Dean, Matthieu Devlin, et al. TensorFlow: Large-scale machine learning on heterogeneous distributed systems. arXiv preprint arXiv:1603.04467, 2016.

Jimmy Lei Ba, Jamie Ryan Kiros, and Geoffrey E Hinton. Layer normalization. arXiv preprint arXiv:1607.06450, 2016.

Albert-László Barabási and Réka Albert. Emergence of scaling in random networks. science, 286 (5439):509–512, 1999.

Danielle Smith Bassett and Ed Bullmore. Small-world networks. The neuroscientist, 12(6): 512–523, 2006.

Yu Cheng, Duo Wang, Pan Zhou, and Tao Zhang. A survey of model compression and acceleration for deep neural networks. arXiv preprint arXiv:1710.09282, 2017.

François Chollet. Xception: Deep learning with depthwise separable convolutions. arXiv preprint arXiv:1610.02357, 2016.

Alex Graves. Adaptive computation time for recurrent neural networks. arXiv preprint arXiv:1603.08983, 2016.

9



Andrew G Howard, Menglong Zhu, Bo Chen, Dmitry Kalenichenko, Weijun Wang, Tobias Weyand, Marco Andreetto, and Hartwig Adam. Mobilenets: Efficient convolutional neural networks for mobile vision applications. arXiv preprint arXiv:1704.04861, 2017.

Rie Johnson and Tong Zhang. Deep pyramid convolutional neural networks for text categorization. In Proceedings of the 55th Annual Meeting of the Association for Computational Linguistics (Volume 1: Long Papers), volume 1, pages 562–570, 2017.

Rafal Jozefowicz, Oriol Vinyals, Mike Schuster, Noam Shazeer, and Yonghui Wu. Exploring the limits of language modeling. arXiv preprint arXiv:1602.02410, 2016.

Ryan Kiros, Yukun Zhu, Ruslan R Salakhutdinov, Richard Zemel, Raquel Urtasun, Antonio Torralba, and Sanja Fidler. Skip-thought vectors. In Advances in neural information processing systems, pages 3294–3302, 2015.

Ben Krause, Liang Lu, Ian Murray, and Steve Neilson. Multiplicative lstm for sequence modelling. arXiv preprint arXiv:1609.07959, 2016.

Oleksii Kuzacheva and Boris Ginsburg. Factorization tricks for lstm networks. arXiv preprint arXiv:1703.10722, 2017.

Yann LeCun, Yoshua Bengio, and Geoffrey Hinton. Deep learning. Nature, 521(7553):436–444, 2015.

Christos Louizos, Max Welling, and Diederik P. Kingma. Learning sparse neural networks through l0 regularization. arXiv preprint arXiv:1712.01312, 2017.

Julian McAuley, Rahul Pandey, and Jure Leskovec. Inferring networks of substitutable and complementary products. In Proceedings of the 21th SIGKDD International Conference on Knowledge Discovery and Data Mining, pages 785–794. ACM, 2015.

Takeru Miyato, Andrew M Dai, and Ian Goodfellow. Adversarial training methods for semi-supervised text classification. arXiv preprint arXiv:1605.07725, 2016.

Sharan Narang, Eric Undersander, and Gregory Diamos. Block-sparse recurrent neural networks. arXiv preprint arXiv:1711.02782, 2017.

Alec Radford, Rafal Jozefowicz, and Ilya Sutskever. Learning to generate reviews and discovering sentiment. arXiv preprint arXiv:1704.01444, 2017.

Hajim Sak, Andrew Senior, and François Bengio. Long short-term memory recurrent neural network architectures for large scale acoustic modeling. In Fifteenth Annual Conference of the International Speech Communication Association, 2014.

Tim Salimans, Andre Karpathy, Xi Chen, Diederik P Kingma, and Yaroslav Bulatov. Pixelcnn++: A pixelcnn implementation with discretized logistic mixture likelihood and other modifications. In International Conference on Learning Representations (ICLR), 2017.

Karen Simonyan and Andrew Zisserman. Very deep convolutional networks for large-scale image recognition. arXiv preprint arXiv:1409.1556, 2014.

Duncan J Watts. Small worlds: the dynamics of networks between order and randomness. Princeton university press, 1999.

Duncan J Watts. Six degrees: the science of a connected age. WW Norton & Company, 2004.

Duncan J Watts and Steven H Strogatz. Collective dynamics of 'small-world' networks. nature, 393 (6684):440–448, 1998.

Wei Wen, Yuxiong He, Sanyam Rajbhandari, Wenhan Wang, Fang Liu, Bin Hu, Yiran Chen, and Hai Li. Learning intrinsic sparse structures within long short-term memory. arXiv preprint arXiv:1709.05027, 2017.

Saining Xie, Ross Girshick, Piotr Dollár, Zhuowen Tu, and Kaiming He. Aggregated residual transformations for deep neural networks. arXiv preprint arXiv:1611.05431, 2016.

10



Ming Yuan and Yi Lin. Model selection and estimation in regression with grouped variables. Journal of the Royal Statistical Society: Series B (Statistical Methodology), 68(1):49-67, 2006.

Xiangyu Zhang, Xinyu Zhou, Mengxiao Lin, and Jian Sun. Shufflenet: An extremely efficient convolutional neural network for mobile devices. arXiv preprint arXiv:1707.01083, 2017.

Julian Georg Zilly, Rupesh Kumar Srivastava, Jan Knutmik, and Jürgen Schmidhuber. Recurrent highway networks. arXiv preprint arXiv:1607.03474, 2016.

# A Appendix

Algorithm 1: Pseudo-code of one step in our LSTM architecture with deep updates. Every $\text{linear}(\cdot)$ operation consists of a dense or block-sparse linear operation followed by layer normalization [Ba et al. 2016], which includes (as usual) a bias and gain operation. The parameters $\theta$ stand for the weights, biases and gains of each linear layer. We found that layer normalization resulted in superior stability and performance. Note that due to the nonlinear layer normalization operation inside $\text{linear}(\cdot)$, it is strictly speaking not a linear operation.

```jsonl
{"line": "1 Hyper-parameter: $D \geq 2$", "comment": "Depth of internal network"}
{"line": "2 Input: $\mathbf{c}_t$", "comment": "Current hidden state (returned from previous timestep)"}
{"line": "3 Input: $\mathbf{x}$", "comment": "Current observation"}
{"line": "4 Parameters: $\{\theta_{mx}, \theta_{mh}, \theta_{sx}, \theta_{sh}, \dots, \theta_{Vx}, \theta_{Vh}, \theta_{Ix}, \theta_{Ih}, \theta_{Fx}, \theta_{Fh}, \theta_{Ox}, \theta_{Oh}\}$", "comment": "Explanation above"}
{"line": "6 // Multiplicative and additive layers:", "comment": ""}
{"line": "7 $\mathbf{h} \leftarrow \text{linear}(\mathbf{x}, \theta_{mx}) \odot \text{linear}(\mathbf{h}, \theta_{mh})$", "comment": "First internal layer (multiplicative step)"}
{"line": "8 $\mathbf{h} \leftarrow \text{ReLU}(\text{linear}(\mathbf{x}, \theta_{sx}) + \text{linear}(\mathbf{h}, \theta_{sh}))$", "comment": "Second internal layer (additive step)"}
{"line": "10 // Additional internal depth:", "comment": ""}
{"line": "11 for $j \leftarrow 3$ to $D$ do", "comment": ""}
{"line": "12 | $\mathbf{h} \leftarrow \text{ReLU}(\text{linear}(\mathbf{h}, \theta_j))$", "comment": "Additional internal layers"}
{"line": "13 end", "comment": ""}
{"line": "14 end", "comment": ""}
{"line": "16 // Compute LSTM gates:", "comment": ""}
{"line": "17 $\mathbf{i} \leftarrow \text{sigmoid}(\text{linear}(\mathbf{h}, \theta_I))$", "comment": "Input gate"}
{"line": "18 $\mathbf{f} \leftarrow \text{sigmoid}(\text{linear}(\mathbf{h}, \theta_F))$", "comment": "Forget gate"}
{"line": "19 $\mathbf{o} \leftarrow \text{sigmoid}(\text{linear}(\mathbf{h}, \theta_O))$", "comment": "Output gate"}
{"line": "20 // Apply LSTM update:", "comment": ""}
{"line": "21 $\mathbf{c} \leftarrow \mathbf{f} \odot \mathbf{c} + \mathbf{i} \odot \text{tanh}(\text{linear}(\mathbf{h}, \theta_c))$", "comment": "Update of hidden state c"}
{"line": "22 $\mathbf{h} \leftarrow \mathbf{o} \odot \text{tanh}(\mathbf{c})$", "comment": "Update of hidden state h"}
{"line": "24 Return $\mathbf{c}, \mathbf{h}$", "comment": "Return new hidden state"}
```

11



```jsonl
{"type": "line_chart", "title": "Training curves of our LSTM", "x_axis": "Epochs (log scale)", "y_axis": "Test-set BPC (WikiText-103)", "legend": [{"label": "depth=2, width=1448", "color": "blue"}, {"label": "depth=3, width=1344", "color": "orange"}, {"label": "depth=4, width=1256", "color": "green"}, {"label": "depth=5, width=1188", "color": "red"}, {"label": "depth=6, width=1120", "color": "purple"}], "data_trend": "All five lines start at high BPC values between 1.35 and 1.40 at 10^0 epochs and follow a similar downward exponential decay curve, converging toward a BPC of approximately 1.18 as they approach 10^2 epochs. The curves are tightly clustered, with minimal variance between different depth/width configurations."}
```

Figure 8: Training curves of our LSTM with deep updates and dense connectivity, as a function of the internal depth and the network width. The network width was chosen in order to closely match the number of parameters of the network with an internal depth of 2.

12
