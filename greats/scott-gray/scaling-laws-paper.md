Scaling Laws for Neural Language Models

Jared Kaplan*
Johns Hopkins University, OpenAI
jaredk@jhu.edu

Sam McCandlish*
OpenAI
sam@openai.com

Tom Henighan
OpenAI
henighan@openai.com

Tom B. Brown
OpenAI
tom@openai.com

Benjamin Chess
OpenAI
bchess@openai.com

Rewon Child
OpenAI
rewon@openai.com

Scott Gray
OpenAI
scott@openai.com

Alec Radford
OpenAI
alec@openai.com

Jeffrey Wu
OpenAI
jeffwu@openai.com

Dario Amodei
OpenAI
damodei@openai.com

Abstract

We study empirical scaling laws for language model performance on the cross-entropy loss. The loss scales as a power-law with model size, dataset size, and the amount of compute used for training, with some trends spanning more than seven orders of magnitude. Other architectural details such as network width or depth have minimal effects within a wide range. Simple equations govern the dependence of overfitting on model/dataset size and the dependence of training speed on model size. These relationships allow us to determine the optimal allocation of a fixed compute budget. Larger models are significantly more sample-efficient, such that optimally compute-efficient training involves training very large models on a relatively modest amount of data and stopping significantly before convergence.

{"text": "arXiv:2001.08361v1 [cs.LG] 23 Jan 2020", "orientation": "vertical", "position": "left-margin"}

*Equal contribution.

Contributions: Jared Kaplan and Sam McCandlish led the research. Tom Henighan contributed the LSTM experiments. Tom Brown, Rewon Child, and Scott Gray, and Alec Radford developed the optimized Transformer implementation. Jeff Wu, Benjamin Chess, and Alec Radford developed the text datasets. Dario Amodei provided guidance throughout the project.



Contents

1 Introduction [2]
2 Background and Methods [6]
3 Empirical Results and Basic Power Laws [7]
4 Charting the Infinite Data Limit and Overfitting [10]
5 Scaling Laws with Model Size and Training Time [12]
6 Optimal Allocation of the Compute Budget [14]
7 Related Work [18]
8 Discussion [18]

Appendices
A Summary of Power Laws [20]
B Empirical Model of Compute-Efficient Frontier [20]
C Caveats [22]
D Supplemental Figures [23]

1 Introduction

Language provides a natural domain for the study of artificial intelligence, as the vast majority of reasoning tasks can be effectively expressed and evaluated in language. The world's text provides a wealth of data for unsupervised learning via generative modeling in deep learning. Learning has recently seen rapid progress in language modeling, with the state of the art models [RNS$\textsuperscript{18}$][DCL$\textsuperscript{18}$][YDY$\textsuperscript{19}$][LOG$\textsuperscript{19}$][RSR$\textsuperscript{19}$] approaching human-level performance on many specific tasks [WPN$\textsuperscript{19}$], including the composition of coherent multi-paragraph prompted text [RWC$\textsuperscript{19}$].

One next logical step was modeling language performance to depend on model architecture, the size of the neural models, the computing power used to train them, and the data available for this training experience. In this work we will empirically investigate the dependence of language modeling loss on all of these factors, focusing on the Transformer architecture [VAS$\textsuperscript{17}$][LSP$\textsuperscript{18}$]. The high ceiling and low floor for performance on language tasks allows us to study trends over more than seven orders of magnitude in scale.

Throughout we will describe precise power-law scalings for performance as a function of training time, context length, dataset size, model size, and compute budget.

1.1 Summary

Our key findings for Transformer language models are as follows:

$\textsuperscript{2}$Here we display predicted compute when using a sufficiently small batch size. See Figure [13] for comparison to the purely empirical data.



[IMAGE: A figure containing two side-by-side line plots.
Left plot: The y-axis is labeled "Test Loss" with a scale from 2 to 7. The x-axis is labeled "Compute PF-days, non-embedding" with a logarithmic scale ranging from 10^-9 to 10^-1. There are multiple blue lines sloping downwards from left to right, representing different model scales. A single orange dashed line also slopes downwards, labeled "L = (C_{exp}/2.3 * 10^8)^{-0.050}".
Right plot: This is a composite of two sub-plots sharing a y-axis. The left sub-plot has an x-axis labeled "Dataset Size tokens" from 10^0 to 10^5. It shows a blue line sloping upwards with the label "L = (D/5.4 * 10^{13})^{-0.095}". The right sub-plot has an x-axis labeled "Parameters non-embedding" from 10^0 to 10^0 (likely meant as a range). It shows a blue line sloping downwards with the label "L = (N/8.8 * 10^{13})^{-0.076}".
The overall caption below the plots reads: "Figure 1 Language modeling performance improves smoothly as we increase the model size, datasetset size, and amount of compute used for training. For optimal performance all three factors must be scaled up in tandem. Empirical performance has a power-law relationship with each individual factor when not bottlenecked by the other two."]

Figure 1 Language modeling performance improves smoothly as we increase the model size, datasetset size, and amount of compute used for training. For optimal performance all three factors must be scaled up in tandem. Empirical performance has a power-law relationship with each individual factor when not bottlenecked by the other two.

Performance depends strongly on scale, weakly on model shape: Model performance depends most strongly on scale, which consists of three factors: the number of model parameters N (excluding embeddings), the size of the dataset D, and the amount of compute C used for training. Within reasonable limits, performance depends very weakly on other architectural parameters such as depth vs. width. (Section 5)

Smooth power laws: Performance has a power-law relationship with each of the three scale factors N, D, and C when not bottlenecked by the other two, with trends spanning more than six orders of magnitude in size (see Figure 1). We observe no signs of deviation from these trends on the upper end, though performance must flatten out eventually before reaching zero loss. (Section 5)

Universality of overfitting: Performance improves predictably as the loss scales up and down in N and D, but enters a regime of diminishing returns if either N or D is held fixed while the other increases. The performance penalty depends predictably on the ratio N^0.74/D, meaning that every time we increase the model size 8x, we only need to increase the data by roughly 5x to avoid a penalty. (Section 4)

Universality of training: Training curves follow predictable power-laws whose parameters are roughly independent of the model size. By extrapolating the early part of a training curve we can predictably predict the loss that would be achieved if we trained for much longer. (Section 5)

Transfer improves with test compactness: When we evaluate models on text with a different distribution than they were trained on, the results are strongly correlated to those on the training validation set with a roughly constant offset in the loss – in other words, a transfer to a different distribution incurs a constant penalty but otherwise improves roughly in line with performance on the training set. (Section 3.2.2)

Sample efficiency: Large models are more sample-efficient than small models, reaching the same level of performance with fewer optimization steps (Figure 2) and using fewer data points (Figure 4).

Convergence is inefficient: When working in a fixed compute budget C but without any other restrictions on the model size N or available data D, we attain optimal performance by training very large models on and stopping significantly short of convergence (see Figure 5). Maximally compute-efficient training would therefore be far more sample efficient than one might expect based on training small models to convergence, with data requirements growing more slowly than on the training compute. (Section 6)

Optimal batch size: The ideal batch size for training these models is roughly a power of the loss only, and continues to be determined by the measurable graining noise (MKAT18); it is roughly 1-2 million tokens at convergence for the largest models we can train. (Section 5.1)

Taken together, these results show that language modeling performance improves smoothly and predictably as we appropriately scale up model size, data, and compute. We expect that larger language models will perform better and be more sample efficient than current models.



Larger models require fewer samples to reach the same performance

The optimal model size grows smoothly with the loss target and compute budget

```jsonl
{"type": "x-y-plot", "title": "Test Loss vs Tokens Processed", "x_axis": "Tokens Processed", "y_axis": "Test Loss", "legend": "Model size (number of parameters)", "series": [{"label": "10^0 Params", "trend": "higher loss, slower convergence"}, {"label": "10^1 Params", "trend": "medium loss, medium convergence"}, {"label": "10^2 Params", "trend": "lower loss, faster convergence"}]}
{"type": "x-y-plot", "title": "Test Loss vs Compute", "x_axis": "Compute (PF-days)", "y_axis": "Test Loss", "legend": "Line color indicates number of parameters", "series": [{"label": "10^0", "trend": "steep curve"}, {"label": "10^1", "trend": "moderate curve"}, {"label": "10^2", "trend": "shallow curve"}]}
```

Figure 2 We show a series of language model training runs, with models ranging in size from $10^3$ to $10^0$ parameters (excluding embeddings).

```jsonl
{"type": "x-y-plot", "title": "Multiplicative Contribution to Compute", "x_axis": "Compute (PF-days)", "y_axis": "Multiplicative Contribution", "regions": [{"label": "<10x Batch Size", "effect": "Minimum serial steps increases negligibly"}, {"label": "100x Batch Size", "effect": "Data requirements grow relatively slowly"}, {"label": ">1,000,000x Model Size", "effect": "Optimal model size increases very quickly"}]}
```

Figure 3 As more compute becomes available, we can choose how much of it allocate towards training larger models, using larger batches, and training for more steps. We illustrate this for a billion-fold increase in compute. For optimally compute-efficient training, most of the increase should go towards increased model size. A relatively small increase in data is needed to avoid reuse. Of the increase in data, most can be used to increase parallelism through larger batch sizes, with only a very small increase in serial training time required.

1.2 Summary of Scaling Laws

The test loss of a Transformer trained to autoregressively model language can be predicted using a power-law when performance is limited only by either the number of non-embedding parameters $N$, the dataset size $D$, or the optimally allocated compute budget $C_{min}$ (see Figure 1).

1. For models with limited number of parameters, trained to convergence on sufficiently large datasets:
$L(N) = (N_c / N)^{\alpha_N} ; \alpha_N \approx 0.076, N_c \sim 8.8 \times 10^{13}$ (non-embedding parameters) (1.1)

2. For large models trained with a limited dataset with early stopping:
$L(D) = (D_c / D)^{\alpha_D} ; \alpha_D \approx 0.095, D_c \sim 3.4 \times 10^{13}$ (tokens) (1.2)

3. When training with a limited amount of compute, a sufficiently large dataset, an optimally-sized model, and a sufficiently small batch size (making optimal use of compute):
$L(C_{min}) = (C_{min}^* / C_{min})^{\alpha_{C_{min}}} ; \alpha_{C_{min}} \approx 0.050, C_{min}^* \sim 3.1 \times 10^{11}$ (PF-days) (1.3)

$^3$We also observe an empirical power-law trend with the training compute $C$ (Figure 1) while training at fixed batch size, but it is the trend with $C_{min}$ that should be used to make predictions. They are related by equation 5.5.

4



[IMAGE]: A scientific figure containing two line plots side-by-side.
Left Plot: Titled "Loss vs Model and Dataset Size". The x-axis is "Tokens in Dataset" on a log scale from $10^7$ to $10^{10}$. The y-axis is "Loss" ranging from 2.5 to 4.5. There are five lines of different colors (blue, green, yellow, orange, red) representing different model parameter counts. A legend on the right lists Params: 102M (blue), 302M (green), 802M (yellow), 3.5B (orange), and 30.5B (red). The lines generally trend downward as dataset size increases, with the larger models (red/orange) achieving lower loss.
Right Plot: Titled "Loss vs Model Size and Training Steps". The x-axis is "Estimated $S_{min}$" on a log scale from $10^3$ to $10^5$. The y-axis is "Loss" ranging from 2.4 to 4.4. The plot contains a dense series of downward-sloping lines. A color bar on the far right maps colors from blue (bottom) to yellow (top) to "Parameters (non-embed)" on a log scale from $10^2$ to $10^5$.

Figure 4 Left: The early-stopping test loss $L_1(N, D)$ varies predictably with the dataset size $D$ and model size $N$ according to Equation (1.5). Right: After an initial interaction period, learning curves for all model sizes $N$ can be fit with Equation (1.6), which is parameterized in terms of $S_{min}$, the number of steps when training at large batch size starts (details in Section 5.1).

These relations hold across eight orders of magnitude in $C_{min}$, six orders of magnitude in $N$, and two orders in magnitude of $D$. They depend very weakly on number of hyperparameters and other model specifications with the Webtext2 training set (see number of self-attention heads), which enables the derivation of performance improvement expected as we scale up $N, D$, or $C_{min}$ for example, doubling the number of parameters yields a loss that is smaller by a factor $2^{-0.3} \approx 0.85$. The precise numerical values of $N_c, C_e$ and $D_e$ depend on the vocabulary size after tokenization and hence do not have a fundamental meaning.

The critical batch size, which determines the speed/efficiency tradeoff for data parallelism (MKAT18) also roughly obeys a power law in $B$.

```jsonl
{"equation_id": "1.4", "formula": "B_{crit}(L) = B_1 \\left(\\frac{L}{L_1}\\right)^{\\beta}, \\quad B_1 \\sim 2 \\cdot 10^5 \\text{ tokens}, \\alpha_B \\sim 0.21"}
```

Equation (1.1) and (1.2) together suggest that as we increase the model size, we should increase the dataset size linearly according to $D \\propto N^{\\alpha} \\sim N^{0.74}$. In fact, since there is a single equation combining (1.1) and (1.2) that governs the simultaneous dependence on $N$ and $D$ and governs the degree of overfitting: (1.1) and (1.2).

```jsonl
{"equation_id": "1.5", "formula": "L(\\frac{N}{N_c}, \\frac{D}{D_e}) = \\left(\\frac{N_c}{N}\\right)^{\\frac{\\alpha}{D}} + \\left(\\frac{D_e}{D}\\right)^{\\alpha D}"}
```

with fits pictured on the left in figure 4. we conjecture that this functional form may also parametrize the training log-likelihood for other generative modeling tasks.

When training a given model for a finite number of parameter update steps $S$ in the infinite data limit, after an initial transient period, the learning curves can be approximately fit (by see the right of figure 4)

```jsonl
{"equation_id": "1.6", "formula": "L(N, S) = \\frac{N_c}{N}^{\\alpha N} + \\left(\\frac{S_{min}(S)}{S_{min}(S)}\\right)^{\\alpha S}"}
```

where $S_c \\approx 2.1 \\times 10^3$ and $\\alpha_S \\approx 0.76$, and $S_{min}(S)$ is the minimum possible number of optimization steps (parameter updates) estimated using Equation (5.4).

When training within a fixed compute budget $C$, but without other constraints, Equation (1.6) leads to the prediction that the optimal model size $N$, optimal batch size $B$, optimal number of steps $S$, and dataset size $D$ should grow as

```jsonl
{"equation_id": "1.7", "formula": "N \\propto C^{\\alpha_{min}/\\alpha_N}, B \\propto C^{\\alpha_{min}/\\alpha_B}, S \\propto C^{\\alpha_{min}/\\alpha_S}, D = B \\cdot S"}
```

with

```jsonl
{"equation_id": "1.8", "formula": "\\alpha_{min}^{-1} = 1/(1/\\alpha_N + 1/\\alpha_B + 1/\\alpha_S)"}
```

which closely matches the empirically optimal results with $\\alpha_N^{min} \\sim 0.73, B \\sim B_{min}^{\\alpha}, S \\sim C^{0.24}$, and $S \\sim C^{0.03}$. As the computational budget $C$ increases, it should be spent primarily on larger models, without dramatic increases in training time or dataset size (see Figure 5). This also implies that as models grow larger, they become increasingly sample efficient. In practice, researchers typically train smaller models for longer than would



be maximally compute-efficient because of universality. Optimal performance depends on the compute as a power law (see Equation (1.3).

We provide some basic theoretical motivation for Equation (1.5), an analysis of learning curve fits and their implications for training time, and a brainstorm of our results on the web. We also make some brief comparisons to LSTMs and Recurrent Transformers (DGV* 18).

1.3 Notation

We use the following notation:

- $L$ – the cross entropy loss in nats. Typically we will be averaged over the tokens in a context, but in some cases, we will be distributed for specific tokens within the context.
- $N$ – the number of model parameters, excluding all vocabulary and positional embeddings
- $C \approx 6 N B S$ – an estimate of the total non-embedding training compute, where $B$ is the batch size, and $S$ is the number of training steps (ie inter iteration steps). We quote numerical values in PF-days, where the PF-day = $10^{15} \times 24 \times 3600 = 8.64 \times 10^{19}$ floating point operations.
- $D$ – the data size
- $B_{crit}$ – the critical batch size (MKAT18). Training at the critical batch size provides a roughly optimal compromise between compute cost and compute efficiency.
- $C_{min}$ – an estimate of the minimum amount of non-embedding compute to reach a given value of the loss. This is the training compute that would be used if the model were trained at a batch size that was the critical batch size.
- $S_{min}$ – an estimate of the minimal number of training steps needed to reach a given value of the loss. This is also the number of training steps that would be used if the model were trained at a batch size much greater than the critical batch size.
- $\alpha X$ – power-law exponents for the scaling of the loss $L(X) \propto 1/X^\alpha$ where $X$ can be any of $N, D, C, S, B, C_{min}$

2 Background and Methods

We train language models on WebText2, an extended version of the WebText (RWC* 19) dataset, tokenized using byte-pair encoding (SHB15) with a vocabulary size $n_{vocab} = 50257$. We optimize the cross-entropy loss (ie cross-entropy loss averaged over a 128-token distribution, on which we also report our primary performance metric. We record the WebText2 test distribution and on a selection of other public distributions. We primarily train decoder-only (LPS* 18, RNNSS18) (VSP* 17) models. We also train LSTM and Transformers (DGV* 18).

2.1 Parameter and Compute Scaling of Transformers

We parameterize the Transformer architecture using hyperparameters $n_{layers}$ (number of layers), $d_{model}$ (dimension of the residual stream), $d_{ff}$ (dimension of the intermediate feed-forward layer), $d_{attn}$ (dimension of the attention output), and $n_{heads}$ (number of attention heads per layer). We include $n_{ctx}$ tokens in the input context, with $n_{ctx} = 1024$ except where otherwise noted.

We use $N$ to denote the model size, which we define as the number of non-embedding parameters

$N \approx 2d_{model}^2 n_{layers} (2d_{attn} + d_{ff}) \text{ (2.1)}$
$= 12n_{layers} d_{model}^2$ with the standard $d_{attn} = d_q/4 = d_{model}$

where we have excluded biases and other sub-leading terms. Our models also have $n_{vocab} d_{model}$ parameters in an embedding matrix, and use $n_{ctx} d_{model}$ parameters for positional embeddings, but we do not include these when discussing the 'model size' $N$; we will see this that this process significantly cleanses linear scaling.

Evaluating a forward pass of the Transformer involves roughly

$C_{forward} \approx 2N + 2n_{layers} n_{ctx} d_{model} \text{ (2.2)}$

add-multiply operations, where the factor of two comes from the multiply-accumulate operation used in matrix multiplication. A more detailed per-operation and parameter compute and count is included in [1]

6



| Operation | Parameters | FLOPs per Token |
| :--- | :--- | :--- |
| Embed | $(n_{\text{vocab}} + n_{\text{ctx}}) d_{\text{model}}$ | $4d_{\text{model}}$ |
| Attention: QKV | $n_{\text{layer}} d_{\text{model}} 3 d_{\text{attn}}$ | $2n_{\text{layer}} d_{\text{model}} 3 d_{\text{attn}}$ |
| Attention: Mask | | $2n_{\text{layer}} n_{\text{ctx}} d_{\text{attn}}$ |
| Attention: Project | $n_{\text{layer}} d_{\text{attn}} d_{\text{model}}$ | $2n_{\text{layer}} d_{\text{attn}} d_{\text{model}}$ |
| Feedforward | $m_{\text{layer}} 2d_{\text{model}} d_{\text{ff}}$ | $2n_{\text{layer}} 2d_{\text{model}} d_{\text{ff}}$ |
| De-embed | | $2d_{\text{model}} n_{\text{vocab}}$ |
| **Total (Non-Embedding)** | $N = 2d_{\text{model}} n_{\text{layer}} (2d_{\text{attn}} + d_{\text{ff}})$ | $C_{\text{forward}} = 2N + 2n_{\text{layer}} n_{\text{ctx}} d_{\text{attn}}$ |

**Table 1** Parameter counts and compute (forward pass) estimates for a Transformer model. For sub-leading terms such as nonlinearities, biases, and layer normalizations are omitted.

For contexts and models with $d_{\text{model}} > n_{\text{ctx}}/12$, the context-dependent computational cost per token is a relatively small fraction of the total compute. Since we primarily study models where $d_{\text{model}} \gg n_{\text{ctx}}/12$, we do not include context-dependent terms in our training compute estimate. Accounting for the backwards pass (approximately twice the compute as the forwards pass), we number the total compute as $C \approx 6N$, the floating point operations per training per token;

**2.2 Training Procedures**

Unless otherwise noted, we train models with the Adam optimizer [KBL15] for a fixed $2.5 \times 10^5$ steps with a batch size of 512 sequences of 1024 tokens. Due to memory constraints, our largest models (more than 1B parameters) were trained with Adapter [SS18]. We experimented with a variety of learning rates and schedules, as discussed in Appendix [D6]. We found that results at convergence were largely independent of learning rate schedule: Unless otherwise noted, all training runs in our used data using the learning rate schedule with a 3000 step linear warmup followed by a cosine decay to zero.

**2.3 Datasets**

We train our models on an expanded version of the WebText dataset described in [RWC+ 19]. The original WebText dataset was a crawl of outbound links from Reddit dubbed Redditdit from December 2017 until at least 3 karna. In the second version, WebText 2, we added outbound Reddit links from the period of January to October 2018, also with a minimum of 3 likes. WebText 2 was served as a heuristic for the websites more people found the link interesting or useful. The text of the new links was extracted using the Newspaper3k python library. In total, the dataset consists of 20.3M documents containing 96 GB of text and $1.62 \times 10^{10}$ words (as defined by $\text{n-grams}$), which we then apply the tokenizer described in [RWC+ 19], which yields $2.29 \times 10^{10}$ tokens. We reserve $6.6 \times 10^8$ of these tokens for use as a test set, and we also test on similarly-prepared samples of Books Corpus [ZKZ+ 15], Common Crawl [Fou], English Wikipedia, and a collection of publicly-available Internet Books.

**3 Empirical Results and Basic Power Laws**

To characterize language model scaling and see if the power laws hold, we train a wide variety of models, varying a number of parameters including:

• Model size (ranging in size from 768 to 1.5 billion non-embedding parameters)
• Dataset size (ranging from 22 million to 23 billion tokens)
• Shape (including depth, width, attention heads, and feed-forward dimension)
• Context length (1024 for most runs, though we also experiment with shorter contexts)
• Batch size ($2^{19}$ for most runs, but we also vary it to measure the critical batch size)

7



Figure 5 Performance depends very mildly on model shape when the total number of non-embedding parameters $N$ is held fixed. The loss varies only a few percent over a wide range of shapes. Small differences in parameter counts are compensated for by scaling the input/output embedding as a baseline. Aspect ratio in particular can vary by a factor of 40 while only slightly impacting the training performance, (RWC`19), reaches a loss within 3% of the (48, 1600) model used in (RWC`19).

```jsonl
{"figure": 5, "type": "x-y plots", "plots": [{"title": "Feed-Forward Ratio (dff / dmodel)", "x_axis": "Feed-Forward Ratio", "y_axis": "Loss Increase", "series": [{"label": "n_head = 8", "data": [[1e-1, 0.02], [1e0, 0.01], [1e1, 0.03]]}, {"label": "dmodel/dhead = 64", "data": [[1e-1, 0.01], [1e0, 0.00], [1e1, 0.02]]}]}, {"title": "Aspect Ratio (Dmodel / Nlayer)", "x_axis": "Aspect Ratio", "y_axis": "Loss Increase", "series": [{"label": "50M Params", "data": [[1e-1, 0.04], [1e0, 0.02], [1e1, 0.05]]}, {"label": "274M Params", "data": [[1e-1, 0.03], [1e0, 0.01], [1e1, 0.04]]}, {"label": "1.5B Params", "data": [[1e-1, 0.02], [1e0, 0.00], [1e1, 0.03]]}]}, {"title": "Attention Head Dimension (Dmodel / nhead)", "x_axis": "Attention Head Dimension", "y_axis": "Loss Increase", "series": [{"label": "dhead = 256", "data": [[1e-1, 0.05], [1e0, 0.04], [1e1, 0.06]]}, {"label": "dhead = 512", "data": [[1e-1, 0.03], [1e0, 0.02], [1e1, 0.04]]}, {"label": "dhead = 1024", "data": [[1e-1, 0.01], [1e0, 0.00], [1e1, 0.02]]}]}]}
```

Figure 6 Left: When we include embedding parameters, performance appears to depend strongly on the number of layers in addition to the number of parameters. Right: When we exclude embedding parameters, the performance of models with different depths converge to a single trend. Only models with few layers than 2 the layers or with extreme depth-to-width ratios deviate significantly from the trend.

```jsonl
{"figure": 6, "type": "x-y plots", "plots": [{"title": "Left: Parameters (with embedding)", "x_axis": "Parameters (with embedding)", "y_axis": "Test Loss", "series": [{"label": "0 Layer", "data": [[2e4, 7], [1e5, 6], [1e6, 5]]}, {"label": "1 Layer", "data": [[2e4, 6.5], [1e5, 5.5], [1e6, 4.5]]}, {"label": "2 Layers", "data": [[2e4, 6], [1e5, 5], [1e6, 4]]}, {"label": "3 Layers", "data": [[2e4, 5.5], [1e5, 4.5], [1e6, 3.5]]}, {"label": "6 Layers", "data": [[2e4, 5], [1e5, 4], [1e6, 3]]}, {"label": "> 6 Layers", "data": [[2e4, 4.5], [1e5, 3.5], [1e6, 2.5]]}]}, {"title": "Right: Parameters (non-embedding)", "x_axis": "Parameters (non-embedding)", "y_axis": "Test Loss", "series": [{"label": "1 Layer", "data": [[2e4, 6.5], [1e5, 5.5], [1e6, 4.5]]}, {"label": "2 Layers", "data": [[2e4, 6], [1e5, 5], [1e6, 4]]}, {"label": "3 Layers", "data": [[2e4, 5.8], [1e5, 4.8], [1e6, 3.8]]}, {"label": "6 Layers", "data": [[2e4, 5.5], [1e5, 4.5], [1e6, 3.5]]}, {"label": "> 6 Layers", "data": [[2e4, 5.3], [1e5, 4.3], [1e6, 3.3]]}]}]}
```

In this section we will display this analysis with experimentally-motivated findings, better differentiating theoretical analysis to later sections.

3.1 Approximate Transformer Shape and Hyperparameter Independence

Transformer performance depends very weakly on the shape parameters $n_{layer}$, $n_{head}$, and $d_{\text{ff}}$ when we hold the total non-embedding parameter count $N$ fixed. To establish these results we trained models with fixed size while varying a single hyperparameter. This was simplest for the case of $n_{head}$. When varying $n_{layer}$, we simultaneously varied $d_{model}$ while keeping $N \approx 12n_{layer}d_{model}^2$ fixed. Similarly, to vary $d_{\text{ff}}$ fixed model size we simultaneously varied $d_{model}$, as the memory is approximated by parameter counts in Table 1. Independence of $n_{layers}$ would follow if deeper Transformers effectively behave as ensembles of shallower models, as has been suggested for ResNets (VBNB`16). The results are shown in Figure 5.

3.2 Performance with Non-Embedding Parameter Count $N$

In Figure 6 we display the performance of a wide variety of models, ranging from small models with shape $(n_{layer}, d_{model}) = (2, 128)$ through billion-parameter models, ranging in shape from (8, 4288) through (207, 768). Here we trained to near convergence on the full WebText2 dataset and observe no outlying effect (except possibly for the largest models).

As shown in Figure 1 we find a steady trend with non-embedding parameter count $N$, which can be fit to the first term of Equation (1.3), so that

$L(N) \approx ( \frac{N}{\alpha} )^{-\alpha N}$ (3.1)

8



Transformers asymptotically outperform LSTMs due to improved use of long contexts

```jsonl
{"type": "x-y plot", "title": "Transformers asymptotically outperform LSTMs", "x_axis": "Parameters (non-embedding)", "x_scale": "log", "y_axis": "Test Loss", "y_scale": "linear", "series": [{"label": "LSTMs", "trend": "nearly flat/slight decrease"}, {"label": "Transformers", "trend": "steep power-law decrease", "sub_labels": ["1 Layer", "2 Layers", "4 Layers"]}]}
{"type": "x-y plot", "title": "LSTM plateaus after <100 tokens Transformer improves through the whole context", "x_axis": "Token Index in Context", "x_scale": "log", "y_axis": "Per-token Test Loss", "y_scale": "linear", "series": [{"label": "400K", "trend": "decrease then plateau"}, {"label": "400K", "trend": "decrease then plateau"}, {"label": "400K", "trend": "decrease then plateau"}, {"label": "2M", "trend": "lower plateau"}, {"label": "3M", "trend": "lower plateau"}, {"label": "200M", "trend": "lower plateau"}, {"label": "300M", "trend": "lowest plateau"}]}
```

Figure 7

To observe these trends it is crucial to study performance as a function of $N$; if we instead use the total parameter count (including the embedding parameters) the trend is somewhat obscured (see Figure 6). This suggests that the embedding matrix can be made smaller without impacting performance, as has been seen in recent work [LGG+ 19].

Although these models have been trained on the WebText2 dataset, their test loss on a variety of other datasets is also a power-law with $N$ identical per dataset, as has been seen in [8].

3.2.1 Comparing to LSTMs and Universal Transformers

In Figure 7 we compare LSTM and Transformer performance as a function of non-embedding parameter count $N$. The LSTMs were trained with the same dataset and the same context and length. We see from these figures that the LSTMs perform as well as Transformers in capturing early openings in the context, but cannot match the Transformer performance for later tokens. We present power-law relationships between performance and context position Appendix [D.5] where increasingly larger power-laws for larger models suggest improved ability to quickly recognize dependencies.

We also compare the performance of Standard Transformers to recurrent Transformers [DGV+ 18] in the appendix. The models re-use parameters, and so perform slightly better as a function of the $N$ at the cost of additional compute per parameter.

3.2.2 Generalization Among Data Distributions

We further test our models on a set of additional non-textual data distributions. The test loss on these datasets as a function of model size is shown in Figure 8. In all cases the models were trained only on the WebText2 dataset. We see that the loss on these other data distributions improves smoothly with model size, in direct parallel with the improvement on WebText2. We find that generalization depends almost exclusively on the in-distribution validation loss, and does not depend on the duration or proximity to convergence. We also observe no dependence on model depth (see Appendix [D.8]).

3.2.3 Performance with Dataset Size and Compute

We display empirical trends for the test loss as a function of dataset size (in $D$ tokens) and training compute $C$ in Figure 1.

For the trend with $D$ we trained a model with $(n_{hyper}, n_{embed}) = (36, 1280)$ on fixed subsets of the WebText2 dataset. We stopped training once the test loss started to decrease. We see that the resulting test losses can be fit with simple power-law

$L(D) \approx \left(\frac{D}{D_0}\right)^{-\alpha}$ (3.2)

in the dataset size. The data and fit appear in Figure 1.

The total amount of non-embedding compute used during training can be estimated as $C = 6NBS$, where $B$ is the batch size, $S$ is the number of update parameters, and the factor of 6 accounts for the forward and backward passes. Thus for a given value of $C$ we can scan over all models with various $N$ to find the model

9



[IMAGE: A figure containing two line graphs side-by-side.
Left graph: The y-axis is labeled "Test Loss" (ranging from 3 to 7). The x-axis is labeled "Parameters (non-embedding)" on a logarithmic scale (from 10^4 to 10^9). There are five colored lines descending from left to right. The legend identifies them as: WebText2 (Test) [blue with dots], Internet Books [orange with dots], Books [green with dots], Wikipedia [red with dots], and Common Crawl [purple with dots]. The WebText2 line is the lowest, while Common Crawl is the highest.
Right graph: The y-axis is labeled "Loss on Other Distribution" (ranging from 2.5 to 5.0). The x-axis is labeled "Test Loss on Training Distribution" (ranging from 5.0 down to 2.5, reversed). There are four lines: "Books during training" [dashed grey], "Wikipedia during training" [dashed orange], "Books at convergence" [blue dots], and "Wikipedia at convergence" [orange dots]. All lines trend downward from top-left to bottom-right.]

Figure 8 Left: Generalization performance to other data distributions improves smoothly with model size, with only a small and very slowly growing offset from the WebText2 training distribution. Right: Generalization performance depends only on training distribution performance and not on the phase of training. We compare generalization of converged models (points) to that of a single large model (dashed curves) as it trains.

with the best performance on step $S = \frac{C_{tot}}{6\eta \text{v}^2}$. Note that in these results the batch size $B$ remains fixed for all models, which means that this empirical results are not truly optimal. We will account for this in later sections using an adjusted $C_{min}$ to produce cleaner trends.

The result appears as the heavy black line on the left-hand plot in Figure $\boxed{1}$ it can be fit with

$$L(C) \approx \left(\frac{C}{C_c}\right)^{\alpha_C} \eqno(3.3)$$

The figure also includes images of individual learning curves to clarify when individual models are optimal. We will study the optimal allocation of compute more closely later on. The data strongly suggests that sample efficiency improves with model size, and we also illustrate this directly in Figure $\boxed{19}$ in the appendix.

# 4 Charting the Infinite Data Limit and Overfitting

In Section $\boxed{3}$ we found a number of basic scaling laws for language modeling performance. Here we will study the performance of a model of size $N$ trained on a dataset with $D$ tokens while varying $N$ and $D$ simultaneously. We will empirically demonstrate that the optimally trained test losses accord with the scaling law of Equation $\boxed{1.5}$. This provides guidance on how much data we would need to train models of increasing size while keeping overfitting under control.

## 4.1 Proposed $L(N, D)$ Equation

We have chosen the parameterization $\boxed{1.5}$ (repeated here for convenience):

$$L(N, D) = \left[ \left( \frac{N_c}{N} \right)^{\alpha_N} D \frac{D_c}{D} \right]^{\alpha_D} \eqno(4.1)$$

using three principles:

1. Changes in vocabulary size or tokenization are expected to rescale the loss by an overall factor. The parameterization of $L(N, D)$ (and all models of the loss) must naturally allow for such rescaling.
2. Fixing $D$ and sending $N \rightarrow \infty$, the overall loss should approach $L(D)$. Conversely, fixing $N$ and sending $D \rightarrow \infty$ the loss must approach $L(N)$.
3. $L(N, D)$ should be analytic at $D = \infty$, so that it has a series expansion in $1/D$ with integer powers. Theoretical support for this principle is significantly weaker than for the first two.

Our choice of $L(N, D)$ satisfies the first requirement because we can rescale $N_c, D_c$ with changes in the vocabulary. This also implies that the values of $N_c, D_c$ have no fundamental meaning.

10



Data Size Bottleneck
{"type": "x-y plot", "title": "Data Size Bottleneck", "x_axis": "Params (non-embed)", "y_axis": "Test Loss", "series": [{"label": "21M", "points": [[1e6, 4.3], [1e7, 3.7], [1e8, 3.5]]}, {"label": "86M", "points": [[1e6, 4.2], [1e7, 3.6], [1e8, 3.4]]}, {"label": "174M", "points": [[1e6, 4.1], [1e7, 3.5], [1e8, 3.3]]}, {"label": "344M", "points": [[1e6, 4.0], [1e7, 3.4], [1e8, 3.2]]}, {"label": "688M", "points": [[1e6, 3.9], [1e7, 3.3], [1e8, 3.1]]}, {"label": "1.4B", "points": [[1e6, 3.8], [1e7, 3.2], [1e8, 3.0]]}, {"label": "22.0B", "points": [[1e6, 3.7], [1e7, 3.1], [1e8, 2.9]]}]}

Overfitting
{"type": "x-y plot", "title": "Overfitting", "x_axis": "N/D_{m/d}", "y_axis": "L(D, N) - L(\u221e, D)", "series": [{"label": "21M", "points": [[1e-4, 0.01], [1e-3, 0.05], [1e-2, 0.15], [1e-1, 0.4]]}, {"label": "86M", "points": [[1e-4, 0.01], [1e-3, 0.04], [1e-2, 0.13], [1e-1, 0.35]]}, {"label": "174M", "points": [[1e-4, 0.01], [1e-3, 0.03], [1e-2, 0.11], [1e-1, 0.3]]}, {"label": "344M", "points": [[1e-4, 0.01], [1e-3, 0.02], [1e-2, 0.09], [1e-1, 0.25]]}, {"label": "688M", "points": [[1e-4, 0.01], [1e-3, 0.01], [1e-2, 0.07], [1e-1, 0.2]]}, {"label": "1.4B", "points": [[1e-4, 0.01], [1e-3, 0.01], [1e-2, 0.05], [1e-1, 0.15]]}, {"label": "22.0B", "points": [[1e-4, 0.01], [1e-3, 0.01], [1e-2, 0.03], [1e-1, 0.1]]}]}

Figure 9 Left: The early-stopped test loss L(N, D) depends predictably on the dataset size D and model size N according to Equation (1.3). For large D, performance is a straight power law in N. For a smaller fixed D, performance stops improving as N increases and the model begins to overfit. (The reverse is also true, see Figure 4). Right: The extent of overfitting depends predominantly on the Ratio N/D = \frac{N}{D}, as predicted in Equation (1.3). The line is our fit to that equation.

Since we stop training early when the test loss ceases to improve and optimize all models in the same way, we do not expect that larger models should always perform better than smaller models. But with fixed finite D, we also do not expect by model size to be capable of approaching the best possible loss (i.e the entropy of text). Similarly, a model with fixed size N will be capacity-limited. These considerations motivate our second principle. Note that if knowledge of L(N) at infinite D and L(D) at infinite N fully determines all the parameters in L(N, D), this implies that knowledge of L(N) at infinite D and L(D) at infinite N fully determines all the parameters in L(N, D).

The third principle is more speculative. Similarly to a general rule and some recent empirical work suggesting that loss scales to 1/\sqrt{D} vary at large D. Overfitting should be related to the variance, or the signal-to-noise ratio of the test set (see [A57]), and this should scale as 1/\sqrt{D}. This expectation should hold for any smooth loss function, since we expect to be able to apply the central limit theorem. However, we do not venture this claim, as the importance of 1/D corrections dominates over such sources of variance, such as the finite batch size and other limits on the efficacy of optimization. Without empirical confirmation, we would not be very confident of its applicability.

Our third principle explains the asymmetry between the roles of N and D in Equation (1.3). Very similar symmetric expressions [7] are possible, but they would not have a 1/D expansion with integer powers, and would require the introduction of an additional parameter.

In any case, we will see that our equation for L(N, D) fits the data well, which is the most important justification for our L(N, D) ansatz.

4.2 Results

We regularize all our models with 10% dropout, and by tracking test loss and stopping once it is no longer decreasing. The results are displayed in Figure 9, including a fit to the four parameters \alpha_N, \alpha_D, N_c, D_c in Equation (1.3).

{"type": "table", "caption": "Table 2 Fits to L(N, D)", "headers": ["Parameter", "\alpha_N", "\alpha_D", "N_c", "D_c"], "rows": [{"Parameter": "Value", "\alpha_N": "0.076", "\alpha_D": "0.103", "N_c": "6.4 \times 10^{13}", "D_c": "1.8 \times 10^{13}"}]}

We obtain an excellent fit, with the exception of the rows where the data runs has been reduced by a factor of 1024, to obtain a few \times 10^5 tokens. With such a small dataset, an epoch consists of only 40 repetitions per data-point. Perhaps such a tiny dataset represents a different regime for language modeling, as overfitting happens very early in training (see Figure 16). Note also that the parameters differ slightly from those obtained in Section 5 as here we are fitting the full L(N, D) rather than just L(N, \infty) or L(\infty, D).

To chart the borderlands of the infinite data limit, we can directly study the extent of overfitting. For all but the largest models, we see no sign of overfitting when training with the full 22B token WebText2 dataset, therefore so we can take it as representative of D = \infty. Thus we can compare it to the finite data limit by

* For example, one might have used L(N, D) = [(\frac{N}{N_c})^{\alpha_N} + (\frac{D}{D_c})^{\alpha_D}]^{-1}, but this does not have a 1/D expansion.

11



Critical Batch Size vs. Performance
```jsonl
{"type": "x-y plot", "title": "Critical Batch Size vs. Performance", "x_axis": "WebText2 Train Loss", "x_scale": "log", "x_ticks": ["10^1", "6 x 10^0", "4 x 10^0", "3 x 10^0"], "y_axis": "Critical Batch Size (Tokens)", "y_scale": "log", "y_ticks": ["10^3", "10^4", "10^5", "10^6"], "series": [{"label": "Empirical Bcrit, N = 3M", "style": "green dots/line"}, {"label": "Empirical Bcrit, N = 85M", "style": "blue dots/line"}, {"label": "Bcrit = 2.1 x 10^9 tokens * L^-4.8", "style": "red dashed line"}, {"label": "Noise Scale Measurement", "style": "orange dots/line"}]}
```

Figure 10 The critical batch size $B_{\text{crit}}$ follows the power law in the loss performance as the performance increase, and does not depend directly on the model size. We find that the critical batch size approximately doubles for every 13% decrease in loss. $B_{\text{crit}}$ is measured empirically from the data shown in Figure [18] but is also roughly predicted by the gradient noise scale, as in [MKAT18].

defining
$\delta L(N, D) = \frac{L(N, D)}{L(N, \infty)} - 1$ (4.2)

and studying it as a function of $N, D$. In fact, we see empirically that $\delta L$ depends only on a specific combination of $N$ and $D$, as shown in Figure [16] from the scaling law of the Equation shown in [L5], which implies

$\delta L \approx \left( 1 + \left( \frac{N}{N_c} \right)^\alpha \frac{D}{D_c} \right)^{-\alpha} - 1$ (4.3)

Note that at large $D$ this formula has a series expansion in powers of $1/D$.

We state that the variation in the loss with different version was resulting in $0.02$, which means that to avoid overfitting we minimum training with that threshold of convergence we require

$D \gtrsim (6 \times 10^5) N^{0.74}$ (4.4)

With this relation, models smaller than $10^8$ parameters can be trained with minimal overfitting on the 22B token WebText2 dataset, but our largest models will encounter some model overfitting. More generally, this relation shows that dataset size may grow sub-linearly in model size while avoiding overfitting. Note however that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training. We should also emphasize that this does not typically represent maximally compute-efficient training.

5 Scaling Laws with Model Size and Training Time

In this section we will demonstrate that a simple scaling law provides a good description for the loss as a function of model size and training time. First we will explain how to use the results of the [MKAT18] to define a universal training step $S_{\text{min}}$, which accounts for the fact that our models are not all trained to the same point at an optimal batch size. Then we will demonstrate that we can fit the model size and training time parameters of the loss equation using [L8]. Later we will use these results to predict the optimal allocation of training compute between model size and training time, and then confirm that prediction.

5.1 Adjustment for Training at $B_{\text{crit}}(L)$

A simple empirical theory for the batch size dependence of training was argued by [MKAT18] (see also [SLA+ 18] [ZLN+ 19]). This suggests that there is a critical batch size $B_{\text{crit}}$ for training; for $B < B_{\text{crit}}$ the batch size can be increased with very diminished degradation in compute-efficiency, whereas for $B > B_{\text{crit}}$ increases in $B$ result in diminishing returns. It was also argued that the gradient noise scale provides a simple

12



prediction for $B_{\text{crit}}$, and that neither the optimal batch size nor the compute diverge on model size except through the value of the loss that has been seen. Details can be used to predict how training time and compute will vary with the batch size. To utilize both training time and compute as effectively as possible, it is best to train with a batch size $B \approx B_{\text{crit}}$. Training at $B \geqslant B_{\text{crit}}$ minimizes the number of training steps, while $B \ll B_{\text{crit}}$ minimizes the use of compute.

More specifically, it was demonstrated for a wide variety of neural network tasks, the number of training steps $S$ and the number of data examples processed $E = BS$ satisfy the relationship

$$E \left( \frac{S}{S_{\min}} - 1 \right) \left( \frac{E}{E_{\min}} - 1 \right) = 1$$ (5.1)

when training to any fixed value of the loss $L$. Here $S_{\min}$ is the minimum number of steps necessary to reach loss $L$, while $E_{\min}$ is the minimum amount of data (non-embedding compute) necessary to reach loss $L$.

We demonstrate the relationship (5.1) for Transformers in Figure 18 in the appendix. This relation defines the critical batch size

$$B_{\text{crit}}(L) \equiv \frac{E_{\min}}{S_{\min}}$$ (5.2)

which is a function of the target value of the loss. Training at the critical batch size makes a roughly optimal time/compute tradeoff, requiring $2S_{\min}$ training steps and processing $E = 2E_{\min}$ data examples.

In Figure 10, we plotted the critical batch size and gradient noise scale as a function of the training loss for loss values in Figure 2. We see that $B_{\text{crit}}(L)$ is independent of model size, and depends only on the loss $L$. So are the predictions of MKAT18.

$B_{\text{crit}}(L) \approx \frac{B_z}{L^{1/\alpha B}}$ (5.3)

where $B_z \approx 2 \times 10^3$ and $\alpha B \approx 0.21$.

We have chosen this parametrization for $B_{\text{crit}}(L)$ because as the loss approaches its minimum value $L_{\min}$, the gradient noise scale is expected to diverge, and we expect $B_{\text{crit}}$ to track this noise scale. We do not know $L_{\min}$, as we see in our models that as approximating are, but limit $L \to 0$ since the entropy of natural language is non-zero. Since apparently $L_{\min}$ is much smaller than the values of $L$ we have achieved, we use a parametrization where $B_{\text{crit}}$ diverges as $L \to 0$,

We use $B_{\text{crit}}(L)$ to estimate the relation between the number of training steps $S$ while training at batch size $B = B_{\text{crit}}$ tokens and the number of training steps while training at $B \gg B_{\text{crit}}$. This is simply

$$S_{\min}(S) \approx \frac{S}{1 + B_{\text{crit}}(L)/B}$$ (minimum steps, at $B \gg B_{\text{crit}}$) (5.4)

for any given target value $L$. The loss also defines a critical value of the compute needed to train $L$ with a model of size $N$ if we were to train at $B = B_{\text{crit}}(L)$. This is

$$C_{\min}(C) \approx \frac{C}{1 + B/B_{\text{crit}}(L)}$$ (minimum compute, at $B \ll B_{\text{crit}}$) (5.5)

where $C = 6NBS$ estimates the (non-embedding) compute used in training.

5.2 Results for $L(N, S_{\min})$ and Performance with Model Size and Compute

Now we will use $S_{\min}$ defined in (5.4) to obtain a simple and universal fit for the dependence of the loss on model size and training time in the infinite data limit. We fit the stable, Adam-optimized training runs using Equation (1.6), repeated here for convenience:

$$L(N, S_{\min}) \approx \left( \frac{N_c}{N} \right)^{\alpha N} + \left( \frac{S_c}{S_{\min}} \right)^{\alpha S}$$ (5.6)

for the loss. We will introduce the actual parameters by stating the warmup period of the learning rate schedule, and a fit for the data.

---
^5 Although the critical batch size roughly matches the gradient noise scale, we are using direct measurements of $B_{\text{crit}}$ from Figures 18 and 10 for all our later analyses.

13



Performance vs Compute Budget plot: A line graph showing Test Loss on the y-axis (ranging from 2 to 8) against Parameters (non-embedding) on the x-axis (log scale from $10^4$ to $10^9$). Multiple curves of different colors represent different compute budgets (PP-days), with a color bar on the right ranging from $10^{-5}$ (blue) to $10^0$ (yellow). The curves generally trend downward as parameters increase, with the lowest loss achieved by the highest compute budget.

Performance vs Steps plot: A line graph showing Test Loss on the y-axis (ranging from 2.4 to 5.4) against Parameters (non-embedding) on the x-axis (log scale from $10^6$ to $10^9$). Multiple curves of different colors represent different numbers of training steps, with a color bar on the right ranging from $10^4$ (blue) to $10^5$ (yellow). The curves show a gradual decline in loss as parameters increase.

Figure 11 When we hold either total compute tuple or number of training steps fixed, performance follows $L(N, S)$ from Equation (5.6). Each value of compute budget has an associated optimal model size that maximizes performance. Mediocre fits at small $S$ are unsurprising, as the power-law equation for the learning curves breaks down very early in training.

```jsonl
{"Parameter": "$\alpha_N$", "Value": "0.077"}
{"Parameter": "$\alpha_S$", "Value": "0.76"}
{"Parameter": "$N_c$", "Value": "$6.5 \times 10^{13}$"}
{"Parameter": "$S_c$", "Value": "$2.1 \times 10^3$"}
```
Table 3 Fits to $L(N, S)$

With these parameters, we obtain the learning curve fits in Figure 4 Though the fits are imperfect, we believe they are quite compelling given the simplicity of Equation (5.6).

The data and fits can be visualized in a different and more interesting way, as shown in Figure 11 There we study the test loss as a function of model size while fixing either the total non-embedding compute $C$ used in training, or the number of steps $S$ for this use the fits we use Equation (5.3) and (5.4) along with the parameters above and Equation (5.6).

The power-law dependence of the loss on $S_{min}$ reflects the interplay of optimizer dynamics and the loss landscape. Since the fits are best late in training, when the loss may be approximately quadratic, the power-law should provide information about the spectrum of the Hessian of the loss. This universality suggests that the Hessian eigenvalue density is roughly independent of model size.

5.3 Lower Bound on Early Stopping Step

The results for $L(N, S_{min})$ can be used to derive a lower-bound (and rough estimate) of the step at which early stopping should occur when training data is limited. It is motivated by the idea that finite and infinite $D$ learning curves for a given model will be very similar until we reach $S_{min} \approx S_{stop}$. Thus overfitting should be proportional to the correction from simply extending from early stopping at $S_{stop}$. This will underestimate $S_{stop}$ because in reality the test loss will decrease more slowly when we have a limited $D$, and therefore we will require more training steps to reach the optimal test loss at finite $D$. This line of reasoning leads to the inequality

$S_{stop}(N, D) \gtrsim \frac{S_c}{[L(N, D) - L(N, \infty)]^{1/\alpha_S}}$. (5.7)

where $L(N, \infty)$ is the converged loss, evaluated with infinite available data. This inequality and its comparison to the empirical data is displayed in Figure 16 in the appendix. In that figure, the validity of $S_{stop}$ and $L(N, D)$ are empirical (though $S_{stop}$ is adjusted to mimic training at $B \gg B_{crit}$), while $L(N, \infty)$ is computed from the fit to $L(N, D)$ evaluated at $D = \infty$.

6 Optimal Allocation of the Compute Budget

We displayed the empirical trend of performance as a function of the computation used during training in the top-right of Figure 1 However, this result involved training fixed at a batch size $B$, whereas we know in

14



{"type": "x-y plot", "title": "Compute vs Model Size", "x_axis": "Deviation from Optimal Model (N/N_efficient)", "y_axis": "Excess Compute (C/C_efficient)", "data_points": "Curve starting at (10^0, 1.0) rising sharply to reach a plateau around (10^1, 3.8). Shaded region between approximately 0.6x and 2.2x optimal size is highlighted.", "annotations": ["Models between 0.6x and 2.2x the optimal size can be trained with a 20% larger compute budget"]}
{"type": "x-y plot", "title": "Steps vs Model Size", "x_axis": "Deviation from Optimal Model (N/N_efficient)", "y_axis": "Excess Steps (S/S_efficient)", "data_points": "Curve starting at (10^0, 1.0) and decaying downward towards (10^1, 0.5).", "annotations": ["Smaller models require more steps to train, while larger models require fewer", "Our framework does not capture early training dynamics"]}

Figure 12 Left: Given a fixed compute budget, a particular model size is optimal, though somewhat larger or smaller models can be trained with minimal additional compute. Right: Models larger than the compute-efficient size require fewer steps to train, allowing for potentially faster training if additional parallel-ism is possible. Note that this equation should not be trusted for very large models, as it is only valid in the power-law region of the learning curve, after initial transient effects.

{"type": "x-y plot", "title": "Test Loss vs Compute", "x_axis": "Compute (PF-days), non-embedding", "y_axis": "Test Loss", "data_points": "Log-log plot showing a downward trend of test loss as compute increases. A solid orange line represents a power-law fit L = (C/2.0 * 10^8)^-0.057. A dashed blue line represents L = (C_min/2.3 * 10^8)^-0.050.", "legend": ["L = (C_min/2.3 * 10^8)^-0.050", "L = (C/2.0 * 10^8)^-0.057"]}

Figure 13 When adjusting performance to simulate training far below the critical batch size, we find a somewhat altered power law for L(C_min) when compared with the fully empirical results. The conspicuous lump at 10^-5 PF-days marks the transition from 1-layer to 2-layers networks; we exclude 1-layer networks in the power-law fits. It is the L(C_min) trend that we expect to provide a reliable extrapolation for larger compute.

that in fact we can train more efficiently[6] by training at the batch size B_crit discussed in Section 5.1. Large and small values of the loss could have been achieved with fewer samples or fewer steps, respectively, and correcting for this inefficiency by synchronizing to the critical batch size results in cleaner and more predictable trends.

In this section we will adjust for this inefficiency. More importantly, and more importantly, we will use the results of Section 5 to determine the optimal allocation of compute between model size N and the quantity of data processed during training, namely 2B_crit S_min. We will determine this allocation both empirically and theoretically, by using the equation for (L, S, N_min), and we will demonstrate that these methods agree.

6.1 Optimal Performance and Allocations

Let us first study the loss as a function of the optimality allocated compute from Equation (5.5). The result is plotted in Figure 13 along with a power-law fit. We see that as compared to the compute plot of Figure 1 the new fit with C_min is somewhat improved.

Given L(C_min), it is natural to ask for the optimal model size N(C_min) that provides the minimal loss with a given quantity of training compute. The optimal model size is shown in Figure 14. We observe that N(C_min)

__________________________________________________________________
6One might ask why we did not simply train at B_crit in the first place. The reason is that it depends not only on the model but also on the target value of the loss we wish to achieve, and so is a moving target.

15



[IMAGE]: A scientific figure containing two line graphs side-by-side.
Left Graph: The y-axis is labeled "Parameters (non-embedding)" on a logarithmic scale from $10^3$ to $10^6$. The x-axis is labeled "Compute (PF-days), non-embedding" on a logarithmic scale from $10^{-7}$ to $10^{-1}$. There are two lines of data points: a blue dashed line and an orange dotted line. The blue line follows a power-law trend with the label $N = (1.3 \cdot 10^9) \cdot C^{0.73}$. The orange line follows a slightly lower trend with the label $N = (1.6 \cdot 10^9) \cdot C^{0.88}$.
Right Graph: The y-axis is labeled "Steps" on a linear scale from 0 to 15000. The x-axis is labeled "Compute (PF-days), excluding embeddings" on a logarithmic scale from $10^{-7}$ to $10^{-1}$. There are three lines: a solid blue line labeled "$S_{min}$ (adjusted)", a dotted blue line labeled $S_{min} = (5.4 \cdot 10^3) \cdot C_{min}^{0.03}$, and a solid orange line labeled "S (fixed-batch)". The blue lines stay relatively flat/low across the x-axis, while the orange line shows a sharp upward trend after $10^{-5}$.

Figure 14 Left: Each value of the compute budget $C_{min}$ has an associated optimal model size $N$. Optimal model size grows very rapidly with $C_{min}$, increasing by 5x for each 10x increase in compute. The number of data examples processed makes up the remainder of the increase, growing relatively modestly by 2x.
Right: The batch-adjusted number of optimization steps also grows very slowly, if at all, meaning that most of the growth in the data examples processed can be used for increased batch sizes.

can be fit very well with a power-law

$$N(C_{min}) \propto (C_{min})^{0.73}.$$ (6.1)

In Figure [12] we show the effects of training models of sub-optimal sizes (see Appendix [B.4]). By definition $C_{min} \equiv 6 N B_{crit} S$, and so we can use $N(C_{min})$ to extract further results. In particular, since prior bots shows $B \propto L^s$ and $L \propto N^{0.6}$, we can conclude that $B_{crit} \propto C_{min}^{0.24}$. This leads us to conclude that the optimal number of steps will only grow very slowly with compute, as

$$S_{min} \propto (C_{min})^{0.03},$$ (6.2)

matching the empirical results in Figure [14]. In the measured exponent the sufficiently small that our results may even be consistent with an exponent of zero.

Thus we conclude that as we scale up language modeling with an optimal allocation of computation, we should predominantly increase the model size $N$, while simultaneously scaling up the batch size via $B \propto B_{crit}$ with negligible increase in the number of serial steps. Since compute-efficient training runs relatively few optimization steps, additional work on speeding up early training dynamics may be warranted.

6.2 Predictions from $L(N, S_{min})$

The results for $L(C_{min})$ and the allocations can be predicted from the $L(N, S_{min})$ equation obtained in Section [5]. Given our equation for $L(N, S_{min})$, we can substitute $S_{min} = S_{min} = \frac{C_{min}}{6NB}$ and then find the minimum of the loss as a function of $N$, while fixing the training compute. We carry out this procedure in detail in Appendix [B] where we also provide some additional predictions.

For the loss as a function of training compute, we predict that

$$L(C_{min}) = \left( \frac{C_{min}^\alpha}{C_{min}} \right)^{\alpha_{min}}$$ (6.3)

where

$$\alpha_{min}^C \equiv \frac{1}{1/\alpha_S + 1/\alpha_B + 1/\alpha_N} \approx 0.054$$ (6.4)

in excellent agreement with the exponent of Figure [13]. We also predict that

$$N(C_{min}) \propto (C_{min})^{\alpha_{min}^C / \alpha_N} \approx (C_{min})^{0.71}$$ (6.5)

which also matches the scaling of Figure [14] to within a few percent. Our scaling laws provide a predictive framework for the performance of language modeling.

16



7.5
6.0
4.5
3.0
1.5
Test Loss
10⁻⁸ 10⁻⁵ 10⁻² 10¹ 10⁴ 10⁷
Compute (PF-days), non-embedding

```jsonl
{"type": "x-y plot", "title": "Test Loss vs Compute", "x_axis": "Compute (PF-days), non-embedding", "y_axis": "Test Loss", "series": [{"label": "L(Cmin)", "line_style": "dashed yellow", "trend": "downward slope"}, {"label": "L(D(C))", "line_style": "solid red", "trend": "downward slope"}], "annotations": [{"text": "The intersection point is sensitive to the precise power-law parameters", "location": "near intersection of lines"}]}
```

Figure 15 Far beyond the model sizes we study empirically, we find a contradiction between the equations our for $L(C_{min})$ and $L(D)$ due to the slow growth of data needed for compute-efficient training. The intersection marks the point before which we expect our predictions to break down. The location of this point is highly sensitive to the precise exponents from our power-law fits.

6.3 Contradictions and a Conjecture

We observe no signs of deviation from straight power-law trends at large values of compute, data, or model size. Our trends must eventually level off, though, since natural language has a non-zero entropy.

Indeed, for the compute-efficient training description in this section already contains an apparent contradiction. At scales several orders of magnitude above those documented here, the performance predicted by the $L(C_{min})$ scaling law decreases below what should be possible given the slow growth in training data with compute. This implies that our scaling laws must break down well before this point, but the conjecture that the intersection point has a deeper meaning: it provides an estimate of the point at which Transformer language models reach maximal performance.

Since the amount of data used by compute-efficient training grows slowly with the compute budget, the performance predicted by $L(C_{min})$ eventually limits a lower bound set by the $L(D)$ power law (see Figure 15). Let us work this out in our notation.

To keep overfitting under control, the results of Section 4 imply that should the dataset size as the dataset size as

$D \propto c^{0.74} \times C_{min}^{0.54}$ (6.6)

where we have used the compute-efficient $N(C_{min})$ from Figure 14

Let us compare this to the data requirements of compute-efficient training. If we train at the critical batch size (i.e., $C = 2C_{min}$) and never re-use training data during training, we find that data usage grows as

$D(C_{min}) = \frac{2C_{min}}{6N(C_{min})} \approx (4 \times 10^{10} \text{ tokens}) \ (C_{min}/\text{PF-Day})^{0.26}$ (6.7)

This is the maximum rate at which the dataset size can decrease asymptotically with compute growth, since it means that we are only training for a single epoch. But it grows that the dataset more slowly than in Equation 6.6. This appears to imply that compute-efficient training will eventually run into a problem with overfitting, even if the training process never re-uses examples.

According to Figure 1, once we are data-limited, once again, we have a data-limited regime. If we then are bottlenecked by the dataset size (i.e by overfitting), the loss should scale as $L(D) \propto D^{-0.05}$. This implies that the loss would scale with compute as $L(D(C_{min})) \propto C_{min}^{-0.01}$ once we are data-limited. Once again, we have a data-limited regime. This will eventually intersect with our prediction for $L(C_{min})$ from Figure 13 where we found a scaling $L(C_{min}) \propto C_{min}^{-0.050}$.

The intersection point of $L(D(C_{min}))$ and $L(C_{min})$ occurs at

$C \sim 10^4 \text{ PF-Days-N} \sim 10^{12} \text{ parameters}, \ D \sim 10^{12} \text{ tokens}, \ L \sim 1.7 \text{ nat/token}$ (6.8)

though the numerical values are only heuristically uncertain, varying by an order of magnitude or either direction depending on the precise values of the exponents from the power-law fits. The most obvious interpretation is that our scaling laws break down at or before we reach this point, which is still many orders of magnitude away in both compute and model size.

17



One might also believe that this intersection point has a deeper meaning, perhaps, if we mean that the model size beyond $N^*$ without which that intersection point disappears has a deeper meaning. In this interpretation, $C_{min}^*$ and $N^*$ may exceed all of the reliable information available in natural language data. In this interpretation, $L^*$ would provide a rough estimate for the entropy-per-token $\mathcal{H}$ of natural language. In this scenario, we would expect the loss trend to level off at $L^*$.

We can guess at the functional form of $L(C_{min})$ as it levels off by considering as levels off by considering a version of our training dataset without loss. For example, we could append a random string of tokens to each context then associate the dataset to the modified asset. However, since we are dealing with a loss boundary, then from the noise floor $L - L_{baseline}$ would be our meaningful error, with a decrease in this distance providing a qualitatively representing a significant boost in usefulness. Since the artificial noise would affect all of our trends equally, the critical point of $\text{[0.6]}$ would not change (aside from the absolute location of $L^*$), and may be meaningful even if it occurs after the $\text{[leveling]}$ leveling.

# 8 Discussion

Power laws from a rare axis with a wide spectrum of universality $\text{[TTH18]}$. Power-law scaling with model and dataset size in density estimation $\text{[Vedgo]}$ and forests models $\text{[Bia12]}$ suggest that power-law exponents have a very rough interpretation as the reverse of the number of relevant parameters of the system.

Some early $\text{[BB01]}$ $\text{[Geo01]}$ work independently discovered power-law scaling with model size; their data size showed similar trends $\text{[HNA*17]}$ $\text{[HAD19]}$ but with a different slope in the $\text{[interpretation]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*17]}$ found super-linear scaling of the per-token loss to the cluster size in the $\text{[logging]}$. Note however, that $\text{[HNA*1



We were able to precisely model the dependence of the loss on N and D, and alternatively on N and S, when these parameters are varied simultaneously. We used these relations to derive the compute scale, magnitude of overfitting, early stopping step, and data requirements when training large language models. So our scaling relations go beyond mere observation to provide a predictive framework. One might interpret these relations as analogues of the ideal gas law, which relates the macroscopic properties of a gas in a universal way, independent of most of the details of its microscopic constituents.

It is natural to conjecture that the scaling relations will apply to other generative modeling tasks with a maximum likelihood loss, and perhaps in other settings as well. To this purpose, it will be interesting to test these relations on other domains, such as images, audio, and video models, and perhaps also for random network distribution at. This point we do not know which of our results depend on the structure of natural language data, and which are universal. It would also be exciting to find a theoretical framework from which the scaling relations can be derived: a “statistical mechanics” underlying the “thermodynamics” we have observed. Such a theory might make it possible to derive other, even more precise predictions, and provide a systematic understanding of the limitations of the scaling laws.

In the domain of natural language, it will be important to investigate whether continued improvement on the loss translates into improvement on relevant language tasks. Smooth quantitative change can mask major qualitative improvements – “more is different”. For example, the smooth aggregate growth of the economy provides no indication of the specific technological breakthroughs that underwrite it. Similarly, the smooth improvements in language model loss may hide seemingly quantitative changes it underwrite it.

Our results strongly suggest that larger models will continue to perform better, and will be much more sample efficient than has been previously appreciate. Big models may be more important than big data. In particular, integrating into model parallelism was partitioned. Deeper models can be trained using deepening pipelining [HIC+ 18], which splits parameters depth-wise between devices, but eventually requires increased batch sizes as more devices are given use. Wider networks on the other hand are more amenable to parallelization [SCP+ 18], since large layers can be split between multiple workers with less serial dependency. Tensory [CGRS19][GRK17] or branching (e.g. [KSH12]) may allow for even faster training of large networks through increased model parallelism. And using methods like [WRH17][WYL19], which grow the networks as they train, it might be possible to push the boundary on the compute-efficient frontier for an entire training run.

Acknowledgements

We would like to thank Shan Carter, Paul Christiano, Ajay Clark, Ajeya Cotra, Ethan Dyer, Jason Eisner, Danny Hernandez, Jacob Hilton, Brice Menard, Chris Olah, and Ilya Sutskever for discussions and for feedback on drafts of this work.

19



# Appendices

## A Summary of Power Laws

For easier reference, we provide a summary below of the key trends described throughout the paper.

```jsonl
{"Parameters": "N", "Data": "∞", "Compute": "", "Batch Size": "Fixed", "Equation": "L(N) = (Nc/N)^αN"}
{"Parameters": "∞", "Data": "D", "Compute": "Early Stop", "Batch Size": "Fixed", "Equation": "L(D) = (Dc/D)^αD"}
{"Parameters": "Optimal", "Data": "∞", "Compute": "C", "Batch Size": "Fixed", "Equation": "L(C) = (Cc/C)^αC (naive)"}
{"Parameters": "Nopt", "Data": "Dopt", "Compute": "Cmin", "Batch Size": "B ≪ Bcrit", "Equation": "L(Cmin) = (Cmin^αN / Cmin)^αC"}
{"Parameters": "N", "Data": "D", "Compute": "Early Stop", "Batch Size": "Fixed", "Equation": "L(N, D) = (N/N0)^αN + D/D0^αD"}
{"Parameters": "N", "Data": "∞", "Compute": "S steps", "Batch Size": "B", "Equation": "L(N, S) = (N/N0)^αN + S/Smin(S,B)^αS"}
```
Table 4

The empirical fitted values for these trends are:

```jsonl
{"Power Law": "αN = 0.076", "Scale (tokenization-dependent)": "Nc = 8.8 × 10^13 params (non-embed)"}
{"Power Law": "αD = 0.095", "Scale (tokenization-dependent)": "Dc = 5.4 × 10^13 tokens"}
{"Power Law": "αC = 0.057", "Scale (tokenization-dependent)": "Cc = 1.6 × 10^7 PF-days"}
{"Power Law": "αmin = 0.050", "Scale (tokenization-dependent)": "Cmin = 3.1 × 10^8 PF-days"}
{"Power Law": "αB = 0.21", "Scale (tokenization-dependent)": "Bα = 2.1 × 10^8 tokens"}
{"Power Law": "αS = 0.76", "Scale (tokenization-dependent)": "Sc = 2.1 × 10^3 steps"}
```
Table 5

The optimal parameters for compute efficient training are given by:

```jsonl
{"Compute-Efficient Value": "Nopt = Ne * Cmin^pN", "Power Law": "pN = 0.73", "Scale": "Ne = 1.3 * 10^9 params"}
{"Compute-Efficient Value": "B ≪ Bcrit = Bα/N^pB * Cmin^pB", "Power Law": "pB = 0.24", "Scale": "Be = 2.0 * 10^6 tokens"}
{"Compute-Efficient Value": "Smin = Se * Cmin^pS (lower bound)", "Power Law": "pS = 0.03", "Scale": "Se = 5.4 * 10^3 steps"}
{"Compute-Efficient Value": "Dopt = De * Cmin^pD (1 epoch)", "Power Law": "pD = 0.27", "Scale": "De = 2 * 10^10 tokens"}
```
Table 6

## B Empirical Model of Compute-Efficient Frontier

Throughout this appendix all values of C, S, and αC are adjusted for training at the critical batch size Bcrit. We have left off the 'adj' label to avoid cluttering the notation.

### B.1 Defining Equations

The power-law fit to the learning curves implies a simple prescription for compute-efficient training. In this appendix, we will derive the optimal performance, model size, and number of training steps as a function of

20



the compute budget. We start with the Equation [1.6], referenced here for convenience:

L (N, S) = (N_c / N)^α_N + (S_c / S)^α_S. (B.1)

Here, S represents the number of parameter updates when training at the critical batch size [MKAT18], which was defined in Equation [5.2]:

B (L) = B_c / L^(1/α_B). (B.2)

We would like to determine optimal training parameters for a fixed compute budget, so we replace S = C / (6NB (L)), where C is the number of FLOPs used:

L (N, C) = (N_c / N)^α_N + (6B_c S_c / L^(1/α_B) C)^α_S. (B.3)

Now, we set ∂_N L|_C = 0 to find the condition for optimality:

0 = ∂_N L|_C
= ∂_N (N_c / N)^α_N + ∂_N (6B_c S_c / L^(1/α_B) C)^α_S
= - (α_N / N) (N_c / N)^α_N + (α_S / N) (6B_c S_c / L^(1/α_B) C)^α_S (1 - 1/5 N ∂_N L / L ∂_N C). (B.4)

Equation [B.3] and [B.4] together determine the compute-efficient frontier.

B.2 Efficient Training

We analyze the implications of [B.3], [B.4]. First, note that inserting [B.4] [B.3] yields

L (N_off (C), C) = (1 + α_S / α_N) L (N_off, ∞), (B.5)

which implies that for compute-efficient training, we should train to a fixed percentage α_N ≈ 10% above the converged loss. Next, let’s determine the optimal N for a given compute budget. Eliminating N yields a power-law dependence of the performance on compute:

L (C) = (C_c / C)^α_C. (B.6)

where we defined

α_C = 1 / (1/α_N + 1/α_B + 1/α_S) ≈ 0.052 (B.7)
C_c = 6 N_c B_c S_c (1 + α_N / α_S)^(1/α_S + 1/α_N)^(1/α_S) (α_S / α_N)^(1/α_S). (B.8)

Similarly, we can eliminate L to find N (C):

N (C) / N_c = (C / C_c)^α_C / α_N (1 + α_N / α_S)^(1/α_N). (B.9)

and

S (C) = C_c / (6 N_c B_c) (1 + α_N / α_S)^-α_N (C / C_c)^α_C / α_S. (B.10)

*There is a slight ambiguity: we can imagine either training at a constant batch size B (L_target), or we could instead train at a variable batch size B (L), where B (L) is the instantaneous critical batch size (as opposed to B, which is the averaged version [MKAT18]). These two prescriptions result in the same instantaneous number of steps, so we can ignore this subtlety (see...



B.3 Comparison to Inefficient

Typically, researchers train models until they appear to be close to convergence. In this section, we compare the efficient training procedure described above to this more typical setup. We define a the convergence factor $f$ as the percent deviation from the converged loss:

$L(N, C) = (1 + f)L(N, \infty)$. (B.11)

For compute-efficient training we have $f = \alpha_S / \alpha_N \approx 10\%$ from the previous section, but researchers typically use a much smaller value. Here, we choose $f' = 2\%$ as an estimate. For a fixed value of the loss, we predict:

{"equation": "N_f / N_{f'} = ((1+f)/(1+f'))^{1/\alpha_N} \approx 2.7"} (B.12)
{"equation": "S_f / S_{f'} = ((1+f)/(1+f'))^{1/\alpha_S} \approx 0.13"} (B.13)
{"equation": "C_f / C_{f'} = (N_f / N_{f'}) \cdot (S_f / S_{f'}) \approx 0.35"} (B.14)

So that compute-efficient training uses 7.7x fewer parameter updates, 2.7x more parameters, and 65% less compute to reach the same loss.

B.4 Suboptimal Model Sizes

We can solve A.1 to find an expression for the amount of compute needed to reach a given value of the loss $L$ with a model of size $N$:

{"equation": "C(N, L) = (6B_* S_{min} / L^{1/\alpha_N}) \cdot (N/N_*)^{\alpha_N} \cdot (L/N)^{-1/\alpha_S}"} (B.15)

Using A.6 and A.9, we can eliminate $L$ in favor of $N_{eff}$ (the model size, which reaches $L$ most efficiently. From there, we find an expression for the excess compute needed as a consequence of using a suboptimal model:

{"equation": "C(N, N_{eff}) / C(N_{eff}, N_{eff}) = (N / N_{eff}) \cdot [1 + (\alpha_S / \alpha_N) \cdot (1 - (N_{eff}/N)^{\alpha_N})^{1/\alpha_S}]"} (B.16)

The result is shown in Figure X. Models between 0.6x and 2.2x the optimal size can be used with only a 20% increase in compute budget. Using a smaller model is useful when accounting for the inference cost. A larger model can be trained to the same level of performance in fewer steps, allowing for more parallelism and faster training if sufficient hardware is available (see Figure Y):

{"equation": "S(N, N_{eff}) / S(N_{eff}, N_{eff}) = [1 + (\alpha_S / \alpha_N) \cdot (1 - (N_{eff}/N)^{\alpha_N})^{-1/\alpha_S}]"} (B.17)

A 2.2x larger model requires 45% fewer steps at a cost of 20% more training compute. Note that this equation should not be trusted for very large models, as it is only valid in the power-law region of the learning curve after initial transient effects.

C Caveats

In this section we list some potential soleworthiness caveats to our analysis.

- At present we do not have a solid theoretical understanding for any of our proposed scaling laws. The scaling relations with model size and compute are experimentally motivated. It may be possible to understand scaling at very large $D$ holding model size fixed [AS17], and also the shape of learning curves late in training, by modeling the loss with a noisy quadratic. But the scaling with $D$ at very large model size still remains mysterious. Without a theory or a systematic understanding of the corrections to our scaling laws, it's difficult to determine in what circumstances they can be trusted.

22



```jsonl
{"type": "plot", "title": "Early Stopping Step", "x_axis": "S_es * (L(N, D) - L(inf))^(-1/alpha)", "y_axis": "S_es", "data_points": "Scatter plot showing a positive linear correlation on a log-log scale. Data points are categorized by 'Data Size' ranging from 23M to 1.4B tokens. A red dashed line indicates a lower bound for early stopping.", "legend": {"23M": "purple", "43M": "blue", "86M": "green", "172M": "yellow", "344M": "orange", "688M": "red", "1.4B": "brown"}}
```

```jsonl
{"type": "plot", "title": "Loss vs Step", "x_axis": "Step", "y_axis": "Loss", "secondary_y_axis": "Dataset Size (Tokens)", "series": [{"label": "Test Loss", "style": "solid_line"}, {"label": "Train Loss", "style": "dashed_line"}], "description": "Log-log plot showing training and test loss decreasing as steps increase. Multiple curves are plotted for different dataset sizes (indicated by color), ranging from 10^8 to 10^10 tokens. Test loss curves eventually curve upward, while train loss continues to decrease."}
```

Figure 16 Left: We characterize the step on which early stopping occurs, as a function of the extent of overfitting. The red line indicates a lower bound for early stopping that is derived in Section 5.3 Right: We display train and test loss for a series of 300M parameter models trained on different sized dataset subsamples. The test loss typically follows that of a run done with unrestricted data until diverging. Note that the degree of overfitting (as compared to the infinite data limit) is significantly overestimated by $L_{test} - L_{train}$ (denoted by a black bar for each run).

* We are not especially confident in the prediction of $B_{crit}(L)$ for values of the loss far outside the range we have explored. Changes in $B_{crit}$ would have a significant impact on trade-offs between data parallelism and the number of serial training repetitions used, which would have a major impact on training time.

* We did not thoroughly investigate the small regime data, and our fits for $L(N, D)$ were poor for the smallest values of $D$ (where an epoch corresponded to only 40 steps). Furthermore, we did not experiment with regularization and data augmentation. Improvements in these could alter our results, quantitatively or qualitatively.

* We used the estimated training compute $\tau \approx 6 \times 10^{22} \text{FLOPs}$, which did not include contributions proportional to $n_{ctx}$ (see Section 2.1). So our scalings with compute may be confounded in practice in the regime of very large $n_{ctx}$, specifically where $n_{ctx} \gtrsim 12n_{model}$.

* We used learned training rates, and we experimented with learning rate schedule rates. But we may have neglected to tune some hyperparameter (e.g. initialization scale or momentum) that have an important effect on scaling.

The optimal choice of learning rate is sensitive to the target loss. When training close to convergence, it may be necessary to use a smaller learning rate to avoid divergences. But when conducting a short training run (eg due to compute limitations), it may be reasonable to use a larger learning rate. We did not experiment with higher learning rates for training runs that did not proceed to convergence.

D Supplemental Figures

D.1 Early Stopping and Test vs Train

In section 5.3, we described the result shown in Figure 16 which provides a prediction for a lower bound on the early stopping step. We also show train and test loss for a given model when trained on different sized datasets.

D.2 Universal Transformers

We compare the performance of standard Transformers to recurrent Transformers [DGV+18] in Figure 17. These models re-use parameters, and so perform slightly better as a function of $N$, but slightly worse as a function of compute $C$. We use several different different possible parameters for reasonable per-parameter re-use.

D.3 Batch Size

We measure the critical batch size using the data displayed in figure 18. This made it possible to estimate $B_{crit}(L)$ in figure 10.

23



[Image Description: A line graph with two side-by-side panels. The y-axis for both is "Test Loss" ranging from 2.5 to 4.5. The x-axis for both is "Parameters (non-embedding)" on a log scale from 10^5 to 10^9. Each panel contains four lines: a dashed black line ("Non-recurrent Models"), a blue line with circles ("2x Reuse"), a blue line with squares ("4x Reuse"), and a yellow line with diamonds ("8x Reuse"). The lines slope downwards from left to right, showing that as parameter count increases, test loss decreases. The 8x Reuse line (yellow) generally stays lower than the others for the same parameter count.]

Figure 17 We compare recurrent Transformers [DGV+18], which re-use parameters, to standard Transformers. Recurrent Transformers perform slightly better when comparing models with equal parameter count, but slightly worse when accounting for reuse and comparing for FLOP.

[Image Description: A figure with two side-by-side heat-map style plots. Both plots have "Step" on the x-axis (log scale from 10^2 to 10^5) and "Tokens Processed" on the y-axis (log scale). The left plot is titled "Batch Size Scan - 3M Params" and the right is titled "Batch Size Scan - 85M Params". The plots consist of multiple curved lines sloping upwards. To the right of each plot is a color bar labeled "Test Loss" ranging from 4 to 10, where colors transition from blue (low loss) to yellow (high loss). The curves represent different batch sizes, showing how tokens processed and loss evolve over training steps.]

Figure 18 These figures demonstrate for intuition (5.1) for a large number of values of the loss L, and for two different Transformer model size. These were used to measure Bcrit(L) for Figure 10.

D.4 Sample Efficiency vs Model Size

It is easy to see from figure 2 that larger models train faster, and are therefore more sample efficient. We provide another way of looking at this phenomenon in figure 19 which shows when different models reach various fixed values of the loss.

[Image Description: A figure with two side-by-side line plots. The x-axis for both is "Parameters (non-embedding)" on a log scale from 10^6 to 10^9. The left plot y-axis is "Minimum Steps (Smin)" on a log scale from 10^1 to 10^5. The right plot y-axis is "Minimum Examples (Emin)" on a log scale from 10^8 to 10^11. Both plots feature several lines of different colors (blue, green, yellow) that slope downwards as parameters increase. Each plot has a color bar on the right labeled "Loss" ranging from 2.5 to 5.5. The lines represent different loss thresholds; the higher the loss threshold, the lower the line sits on the y-axis.]

Figure 19 The number of minimum serial steps needed to reach any fixed value of the test loss decreases precipitously with model size. Sample efficiency (show here for training far below the critical batch size) improves greatly as well, improving by a factor of almost 100 when comparing the smallest possible model to a very large one.

24



[{"type": "x-y plot", "title": "Per-token Loss (774M Params)", "x_axis": "Token Index", "y_axis": "Per-Token Test Loss", "data_series": [{"label": "4.0 + 3.2 * T^-0.47", "trend": "decreasing power-law"}, {"label": "3.4 + 4.0 * T^-0.55", "trend": "decreasing power-law"}, {"label": "3.2 + 4.3 * T^-0.62", "trend": "decreasing power-law"}, {"label": "2.7 + 4.9 * T^-0.70", "trend": "decreasing power-law"}, {"label": "2.5 + 5.2 * T^-0.77", "trend": "decreasing power-law"}, {"label": "2.3 + 5.4 * T^-0.83", "trend": "decreasing power-law"}], "notes": "The plot shows a series of curves shifting downwards as model parameters increase (indicated by a color bar from 10^6 to 10^9).}"], [{"type": "x-y plot", "title": "Test Loss vs Model Parameters", "x_axis": "Parameters (excl. embedding)", "y_axis": "Test Loss", "data_series": [{"label": "Token 1/1024", "trend": "decreasing"}, {"label": "Token 2/1024", "trend": "decreasing"}, {"label": "Token 4/1024", "trend": "decreasing"}, {"label": "Token 8/1024", "trend": "decreasing"}, {"label": "Token 16/1024", "trend": "decreasing"}, {"label": "Token 32/1024", "trend": "decreasing"}, {"label": "Token 64/1024", "trend": "decreasing"}, {"label": "Token 128/1024", "trend": "decreasing"}, {"label": "Token 256/1024", "trend": "decreasing"}, {"label": "Token 512/1024", "trend": "decreasing"}, {"label": "Token 1024/1024", "trend": "decreasing"}, {"label": "Token n=8 (dashed)", "trend": "low baseline"}], "notes": "The x-axis is logarithmic from 10^4 to 10^9. Multiple lines represent different token positions within a 1024-token context."}]

Figure 20 This figure provides information about the performance per token as a function of model size or training time: Left: Loss per token as a function of its position T in the 1024-token context. Loss scales predictably as a power-law in T. Right: Test loss per token as a function of training step.

Figure 21 In addition to the averages, individual tokens within the 1024-token context also improve asymptotically as model size increases. Training runs with shorter context n_ctex = 8 (dashed lines) perform better on early tokens, since they allocate all their capacity to them.

D.5 Context Dependence

The trends for loss as a function of model size are different for different tokens in the context in Figure 20 [21]. We see that models trained on n_ctex = 1024 show steady power-law improvement in position of model size but all first token loss.

Fixing model size, it appears that the loss scales as a power-law as a function of position in the context in Figure 20. This may be a consequence of underlying power-law correlations in language [EP94] [ACDE12] [LT16], or a more general feature of the model architecture and optimization. It suggests some possibilities for better performance at T = 0 for 1024, but they also improve more quickly at early tokens, suggesting that larger contexts are more efficient at detecting short-range correlations and using that information. In the right-hand plot we show how per-token performance varies for a fixed model as a function of the training step. The training begins by learning short-range information, and over longer runs learns longer-range correlations in later in training.

We have also included models trained with a tiny context n_ctex = 8 in order to compare with our longer context models. Even modestly sized models trained on n_ctex = 8 can dominate our largest n_ctex = 1024 models on very early tokens. This also suggests that further improvements should be possible with much larger models trained on large contexts.

D.6 Learning Rate Schedules and Error Analysis

We experimented with a variety of learning rates and learning schedules. A host of schedules and resulting test performances for a small language model are plotted in Figure 22. We conclude that the choice of learning rate schedule is mostly irrelevant, as long as the total summed learning rate is sufficiently large, and the schedule includes a warmup period and a final decay to near-vanishing learning rate. Variations among

25



[{"type": "x-y plot", "title": "Learning Rate vs Step", "x_axis": "Step", "y_axis": "Learning Rate", "curves": [{"label": "cosine decay", "points": [[0, 0.001], [250000, 0]]}, {"label": "linear decay", "points": [[0, 0.001], [250000, 0]]}, {"label": "fast decay", "points": [[0, 0.001], [100000, 0.0002]]}, {"label": "slow decay", "points": [[0, 0.001], [250000, 0.0004]]}]}]
[{"type": "x-y plot", "title": "Loss vs LR Summed Over Steps", "x_axis": "LR Summed Over Steps", "y_axis": "Loss", "data_points": "Scatter plot showing a U-shaped trend where loss decreases from 3.85 down to 3.70 around 125 steps, then trends back up toward 3.80 by 250 steps."}]

Figure 22 We test a variety of learning rate schedules including cosine decay, linear decay, as well as other faster/slower decays schedules on a 3 million parameter model, shown on the left. For these experiments we do not decay to zero, since we find that this tends to give a fixed improvement upon close to the end of training. We find that, as long as the learning rate is not too small and does not decay too quickly, performance does not depend strongly on learning rate. Run-to-run variation is at the level of 0.05 in the loss, so averaging multiple runs is necessary to validate performance changes smaller than this level.

[{"type": "x-y plot", "title": "Test Loss vs Parameters", "x_axis": "Parameters (non-embedding)", "y_axis": "Test Loss (at convergence)", "x_scale": "log10", "curves": [{"label": "L = (N/8.8 * 10^13)^-0.076", "type": "power law fit", "points": [[1e4, 6], [1e9, 2]]}, {"label": "L = -0.25log(N/7.1 * 10^12)", "type": "logarithmic fit", "points": [[1e4, 6], [1e9, 2]]}], "data_points": "Scatter plot of black dots following the power law curve closely from 10^4 to 10^9 parameters."}]

Figure 23 The trend for performance as a function of parameter count, L(N), is better fit by a power law than by other functions such as a logarithm at a qualitative level.

schedules appear to be statistical noise, and provide a rough gauge for the scale of variation between different training runs. Experiments on larger models suggest that the variation in the final test loss between different random seeds is roughly constant in magnitude for different model sizes.

We found that larger models require a smaller learning rate to prevent divergence, while smaller models can tolerate a larger learning rate. To implement this, the following rule of thumb was used for most runs:

LR(N) ≈ 0.0032039 + −0.0001395 log(N) (D.1)

We expect that this formula could be improved. There may be a dependence on network width, likely set by the initialization scale. The formula also breaks down for N > 10^10 parameters. Nevertheless, we found that it works sufficiently well for the models we considered.

D.7 Fit Details and Power Law Quality

We experimented with a number of functional forms for the fits to L(N), L(C), and L(D); the power-law fits were qualitatively much more accurate than other functions such as logarithms (see Figure 23).

For L(C), we do not include small models with only 1 layer in the fit, as the transition from 1 to 2 layers causes a noticeable lump in the data. For L(N) we also do not include very small models with only 1 layer in the fit, and we exclude the largest models that have not trained fully to convergence. Fit parameters change marginally if we do include them, and the trend extrapolates well in both directions regardless.

D.8 Generalization and Architecture

In figure 24 we show that generalization to other data distributions does not depend on network depth when we hold the total parameter count fixed. It seems to depend only on the performance on the training distribution.

26



{"type": "line_plot", "title": "First Loss vs Depth", "x_axis": "Depth", "y_axis": "First Loss", "legend": ["Wikipedia", "Books", "Internet Books", "Common Crawl", "WebText2 (Train)", "WebText2 (Test)"], "data": [{"label": "Wikipedia", "points": [[10, 2.7], [30, 2.68], [100, 2.65]]}, {"label": "Books", "points": [[10, 2.8], [30, 2.78], [100, 2.7]]}, {"label": "Internet Books", "points": [[10, 2.75], [30, 2.72], [100, 2.62]]}, {"label": "Common Crawl", "points": [[10, 2.5], [30, 2.48], [100, 2.45]]}, {"label": "WebText2 (Train)", "points": [[10, 2.4], [30, 2.38], [100, 2.35]]}, {"label": "WebText2 (Test)", "points": [[10, 2.35], [30, 2.32], [100, 2.3]]}]}

Figure 24 We show evaluations on a series of datasets for models with approximately 1.5 Billion parameters. We observe no effect of depth on generalization; generalization performance depends primarily on training distribution performance. The 12-layer model overfit the Internet Books dataset and we show the early-stopped performance; we have not seen this surprising result in other experiments.

List of Figures

{"type": "table", "columns": ["Index", "Description", "Page"], "rows": [{"Index": 1, "Description": "Summary of simple power laws.", "Page": 3}, {"Index": 2, "Description": "Illustration of sample efficiency and compute efficiency.", "Page": 4}, {"Index": 3, "Description": "How to scale up model size, batch size, and serial steps", "Page": 4}, {"Index": 4, "Description": "Performance when varying model and data size, or model and training steps, simultaneously", "Page": 5}, {"Index": 5, "Description": "Weak dependence of performance on hyperparameter tuning", "Page": 8}, {"Index": 6, "Description": "Comparison of performance trend when including or excluding embeddings", "Page": 8}, {"Index": 7, "Description": "LSTM and Transformer performance comparison", "Page": 9}, {"Index": 8, "Description": "Generalization to other test datasets", "Page": 10}, {"Index": 9, "Description": "Universality of overfitting", "Page": 11}, {"Index": 10, "Description": "Critical batch size", "Page": 12}, {"Index": 11, "Description": "Performance versus compute budget or number of parameter updates", "Page": 14}, {"Index": 12, "Description": "Training on suboptimal models", "Page": 15}, {"Index": 13, "Description": "Comparison between empirical and adjusted compute trends", "Page": 15}, {"Index": 14, "Description": "Optimal model size and serial number of steps versus compute budget", "Page": 16}, {"Index": 15, "Description": "Contradiction between compute and data trends", "Page": 17}, {"Index": 16, "Description": "Early stopping lower bound and training curves for overfit models", "Page": 23}, {"Index": 17, "Description": "Universal transformers", "Page": 24}, {"Index": 18, "Description": "Batch size scans", "Page": 24}, {"Index": 19, "Description": "Another look at sample efficiency", "Page": 24}, {"Index": 20, "Description": "Power-law dependence of performance on position in context", "Page": 25}, {"Index": 21, "Description": "Performance at different context positions versus model size", "Page": 25}, {"Index": 22, "Description": "Learning rate schedule scan", "Page": 26}, {"Index": 23, "Description": "Comparison of Power-Law and Logarithmic Fits", "Page": 26}, {"Index": 24, "Description": "Generalization versus depth", "Page": 27}]}

27



List of Tables

{"table_id": 1, "content": {"1": "Parameter and compute counts for Transformer", "page": "7"}}
{"table_id": 2, "content": {"2": "Fits to (L, N, D)", "page": "11"}}
{"table_id": 3, "content": {"3": "Fits to (L, N, S)", "page": "14"}}
{"table_id": 4, "content": {"4": "Key trend equations", "page": "20"}}
{"table_id": 5, "content": {"5": "Key parameters to fit trends", "page": "20"}}
{"table_id": 6, "content": {"6": "Trends for compute-efficiency", "page": "20"}}

List of References

[ACDE12] Eduardo G Altman, Giampiero Costaldo, and Mirko Degli Esposti. On the general dimensionality of consciousness. *International Journal of Consciousness Studies*, 10(29):159–225, 2012. [25]

[AS17] Madhu A Svrnov and Alexei G Dimkovsky. On the dimensionality of dynamics generalization error in neural networks. *arXiv*, 2017. [1710.03667] [11] [18] [22]

[BB01] Michele Banko and Eric Brill. Scaling to very large corpora for commonplace linguistic bigation. In *Proceedings of the Ninth annual meeting of the Association for computational linguistics: proceedings*, pages 23–26. Association for Computational Linguistics, 2001. [18]

[BKM18] Michael Bekbulat, Denis Hubins, and Siyuuan Mo. Scaling laws for neural language model performance on downstream tasks. *arXiv*, 2018. [1812.11118] [18]

[Bregman13] G Bregman. Analysis of a random forest voter. *Journal of Machine Learning Research*, 14:1593–1611, 2013. [18]

[CCLS19] George Constantinou, Scott Lee, and Alejandro Ruiz. Scaling laws for language model performance. [1904.10509] http://arxiv.org/abs/1904.10509 [19]

[CKT18] George Constantinou, Wen-Chang Kenton, Lee Chen, and Kristian Toutanova. Bidirectional transformers for language understanding. [arXiv:1810.04805] [2]

[DGM18] G Dimkovsky, George Constantinou, and Lukas Kaiser. Universal scaling laws for language model generalization. *arXiv*, 2018. [1807.03819] http://arxiv.org/abs/1807.03819 [6] [23] [24]

[Ewing44] Ewings, Theoretical and Experimental Physics, 24(4), 1944. [25]

[EP] Common Crawl Foundation. [http://commoncrawl.org/] [?]

[GDR18] Guy Gur, Anirudha D. Roy, and Eivind Hauge. Other data, other weights: A study of data-dependent weight initialization. [arXiv:18.120475] [18]

[GZ19] George Constantinou, Arthur Vainshtein, Stefano Gagliardi, and Spencer Fischer. Generalizing a dense linyin a bidirectional transformer. *arXiv*, 2019. [1901.01608] [18]

[HGD19] Deborah Ghorbani, Shayan Ghorshani, and Behzad Yazdani. On the generalization of bidirectional transformers. *arXiv*, 2019. [1901.10159] http://arxiv.org/abs/1901.10159 [18]

[Gloo18] Gloo. A progress in language modeling. CCR, 0108005, 2018. http://arxiv.org/abs/cs.CL/0108005 [18]

[GK18] Scott R Gray and Diederik P Kingma. Gpu kernels for weights. weights.nai.com, 2018. [19]

[HAD19] Josh Newnum, Weshuarli Adrunai, and Ghorbani. Beyond high-level accuracy: Comprehensive challenges in Parallel Programming. In *PP'19, The 24th Symposium on Principles and Practice of Parallel Programming*, pp 1–19, New York, NY, USA, 2019. ACM. [doi:10.1145/3293883.3295710] [18]



[HCC+ 18] Yanping Zheng and Yunhang Chen, Optimizing Gnn Neural Networking, Liguan Nippon Ngiam, Puguinn Parallelism, CoRR, 1811.06965, 2018, 1811.06965 URL: http://arxiv.org/abs/1811.06965 [19]

[HNA+ 17] Joel Hetson, Sharanne Navas, Geoffrey E. Hinton, and others, Neural-network-based handwriting recognition, etc., edited, Md. Mostafiz Ali, 1712.00409 [18]

[Hoc+ 18] Arthur GAndWait, Franck Gabriel, and others, On the interaction between attention and generalization in Transformer-based networks, CoRR, 1808.05755, 2018 [18]

[HP 18] Benjamin P. Kingma and Jimmy Ba, Adam: A method for stochastic optimization, 1412.6980 [7]

Aaron Konat Kurumozaki, One epoch is all you need, arXiv:1906.06669 [18]

[IKS+ 19] Aleksandrs Grigorevs, Ilya Sutskever, and Geoffrey E. Hinton, Image classification with declarative networks, NIPS 2019, pages 102 - 115 [19] URL: http://dl.acm.org/citation_cfm?id=2999134_2999257 [19]

[Lao+ 19] Maxim Lupanov, Sean Gabonban, and others, BERT: a self-supervised learning framework for language understanding, 1909.11942 [9]

[LNB+ 19] Yulin Li, Nobu Yetter, and others, Robustly optimizing BERT, 1907.11692 [9] URL: http://arxiv.org/abs/1907.11692 [9]

[Lup+ 19] Mohammad Saleh, et al., Generating wikipedia summaries from long documents, 1907.11692 [9]

[Neu+ 18] Noam Shazeer, J. Liu, et al., 1801.10198 URL: http://arxiv.org/abs/1801_10198 [2 6]

[Lre+ 18] criticality and localization, 25 [25]

[Lee+ 19] Jaehoon Lee, et al., 1902.06720 [18]

[Luo+ 19] Xinyun Luan, et al., arXiv:1812.06162 [3 5 12 13 21]

[Pap+ 18] Radu Parasumanathan, et al., 1811.07062 URL: http://arxiv.org/abs/1811.07062 [18]

[LRS+ 19] Understanding BERT, 1909.12673 [2 6]

[RRBS18] Jonathan S. Rosenfeld, et al., 1909.12673 [18]

[RRS+ 19] Jonathan S. Rosenfeld, et al., arXiv:1909.12673 [18]

[Ruo+ 19] Colin Raffel, Noam Shazeer, et al., arXiv:1910.06833 [2]

[RWC+ 19] Alec Radford, et al., openai.com, [2 5 6 7 8]

[SCP+ 18] Noam Shazeer, et al., 1811.02084 [19]

[SHB15] Rico Sennrich, Barry Haddow, and others, 1508.07906 [6]



[SLA+ 18] Christopher E. Shin, Jaehoon Lee, Joe Benson, Stefan Adafaczky: Learning adaptive learning rates with sinusoidal memory. arXiv:1811.03600 [12]

[SS 18] Noonan Shaezer and Stefan Adafaczky: Adaptive learning rates with sinusoidal memory. CoRR, 1804.04235, http://arxiv.org/abs/1804.04235 [7]

[RR 18] Stefan Thurner, Rudolf Hahn, and Peter Holzapfel: On the connection of theory of convex systems to theory of optimization. CoRR, abs/1905.11946, 2019, [1905.11946, http://arxiv.org/abs/1905.11946 [18]

[VNB+ 17] Ashish Vaswani, Noam Shazeer, Niki Parmar, Jakob Uszkoreit, Llionaire Jones, Aidan N Gomez, Łukasz Kaiser, and Illia Polosukhin, et al. Attention is all you need. In Advances in Neural Information Processing Systems, 30 pages, 2017, www.nips.cc, [2] [6] http://papers.nips.cc/paper/7181-attention-is-all-you-need.pdf

[VNZ 18] Andreas Veit, Michael Servatius, and binge Weber, Singularities of neural networks. arXiv:1605.06431 [8] [18]

[Larr 06] Larry Wasserman, All of Statistics. Springer Science & Business Media, 2006. [18]

[WYA+ 19] Alex Wang, Yada Pruksachatkun, Nikita Nangia, Amanpreet Singh, Julian Michael, Felix Hill, Omer Levy, and Samuel R. Bowman, BERTScore: a stickier evaluation for text generation and language understanding systems, 2019, [1905.00057] [2]

[ZDY+ 19] Wen-you Deng, Romana Marianelli, and Vitaly Zavrel, CP-VCR: A Tuning-by-tuning in-context learning for VCR. In Proceedings on Computer Vision and Pattern Recognition (CVPR), 2019, doi:10.1109/cvpr2017.7323 [19] [19]

[WYL+ 19] Wei Wen, Peng Yan, and Hongyu Liu, Improving gradients in convolutional networks. arXiv:1906.02909 [19]

[ZHY+ 19] Yihong Zhu, Zianjing Yao, Dingying Jamie, Yaoming Janeiro, Ruslan Salakhutdinov, and Qingyu V. Xin, Generative attention: Generalizing autoregressive pre-training for language understanding, 2019, arXiv:1906.08237 [2]

[ZKN+ 16] Sergey Zagoruyko and Nikos Komodakis. Wide residual networks. Proceedings of the British Machine Conference 2016, 2016, doi:10.52244cc.30.87 [18]

[ZYK+ 18] Yukan Zhu, Ryan Kinos, Rich Zernik, Ruslan Salakhutdinov, Roque Tarquino, UlfromJson T. Yuksekov, and Banana Sliders. Finding alloys: A study on the correlation between the quality of movie reviews and watching movies. 2018. In Proceedings of the IEEE International Conference on Computer Vision (ICCV), Dec 2015, doi:10.1109/iccv.2015.11 [7]

[ZLN+ 19] Guodong Zhang, Lacha Nadory, Jamie Hansen, Sutskever, Sashanka Hegde, David C. Bassett, Christopher L. Shalev, and Roger B. grosse. Which alternatives match at which batch sizes? insights from a noisy quadratic model. CoRR, abs/1907.04164, 2019, [1907.04164 URL http://arxiv.org/abs/1907.04164 [12] [18]
