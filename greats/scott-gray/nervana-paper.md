Fast Algorithms for Convolutional Neural Networks

Andrew Lavin
alavin@acm.org

Scott Gray
Nervana Systems
sgray@nervanasys.com

Abstract

Deep convolutional neural networks take GPU days of compute time to train on large data sets. Pedestrian detection for self driving cars requires very low latency. Image recognition for mobile phones is constrained by limited processing resources. The success of convolutional neural networks in these situations is limited by how fast we can compute them. Conventional FFT based convolution is fast for large filters, but state of the art convolutional neural networks use small, $3 \times 3$ filters. We introduce a new class of fast algorithms for convolutional neural networks using Winograd's minimal filtering algorithms. The algorithms compute minimal complexity convolution over small tiles, which makes them fast with small filters and small batch sizes. We benchmark a GPU implementation of our algorithm with the VGG network and show state of the art throughput at batch sizes from 1 to 64.

1. Introduction

Deep convolutional neural networks (convnets) achieve state of the art results on image recognition problems [11, 7]. The networks take several days of GPU time to train and require significant compute resources during classification. Overall, deeper data sets and models lead to better accuracy but also increase computation time. Therefore progress in deep neural networks is limited by how fast the networks can be computed.

Likewise the application of convnets to low latency inference problems, such as pedestrian detection in self driving car video imagery, is limited by how fast a small set of images, possibly a single image, can be classified.

Distributed training of convnets can be achieved by partitioning each batch of examples across nodes of a cluster and accumulating weight updates across the nodes. Large batch sizes adversely affect convergence of the network, so the minimum batch size that can be computed efficiently places an upper limit on cluster size [8, 6].

State of the art convnet architectures for image recognition use deep networks of $3 \times 3$ convolutional layers, because they achieve better accuracy with fewer weights than shallow networks with larger filters [11, 7].

Therefore there is a strong need for fast convnet algorithms for small batch sizes and small filters. However conventional convnet libraries require large batch sizes and large filters for fast operation.

This paper introduces a new class of fast algorithms for convolutional neural networks based on the minimal filtering algorithms pioneered by Winograd [13]. The algorithms can reduce the arithmetic complexity of a convnet layer by up to a factor of 4 compared to direct convolution. Almost all of the arithmetic is performed by dense matrix multiplications of sufficient precision to be computed efficiently, even when the batch size is very small. The memory requirements are also light compared to the conventional FFT convolution algorithms. These factors make practical implementations possible. Our implementation for NVIDIA Maxwell GPUs achieves state of the art throughput for all batch sizes measured, from 1 to 64, while using at most 16MB of workspace memory.

2. Related Work

The FFT and convolution theorem have been used to reduce the arithmetic complexity of convnet layers, first by Mattson et al. [10], then refined by Vilasisa et al. [12], and then implemented in the NVIDIA cuDNN library [1].

The Strassen algorithm for fast matrix multiplication was used by Caya and Xiao [13] to reduce the number of convolutions in a convnet layer, thereby reducing its total arithmetic complexity. The authors also suggested that more techniques from arithmetic complexity theory might be applicable to convnets.

Various approaches have been attempted to reduce the complexity of convnets by quantizing or otherwise approximating the convolutional layer. We consider these approaches as orthogonal and complementary to those that exploit algebraic structure, and therefore declare them outside the scope of this paper.

1



3. Convolutional Neural Networks

A convnet layer correlates a bank of $K$ filters with $C$ channels and size $R \times S$ against a minibatch of $N$ images with $C$ channels and size $H \times W$. We denote filter elements as $G_{k,c,u,v}$ and image elements as $D_{i,c,x,y}$.
The computation of a single convnet layer output $Y_{i,k,x,y}$ is given by the formula:

$Y_{i,k,x,y} = \sum_{c=1}^{C} \sum_{u=1}^{R} \sum_{v=1}^{S} D_{i,c,x+u,y+v} G_{k,c,u,v}$ (1)

and we can write the output of an entire image/filter pair as

$Y_{i,k} = \sum_{c=1}^{C} D_{i,c} * G_{k,c}$ (2)

where $*$ denotes 2D correlation.

4. Fast Algorithms

It has been known since 1980 that the minimal number of filtering operations for computing $m$ outputs with an $r$-tap FIR filter, which we call $F(m,r)$, requires

$\mu(F(m,r)) = m + r - 1$ (3)

multiplications [13, p. 39]. Also, we can nest minimal 1D algorithms $F(m,r)$ and $F(n,s)$ to form minimal 2D algorithms for computing $m \times n$ outputs with an $r \times s$ filter, which we call $F(m \times n, r \times s)$. These require

$\mu(F(m \times n, r \times s)) = \mu(F(m,r))\mu(F(n,s))$
$= (m + r - 1)(n + s - 1)$ (4)

multiplications [14]. We can continue to nest 1D algorithms to form algorithms for multi-dimensional FIR filters.
It is interesting to note that in 1D, 2D, and multi-dimensions, the minimal number of multiplications is the minimum number of multiplications equal to the number of inputs. In other words, to compute $F(m,r)$, we must access an array of $m + r - 1$ data values, and to compute $F(m \times n, r \times s)$, we must access a tile of $(m + r - 1) \times (n + s - 1)$ data values. Therefore the minimal filtering algorithm requires one multiplication per input.

4.1. $F(2\text{x}2, 3\text{x}3)$

The standard algorithm for $F(2,3)$ uses $2 \times 3 = 6$ multiplications. Winograd [13, p. 43] documented the following minimal algorithm:

```jsonl
{"function": "F(2,3)", "inputs": "[[d0, d1], [d1, d2], [d2, d3]]", "intermediate_g": "[g0, g1]", "outputs": "[m1 + m2 + m3, m2 - m3 - m4]", "equation_id": "5"}
```

where

```jsonl
{"m1": "(d0 - d2)g0", "m2": "(d1 + d2)(g0 + g1 + g2)/2", "m4": "(d1 - d3)g2", "m3": "(d2 - d1)(g0 - g1 + g2)/2"}
```

This algorithm uses just 4 multiplications and is three-times more minimal by the formula $\mu(F(2,3)) = 2 + 3 - 1 = 4$. It also uses 4 additions involving the data, 3 additions and 2 multiplications by a constant involving the filter (the sum $g0 + g2$ can be computed just once), and 4 additions to reduce the products to the final result.
Fast filtering algorithms can be written in matrix form as:

$Y = A^T [(Gg) \odot (B^T d)]$ (6)

where $\odot$ indicates element-wise multiplication. For $F(2,3)$, the matrices are:

```jsonl
{"B_transpose": [[1, 0, -1], [0, 1, 0], [0, -1, 1], [0, 0, -1]], "G": [[1, 0, 0], [1/2, 1, 0], [0, 1, 1/2]], "A_transpose": [[1, 1, 1, 0], [0, 1, -1, -1]], "g": "[g0, g1, g2]^T", "d": "[d0, d1, d2, d3]^T", "equation_id": "7"}
```

A minimal 1D algorithm $F(m,r)$ is nested with itself to obtain a minimal 2D algorithm, $F(m \times m, r \times r)$ like so:

$Y = A^T [ (Gg^T) \odot (B^T d) ] A$ (8)

where now $g$ is an $r \times r$ filter and $d$ is an $(m + r - 1) \times (m + r - 1)$ image tile. The nesting technique can be generalized for non-square filters and outputs, $F(m \times n, r \times s)$, by nesting an algorithm for $F(m,r)$ with an algorithm for $F(n,s)$.

$F(2 \times 2, 3 \times 3)$ uses $4 \times 16 = 16$ multiplications, whereas the standard algorithm uses $2 \times 3 \times 2 \times 3 = 36$. This is an arithmetic complexity reduction of $\frac{36}{16} = 2.25$. The data transform uses 32 additions, the filter transform uses 28 floating point instructions, and the inverse transform uses 24 additions.

Algorithms for $F(m \times m, r \times r)$ can be used to compute convnet layers with $r \times r$ kernels. Each image channel is divided into tiles of size $(m + r - 1) \times (m + r - 1)$, with $r - 1$ elements of overlap between neighboring tiles, yielding



Algorithm 1 Compute Convnet Layer with Winograd Minimal Filtering Algorithm $F(m \times r, m \times r)$

```jsonl
{"algorithm": "Compute Convnet Layer with Winograd Minimal Filtering Algorithm", "notation": "F(m x r, m x r)", "steps": [{"step": 1, "definition": "P = [H/m][W/m] is the number of image tiles."}, {"step": 2, "definition": "alpha = m + r - 1 is the input tile size."}, {"step": 3, "definition": "Neighboring tiles overlap by r - 1."}, {"step": 4, "definition": "d_{c,b} in R^{alpha x alpha} is input tile b in channel c."}, {"step": 5, "definition": "g_{b,c} in R^{m x r} is filter k in channel c."}, {"step": 6, "definition": "G, B, and T are filter data, and inverse transforms."}, {"step": 7, "definition": "y_{k,b} in R^{m x m} is output tile b in filter k."}, {"step": 8, "loop": "for k = 0 to K do", "sub-steps": [{"loop": "for c = 0 to C do", "action": "U = g_{b,c}G^T in R^{alpha x alpha}"}, {"action": "Scatter U to matrices U: U_{v,c} = u_{xi,nu}"}]}, {"step": 9, "loop": "for b = 0 to P do", "sub-steps": [{"loop": "for c = 0 to C do", "action": "V = b^T dB in R^{alpha x alpha}"}, {"action": "Scatter V to matrices V: V_{nu,b} = v_{xi,nu}"}]}, {"step": 10, "loop": "for xi = 0 to alpha do", "sub-steps": [{"loop": "for nu = 0 to alpha do", "action": "M_{xi,nu} = U_{xi,nu}V_{xi,nu}"}]}, {"step": 11, "loop": "for k = 0 to K do", "sub-steps": [{"loop": "for b = 0 to P do", "action": "Gather M from matrices M: m_{xi,nu} = M_{k,b}"}, {"action": "y_{k,b} = M^T m"}]}]}
```

Let $k = [H/m] [W/m]$ tiles per channel $C, F(m \times r, m \times r)$ is then computed for each tile and filter combination in each channel, and the results are summed over all channels. Substituting $U = Gg^T$ and $V = B^T dB$, we have:

$Y = A^T [U \odot V] A$ (9)

Labeling tile coordinates as $(x, y)$, we rewrite the convnet layer formula (2) for a single image $\tilde{x}$, filter $k$, and tile coordinate $(x, y)$ as:

$Y_{i,k,x,y} = \sum_{c=1}^{C} D_{i,x,y} * G_{k,c}$
$= \sum_{c=1}^{C} A^T [U_{k,c} \odot V_{c,i,x,y}] A$ (10)
$= A^T [\sum_{c=1}^{C} U_{k,c} \odot V_{c,i,x,y}] A$

Thus we can reduce $C$ channels in transform space, and only then apply the inverse transform $A$ to the sum. This amounts to the cost of the inverse transform over a single number of the channels.

We examine the sum

$M_{k,i,x,y} = \sum_{c=1}^{C} U_{k,c} \odot V_{c,i,x,y}$ (11)

and simplify the notation by collapsing the image/tile coordinates $(i, x, y)$ down to a single dimension, $b$. We also label each component of the element-wise multiplication separately, as $(\xi, \nu)$, yielding:

$M_{k,b}^{(\xi, \nu)} = \sum_{c=1}^{C} U_{k,c}^{(\xi, \nu)} V_{c,b}^{(\xi, \nu)}$ (12)

This equation is just a matrix multiplication, so we can write:

$M_{k,b}^{(\xi, \nu)} = U^{(\xi, \nu)} V^{(\xi, \nu)}$ (13)

Matrix multiply has efficient implementations on CPU, GPU, FPGA and FPGA platforms, owing to its high computational intensity. Thus we have arrived at the practical implementation. Thus we have arrived at the practical implementation in Algorithm 1.

Winograd documented a technique for bringing the minimal filtering algorithm $F(m, r)$ for any choice of $m$ and $r$. The construction uses the Chinese remainder theorem to obtain the minimal algorithm for any given convolution, which is equivalent to polynomial multiplication, then transposes the linear convolution algorithm to yield a minimal filtering algorithm. The reader is referred to Winograd's seminal book [13], or Blahut's book [2] for a modern treatment of the subject. We provide derivations of the specific algorithms used in this paper in the supplementary material.

4.2. $F(3\times3, 2\times2)$

Training a network using stochastic gradient descent requires computations of the gradients with respect to the inputs and weights. For a given layer, the gradient with respect to its inputs is a convolution of the next layer's backpropagated error, of dimension $N \times K \times H \times W$, with a flipped version of the layer's $R \times S$ filters. Therefore it can be computed using the same algorithm that is used for forward propagation.

The gradient with respect to the weights is a convolution of the layer inputs with the backpropagated error, producing $R \times S$ outputs per filter and channel. Therefore we need to compute the convolution $F(R \times S, H \times W)$, which is imaging practical because $H \times W$ is too large for our fast algorithms. Instead we decompose this convolution into a direct sum of smaller convolutions, for example $F(3 \times 2, 3 \times 2)$. Here the algorithm's $4 \times 4$ tiles are overlapped by 2 pixels in each dimension, and the $3 \times 3$ outputs are summed over all tiles to form $F(3 \times 3, H \times W)$.



The transforms for $F(3 \times 3, 2 \times 2)$ are given by:

```jsonl
{"type": "matrix", "label": "B^T", "rows": [[1, 0, -1, 0], [0, 1, 1, 0], [0, -1, 1, 0], [0, 0, -1, 0]]}
{"type": "matrix", "label": "G", "rows": [[1/2], [1/2], [1/2], [0]]}
{"type": "matrix", "label": "A^T", "rows": [[1, 1, 1, 0], [0, 1, -1, 0], [0, 0, 1, 1]]}
```
(14)

With $(3+2-1)^2 = 16$ multiplies versus direct convolution $3 \times 3 \times 2 \times 2 = 36$ multiplies, it achieves the same $36/16 = 2.25$ arithmetic complexity reduction as the corresponding spread forward propagation algorithm.

4.3. $F(4\text{x}4,3\text{x}3)$

A minimal algorithm for $F(4, 3)$ has the form:

```jsonl
{"type": "matrix", "label": "B^T", "rows": [[0, -0, 5, -1, 0], [0, -4, -4, 1, 1, 0], [0, 4, -4, -1, 1, 0], [0, -2, -2, 1, 1, 0], [0, 2, -2, 1, 1, 0], [0, 4, 0, -5, 0, 1]]}
{"type": "matrix", "label": "G", "rows": [[0], [1/6], [1/6], [1/6], [1/6], [1/6], [5/6], [1/12], [1/12], [0]]}
{"type": "matrix", "label": "A^T", "rows": [[1, 1, 1, 1, 0], [0, 1, -1, 2, -0], [0, 1, 1, -1, 4, 0], [0, 1, -1, -8, 8, -1]]}
```
(15)

The data transform uses 13 floating point instructions, the filter transform uses 8, and the inverse transform uses 10.

Applying the nesting formula yields a minimal algorithm for $F(4 \times 4, 3 \times 3)$, which uses $6 \times 3 = 18$ multiplications, while the standard algorithm uses $4 \times 4 \times 3 \times 3 = 144$. This is an arithmetic complexity reduction of 4.

The 2D data transform uses $13(6 + 6) = 156$ floating point instructions, the filter transform uses $8(3 + 6) = 72$, and the inverse transform uses $10(4 + 4) = 100$.

The number of additions and subtractions multiplications required by the minimal Winograd transforms increases quadratically with the tile size [9, p. 211]. Thus for large tiles, the complexity of the transforms will overwhelm any savings in the number of multiplications.

The magnitude of the transform matrix elements also increases with increasing transform tile size. This effectively reduces the numeric accuracy of the computation, so that for large tiles, the transforms cannot be computed accurately [13, p. 28].

Convnets require surprisingly little numeric precision [4, 5]. This implies that we can sacrifice some numeric accuracy in the filtering computation without affecting the accuracy of the convnets. We examine the possibility of $F(6 \times 6, 3 \times 3)$ in the supplementary material.

4.4. Fast Fourier Transform

The Fast Fourier Transform (FFT) can be used to produce a tiled convolution algorithm that has the same form as Algorithm 1. The main difference is that the transform matrices are replaced with FFT and inverse FFT, and pointwise multiplication of complex FFT components yields cyclic convolution. Only $m \times n$ multiplications with $(m+r-1) \times (n+s-1)$ cyclic convolution are valid, the rest must be discarded, and the tiles must be overlapped by $r-1$ and $s-1$ in order to recompute the discarded outputs. This technique is referred to as overlap and save [2, p. 195].

The similarity of overlap and save and our approach makes for an easy comparison. With FFT based convolution, the multiply stage will uses 1 multiply per complex number, where the operands are complex numbers. Direct multiplication of complex numbers requires 4 real multiplications. Thankfully, a couple of tricks reduces the complexity further.

The Fourier transform of a real signal has the Hermitian symmetry, which reduces the number of unique products in each $U \cup V \cup W$ by almost half, FFT based convnet implementations have exploited this property [10, 12]. Specifically, the discrete Fourier transform of an $\alpha \times \alpha$ array of real values can be represented with an array of $\alpha \times \lfloor \frac{\alpha}{2} + 1 \rfloor$ complex values. Furthermore $U_{HW}^{\text{IFFT}} = (UV)^R$, so the products of the missing values can be reconstructed simply by taking the complex conjugate of the computed values. Thus the multiply stage of the FFT convnet algorithm with tile size $\alpha = m + r$ requires $N \frac{\alpha \text{m}}{12} \lfloor \frac{\alpha}{2} \rfloor [C K \alpha \lfloor \frac{\alpha}{2} \rfloor + 1]$ complex multiplications, or $\lfloor \frac{\alpha}{2} \rfloor + 1 \rfloor \alpha / 2$ complex multiplies per input.

Using the standard algorithm for multiplying complex numbers, this equals $\lfloor \frac{\alpha}{2} + 1 \rfloor / \alpha > 2$ real multiplications per input.

Another technique, which to our knowledge has not been used in convnets, is to use a fast algorithm to multiply complex numbers with 3 real multiplications [13]:

```jsonl
{"type": "equation", "label": "16", "content": "(x_0 + i x_1)(y_0 + i y_1) = [x_0 y_0 - x_1 y_1, i(x_0 y_1 + x_1 y_0)] = [u_0 + v_0, u_0 + v_0, i(u_1 - v_0, u_1 - v_0, u_0 - v_1)]"}
```

where

```jsonl
{"type": "equation", "label": "17", "content": "u_0 = x_0 v_0 = y_0, v_0 = y_0, u_1 = x_0 + x_1, v_1 = y_0, u_2 = x_1 - x_0 v_0 = y_0 + y_1"}
```



Table 1. Multiply ($\alpha'$) data transform ($\beta'$) filter transform ($\gamma'$) inverse transform ($\delta'$) normalized arithmetic complexity for both Winograd and FFT based convolution. Fig(4x4,3x3) has tile size 6. Direct convolutions has tile size 3.

```jsonl
{"type": "table", "title": "Table 1", "headers": ["Tile", "Winograd alpha'", "Winograd beta'", "Winograd gamma'", "Winograd delta'", "FFT alpha'", "FFT beta', gamma', delta'"], "rows": [["3", "9.00", "", "", "", "", ""], ["4", "4.00", "2.00", "1.75", "1.50", "", ""], ["5", "2.78", "3.60", "2.24", "2.24", "", ""], ["6", "2.25", "4.33", "2.00", "2.78", "", ""], ["8", "1.78", "6.50", "2.23", "4.38", "4.44", "2.42"], ["16", "", "", "", "", "2.94", "4.23"], ["32", "", "", "", "", "2.42", "6.24"], ["64", "", "", "", "", "2.20", "8.30"], ["128", "", "", "", "", "2.10", "10.37"], ["256", "", "", "", "", "2.05", "12.42"]]}
```

Table 2. Normalized arithmetic complexity for FFT with filtering using fast CGEMM. Fast CGEMM uses 3 multiplies per complex multiply instead of 4, but has slightly greater transform overhead and uses more memory.

```jsonl
{"type": "table", "title": "Table 2", "headers": ["Tile", "alpha'", "beta'", "gamma'", "delta'"], "rows": [["8", "3.33", "3.77", "4.30", "4.30"], ["16", "2.20", "6.23", "6.82", "6.82"], ["32", "1.81", "8.94", "9.57", "9.57"], ["64", "1.81", "11.52", "12.36", "12.36"], ["128", "1.57", "14.48", "15.14", "15.14"], ["256", "1.54", "17.22", "17.88", "17.88"]]}
```

An FFT based convent transforms algorithm can conversely improve this by modifying the FFT transforms of the filter and data to output the real valued matrices ($U_{\alpha}, V_{\alpha}, U_{\beta}, V_{\beta}$) and ($U_{\gamma}, V_{\gamma}, U_{\delta}, V_{\delta}$) instead of the complex valued matrices $U$ and $V$. This avoids the 2d floating point instructions per output to the filter transforms, and 1 to the data transform. It also increases the memory footprint of each matrix by half.

Then we can calculate $M = UV$ using 3 calls to a standard real data matrix multiply function (e.g. SGEMM):

```jsonl
{"type": "equation", "label": "18", "content": "T = U_{\alpha}V_{\alpha}, M_{0} = U_{\gamma}V_{\gamma} + T, M_{1} = -U_{\beta}V_{\beta} + T, M = (M_{0}, iM_{1})"}
```

The accumulation of temporary matrix $T$ is performed using regular SGEMM with $\beta = 1$ and $C = T$, at the cost of adding 2 floating point instructions per output. We can think of these instructions as adding to the inverse transform cost. The temporary matrix $T$ increases necessary memory use by half, so that the total workspace size is approximately twice that of FFT based convolution with direct WGEMM.

Combining Hermitian symmetry with fast matrix multiplication gives us a multiplication stage with $3\{(\frac{3}{2}) + 1\} / \alpha > 1.5$ real multiplies per output. Recall that the multiply stage of the Winograd algorithms is always 1 real multiply per output. Even with the fast CGEMM, FFT based convolution must use a significantly larger tile size in order to rival the arithmetic complexity of the Winograd algorithms.

For the FFT transform itself, we consider the split-radix FFT algorithm, which is the minimal practical FFT algorithm when $N$ is a power of 2 [9, p. 150]. We assume the 2D FFT transform is constructed using row-column composition, and borrow the complexity figures from the DSP Handbook [2, pp. 173,175] for Table 1.

5. Arithmetic Complexity Analysis

In our model of fast convnets, the arithmetic complexity of the multiplication stage is:

```jsonl
{"type": "equation", "label": "19", "content": "M = N [W/H/m] [W/N] C K (m + R - 1) (n + S - 1)"}
```

When $m = 1$, the formula equals the arithmetic complexity of direct convolution. Therefore direct convolution is the minimal algorithm for $F(1 \times 1, R \times S)$.

Our analysis employs narrow convolutions, the convolution layer itself is not similar to most performant layers since our convolutions are strictly necessary. We could reduce the number of convolutions by employing Strassen recursions as in [1], but such a method reduces all 3 dimensions of our matrices by half while providing only $\frac{7}{8}$ reduction in arithmetic complexity. The matrix multiplications cannot be computed efficiently if $C$ or $K$ is too small. Fast convolution alone provides a 2.25 or larger arithmetic complexity reduction while shrinking only the largest dimension of the matrix, $P$. Still, for layers with $C = K = P$, it may be worthwhile to perform Strassen recursions in addition to fast convolution. We leave this as an area for further research.

In order to simplify the equations, we will henceforth assume that $W/H$ and $H/n$ have no remainders. We also assume square filters and blocks, $R = S$ and $m = n$.

The multiplication complexity can be rewritten as:

```jsonl
{"type": "equation", "label": "20", "content": "M = (m + (R - 1)^2 / m^2) N H W C K K = \alpha' N H W C K K"}
```

where $\alpha = (m + (R - 1)^2$ and $\alpha' = \alpha / m^2$

The total arithmetic complexities of the data, filter, and inverse transforms can be written as:

```jsonl
{"type": "equation", "label": "21", "content": "T(D) = \beta / m^2 N H W C, T(F) = \gamma C K, T(I) = \delta / m^2 N H W K"}
```



where $\beta$, $\gamma$, and $\delta$ are the number of floating point instructions used by the corresponding transforms for single tiles.

Dividing the complexity of each transform by $M$ yields its relative complexity:

```jsonl
{"equation": "T(D)/M = \beta / M (\beta / K^2) = \beta' / K"}
{"equation": "T(F)/M = \gamma / M (\gamma / (NHW^2 m^2 / m^2)) = \gamma / (Pa^2) = \gamma' / P"}
{"equation": "T(I)/M = \delta / M (\delta / C^2) = \delta' / C"}
```
(22)

We call $\beta'$, $\gamma'$, and $\delta'$ the normalized arithmetic complexities of the data, filter, and inverse transforms, respectively. $P = NHW/m^2$ is the number of tiles per channel.

Adding the terms for each transform gives the total arithmetic complexity of convolution $L$:

```jsonl
{"equation": "L = \alpha' (1 + \beta' / K + \gamma' / P + \delta' / C) NHWCK"}
```
(23)

In order to achieve a large speedup, the multiplication complexity $\alpha' M$ must be as small as possible, and the transform complexities $\beta'$, $\gamma'$, and $\delta'$ must each be small compared with $K$, $P$, and $C$, respectively.

For direct convolution, $\alpha' = \alpha_0$, $\beta' = R^2$ and $\gamma' = \gamma' = 0$. Therefore the maximum speedup of a fast transform convolution is $R^2 / \alpha_0$.

We listed the normalized transform complexity for different tile sizes and algorithms in Table 1 and 2. Due to its similarity to our approach, FFT based convolution complexity can also be measured with Equation 23.

FFT based convent layers with direct xGEMM compute must use tile size at least $64 \times 64$ to avoid excessive multiplication stage complexity. We tried FFT grids $(F \times 4.3 \times 3)$ and its $6 \times 6$ tile, but this then incurs much greater transform overhead. Also, a $64 \times 64$ tile will mean waste computations on many unwanted pixels for images with sizes that are not close to a multiple of $62 \times 62$. Even for moderate size layers, a moderate to large minibatch must be used, or there will be few tiles to compute the xGEMM efficiently. Finally, the memory used by the fast Fourier transformed filter channel is $64 \times 64 \times \alpha = 4096$ units, which is a large expansion of the $3 \times 3 \times \alpha$ filter. The tiling size of $F \times 64 \times 64$ expands the same filter to $6 \times 6 = 36$ units.

FFT based convent layers with fast xGEMM can be more competitive with Winograd layers. They have reasonable partition stage complexity with tile size 16, and reasonable transform complexity. Also tile size 16 generates a reasonable number of tiles with large kernels to avoid over-moderation or moderate batch size.

Even with fast xGEMM, the larger tile size compared to Winograd means FFT based convent implementations must have a large memory workspace to hold transformed data. A decent amount of transformed data must be held in order to automotive transform and cost to generate matrices with large enough dimensions so that the multiply stage is efficient. This is problematic for current GPUs, which have a limited amount of on chip memory. CPUs have large caches and might therefore compute FFT based convolution more efficiently.

6. GPU Implementation

We implemented $F(2 \times 2.3 \times 3)$ for Nvidia Maxwell GPUs and tested on the Nvidia Titan X model.

The small $4 \times 4$ or tile size and light weight transformations of $F(2 \times 2.3 \times 3)$ make possible a fused implementation of the algorithm stages, where the data and filter transform, 16 batched matrix multiplications (GEMMs), and inverse transforms are all computed in the same block cache. Another result limit is the instruction cache, which can only fit about 720 instructions. Our main loop is larger than this, but aligning the start of the loop with the 128 byte instruction cache-line boundary reduces the cost of a cache-line miss.

The 16 batched GEMMs compute $32 \times 32$ outputs, which enables us to fit the workspace in the registers and shared memory of a single block and still have 2 active blocks per SM for latency hiding. Zero padding is implicitly run through use of predicates. If the predicate deselects a global image load, the zero value is loaded with a dual issued 121 instruction.

Image data is stored in CHWN order, which makes the transforms contiguous, and aligned memory loads, significantly reducing over-fetch. We employ a "slower" blocking strategy - loaded tiles of 32 sizes of four $\times 4$ from a configurable number of images, rows, and columns. For $N > 32$, we load 32 tiles from separate images. For $N < 32$, we load separate blocks of $X \times Y$ for $N \times 32 / N$ tiles per image. This strategy facilitates efficient usage of large batch sizes, and ensures that dimensions of the input data are contiguous in memory. Furthermore, the 2 pixel overlap between adjacent tiles causes this L1 cache hit rates when using several tiles in a super block.

We also employ L2 cache blocking to increase the re-use of overlapping blocks. Since the number of image tiles is typically much larger than the number of filter blocks, we typically iterate over a group of 32 to 128 filter tiles in the inner loop, and then iterate over all image tiles in the second loop. All channels of the filter group fit in L2 cache, so each filter will only be loaded once from DDR memory, and each image tile will be loaded $\lceil K / 128 \rceil$ times as we iterate over the filter groups. This strategy reduces DDR memory bandwidth by almost half.

We implemented a version of our kernel that loads fp16 data, which doubled the global memory bandwidth. We also implemented a variant that we call "FF-ex" that runs a filter transform kernel first and stores the result in a workspace buffer. The convolution kernel loads transformed filter values from the workspace as needed. The size of the workspace is only $16KC$ units of memory, which equals just 16MB when $K = C = 512$ and data is fp32.



Layer | Depth | $C \times H \times W$ | K | GFLOPs
--- | --- | --- | --- | ---
conv 1.1 | 1 | $3 \times 224 \times 224$ | 64 | 0.17
conv 1.2 | 1 | $64 \times 224 \times 224$ | 64 | 3.70
conv 2.1 | 1 | $64 \times 112 \times 112$ | 128 | 1.85
conv 2.2 | 1 | $128 \times 112 \times 112$ | 128 | 3.70
conv 3.1 | 1 | $128 \times 56 \times 56$ | 256 | 1.85
conv 3.2 | 3 | $256 \times 56 \times 56$ | 256 | 11.10
conv 3.3 | 3 | $256 \times 56 \times 56$ | 256 | 11.10
conv 4.2 | 3 | $512 \times 28 \times 28$ | 512 | 11.10
conv 5.2 | 3 | $512 \times 14 \times 14$ | 512 | 3.70
Total | | | | 39.02

Table 3. Convolution layers of VGG network E. All layers uses $3 \times 3$ filters. Depth indicates the number of times a given layer shape occurs in the network. GFLOPs is weighted by depth and assumes $N=1$.

7. Experiments

We run accuracy and speed experiments with VGG Network E [11]. This is a deep network that uses $3 \times 3$ filters exclusively in the convolution layers, which are summarized in Table 3.

We tested the accuracy of our fast algorithms with both single precision (fp32) and half precision (fp16) data and filters. In all tests we used fp32 arithmetic instructions. We used random data and filters from the uniform distribution $[-1, 1]$ and measured absolute element-wise error. Ground truth was computed using a direct convolution building a double precision accumulator for reductions.

We measured the speed of our GPU implementation of $F(2 \times 2, 3 \times 3)$ and compared with cuDNN v3 [1] on a super-clocked NVIDIA Titan DX GPU. We stabilized the boost clock and observed a maximum core clock of 1126MHz. The GPU has 3072 cores, yielding a device peak throughput of $49.28 \times 10^{12}$ FLOPS.

Speed for a given layer was calculated by dividing the number of GFLOPs of computation required by direct convolution, as tabulated in Table 3, by the run time in milliseconds to yield effective TFLOPS. The reduction of arithmetic complexity allows fast algorithms to have effective TFLOPS that can exceed device peak throughput.

Total GFLOPs and run time were calculated by weighted averaging the GFLOPs and run time for each layer by its depth, and total throughput was calculated as the ratio of the two.

8. Results

Table 4 shows the numeric accuracy of the different convolution layer algorithms tested with single precision (fp32) and half precision (fp16) input data and filters.

$F(2 \times 2, 3 \times 3)$ is actually slightly more accurate than direct convolution. Its simple transformations do not lose much precision, and its multiplication stage performs a reduction over $C$ channels, rather than the $RSC$ filter elements reduction by direct convolution. $F(4 \times 4, 3 \times 3)$ has a larger error, but it is still more accurate than direct convolution with fp16 data.

All tested algorithms are equally accurate with fp16 data. Here accuracy is limited by the precision of the inputs. Because direct convolution is accurate enough for training and inference with low precision data [4, 5], we conclude that $F(4 \times 4, 3 \times 3)$ is too lossy.

Table 5 and Table 6 show the total throughput for VGG Networks for our cuDNN and our $F(2 \times 2, 3 \times 3)$ implementation steps for fp32 and fp16 data for different batch sizes.

For fp32 data, $F(2 \times 2, 3 \times 3)$ is $1.89\times$ at $N = 64$ and $2.23\times$ at $N = 128$ compared with cuDNN v3, yielding $11.9$ and $13.0$ TFLOPS. For fp16 data, $F(2 \times 2, 3 \times 3)$ extends its lead over cuDNN, recording 10.28 TFLOPS throughput for $N = 64$. $N = 8$ performance is still very good at 9.57 TFLOPS.

Figure 1 shows throughput by layer. Hatch marks indicate the layers where cuDNN used the FFT algorithm, otherwise direct convolution was used. For $F(2 \times 2, 3 \times 3)$, hatch marks indicate the extended bilinear transfer from FX was used, otherwise the fused transformation stage was used. cuDNN appears to erroneously select its FFT algorithm for intermediate values of $N$ despite the fact that it performs very poorly, under 2 TFLOPS. While this probably is just a bug, it is revealing. Low performance at moderate values of $N$ suggests that the FFT convolution implementation either uses large tiles, or possibly just a single tile per image, as in [12]. This leads to inefficient multiplication stages unless $N$ is large. At large $N$, cuDNN FFT performs much better, but still yields under 8 TFLOPS.

$F(2 \times 2, 3 \times 3)$ performs better than cuDNN at every layer and batch size, except layer conv1.1, which contributes less than 0.5% of the total network computation.

In general, we see that the FX variant of our implementation performed best unless the number of filters and channels was very large. Computing the filter transform is heavily memory bound, therefore transforming a larger filter bank decreases computational efficiency.

The worst $F(2 \times 2, 3 \times 3)$ performance occurs on the $14 \times 14$ layers when $N = 1$. In this case the $8 \times 8$ superblocks overlap on the image boundary and compute unwanted padded pixels. Through this particular layer configuration is still over 5 TFLOPS, where cuDNN performance is just 1.6 TFLOPS. cuDNN FFT uses a global memory workspace up to $2.6$ GB in our experiments. By contrast, our fused $F(2 \times 2, 3 \times 3)$ implementation does not use any global workspace, and the FX variant uses no more than 16 MB.

$F(2 \times 2, 3 \times 3)$ performance shows new capabilities for high throughput and small batch size with state of the art convolutional neural networks. We expect performance to increase again when $F(4 \times 4, 3 \times 3)$ is implemented.



Table 4. Maximum element error on VGG network variations. With fp32 data, $F(2 \times 2, 3 \times 3)$ is more accurate than direct convolution. With fp16 data, all algorithms are equally accurate.

```jsonl
{"type": "table", "header": ["Layer", "fp32 Direct", "fp32 F(2x2,3x3)", "fp32 F(4x4,3x3)", "fp16"], "rows": [["1.2", "4.01E-05", "1.53E-05", "2.84E-04", "1.14E-02"], ["2.2", "8.01E-05", "2.86E-05", "5.41E-04", "1.45E-02"], ["3.2", "1.53E-04", "5.34E-05", "9.06E-04", "1.99E-02"], ["4.2", "3.20E-04", "5.34E-05", "1.04E-03", "3.17E-02"], ["5", "3.43E-04", "4.20E-05", "1.08E-03", "2.61E-02"]]}
```

Table 5. cuDNN versus $F(2 \times 2, 3 \times 3)$ performance on VGG Network E with fp32 data. Throughput is measured in Effective TFLOPs, the ratio of direct algorithm GFLOPs to run time.

```jsonl
{"type": "table", "header": ["N", "cuDNN msec", "cuDNN TFLOPS", "F(2x2,3x3) msec", "F(2x2,3x3) TFLOPS", "Speedup"], "rows": [["1", "12.52", "3.12", "5.55", "7.03", "2.26X"], ["2", "20.36", "3.83", "9.89", "7.89", "2.06X"], ["4", "104.70", "1.49", "17.72", "8.81", "5.91X"], ["8", "241.21", "1.29", "33.11", "9.43", "7.28X"], ["16", "203.09", "3.07", "65.79", "9.49", "3.09X"], ["32", "321.05", "5.27", "122.36", "9.45", "1.79X"], ["64", "394.05", "6.34", "266.48", "9.37", "1.48X"]]}
```

Table 6. cuDNN versus $F(2 \times 2, 3 \times 3)$ performance on VGG Network E with fp16 data.

```jsonl
{"type": "table", "header": ["N", "cuDNN msec", "cuDNN TFLOPS", "F(2x2,3x3) msec", "F(2x2,3x3) TFLOPS", "Speedup"], "rows": [["1", "14.58", "2.68", "5.53", "7.06", "2.64X"], ["2", "20.94", "3.73", "9.83", "7.94", "2.13X"], ["4", "104.19", "1.50", "17.50", "8.92", "5.95X"], ["8", "241.87", "1.29", "32.61", "9.57", "7.42X"], ["16", "204.01", "3.06", "62.93", "9.92", "3.24X"], ["32", "236.13", "5.29", "123.12", "10.14", "1.92X"], ["64", "395.93", "6.31", "242.98", "10.28", "1.63X"]]}
```

[Image: A vertical stack of 8 bar charts showing "Effective TFLOPs" on the Y-axis (scaled 0 to 12) and "Batch Size" on the X-axis (labeled 1, 2, 4, 8, 16, 32, 64 for the bottom chart). Each chart represents a different VGG layer: vgg.conv1.1, vgg.conv1.2, vgg.conv2.1, vgg.conv2.2, vgg.conv3.1, vgg.conv3.2, vgg.conv4.1, vgg.conv4.2, and vgg.conv5. For each batch size, there are four colored bars. The colors are: Orange (cuDNN), Light Blue (cuDNN FFT), Red (cuDNN fp16), and Dark Blue (cuDNN FFT fp16). To the right of these are four corresponding bars for F(2x2,3x3): Light Grey (F(2x2,3x3)), Medium Grey (F(2x2,3x3) FX), Dark Red (F(2x2,3x3) fp16), and Dark Blue (F(2x2,3x3) FX fp16). A legend at the bottom explains these color mappings. The bars generally trend upwards as batch size increases, with the F(2x2,3x3) variants typically outperforming cuDNN at smaller batch sizes.]

Figure 1. VGG net Effective TFLOPS vs. batch size for cuDNN and $F(2 \times 2, 3 \times 3)$ on a 6.96 TFLOPS NVIDIA Titan X GPU.



References

[1] cuDNN. https://developer.nvidia.com/cudnn.
Accessed: 2015-11-01. 1, 7

[2] Richard E Blahut. Fast algorithms for signal processing. Cambridge University Press, 2010. 3, 4

[3] Jason Cong and Bingjun Xiao. Minimizing computation in convolutional neural networks. In Artificial Neural Networks and Machine Learning-ICANN 2014, pages 281–290. Springer, 2014. 1, 5

[4] Matthieu Courbariaux, Yoshua Bengio, and Jean-Pierre David. Low precision arithmetic for deep learning. CoRR, abs/1412.7024, 2014. 4, 7

[5] Suyog Gupta, Ankur Agrawal, Kailash Gopalakrishnan, and Pritish Narayanan. Deep learning with limited numerical precision. arXiv preprint arXiv:1502.02551, 2015. 4, 7

[6] Suyog Gupta, Wei Zhang, and Josh Mithrope. Model accuracy and runtime tradeoff in distributed deep learning. arXiv preprint arXiv:1509.04210, 2015. 1

[7] Sergey Ioffe and Christian Szegedy. Batch normalization: Accelerating deep network training by reducing internal covariate shift. arXiv preprint arXiv:1502.03167, 2015. 1

[8] Alex Krizhevsky. One weird trick for parallelizing convolutional neural networks. arXiv preprint arXiv:1404.5997, 2014. 1

[9] V. Madisetti. The Digital Signal Processing Handbook. Number. v. 2 in Electrical engineering handbook series. CRC, 2010. 4, 5

[10] Michaël Mathieu, Mikael Henaff, and Yann LeCun. Fast training of convolutional networks through ffts. CoRR, abs/1312.5851, 2013. 1, 4

[11] Karen Simonyan and Andrew Zisserman. Very deep convolutional networks for large-scale image recognition. arXiv preprint arXiv:1409.1556, 2014. 1, 7

[12] Nicolas Vasilaé, Jeff Johnson, Michaël Mathieu, Soumith Chintala, Serkan Piantino, and Yann LeCun. Fast convolutional nets with fbufft: A GPU performance evaluation. CoRR, abs/1412.7580, 2014. 1, 4, 7

[13] Shmuel Winograd. Arithmetic complexity of computations, volume 33. Siam, 1980. 1, 2, 3, 4

[14] Shmuel Winograd. On multiplication of polynomials modulo a polynomial. SIAM Journal on Computing, 9(2):225–229, 1980. 2
