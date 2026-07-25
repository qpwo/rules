INFINI-GRAM MINI: Exact n-gram Search at the Internet Scale with FM-Index

Hao Xu $\diamondsuit$ Jiacheng Liu $\diamondsuit$ Yejin Choi $\diamondsuit$ Noah A. Smith $\diamondsuit \spadesuit$ Hannaneh Hajishirzi $\diamondsuit \spadesuit$
$\diamondsuit$ Paul G. Allen School of Computer Science & Engineering, University of Washington
$\spadesuit$ Allen Institute for AI $\diamondsuit$ Stanford University

Abstract

Language models are trained mainly on massive text data from the Internet, and it becomes increasingly important to understand this data source. Exact match-search engines enable searching in large text corpora — counting string appearances and retrieving the enclosing documents — yet they have high storage overheads their search efficiency on Internet-scale data. We present INFINI-GRAM MINI, an efficient and scalable system that can make petabyte-level text corpora searchable. Based on the FM-index data structure (Ferragina and Manzini, 2000), which simultaneously indexes and compresses text, our system creates an FM-index of the corpus with only size 4% of the corpus expenses. INFINI-GRAM mini greatly improves upon the best existing implementations of FM-indexes in terms of indexing speed (8$\times$) and memory during index building (32$\times$ reduction) and search latency (reduction to a negligible amount). We index 83TB of Internet text with 199 days on a single CPU node with 128 vCPUs (or 19 hours if using 137 such nodes). We show one important use case of INFINI-GRAM MINI in ML by using it as a source of benchmark contamination. We find significant core LM evaluation benchmarks to be heavily contaminated in Internet crawls (up to 74.2% in GSM8K), which could lead to overestimating the capabilities of language models if trained on such data. We host a benchmark contamination continuum to share the contamination rate of many core and community-contributed benchmarks. We also release a web interface and an API endpoint to serve general search queries on INFINI-GRAM MINI indexes.

[IMAGE: A technical diagram illustrating the workflow of the INFINI-GRAM MINI system. 
- On the left, a block labeled "Text Corpus" is represented by stacked horizontal lines. 
- An arrow points from the corpus to a central processing area labeled "Query string Q: Gravity on Earth is caused by" (where the phrase "Gravity on Earth is caused by" is highlighted in a yellow box). 
- Above this arrow, complexity notations "O(n)" and "O(|Q| log n)" are written. 
- To the right, a block labeled "Canonical Suffix Array" shows a vertical list of numbers, with a specific segment "6n" highlighted in a yellow box. 
- An arrow points from the suffix array back toward the query area with the notation "O(|Q| + Ha)". 
- Below the query, there is a box labeled "0.44e-11 (p)" and a small graph showing a count of "2 occurrences found". 
- To the right of this, a simulated document window is shown with the title "Document Living in Space". The text inside says "Gravity on Earth is caused by the mass of the planet, while on a space colony gravity is..." with the phrase "Gravity on Earth is caused by" highlighted in yellow. 
- The entire figure is captioned "Figure 1: Overview of INFINI-GRAM MINI. Based on the FM-index data structure, INFINI-GRAM MINI permits efficient exact match search in massive text corpora by compressing the index while reducing the search size down to 7% compared to a canonical suffix array index. Searching naively in the corpora would have time complexity of O(n) and is thus impractical; with INFINI-GRAM MINI, the search time complexity is independent of n (where n x 10^12 bytes is the length of string and query can be arbitrarily long, and Ha ≈ 2.1 is the zeroth-order entropy of the text corpus."]

1 Introduction

Modern language models (LMs) are trained mainly on text datasets downsampled from massive, petabyte-level text corpora like Common Crawl (Common Crawl Foundation, 2025). As these LMs are deployed more broadly, it becomes more pressing to understand the training data and its effects on model behavior (Liu et al., 2025; Han and Tsvetkov, 2022). As a starting point, we want to make these text corpora searchable; in particular, searching for exact matches of long sequences has gained increasing interest (Elazar et al., 2024; Merrill et al., 2024; Lu et al., 2024). The size of these corpora makes this problem extremely challenging, creat-

{"Project Home": "infini-gram-mini.io"}
{"Web Interface": "infini-gram-mini.io/demo"}
{"API Endpoint": "api.infini-gram-mini.io"}
{"Documentation": "infini-gram-mini.io/docs"}
{"Source Code": "infini-gram-mini.io/code"}
{"Contam Bulletin": "infini-gram-mini.io/bulletin"}



ing demand for more efficient indexing techniques.

Prior systems for exact-match search build indexes several times as large as the text corpora. Merrill et al. (2024) index 1.3TB of text with a suffix automaton (storage multiplier $\approx 29\times$). Liu et al. (2024) index 12TB of text with a suffix array (storage multiplier $\approx 6\times$). Elazar et al. (2024) index 1.2TB of text with the proprietary ElasticSearch engine (storage multiplier $\approx 2\times$). The size of these indexes renders it impractical to apply them to petabyte-level corpora.

We address this by indexing text corpora with FM-index (Ferragina and Manzini, 2000), a compact data structure frequently used in bioinformatics (Guo, 2020; Depuydt et al., 2007; Wang et al., 2018; Li, 2014), but not yet used for natural language data at scale. Prior work only applies this data structure to 13.4 GB dataset (Bevilacqua et al., 2022). We explain the FM-index in detail in $\S 2$. For natural text, the size of FM-index can be made as small as $0.26\times$ of the corpus size. To achieve low query latency, we build FM-index with a storage multiplier of $0.44\times$, or 7%, compared to the canonical suffix array.

INFINI-GRAM mini is the system we developed for efficiently constructing the FM-index at scale and answering search queries. For indexing, we extend and combine components from Liu et al. (2024), Gog et al. (2014), and Labret et al. (2017) into a highly parallelized program, which achieves $18\times$ speedup and uses only 32% as much RAM compared to the best existing implementation by SDSL (Gong et al., 2014). We use INFINI-GRAM mini to construct FM-index for two LM pretraining datasets — the Pile and DCLM-c saline — and 7 recent Common Crawl dumps from January to July 2025. Altogether, we index 83TB of text in 99 days with a single CPU node 128 with 28 CPUs, and this could be done over 19 hours if in parallelly organized parallelized across 1,500 nodes. We estimate that indexing the full Common Crawl would take 1,200 node-days, or 19 hours if parallelized across 1,500 nodes. For answering queries, we extend SDSL to work with on-disk index, reducing the RAM requirement to a negligible amount. Our inference engine supports retrieving the occurrences of a query string and retrieving documents that contain the query string, both within seconds when working on the above corpora.

We apply INFINI-GRAM mini to analyzing con-

---
$^1$https://github.com/simongog/sdsl-lite

**tamination of benchmarks** widely used in state-of-the-art LM evaluations ($\S 4$). INFINI-GRAM mini allows us to do the analysis on larger corpora than prior works and on new benchmarks uploaded. This would be much more expensive if using other indexing methods. We find several core benchmarks to be heavily contaminated ($\S 4.2$). INFINI-GRAM mini detects exact overlap for question of contaminated entries in text corpora, among which a large majority of questions appear together with the correct answer. This reveals a dire evaluation crisis: as benchmarks get increasingly contaminated by Internet crawls and consequently LM training data, evaluation results may give inflated estimates of true model capabilities. As such, we host a benchmark contamination bulletin to continually monitor contamination of core and community-contributed benchmarks on new Internet snapshots. We call for more community collaboration on this matter.

Beyond contamination analysis, INFINI-GRAM mini opens up more impactful use cases such as task-specific dataset construction and pretraining data curation. We release a web interface and API endpoint of INFINI-GRAM mini, so that everyone can search in the text corpora that we have indexed. We plan to continue indexing new corpora and share regular updates. We also release our source code. We hope this tool can enable more insightful analysis and better use of massive text corpora.

**2 Background: FM-index**

The FM-index (Ferragina and Manzini, 2000) is a full-text index that supports efficient pattern matching, counting, and text retrieval on a highly compressed representation of the text corpus. Compared with a canonical suffix array, FM-index stores a compact variation of a suffix array and the text corpus, greatly reducing storage overhead.

**2.1 Data Structures and Implementation**

On a high level, FM-index has two core components: (1) a sampled suffix array and its inverse, and (2) the Burrows-Wheeler transform represented using a Huffman-shaped wavelet tree. Below we describe each component as applied to a single string (appropriate for our application; see $\S 3$). Figure 2 shows a toy example illustrating the data structure.

**Suffix array and sampling.** The suffix array $SA$ of a string $T$ of length $n$ is an array of integers



Figure 2: The FM-index data structure (§2.1) used in INFINI-GRAM MINI, shown for a toy string with length $n = 7$. The suffix array is sampled with a sampling rate $\alpha = 3$ and only entries corresponding to bolded suffixes are stored. The BWT can be derived from the SA, and is stored compressed as a Huffman-shaped wavelet tree.

```jsonl
{"type": "diagram", "content": {"title": "FM-index Data Structure", "components": [{"name": "SA(T)", "values": [6, 5, 3, 1, 0, 4, 2]}, {"name": "Suffix Array", "mapping": {"0": "banana$", "1": "anana$", "2": "nana$", "3": "ana$", "4": "na$", "5": "a$", "6": "banana$"}}, {"name": "Text T", "value": "banana$"}, {"name": "Burrows-Wheeler Transform", "mapping": {"BWT(T)": "annbba$"}}, {"name": "Huffman-shaped wavelet tree", "structure": "Hierarchical tree mapping symbols to binary paths"}]}}
```

from the index.

**Huffman-shaped wavelet trees.** Given the BWT $L$ and the sampled SA, we can reconstruct the original string $T$ from these data structures so we don't need to store $T$. However, $L$ still takes as much storage as $T$. To further compress $L$, a wavelet tree is used to represent $L$ by hierarchically partitioning the alphabet. In the wavelet tree, each leaf node represents a symbol in the alphabet, and each non-leaf node stores a bitvector marking whether the symbol at each position of $L$ belongs to the left or right subtree. A Huffman-shaped wavelet tree (Mäkinen and Navarro, 2005) optimizes the hierarchy based on group and symbol frequencies based on their share distribution in $L$. Storing in costs $(n H_0 + 2 \log \sigma)$ n bits, where $\sigma$ is the alphabet size, and $H_0$ is the zeroth-order entropy of $L$ and $H_0 \approx 0.26 \log \sigma$ in our experiments with natural language text; this is smaller than storing $L$ directly, which is $n \log_2 \sigma$ bits. Besides storage, the compression tree can efficiently support two operations crucial to $LF$ mapping: $\text{rank}(c, \ell)$ counts the number of occurrences of a character $c$ in $L[0 \dots \ell-1]$, and $\text{select}(c, m)$ finds the position of the $m$th occurrence of $c$ in $L$. Both operations have a time complexity of $O(H_0)$.

**Inverse suffix array.** To allow recovery of the original string $T$ from any given position $\rho$, an FM-index also stores a sampled version of the inverse suffix array $ISA$. $ISA$ is the inverse of the permutation $SA$ specified in $\text{SA}$ and is defined as the permutation $ISA[i] = j$ if $SA[j] = i$. When reconstructing $T$ from a given position, $ISA$ is used to identify the corresponding rank in the suffix array to steer the current reconstruction. In FM-index, we can use a different sampling rate for $ISA$, storing only every $k$th entry, where $k$ can be larger than $\alpha$ as used in $ISA$ is used.

**2.2 Querying the FM-index**

There are three basic operations supported by FM-index: *find*, *locate*, and *recount*. Counting a pattern can be done with a find operation, and retrieving segments containing the pattern from the original string can be done with a combination of all three operations. See App. $\S$A for more details.

**3 INFINI-GRAM MINI**

We develop INFINI-GRAM MINI as a scalable and efficient implementation of FM-index on natural

____________________
$^2$zeroth-order entropy measures the unigram probability distribution of individual symbols (i.e. UTF-8 characters).



language data. For indexing, INFINI-GRAM MINI achieves $18\times$ speedup and reduces RAM usage by $3.2\times$ compared to the previous best existing implementation in SDSL. This paves the way for indexing petabyte-level text corpora with reasonable time and compute. For querying, we extended the implementation in SDSL to work with indexes stored on disk with offsets, which allows us to query large indexes on machines with limited RAM.

In contrast to the original infini-gram which tokenizes the text, we construct the index directly on raw UTF-8 bytes, which saves tokenization time and allows more flexibility when querying. Tokenization is a technique to reduce the index size, but since FM-index is already a compressed index, tokenization would not be helpful here.

3.1 Index Construction

Optimizing the indexing steps. We use a parallelized implementation of each indexing step. For building SA and BWT, we adapt from the parallel implementation in Liu et al. (2024). For building the wavelet tree, we use the parallel implementation in Labeit et al. (2017). For sampling SA and creating a sampled version of ISA, we parallelized the implementation in SDSL. All these steps were single-threaded in SDSL, and by parallelizing them we achieve significant speedup.

We measured the time and peak RAM usage for indexing an 8.7 GB corpus (a single file from DCLM-baseline) using implementation of SDSL and INFINI-GRAM MINI. Their implementation required 5,847 seconds and 74,807 MB of peak RAM, whereas INFINI-GRAM MINI completed indexing in 324 seconds ($18\times$ speedup) and peak RAM of 23,742 MB ($3.2\times$ reduction).

Preprocessing. FM-index is designed to work on a single string. To index a text corpora, which consists of a collection of documents, we encode all documents with UTF-8 and concatenate all these byte arrays with the \x00 byte (not used by UTF-8) to mark document boundaries. We then construct FM-index for this big byte string. We work on UTF-8 bytes rather than characters to keep the alphabet size small ($\sigma = 256$). Along with this text index, we store a store offset file that records the starting position of each document, which is useful for retrieving whole document and metadata. Aside from the actual text, we also index the metadata of the documents to make the metadata searchable while storing it in a similar compressed form.

Partitioning. For large corpora, we partition it into multiple shards and index each shard independently. Searching across multiple shards produces the same result as searching a single, larger index, and this allows us to build indexes with limited RAM per node as well as embarrassingly parallelize across multiple nodes.

Indexed corpora. We have built the index for the following corpora: the Pile (1.3 TB training set, 1.4G validation set; Gao et al., 2020), DCLM-baseline (17TB; Li et al., 2024), and the Common Crawl between January and July 2025 (CC "2025-05-20" to "CC-2025-07", 6 crawls and 765B total; Common Crawl Foundation, 2025).

Indexing time. We use CPU nodes with 128 vCPUs and 2TB RAM to construct indexes, and under this constraint, each shard can be as large as 700GB which can be indexed in 12-19 hours. Table 1 reports the time for indexing the index based on the above corpora. Indexing time shows a slightly super-linear increase with respect to shard size, and we report detailed stepwise breakdown in App. §B. If we were to index the full Common Crawl dataset (about 1PB), we would split it into 1,500 shards, which can be indexed in 1,200-node hours.

Index size. We choose the sampling rate empirically to balance storage savings and query latency. In our implementation, we sample every $a = 32$ entries of SA and every $b = 64$ entries of ISA. This yields indexes with $0.44\times$ the size of the corpora (Table 1). Conceptually, if we set $a$ and $b$ to be very big, then the SA and ISA would have negligible size, and the index can be as small as $0.26\times$ the size of the corpus.

3.2 Querying with INFINI-GRAM MINI

Similar to other exact-match search engines, INFINI-GRAM MINI supports two types of query: counting the number of occurrences of a string, and retrieving documents that contain a string. At query time, INFINI-GRAM MINI keeps all index files on disk as memory-mapped files, thus requiring only minimal CPU and RAM (loading all indexes uses only $\sim$ 30 MB RAM if 1 vCPU in our setting).

Querying in INFINI-GRAM MINI is slower and more complex than in canonical suffix arrays due to the compressive nature of FM-index. The SA is subsampled, and the original text is shuffled and

__________________________________________________________________
³We extract text from the CC crawls with resiliparse, following Li et al. (2024).



Dataset | Original Size (TB) | Indexing Time (CPU node-days) | Index Size (TB) | Num. Shards
--- | --- | --- | --- | ---
{"Pile-validation": {"original_size": "0.0001345", "indexing_time": "0.00057", "index_size": "0.000602 (0.45x)", "num_shards": "1"}}
{"Pile-main": {"original_size": "1.308", "indexing_time": "1.31", "index_size": "0.588 (0.45x)", "num_shards": "2"}}
{"DCLM-baseline": {"original_size": "16.666", "indexing_time": "12.6", "index_size": "7.523 (0.45x)", "num_shards": "25"}}
{"CC-2025-05": {"original_size": "9.079", "indexing_time": "11.8", "index_size": "3.972 (0.44x)", "num_shards": "15"}}
{"CC-2025-08": {"original_size": "8.163", "indexing_time": "10.6", "index_size": "3.574 (0.44x)", "num_shards": "15"}}
{"CC-2025-13": {"original_size": "10.393", "indexing_time": "13.6", "index_size": "4.563 (0.44x)", "num_shards": "17"}}
{"CC-2025-18": {"original_size": "10.498", "indexing_time": "13.7", "index_size": "4.664 (0.44x)", "num_shards": "17"}}
{"CC-2025-21": {"original_size": "9.221", "indexing_time": "12.1", "index_size": "4.302 (0.46x)", "num_shards": "15"}}
{"CC-2025-26": {"original_size": "8.724", "indexing_time": "11.4", "index_size": "3.835 (0.44x)", "num_shards": "15"}}
{"CC-2025-30": {"original_size": "9.006", "indexing_time": "11.7", "index_size": "3.952 (0.44x)", "num_shards": "15"}}
{"Total": {"original_size": "83.059", "indexing_time": "98.8", "index_size": "36.884 (0.44x)", "num_shards": "137"}}

Table 1: Text corpora used for extracting INFINI-GRAM MINI, along with indexing time and index size. The reported numbers only include actual document content and do not include metadata.

compressed, both requiring additional random disk reads to recover. The latency of queries is largely determined by the disk I/O performance.

Counting a string. We use FM-index's find op-eration to count the number of occurrences of a query string. We parallelize this operation across all k shards, and report the sum of counts across all shards. The total number of random disk reads is O(k(ql/H)ln), where ql is the length of the query string. If the disk I/O's throughput is high enough, the query latency can be as low as O(ql/H).

Retrieving documents. We first use the find op-eration to get the range in the SA that corresponds to the query string. Each occurrence in this range indi-cates one appearance of the query string in the text corpus. If the index has multiple shards, we will have one range for each shard. For each occurrence in this range, we use the locate operation to find the position of the match in the original text T, then get the boundaries of the enclosing document with a binary search in the text offset file, and finally use the reconstruct operation to get the document text. The number of random disk reads for locate is O(Hln), and for reconstruct is O((b + d - H)ln) where d is the document length. We implement reconstruct operation by dividing document text into up to t = 10 chunks, or chunks of length 100 if d < 1000. Retrieving a single document is parallelized across t threads, and it can be further parallelized for retrieving multiple documents.

Query latency. INFINI-GRAM MINI presents second-level latency on both types of queries. We benchmark the query latency with the index files stored on Google Cloud Platform (GCP) SSD disks with 80,000 IOPS and 1200 MB/s throughput. We measure the average latency over 100 queries for each query type and setting. For counting, short queries (|ql| <= 10) can be handled within 0.4 seconds for all corpora we indexed, while longer queries (|ql| = 1000) are handled in 8 seconds for CC-2025-05 and 25 seconds for DCLM-baseline. The difference in query latency is caused by the different number of shards in the corpora. For doc-ument retrieval, retrieving a snippet of d = 100 bytes can be done within 2 seconds for all corpora, and retrieving d = 3000 bytes takes up to 4.5 seconds. See App. B§ for benchmarking details.

3.3 Web Interface and API Endpoint
We host a Hugging Face interface for easy access to counting and document retrieval with 83 TB corpora (App. E). We also release an API endpoint that allows programmatically submitting requests.

4 Analyzing and Monitoring Benchmark Contamination with INFINI-GRAM MINI
In this section, we showcase how INFINI-GRAM MINI can be used to analyze benchmark contam-ination at scale. INFINI-GRAM MINI allows us to search in the largest body of text ever in the open-source community with minimal storage over-head, enabling large-scale benchmark contamination analysis at low cost. Our work also supports analyzing new benchmarks uploaded later.

By identifying lexical overlap between 24 widely-used evaluation benchmarks and three ma-



jor corpora, we find non-trivial contamination across 12 benchmarks (§4.2). We then retrieve documents accounting for contamination for further analysis (§4.3). Powered by INFINI-GRAM MINI, we develop a monitoring system with a benchmark contamination bulletin, where we continuously monitor new crawls and report benchmarks' contamination status over time (§4.4). We also invite the community to contribute by suggesting additional benchmarks or uploading new ones for analysis.

INFINI-GRAM MINI has further potential applications where exact-match search is needed, including (1) task-specific dataset construction, where creation of these datasets requires retrieving documents containing specific terms or phrases, and (2) data curation, where INFINI-GRAM MINI can assist in identifying and removing duplicate, low-quality, or sensitive text and documents. We leave these directions for future exploration.

4.1 Setup

Contamination detection method. We check contamination of test examples by measuring lexical overlap with the text corpus, following standard practice in the literature (Brown et al., 2020; Liama Team, 2024). Given a text entry, we extract all 50-character substrings S with a stride of one word, and determine entry contamination based on the proportion of substrings that appear at least once in the corpus:

```jsonl
{"formula": "\u03b7 = \u03c3_{s \in S} \u2208 \u2113 [count(s) > 0] / |S|"}
```

We classify an entry into three contamination levels using the same thresholds with different naming as Touvron et al. (2023):

* Clean, if $\eta < 20\%$
* Suspicious, if $20\% \le \eta < 80\%$
* Dirty, if $\eta \ge 80\%$

Benchmarks. To comprehensively evaluate contamination, we analyze a broad range of benchmarks widely used to evaluate state-of-the-art LLMs. We categorize benchmarks into five groups based on their primary focus: (1) knowledge and reasoning, i.e., understanding and reasoning over factual and conceptual knowledge; (2) math, i.e., mathematical reasoning ability; (3) code generation and modification abilities; (4) commonsense understanding, i.e., commonsense knowledge and reasoning abilities; and (5) reading comprehension of textual context. In total, we analyze 24 benchmarks; see App. §F for the full list and citations.

For benchmarks with question-answering format, we specifically check the question entries for contamination. Questions are usually longer and contain sufficient contextual information to uniquely identify the benchmark example, while the answers can be short, such as a multiple choice option or a number. For language understanding tasks that involve reading a context or paragraph before answering, we check the context entries for contamination.

We evaluate on only the test set of benchmarks. For benchmarks with large test set, we downsample to 1,000 entries for efficiency. For benchmarks with multiple subtasks, we sample proportionally from each subtask to maintain representative distribution.

4.2 Results

Table 2 reports the percentage of dirty entries in benchmarks against the Pile (knowledge cutoff in 2020), DCLM-baseline (knowledge cutoff in 2022), and seven CC crawls (knowledge cutoff in January-July 2025). The detailed count of dirty and suspicious entries can be found in App. §G.

Many widely-used benchmarks are highly contaminated. When checking against DCLM-baseline, MLMU has 27.70% dirty entries, MMLU-Pro has 16.20%, ARC-Challenge has 32.6%, and ARC-Easy has 32.3%. These benchmarks, especially MMLU, have been used to evaluate virtually every new LLM in recent years. We believe that the observed contamination levels are a strong signal that many recently reported results may overestimate language model abilities on truly new, unseen evaluation items.

Larger and newer corpora show greater contamination. Compared with Pile-train, most benchmarks show a higher dirty rate on DCLM-baseline. For example, ARC-Challenge has $17\times$ more and MMLU has $1.1\times$ more dirty entries. However, CC-2025-05 has lower contamination rate than DCLM-baseline on most benchmarks, which is likely because DCLM-baseline corpus is larger and is a high-quality subset of crawls spanning a decade, while CC-2025-05 is an unfiltered single crawl.

Contamination level varies by domain. Benchmarks in historically-significant domains, such as commonsense understanding (e.g., ARC,



OpenbookQA) and reading comprehension (e.g., SQuAD, CoQA), tend to show higher dirty rates on all corpora, while those in domains like math (e.g., HumanEval, GSM8K, MATH-500) and code (e.g., HumanEval, LiveCodeBench) remain relatively clean at this century.

**New benchmarks are gradually getting contaminated in recent corpora.** Newer benchmarks are initially clean on earlier corpora. For example, AIME-2024 is uncontaminated on Pile-train and DCLM-baseline, but show 40.00% dirty rate on the more recent CC-2025-21; similarly, GPQA shows 2.70% dirty rate on CC-2025-21. GSM8K is very clean on Pile-train, DCLM-baseline, and CC-2025-crawls, but has 74.2% dirty entries on CC-2025-21.

4.3 Analysis

With INFINI-GRAM MINI, we can retrieve documents from the corpora that contain contaminated examples for further analysis. We use LLM-as-a-judge to categorize all dirty instances in Pile-train, DCLM-baseline, and CC-2025-05 into following scenarios (see App. §1 for more details), with examples in Table 3:

• Type 1: Question and answer appear as-is in corpus. The question and correct answer appear in the corpus in the exact format as well as in the benchmark entry. The model may memorize entirely from these data instead of performing capabilities tested in examined benchmarks. This accounts for 72.5% of all dirty entries in Pile-train, 82.6% on DCLM-baseline, and 58% on CC-2025-05.

• Type 2: Question appeared, answered in natural language. The answer is expressed in natural language rather than the exact benchmark format, and model may indirectly infer the correct answer from it. This accounts for 4.5% of all dirty entries in Pile-train, 2.1% on DCLM-baseline, and 5.2% on CC-2025-05.

• Type 3: Question appeared, no corresponding answer. The question (and maybe the multiple-choice options) appeared in the corpus, but correct answer is missing. This accounts for 18.1% of all dirty entries in Pile-train, 10.9% on DCLM-baseline, and 30.2% on CC-2025-05.

• Type 4: False positives. There are documents that superficially match the question but are unrelated to the benchmark example. This happens on entries with a very short question. This accounts for 3.1% of all dirty entries in Pile-train, 1% on DCLM-baseline, and 3% on CC-2025-05.

We show 7 contamination examples in App. §H and their source of contamination. We found that contamination is caused by (1) the benchmark is sourced from the Internet (e.g., AIME-2024, Figure 6; SWE-bench, Figure 8), (2) there are other contaminations or online quizzes that are sourced from the benchmark (e.g., GSM8K, Figure 10), (3) blogs and papers cite a benchmark entry as example (e.g., GPQA, Figure 9; BigBenchHard, Figure 11; OpenbookQA, Figure 12), and (4) benchmarks are co-contained in online courses (e.g., MMLU, Figure 7).

Contamination could cause LLM to overperform on evaluation benchmarks by enabling models to retrieve memorized answers from training data rather than performing task-specific reasoning. Our finding shows that a large majority of dirty entries contain exact matches of both question and answer, though even subtler forms of contamination such as question-only matches and paraphrased answers could inflate benchmark scores. As training corpora grow, the risk of benchmark contamination and the potential for roto memorization increase, making it more important to document training corpora and construct contamination-free benchmarks to avoid overestimating modeling capabilities.

4.4 Benchmark Contamination Bulletin

Using INFINI-GRAM MINI, we implement a benchmark contamination monitoring system that tracks benchmark contamination. In the future, we will keep indexing the latest crawl in Common Crawl and update contamination results to track benchmark contamination as corpora evolve. The system also allows anyone to upload or upload new benchmarks to be monitored, fostering collaboration in benchmark monitoring. See App. §I for system interface.

5 Related Work

Exact-match search in large text corpora. Prior work has used different techniques to enable exact-match full text search in large text corpora, including suffix arrays, suffix automata, and proprietary search engines. Below we survey the largest-scale implementation of each technique known to us. Merrill et al. (2024) apply a suffix automaton to 1.3TB of text. Liu et al. (2024) apply a suffix array to 12TB of text. Elazar et al. (2024) use the proprietary ElasticSearch to index and analyze 35TB of text. All these methods have significant storage



**Knowledge and Reasoning**
{"benchmark": "MMLU", "Test size": 1000, "Pile train": 13.20, "DCLM baseline": 28.40, "CC 2005-25": 13.50, "CC 2005-28": 9.00, "CC 2005-23": 12.10, "CC 2005-18": 11.50, "CC 2005-21": 11.70, "CC 2005-26": 9.20}
{"benchmark": "MMLU-Pro", "Test size": 1000, "Pile train": 5.50, "DCLM baseline": 16.20, "CC 2005-25": 7.10, "CC 2005-28": 5.40, "CC 2005-23": 6.00, "CC 2005-18": 6.30, "CC 2005-21": 7.40, "CC 2005-26": 6.90}
{"benchmark": "BigBenchHard", "Test size": 1000, "Pile train": 0.00, "DCLM baseline": 0.10, "CC 2005-25": 1.40, "CC 2005-28": 1.40, "CC 2005-23": 3.20, "CC 2005-18": 2.30, "CC 2005-21": 1.80, "CC 2005-26": 1.70}
{"benchmark": "AGIEval", "Test size": 1000, "Pile train": 0.80, "DCLM baseline": 3.10, "CC 2005-25": 2.70, "CC 2005-28": 3.00, "CC 2005-23": 7.00, "CC 2005-18": 9.40, "CC 2005-21": 4.60, "CC 2005-26": "N/A"}
{"benchmark": "GPQA", "Test size": 448, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.90, "CC 2005-28": 0.00, "CC 2005-23": 1.30, "CC 2005-18": 0.70, "CC 2005-21": 2.70, "CC 2005-26": "N/A"}
{"benchmark": "HLE", "Test size": 881, "Pile train": 0.00, "DCLM baseline": 0.30, "CC 2005-25": 0.10, "CC 2005-28": 0.00, "CC 2005-23": 0.10, "CC 2005-18": 0.00, "CC 2005-21": 0.00, "CC 2005-26": 0.00}

**Math**
{"benchmark": "AIMath-2024", "Test size": 30, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 10.00, "CC 2005-28": 3.30, "CC 2005-23": 6.70, "CC 2005-18": 40.00, "CC 2005-21": 40.00, "CC 2005-26": 13.30}
{"benchmark": "GSM8K", "Test size": 1000, "Pile train": 0.00, "DCLM baseline": 5.00, "CC 2005-25": 5.00, "CC 2005-28": 0.80, "CC 2005-23": 6.90, "CC 2005-18": 0.70, "CC 2005-21": 74.20, "CC 2005-26": 7.30}
{"benchmark": "MATH-500", "Test size": 500, "Pile train": 0.60, "DCLM baseline": 3.20, "CC 2005-25": 0.60, "CC 2005-28": 7.80, "CC 2005-23": 0.80, "CC 2005-18": 0.80, "CC 2005-21": 0.80, "CC 2005-26": 8.20}
{"benchmark": "MGSM", "Test size": 200, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 5.60, "CC 2005-28": 1.60, "CC 2005-23": 35.60, "CC 2005-18": 0.80, "CC 2005-21": 72.80, "CC 2005-26": 6.00}

**Code**
{"benchmark": "HumanEval", "Test size": 164, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.00, "CC 2005-28": 0.60, "CC 2005-23": 0.60, "CC 2005-18": 0.60, "CC 2005-21": 0.00, "CC 2005-26": 0.00}
{"benchmark": "HumanEval+", "Test size": 164, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.00, "CC 2005-28": 0.60, "CC 2005-23": 0.60, "CC 2005-18": 0.60, "CC 2005-21": 0.00, "CC 2005-26": 0.00}
{"benchmark": "LiveCodeBench", "Test size": 880, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.00, "CC 2005-28": 0.00, "CC 2005-23": 0.00, "CC 2005-18": 0.00, "CC 2005-21": 0.00, "CC 2005-26": 0.00}
{"benchmark": "SWE-bench", "Test size": 500, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.20, "CC 2005-28": 0.20, "CC 2005-23": 0.00, "CC 2005-18": 0.00, "CC 2005-21": 0.00, "CC 2005-26": 0.00}
{"benchmark": "HumanEval+", "Test size": 500, "Pile train": 0.00, "DCLM baseline": 0.40, "CC 2005-25": 1.20, "CC 2005-28": 1.40, "CC 2005-23": 1.20, "CC 2005-18": 1.80, "CC 2005-21": 1.00, "CC 2005-26": 1.40}

**Commonsense Understanding**
{"benchmark": "ARC-Challenge", "Test size": 1000, "Pile train": 1.80, "DCLM baseline": 34.10, "CC 2005-25": 11.90, "CC 2005-28": 4.00, "CC 2005-23": 3.10, "CC 2005-18": 3.80, "CC 2005-21": 4.20, "CC 2005-26": 4.80}
{"benchmark": "ARC-Easy", "Test size": 1000, "Pile train": 1.30, "DCLM baseline": 31.70, "CC 2005-25": 5.40, "CC 2005-28": 9.50, "CC 2005-23": 5.50, "CC 2005-18": 5.50, "CC 2005-21": 6.10, "CC 2005-26": 6.20}
{"benchmark": "CSQA", "Test size": 1000, "Pile train": 0.10, "DCLM baseline": 1.00, "CC 2005-25": 0.10, "CC 2005-28": 0.10, "CC 2005-23": 0.50, "CC 2005-18": 0.10, "CC 2005-21": 0.00, "CC 2005-26": 0.10}
{"benchmark": "HellaSwag", "Test size": 1000, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.00, "CC 2005-28": 0.00, "CC 2005-23": 0.00, "CC 2005-18": 0.00, "CC 2005-21": 0.00, "CC 2005-26": 0.00}
{"benchmark": "OpenBookQA", "Test size": 500, "Pile train": 10.80, "DCLM baseline": 15.60, "CC 2005-25": 14.60, "CC 2005-28": 30.20, "CC 2005-23": 13.20, "CC 2005-18": 13.40, "CC 2005-21": 13.20, "CC 2005-26": 12.20}
{"benchmark": "SocialIQA", "Test size": 1000, "Pile train": 0.00, "DCLM baseline": 0.50, "CC 2005-25": 0.20, "CC 2005-28": 4.40, "CC 2005-23": 0.20, "CC 2005-18": 0.30, "CC 2005-21": 0.20, "CC 2005-26": 0.10}
{"benchmark": "WinoGrande", "Test size": 1000, "Pile train": 0.00, "DCLM baseline": 0.00, "CC 2005-25": 0.00, "CC 2005-28": 0.00, "CC 2005-23": 0.00, "CC 2005-18": 0.00, "CC 2005-21": 0.00, "CC 2005-26": 0.00}

**Reading Comprehension**
{"benchmark": "CoQA", "Test size": 500, "Pile train": 8.00, "DCLM baseline": 18.40, "CC 2005-25": 7.40, "CC 2005-28": 8.80, "CC 2005-23": 8.60, "CC 2005-18": 7.20, "CC 2005-21": 7.60, "CC 2005-26": 8.80}
{"benchmark": "SQuAD", "Test size": 1000, "Pile train": 2.80, "DCLM baseline": 40.10, "CC 2005-25": 2.70, "CC 2005-28": 33.00, "CC 2005-23": 10.10, "CC 2005-18": 1.50, "CC 2005-21": 2.00, "CC 2005-26": 8.50}

Table 2: Dirty rates for benchmarks across the Pile, DCLM-baseline, and Common Crawl from January to June 2025. Full result is reported in Table 7. For benchmark entries with over 1000 entries, we report dirty rate on the downsampled subset. Cell background color indicates benchmark cleanliness, with red more representing increasing levels of contamination.

multiplier wrt the size of text indexed, ranging from ElasticSearch's 2× to suffix automata's 29×. In contrast, our FM-index-based index has a storage multiplier as small as 0.26×, allowing us to index the largest body of text ever in the open-source community.

Benchmark contamination. Benchmark contamination appeared as a critical concern in LLM evaluations in recent studies. Prior works has quantified benchmark contamination on open-sourced models using various matching strategies, including n-gram or token overlap (Soldaini et al., 2024; Llama Team, 2024; Soldaini et al., 2024; OLMo et al., 2025), longest substring matching (Singh et al., 2024), skipgram match (Touvron et al., 2023), and full-text exact match results (Elazar et al., 2024). Sainz et al. (2024) reports data contamination from multiple sources through shared efforts. However, indexing large-scale pretraining corpora and performing exhaustive searches is computationally expensive, and prior studies on open corpora are limited in their scale (up to RedPajama-1T and Dolma, 12TB). To the best of our knowledge, our work conducts contamination analysis on the largest open corpora to date.

6 Conclusion

We introduce INFINI-GRAM MINI, an efficient system for indexing text with 0.44× their original size, enabling efficient counting and searching in mas-



Figure 3: Examples of four contamination types. Violet text is the text overlap between benchmark entry and corpus. Magenta text is the mapping of answers.

{"type": "table", "headers": ["Test Question", "Document Snippet"], "rows": [
  {
    "Test Question": "Question: increasing the credit period from 30 to 60 days, in response to similar action taken by all of our competitors, would likely result in\n0) an increase in the average collection period. ((handwriting1))\n1) decrease in bad debt losses.\n2) an increase in sales.\n3) higher profits.\n(benchmark: MMLU)",
    "Document Snippet": "Increasing the credit period from 30 to 60 days, in response to a similar action taken by all of our competitors, would likely result in: A) an increase in the average collection period. ((handwriting1)) B. A decrease in bad debt losses. C) an increase in sales. D. higher profits. Ans: A. ((handwriting1))\n(corpus: CC-2025-05; source: website page)"
  },
  {
    "Test Question": "Question: A red-tailed hawk is searching for prey. It is most likely to swoop down on\nA. an eagle\nB. a cow\nC. a gecko\nD. a deer\n(benchmark: OpenbookQA)",
    "Document Snippet": "\"** Generative Hypothesis**: H: A red-tailed hawk is searching for prey. It is most likely to swoop down on a gecko.\" Retrieved From text OpenBook; F: hawks eat lizards\n(corpus: Pile; source: Arxiv)"
  },
  {
    "Test Question": "Question: To express the distance between the Milky Way galaxy and other galaxies, the most appropriate unit of measurement is the\nA. meter\nB. kilometer\nC. light-year\nD. astronomical unit.\n(benchmark: ARC-Easy)",
    "Document Snippet": "To express the distance between the Milky Way galaxy, the most appropriate unit of measurement is the (A) meter. (B) kilometer. (C) light-year. (D) astronomical unit.\n(corpus: DCLM-baseline; source: CC-MAIN-2019-4817)"
  },
  {
    "Test Question": "Question: Where do you buy vegetables?\nA. supermarket\nB. fridge\nC. refrigerator\nD. cashier\nE. vegetable garden\n(benchmark: CSQA)",
    "Document Snippet": "- What makes you think Suu will like it?\n- Where do you buy vegetables?\n- I am not exaggerating. I'm just honest about my emotions\n(corpus: DCLM-baseline; source: CC-MAIN-2019-18)"
  }
]}

sive text corpus, and we show its scalability to a petabyte-scale corpus. We showcase INFINI-GRAM MINI's application on benchmark contamination analysis at scale.

Limitations

Although INFINI-GRAM MINI shows significant improvements on text compression rate, document retrieval latency remains higher than a canonical suffix array. For example, reconstructing a 3000-character document takes 1.8 seconds on Pile and 4.5 seconds on DCLM-baseline, compared to millisecond-level retrieval in system like Infinigram (Liu et al., 2024). This is a trade-off of INFINI-GRAM MINI's compression rate: INFINI-GRAM MINI does not store the original text in a contiguous block. To retrieve a document, we need to reconstruct it character-by-character, leading to a large number of reads in random addresses. These process takes longer time since the entire index is kept on disk at inference time. The high latency can potentially be reduced using techniques like disk page prefetching.

Identifying co-occurrences of multiple patterns is inefficient with the FM-index, since mapping every match from the suffix array range back to its position in original text takes very long time thus making this operation impractical. In contrast, the original infini-gram supports this operation efficiently.

INFINI-GRAM MINI only supports exact-match searches. As a result, our benchmark contamination analysis is limited to case-sensitive exact matching, which may fail to detect contamination of instances with minor textual discrepancies.

As the text corpora may contain biased or toxic content, document retrieval output may contain content that can be perceived as ethically problematic, and may contain sensitive information. The output of INFINI-GRAM MINI does not reflect authors' views.

Acknowledgments

We thank Christina Boucher (University of Florida) for telling us about the FM-index data structure. We would like to thank members of H2lab and AllenNLP for their valuable feedback on this project. This work was funded in part by NSF IIS-2044660 and IIS-2113530, and by RS-2024-00457882, National AI Research Lab Project.



References

Jacob Austin, Augustus Odena, Maxwell Nye, Maarten Bosma, Henryk Michalewski, David Dohan, Ellen Jiang, Carrie Cai, Michael Terry, Quoc Le, and l others. 2021. Program synthesis with large language models. arXiv preprint arXiv:2108.07732.

Michele Bevilacqua, Giuseppe Ottaviano, Patrick Lewis, Wen tau Yeh, Sebastian Riedel, and Fabio Petroni. 2022. Autoregressive search engines: Generating substrings as document identifiers. Preprint, arXiv:2205.10628.

Tom B. Brown, Benjamin Mann, Nick Ryder, Melanie Subbiah, Nenad Jardic, Prafulla Dhariwal, Arvind Neelakantan, Pranav Shyam, Girish Sastry, Amanda Askell, Sandhini Agarwal, Ariel Herbert-Voss, Gretchen Krueger, Tom Henighan, Daniel M. Ziegler, Jeffrey Clemens Winter, and 12 others. 2020. Language models are few-shot learners. Preprint, arXiv:2005.14165.

Michael Burrows, J D Wheeler G I T A L R Robert W. Taylor, D David J D Wheeler, and David Wheeler. 1994. A block-sorting lossless data compression algorithm.

Mark Chen, Jerry Tworek, Heewoo Jun, Qiming Yan, Henrique Penido de Oliveira, Pinto Jardein, Klaus RockflatMap, Ernie Ward, Ryan Bidicholas, Joseph Greg Brockman, Alex Ray, Rudi Prauli, Gretchen Krueger, Michael Petroni, Henryfläche, Krish Gürisy, Samya M. Mitchell, Brooke Chron, Sean Wogelius, and 39 others. 2021. Evaluating large language models trained on code. Preprint, arXiv:2107.03374.

Peter Clark, Isaac Cowhey, Oren Etzioni, Tushar Khot, Ashish Sabharwal, Carissa Schoenick, and Oyvind Tafjord. 2018, Thank you solved have question answered consistency ity, are the a2 reasoning challenge. Preprint, arXiv:1803.05475v1.

Karl Cobbe, Vincent Kosaraju, Mohammad Bavarian, Mark Carboun, Heewoo Jun, Lukasz Kaiser, Matthias Plopper, Jerry Tworek, Jacob Hilton, Reiichiro Nakano, Christopher Hesse, and John Schulman. 2021. Training verifiers to solve math word problems. arXiv preprint arXiv:2110.14168.

Common Crawl Foundation. 2020. Common crawl.

Lore Depoyci, Ludens Remora, Thomas Abel, and Jonas Fotiér. 2023. Pan-genome de bruijn graph using the bidirectional fm-index. BMC Bioinformatics, 24(1).

Yanal Elazar, Akhista Bhrugun, Ian Magnusson, Abhi lasha Washikrew, Duncan Sweet, Dwayne Alan Sulte, Pesh Watke, Dishrn Growendeck, Luna Soldaini, Sameer Singh, Hanna Hajashiri, Noah S. Smith, and Jesse Dodge. 2024. What’s in my big data? Preprint, arXiv:2310.20707.

P Ferragina and G Manzini. 2000. Opportunistic data structures with applications. In Proceedings 41st Annual Symposium on Foundations of Computer Science, pages 390–398.

Leo Gao, Stella Biderman, Sid Black, Laurence Gold ing, Travis Hoppe, Charles Foster, Jason Phang, Horace He, Anish Thite, Noa Nabeshima, Shawn Presser, and Connor Leahy. 2020. The Pile: An 800gb dataset of diverse text for language modeling. arXiv preprint arXiv:2101.00027.

Simon Gog, Timo Bellere, Alistair Moffat, and Matthias Petri. 2014. From theory to practice: Plug and play with succinct data structures. In 13th International Symposium on Experimental Algorithmics, (SEA 2014), pages 326–337.

Li-Gu Yuos. 2022. Efficon: An efficient locating algo rithm for mass-occurrence biological patterns with fm-index. Journal of computational biology : a jour nal of computational molecular cell biology.

Xiaochengnan Han and Yulia Tsvelev. 2022. Ora. In: Interpreting pretrained language models via locating supervised substrings in the ocean of pretraining data. arXiv, abs/2205.12600.

Dan Hendrycks, Collin Burns, Steven Bastani, Andy Zou, Mantas Mazeika, Dawn Song, and Jacob Sternbild. 2021. Measuring massive multitask language understanding. Preprint, arXiv:2009.03300.

Naman Jain, Gaming Han, Alex Gu, Wen-Ding Li, Fajian Yan, Nanjing Tian, Zhang Sida, Hai Wang, and Leonardo Silvera, et al. 2024, and Joe Siona. 2024. Live codebench: Holistic and contamination free evalu ation of large language models for code. Preprint, arXiv:2403.07974.

Carlos E. Jimenez, John Yang, Alexander Wetting, Shuyu Yan, Kevin Pei, elixir Off, and Karthik Narasimhan. 2022. Swe-bench: Can language mod els resolve real-world github issues? Preprint, arXiv:2310.06770.

Julian Jubian, Julian Shun, and Guy E. Bleloch. 2017. Parallel lightweight wavelets, suffix array and fm index construction. Journal of Discrete Algorithms, 43:2-17.

Katherine Lee, Daphne Ippolito, Andrew Nystrom, Chipusan Zhang, Douglas Eck, Chris Callison-Burch, and Nicholas Carlini. 2022. Deduplicating training data makes language models better. In Proceedings of the 60th Annual Meeting of the Association for Computational Linguistics (Volume 1: Long Papers), pages 8424–8440, Dublin, Ireland. Association for Computational Linguistics.

Heng Li. 2014. Fast construction of fm-index for long sequence reads. Bioinformatics, 30(22):3274-3275.

Jeffrey Li, Alex Fang, Georgios Smyrniou, Major Aviv, Matt Jordan, Samir Gadge, British Hnatsal, Sabah E. Seddik, Keish Ruhal, Aurora Sasser, Garg Rui, Xin Nicksell Muhongmei, Reinhard Heckel, Jean Gercat, Moe Taslim, Shalin Gonsalves, and others. 2024. Datacomp lm: In search of the next generation of training sets for language models. Preprint, arXiv:2406.11794.



Hunter Lightman, Vineet Kosaraju, Yura Burda, Harri Edwards, Bowen Baker, Teddy Lee, Jan Leike, John Schulman, Ilya Sutskever, and Karl Cobbe. 2023. Let's verify step by step. Preprint, arXiv:2305.2000.

Jiacheng Liu, Taylor Blanton, Yanai Elazar, Sewon Min, YenSung Chen, Arvin Sheurey, Chitta Reddy, Hunter Cron, Bryan Bethune, Eric Stuart, Marsh Sellam, Cassandra Tracy, Irina Sort, Aurant Semaan, Janie Stephens, Ion Borchardt, Bailee Kuehl, Eric Cheng, Karen Farley, Srileela Curry, Stearan Ira, and Anderson Cho. 2025. Olmotrace: Tracing language model outputs back to millions of training tokens.

Jiacheng Liu, Sewon Min, Luke Zettlemoyer, Yein Choi, and Nenad Haznedjaric. 2024. Infini-gram: Scaling unbounded n-gram text likelihood tokens. Preprint, arXiv preprint arXiv:2401.17377.

Jiawei Liu, Chungwen Setu, Xia Yuyao, Wang, and Ling-ming Zhang. 2023. Is your code generated by chatgpt really correct? rigorous evaluation of large language models for code generation. In Thirty-seventh Conference on Neural Information Processing Systems.

AI @ Meta Llama Team. 2024. The llama 3 herd of models. Preprint, arXiv:2407.21883.

Ximing Lu, Melanie Schlar, Skyller Hallinan, Nilofar Ettesmihraghian, Jiahui Liu, Seungju Han, Allyson Chithetty, Inwon Liu, Jiahgyi Khanty, Dhunu Naz, and Yein Choi. 2024. As a matter of saliency's quality: Quantifying the creative authenticity of language models via systematic attribution of archive text against web text. arXiv:2405.04265.

Veli Mäkinen and Gonzalo Navarro. 2005. Succinct suffix arrays based on run-length encoding. In Combinatorical Pattern Matching, pages 45–56, Berlin, Heidelberg. Springer ben Berlin Heidelberg.

William Merrill, Noah A. Smith, and Yanai Elazar. 2024. Evaluating n-gram novelty of language models using rasti metrics. arXiv:abs/2406.14264.

Idoor Mihovay, Peter Tushar, Kotoh and Ashish Sabharwal. 2018. Can a suit of armor conduct electricity? a new dataset for open book question answer-ing. In EMNLP.

Art of Problem Solving. 2024. Aime problems and solutions.

Team OLMo, Pete Welsh, Luca Soldani, Dirk Groen-eveld, Kyle Lo, Shane Arora, Akshita Bhugya, Dustin Young, Su Ghengui, Huang Yi, Jordan Mattox, Nathan Lambert, Dustin Boswell, Oxyna Taylor, Tara Anderson, David Aktonson, Fauze Brizman, Christopher Clark, Pradeep Dasigni, Nouha Dziri, and others. 2025. 2. olmo 2 furios. Preprint, arXiv:2501.00656.

Long Phan, Alice Gatti, Ziwen Han, Nathaniel Li, Josephine Hu, Hugh Zhang, Chen Bo Calvin Zhang, Mohamed Shaaban, Bing Long, and Sean Shi et al. 2025. Humanity's last exam. Preprint, arXiv:2501.14249.

Pranav Rajkumar, Jian Zhang, Konstantin Loprev, and Percy Liang. 2016. SQuAD:100 questions for machine comprehension of text. In Proceedings of the 2016 Conference on Empirical Methods in Natural Language Processing, pages 2383–2392, Austin, Texas. Association for Computational Linguistics.

Siva Reddy, Danqi Chen, and Christopher D. Manning. 2019. CoQA: A conversational question answering challenge. Transactions of the Association for Computational Linguistics, 7:239–256.

David Remzi, Liang Li, Hou Asa, Cooper Stuckland, Jackson Petty, Richard Yuan Zhang, Pangjie own, Julian Dirani, Julian Michael, and Samuel R. Bowman. 2023. Gqa: A graduate-level probe google-proof hqa benchmark. Preprint, arXiv:2311.12022.

Oscar Sainz, Iker Garcia-Ferrero, Alon Jacovi, Jon Ander Campanos, Yanai Elazar, Enike Agero, You Chosen Berg, Wei-Lin Chen, Jenia Chen, Elena Chouzen Lourenco, \text{...}, Melissa Dell, Rui Ze Pan, Shahrier Polachek, Yugu Peng, Liu Fei, Bhavish Palwe, Amreya Prabhu, Suryansha Sharma, and 9 others. 2024. Data contamination report from the 2024 competition. Preprint, arXiv:2410.15125.

Keisuke Sakaguchi, Ron Le Bras, Chandra Bhagavatula, and Yein Choi. 2019. Winogrande: An adversarial winograd schema challenge at scale. Preprint, arXiv:1907.10641.

Maarten Sap, Hannah M. Rashkin, Derek Chen, Ronan LeBras, and Yein Choi. 2019. Socialiqa: Commonsense reasoning about social interactions. Preprint, arXiv:1904.09728.

Freda Shi, Mirac Sungur, Markus Freitag, Xuezhi Wang, Suraj Srivastava, Sorosh Houshyar, Ying Liu, Da Yi, Tao Xiang, Bastian Ruder, Denny Zhou, Dipanjan Das, and Jason Wei. 2022. Language models are multilingual challenge-of-thought reasoners. Preprint, arXiv:2210.3057.

Aaditya S. King, Muhammed Yusuf Kescirgy, Andrew Poulton, David Esbola, Mario Lonneli, Gergely Szilvasy, and Diva Nayak. 2024. Evaluation data contamination in llms: how do we measure it and when (does) it matter? Preprint, arXiv:2411.03923.

Luca Soldani, Rodney Kinney, Akshita Bhugya, Dustin Schenck, David Aktonson, Russell Arthur, Ben Bogin, Khyathi Chandu, Jennifer Dumas, Yanai Elazar, Valentin Hoffmann, Ananya Jha, Sychin Kumar, Jordan Mattox, Nathan Lambert, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text{...}, \text



Mirae Suzgun, Nathan Scales, Nathanael Schärli, Sebastian Gehrmann, Yi Tay, Hyung Won Chung, Aakanksha Chowdhery, Quoc V Le, Ed H Chi, Denny Zhou, , and Jason Wei. 2022. Challenging big-bench tasks and whether chain-of-thought can solve them. arXiv preprint arXiv:2210.09261.

Alon Talmor, Jonathan Herzig, Nicholas Lourie, and Jonathan Berant. 2019. CommonsenseQA: A question answering challenge targeting commonsense knowledge. In Proceedings of the 2019 Conference of the North American Chapter of the Association for Computational Linguistics: Human Language Technologies, Volume 1 (Long and Short Papers), pages 4149–4158, Minneapolis, Minnesota. Association for Computational Linguistics.

Hugo Touvron, Louis Martin, Kevin J. Stone, Peter Albert, Amjad Alrmsei, Yasaman Bahri, Prajit Bhargava, Shruti Bhosale, Daniel M. Bikel, Lukas Blecher, Cristian Fanón Cetrero, Mya Chen, Guillaume Cucurull, David Esiobu, Jude Fernandes, Jeremy Fu, Wenyi Yen, Fu, and 49 others. 2023. Llama 2: Open foundation and fine-tuned chat models. arXiv, abs/2307.09288.

Yuanmeng Wang, Xiang Li, Dacxis Zage, Guangming Tan, and Ninghui Sun. 2018. Accelerating gm-index search for genomic data processing. In Proceedings of the 47th International Conference on Parallel Processing, ICPP ’18, New York, NY, USA. Association for Computing Machinery.

Yuhu Wang, Xueguang Ma, Ge Zhang, Yuansheng Ni, Abhinav Chandra, Shiguang Guo, Weiming Ren, Aaron Arulraj, Xuan He, Ziyan Jiang, Tianle Li, Max Ku, Kai Wang, Xie Zhang, Rongqi Fan, Xiang Yue, and Wenhu Chen. 2024. Mmlu-pro: A more robust and challenging multi-task language understanding benchmark. Preprint, arXiv:2406.01574.

Rowan Zellers, Ari Holtzman, Yonatan Bisk, Ali Farhadi, and Yejin Choi. 2019. Hellaswag: Can a machine really finish your sentence? In Proceedings of the 57th Annual Meeting of the Association for Computational Linguistics.

Wanjun Zhong, Ruixiang Cui, Yiduo Guo, Yaobo Liang, Shuai Lu, Yanlin Wang, Amin Saied, Weizhu Chen, and Nan Duan. 2023. Agieval: A human-centric benchmark for evaluating foundation models. Preprint, arXiv:2304.00304.



A Querying the FM-Index

This section introduces in details three operations supported by FM-index: find, locate, and reconstruct.

Find. Each occurrence of a pattern string Q in the haystack string T corresponds to an element in the SA, and all occurrences live in a consecutive range in the SA. The find operation computes this range with backward search: starting with the range in full, it iterates through the symbols in Q in reverse order, with each iteration narrowing the range using the character table C and the rank operation of the wavelet tree. The length of the final range is the count of Q in T. Figure 4 (left) illustrates finding pattern "ana" in string "banana". The time complexity of find is O(|Q|(H_0)), where |Q| is the length of the pattern.

Locate. The locate operation maps any position in the SA in its corresponding position in T. Since the SA is sampled at interval a, for an unsampled position i, the algorithm applies LF mapping at most a times to locate the nearest sampled SA entry. Figure 4 (middle) illustrates locating the two occurrences of "ana" in "banana". The time complexity of locating each position is O(t_{SA}), where t_{SA} is the complexity of accessing an SA entry, which is O(1) for sampled entries and O(h_0) for unsampled entries.

Reconstruct. After getting the position of a pattern occurrence in T, we can reconstruct a substring of T containing that occurrence with some additional context. Starting at the first position of the desired substring, we apply LF mapping to traverse through the BWT and use ISA to recover the symbol, and reconstruct symbols in reverse order until reaching the start position of the desired substring. Figure 4 (right) shows reconstructing the second occurrence of "ana" with context length of 1. The time complexity of reconstruct is O(d h_0 + t_{ISA}), where d is the length to reconstruct, and t_{ISA} is the complexity of accessing an ISA entry, which is O(1) for sampled entries and O(h_0) for unsampled entries.

B Indexing Time

Table 3 presents indexing time for each shard of text corpora with stepwise breakdown. Suffix array construction scales super-linearly depending on the level of duplication (Lee et al., 2022). Constructing alphabet, wavelet tree, and sampling are linear operations. Indexing time also varies across similar-size shards originated from different text corpora, showing that time also depend on text corpora properties. For example, over 60% duplicate documents in the Pile (Elazar et al., 2024) causes SA construction to take significantly longer compared to DCLM-baseline.

C Benchmarking Query Latency

We benchmark the query latency of INFINIGRAMINI, and report results in Table 4. For counting, we experiment with query lengths |Q| ∈ {1, 2, 5, 10, 50, 100, 500, 1000}, and for each length, randomly sample the query strings from the corpus. For retrieving documents, we use queries of length 10 sampled from the corpus, and reconstruct text surrounding the query with total length of d ∈ {10, 50, 100, 500, 1000}. For each corpus and parameter setting, we repeat with 100 random queries and report the average latency. For benchmarking, we store the indexes on pd-standard-ssd SSD disk on GCP which has a max IOPS of 80,000 and max throughput of 1200 MB/s, and we experiment on an n2-highcpu-64 node where the max disk I/O performance can be achieved.

D Query Latency Comparison with Prior Works

Table 5 reports query latency on Pile-train corpus using our method compared to prior work by Liu et al. (2024).

E INFINI-GRAM MINI Web Interface

We host web interface for easy access to counting (Figure 5 left) and document retrieval (Figure 5 right) with the indexes we have built.

F Details for Benchmarks in Contamination Analysis

Table 6 shows the benchmarks we analyze under each category with citation and source.

G Detailed Benchmark Consultation Result

Table 7 reports the number of suspicious and dirty entries of each benchmark.



[IMAGE: A technical diagram consisting of four panels illustrating string matching operations. 
Panel 1: A vertical list of strings representing suffixes of "banana$": "$banana$", "a$banana$", "na$banana$", "ana$banana$", "nana$banana$", "anana$banana$", "banana$". A red vertical bar highlights "ana$banana$".
Panel 2: The same list of suffixes. A blue box highlights the prefix "ana" in the strings "ana$banana$" and "anana$banana$".
Panel 3: The same list. Red arrows point from the "ana" prefix in the strings to a count of "3 ana$".
Panel 4: A conceptual diagram showing the word "banana$" with arrows indicating "construction direction" and a mapping of characters to positions, illustrating how a substring is reconstructed from the suffix array.
The panels are labeled 1, 2, 3, and 4 respectively, with accompanying logic text: 'find("ana")=2,4 count = 2', 'locate("ana")=1,3 context length = 1', and 'reconstruct("ana", 2)'.]

Figure 4: Illustration of operations on FM-index (§2.2, App. §A). Left: the find operation computes the SA range corresponding to all occurrences of the pattern. Middle: locate operation computes the position of pattern occurrence in the original string for each position in the SA range. Right: reconstruct operation gets a substring of the original string enclosing the second position pattern occurrence with a context length of 1. The occurrence ranking is based on its order in SA.

```jsonl
{"Step": "SA+BWT", "Pile-val (1.4 GB)": "29 s", "Pile-train (653 GB)": "41710 s", "DCLM-baseline (667 GB)": "29543 s", "CC-2025-05 (654 GB)": "55692 s"}
{"Step": "alphabet", "Pile-val (1.4 GB)": "4 s", "Pile-train (653 GB)": "2584 s", "DCLM-baseline (667 GB)": "2895 s", "CC-2025-05 (654 GB)": "2580 s"}
{"Step": "wavelet tree", "Pile-val (1.4 GB)": "13 s", "Pile-train (653 GB)": "5773 s", "DCLM-baseline (667 GB)": "6257 s", "CC-2025-05 (654 GB)": "5325 s"}
{"Step": "sample SA", "Pile-val (1.4 GB)": "1 s", "Pile-train (653 GB)": "2540 s", "DCLM-baseline (667 GB)": "2232 s", "CC-2025-05 (654 GB)": "2013 s"}
{"Step": "sample ISA", "Pile-val (1.4 GB)": "2 s", "Pile-train (653 GB)": "3975 s", "DCLM-baseline (667 GB)": "2659 s", "CC-2025-05 (654 GB)": "2313 s"}
{"Step": "Total", "Pile-val (1.4 GB)": "49 s", "Pile-train (653 GB)": "15.7 h", "DCLM-baseline (667 GB)": "12.1 h", "CC-2025-05 (654 GB)": "18.9 h"}
```

Table 3: Index construction time for each shard of the text corpora, with step-wise breakdown. The size of text in each shard is noted at the top. To get the indexing time of the full corpus on a single node, roughly multiply by the number of shards; though this can be embarrassingly parallelized across multiple nodes. Metadata size and metadata indexing time are excluded.

[IMAGE: A screenshot of a web user interface for "INFINI-GRAM MINI" divided into two main views.
Left view (Counting a string): 
- A sidebar titled "Corpus" contains a list of checkboxes with labels: "Pile-val (1.3 GB)", "Pile-train (1.3 TB)", "DCLM-baseline (16.7 TB)", "CC-2025-05 (9.1 TB)", "CC-2025-08 (8.7 TB)", "CC-2025-13 (10.8 TB)", "CC-2025-18 (10.5 TB)", "CC-2025-21 (9.3 TB)", "CC-2025-26 (8.7 TB)", and "CC-2025-30 (9.0 TB)". The "Pile-val" checkbox is checked.
- A main panel titled "1. Count a string" contains a "Click to view instructions" button.
- A "Query" section with a text input box containing "natural language processing", a "Clear" button, and a "Submit" button.
- A "Latency (milliseconds)" display showing "3.747".
- A "Count" display showing "83,470".

Right view (Retrieving documents):
- A main panel titled "2. Search documents" containing a heading "Search for documents containing a string".
- A "Query" section with a text input box containing "natural language processing" and a "Max length of context to display" input box containing "200".
- A results count showing "72892".
- A "Metadata" section showing "Document ID: 81189665455", "Length: 15761 chars", and a JSON snippet: {"path": "16", "jsonline": "6643966", "metadata": {"pile_set_name": "4kTv[3]"}}.
- A "Document" section showing a snippet of text: "...most popular classification methods in machine learning and has been successfully utilized in many applications. Such applications include image classification, speech recognition, cancer diagnosis etc. In natural language processing [highlighted in green] ...".]

Figure 5: The web interface of INFINI-GRAM MINI. Left: counting a string. Right: retrieving documents.



Table 4: Inference time latency of INFINI-GRAM MINI. Average latency of each query is reported. Notations: n = number of bytes in the text corpus, S = number of shards for the index, |Q| = length of query in bytes, d = length of text (in bytes) to reconstruct from the index, a = sampling rate of SA, b = sampling rate of ISA, H0 = zeroth-order entropy of the corpus.

```jsonl
{"row": "Counting a query of length |Q|", "Pile-train (n = 1.3T, S = 2)": "", "DCLM-baseline (n = 16.7T, S = 25)": "", "CC-2025-05 (n = 9.1T, S = 15)": "", "Time Complexity": "O(|Q|H0)"}
{"row": "... (|Q| = 1)", "Pile-train (n = 1.3T, S = 2)": "0.004 s", "DCLM-baseline (n = 16.7T, S = 25)": "0.005 s", "CC-2025-05 (n = 9.1T, S = 15)": "0.032 s", "Time Complexity": ""}
{"row": "... (|Q| = 2)", "Pile-train (n = 1.3T, S = 2)": "0.015 s", "DCLM-baseline (n = 16.7T, S = 25)": "0.017 s", "CC-2025-05 (n = 9.1T, S = 15)": "0.094 s", "Time Complexity": ""}
{"row": "... (|Q| = 5)", "Pile-train (n = 1.3T, S = 2)": "0.061 s", "DCLM-baseline (n = 16.7T, S = 25)": "0.207 s", "CC-2025-05 (n = 9.1T, S = 15)": "0.206 s", "Time Complexity": ""}
{"row": "... (|Q| = 10)", "Pile-train (n = 1.3T, S = 2)": "0.106 s", "DCLM-baseline (n = 16.7T, S = 25)": "0.402 s", "CC-2025-05 (n = 9.1T, S = 15)": "0.350 s", "Time Complexity": ""}
{"row": "... (|Q| = 20)", "Pile-train (n = 1.3T, S = 2)": "0.182 s", "DCLM-baseline (n = 16.7T, S = 25)": "0.868 s", "CC-2025-05 (n = 9.1T, S = 15)": "0.638 s", "Time Complexity": ""}
{"row": "... (|Q| = 50)", "Pile-train (n = 1.3T, S = 2)": "0.393 s", "DCLM-baseline (n = 16.7T, S = 25)": "1.743 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.063 s", "Time Complexity": ""}
{"row": "... (|Q| = 100)", "Pile-train (n = 1.3T, S = 2)": "0.696 s", "DCLM-baseline (n = 16.7T, S = 25)": "2.857 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.642 s", "Time Complexity": ""}
{"row": "... (|Q| = 200)", "Pile-train (n = 1.3T, S = 2)": "1.281 s", "DCLM-baseline (n = 16.7T, S = 25)": "5.699 s", "CC-2025-05 (n = 9.1T, S = 15)": "2.753 s", "Time Complexity": ""}
{"row": "... (|Q| = 500)", "Pile-train (n = 1.3T, S = 2)": "2.763 s", "DCLM-baseline (n = 16.7T, S = 25)": "12.46 s", "CC-2025-05 (n = 9.1T, S = 15)": "4.626 s", "Time Complexity": ""}
{"row": "... (|Q| = 1000)", "Pile-train (n = 1.3T, S = 2)": "4.808 s", "DCLM-baseline (n = 16.7T, S = 25)": "25.47 s", "CC-2025-05 (n = 9.1T, S = 15)": "7.957 s", "Time Complexity": ""}
{"row": "Retrieving a text of length d", "Pile-train (n = 1.3T, S = 2)": "", "DCLM-baseline (n = 16.7T, S = 25)": "", "CC-2025-05 (n = 9.1T, S = 15)": "", "Time Complexity": "O((a + b + d)H0)"}
{"row": "... (d = 10)", "Pile-train (n = 1.3T, S = 2)": "0.426 s", "DCLM-baseline (n = 16.7T, S = 25)": "0.895 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.101 s", "Time Complexity": ""}
{"row": "... (d = 50)", "Pile-train (n = 1.3T, S = 2)": "0.634 s", "DCLM-baseline (n = 16.7T, S = 25)": "1.549 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.302 s", "Time Complexity": ""}
{"row": "... (d = 100)", "Pile-train (n = 1.3T, S = 2)": "0.734 s", "DCLM-baseline (n = 16.7T, S = 25)": "1.991 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.326 s", "Time Complexity": ""}
{"row": "... (d = 500)", "Pile-train (n = 1.3T, S = 2)": "0.874 s", "DCLM-baseline (n = 16.7T, S = 25)": "2.363 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.609 s", "Time Complexity": ""}
{"row": "... (d = 1000)", "Pile-train (n = 1.3T, S = 2)": "0.94 s", "DCLM-baseline (n = 16.7T, S = 25)": "2.385 s", "CC-2025-05 (n = 9.1T, S = 15)": "1.705 s", "Time Complexity": ""}
{"row": "... (d = 2000)", "Pile-train (n = 1.3T, S = 2)": "1.213 s", "DCLM-baseline (n = 16.7T, S = 25)": "3.464 s", "CC-2025-05 (n = 9.1T, S = 15)": "2.849 s", "Time Complexity": ""}
{"row": "... (d = 3000)", "Pile-train (n = 1.3T, S = 2)": "1.858 s", "DCLM-baseline (n = 16.7T, S = 25)": "4.456 s", "CC-2025-05 (n = 9.1T, S = 15)": "3.330 s", "Time Complexity": ""}
```

Table 5: Test retrieving latency comparison between infini-gram and INFINI-GRAM MINI1 on Pile-train corpus.

```jsonl
{"model": "infini-gram (Liu et al., 2024)", "|Q| = 10 bytes": "13 ms", "|Q| = 20 bytes": "14 ms", "|Q| = 100 bytes": "13 ms"}
{"model": "INFINI-GRAM MINI", "|Q| = 10 bytes": "106 ms", "|Q| = 20 bytes": "182 ms", "|Q| = 100 bytes": "696 ms"}
```



```jsonl
{"benchmark": "MMLU", "citation": "Hendrycks et al., 2021", "source": "https://huggingface.co/datasets/cais/mmlu"}
{"benchmark": "MMLU-Pro", "citation": "Wang et al., 2024", "source": "https://huggingface.co/datasets/TIGER-Lab/MMLU-Pro"}
{"benchmark": "BigBenchHard", "citation": "Suzgun et al., 2021", "source": "https://github.com/suzgunmirac/BIG-Bench-Hard/tree/main/bbh"}
{"benchmark": "AGIEval", "citation": "Zhong et al., 2023", "source": "https://github.com/ruixiancgu/AGIEval/tree/main/data/v1_1"}
{"benchmark": "GPQA", "citation": "Rein et al., 2023", "source": "https://huggingface.co/datasets/ldavirdrein/gpqa"}
{"benchmark": "HLE", "citation": "Phan et al., 2025", "source": "https://huggingface.co/datasets/cais/hle"}
{"benchmark": "AIME-2024", "citation": "of Problem Solving, 2024", "source": "https://huggingface.co/datasets/Maxwell-Jia/AIME_2024"}
{"benchmark": "GSM8K", "citation": "Cobbe et al., 2021", "source": "https://huggingface.co/datasets/openai/gsm8k"}
{"benchmark": "MATH-500", "citation": "Lightman et al., 2023", "source": "https://huggingface.co/datasets/HuggingFaceH4/MATH-500"}
{"benchmark": "MGSM", "citation": "Shi et al., 2022", "source": "https://huggingface.co/datasets/juletxara/mgsm"}
{"benchmark": "HumanEval", "citation": "Chen et al., 2021", "source": "https://huggingface.co/datasets/openai_humaneval"}
{"benchmark": "HumanEval+", "citation": "Liu et al., 2023", "source": "https://huggingface.co/datasets/evalplus/humanevalplus"}
{"benchmark": "LiveCodeBench (code generation)", "citation": "Jain et al., 2024", "source": "https://huggingface.co/datasets/livecodebench/code_generation"}
{"benchmark": "SWE-bench", "citation": "Jimenez et al., 2024", "source": "https://huggingface.co/datasets/princeton-nlp/SWE-bench_Verified"}
{"benchmark": "MBPP", "citation": "Austin et al., 2021", "source": "https://huggingface.co/datasets/google-research-datasets/mbpp"}
{"benchmark": "ARC-Challenge", "citation": "Clark et al., 2018", "source": "https://huggingface.co/datasets/allenai/ai2_arc"}
{"benchmark": "ARC-Easy", "citation": "Clark et al., 2018", "source": "https://huggingface.co/datasets/allenai/ai2_arc"}
{"benchmark": "CSQA", "citation": "Talmor et al., 2019", "source": "https://huggingface.co/datasets/tua/commonsense_qa"}
{"benchmark": "HellaSwag", "citation": "Zellers et al., 2019", "source": "https://huggingface.co/datasets/rowan/hellaswag"}
{"benchmark": "Openbook QA", "citation": "Mihaylov et al., 2018", "source": "https://huggingface.co/datasets/allenai/openbookqa"}
{"benchmark": "Social Iqa", "citation": "Sap et al., 2019", "source": "https://huggingface.co/datasets/allenai/social_l_qa"}
{"benchmark": "WinoGrande", "citation": "Sakaguchi et al., 2019", "source": "https://huggingface.co/datasets/allenai/winogrande"}
{"benchmark": "CoQA", "citation": "Reddy et al., 2019", "source": "https://huggingface.co/datasets/stanfordnlp/coqa"}
{"benchmark": "SQuAD", "citation": "Rajpurkar et al., 2016", "source": "https://huggingface.co/datasets/rajpurkar/squad"}
```

Table 6: Citation and source of each benchmark analyzed in this paper.



Table 7: Detailed benchmark contamination results. For test sets with more than 1000 entries, we randomly downsample to 1000.

```jsonl
{"benchmark": "MMLU", "test_set": 1000, "pile-train_size": 57, "pile-train_dirty": 133, "dclm-baseline_dirty": 277, "cc-2005_dirty": 122, "cc-2008_dirty": 120, "cc-2013_dirty": 121, "cc-2018_dirty": 120, "cc-2021_dirty": 125, "cc-2026_dirty": 124}
{"benchmark": "MMLU-Pro", "test_set": 1000, "pile-train_size": 59, "pile-train_dirty": 142, "dclm-baseline_dirty": 162, "cc-2005_dirty": 112, "cc-2008_dirty": 115, "cc-2013_dirty": 115, "cc-2018_dirty": 117, "cc-2021_dirty": 118, "cc-2026_dirty": 118}
{"benchmark": "BigBenchHard", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 54, "dclm-baseline_dirty": 101, "cc-2005_dirty": 52, "cc-2008_dirty": 55, "cc-2013_dirty": 52, "cc-2018_dirty": 53, "cc-2021_dirty": 60, "cc-2026_dirty": 60}
{"benchmark": "AGIEval", "test_set": 1000, "pile-train_size": 5, "pile-train_dirty": 85, "dclm-baseline_dirty": 337, "cc-2005_dirty": 14, "cc-2008_dirty": 29, "cc-2013_dirty": 30, "cc-2018_dirty": 32, "cc-2021_dirty": 38, "cc-2026_dirty": 46}
{"benchmark": "HLE", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 5, "dclm-baseline_dirty": 31, "cc-2005_dirty": 107, "cc-2008_dirty": 96, "cc-2013_dirty": 35, "cc-2018_dirty": 30, "cc-2021_dirty": 70, "cc-2026_dirty": 104}
{"benchmark": "GPQA", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 5, "dclm-baseline_dirty": 45, "cc-2005_dirty": 6, "cc-2008_dirty": 27, "cc-2013_dirty": 10, "cc-2018_dirty": 9, "cc-2021_dirty": 95, "cc-2026_dirty": 106}
{"benchmark": "HPL", "test_set": 881, "pile-train_size": 3, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 4, "cc-2008_dirty": 10, "cc-2013_dirty": 6, "cc-2018_dirty": 5, "cc-2021_dirty": 15, "cc-2026_dirty": 12}
```

Knowledge and Reasoning

```jsonl
{"benchmark": "AIME-2024", "test_set": 30, "pile-train_size": 2, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 0, "cc-2008_dirty": 0, "cc-2013_dirty": 1, "cc-2018_dirty": 1, "cc-2021_dirty": 12, "cc-2026_dirty": 12}
{"benchmark": "GSM8K", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 4, "cc-2005_dirty": 3, "cc-2008_dirty": 3, "cc-2013_dirty": 3, "cc-2018_dirty": 3, "cc-2021_dirty": 742, "cc-2026_dirty": 70}
{"benchmark": "MATH", "test_set": 500, "pile-train_size": 9, "pile-train_dirty": 0, "dclm-baseline_dirty": 48, "cc-2005_dirty": 16, "cc-2008_dirty": 36, "cc-2013_dirty": 39, "cc-2018_dirty": 25, "cc-2021_dirty": 20, "cc-2026_dirty": 73}
{"benchmark": "MGSUM", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 26, "cc-2005_dirty": 2, "cc-2008_dirty": 9, "cc-2013_dirty": 4, "cc-2018_dirty": 4, "cc-2021_dirty": 182, "cc-2026_dirty": 6}
{"benchmark": "MBP", "test_set": 250, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 2, "cc-2005_dirty": 0, "cc-2008_dirty": 14, "cc-2013_dirty": 3, "cc-2018_dirty": 2, "cc-2021_dirty": 18, "cc-2026_dirty": 15}
```

```jsonl
{"benchmark": "HumanEval", "test_set": 164, "pile-train_size": 1, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 0, "cc-2008_dirty": 1, "cc-2013_dirty": 1, "cc-2018_dirty": 1, "cc-2021_dirty": 65, "cc-2026_dirty": 77}
{"benchmark": "HumanEval+", "test_set": 164, "pile-train_size": 1, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 0, "cc-2008_dirty": 0, "cc-2013_dirty": 1, "cc-2018_dirty": 1, "cc-2021_dirty": 65, "cc-2026_dirty": 77}
{"benchmark": "LiveCodeBench", "test_set": 88, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 2, "cc-2005_dirty": 0, "cc-2008_dirty": 0, "cc-2013_dirty": 1, "cc-2018_dirty": 1, "cc-2021_dirty": 0, "cc-2026_dirty": 0}
{"benchmark": "SWE-bench", "test_set": 500, "pile-train_size": 0, "pile-train_dirty": 5, "dclm-baseline_dirty": 11, "cc-2005_dirty": 0, "cc-2008_dirty": 241, "cc-2013_dirty": 0, "cc-2018_dirty": 0, "cc-2021_dirty": 168, "cc-2026_dirty": 0}
{"benchmark": "MBPP", "test_set": 500, "pile-train_size": 16, "pile-train_dirty": 6, "dclm-baseline_dirty": 64, "cc-2005_dirty": 0, "cc-2008_dirty": 2, "cc-2013_dirty": 0, "cc-2018_dirty": 0, "cc-2021_dirty": 79, "cc-2026_dirty": 0}
```

Commonsense Understanding

```jsonl
{"benchmark": "ARC-Challenge", "test_set": 1000, "pile-train_size": 6, "pile-train_dirty": 14, "dclm-baseline_dirty": 341, "cc-2005_dirty": 18, "cc-2008_dirty": 119, "cc-2013_dirty": 29, "cc-2018_dirty": 38, "cc-2021_dirty": 48, "cc-2026_dirty": 49}
{"benchmark": "ARC-Easy", "test_set": 1000, "pile-train_size": 15, "pile-train_dirty": 5, "dclm-baseline_dirty": 317, "cc-2005_dirty": 31, "cc-2008_dirty": 54, "cc-2013_dirty": 30, "cc-2018_dirty": 35, "cc-2021_dirty": 61, "cc-2026_dirty": 62}
{"benchmark": "CSQA", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 11, "cc-2005_dirty": 0, "cc-2008_dirty": 29, "cc-2013_dirty": 35, "cc-2018_dirty": 37, "cc-2021_dirty": 66, "cc-2026_dirty": 60}
{"benchmark": "HellaSwag", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 341, "dclm-baseline_dirty": 0, "cc-2005_dirty": 36, "cc-2008_dirty": 0, "cc-2013_dirty": 29, "cc-2018_dirty": 67, "cc-2021_dirty": 30, "cc-2026_dirty": 28}
{"benchmark": "OpenbookQA", "test_set": 27, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 0, "cc-2008_dirty": 0, "cc-2013_dirty": 0, "cc-2018_dirty": 30, "cc-2021_dirty": 0, "cc-2026_dirty": 0}
{"benchmark": "Social IQA", "test_set": 500, "pile-train_size": 2, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 0, "cc-2008_dirty": 2, "cc-2013_dirty": 0, "cc-2018_dirty": 3, "cc-2021_dirty": 2, "cc-2026_dirty": 0}
{"benchmark": "WinoGrande", "test_set": 1000, "pile-train_size": 0, "pile-train_dirty": 0, "dclm-baseline_dirty": 0, "cc-2005_dirty": 0, "cc-2008_dirty": 0, "cc-2013_dirty": 0, "cc-2018_dirty": 0, "cc-2021_dirty": 0, "cc-2026_dirty": 0}
```

Reading Comprehension

```jsonl
{"benchmark": "CoQA", "test_set": 500, "pile-train_size": 189, "pile-train_dirty": 40, "dclm-baseline_dirty": 92, "cc-2005_dirty": 121, "cc-2008_dirty": 37, "cc-2013_dirty": 113, "cc-2018_dirty": 43, "cc-2021_dirty": 136, "cc-2026_dirty": 44}
{"benchmark": "SQuAD", "test_set": 1000, "pile-train_size": 16, "pile-train_dirty": 28, "dclm-baseline_dirty": 9, "cc-2005_dirty": 11, "cc-2008_dirty": 27, "cc-2013_dirty": 12, "cc-2018_dirty": 13, "cc-2021_dirty": 38, "cc-2026_dirty": 85}
```



H Contamination Examples

Figure 6 to Figure 12 shows example dirty entries in seven benchmarks and its contamination source retrieved from one of three corpus using INFINIGRAM MINI. We also present the original webpage that is responsible for the contamination.

I Dirty Entry Categorization using LLM-as-a-judge

We use gpt-4o-mini to categorize all dirty entries into one of four categories described in §4.3. For each dirty entry, we extract the 50-character substring that has least occurrence in the corpus. We then retrieve text snippet with context length of 400 characters (850 characters in total). We prompt gpt-4o-mini by providing both the dirty entry and the text snippet and let the model decide which category the dirty entry belongs. We use the following prompt:

You are an expert in evaluating benchmark contamination. Given an entry and detected overlap in the corpus, categorize how the entry is contaminated in corpus into one of four categories:
1- You can find exact match of question stem and the correct answer (correct choice if multiple choices, or answer matching exactly the answer field) in corpus.
2. You can find exact match of question stem, and the correct answer appears, though not in exact match.
3. You can find exact match of question stem, but you cannot find the correct answer in any form.
4. False positive
Output only the category number.

J Interface of the Benchmark Contamination Monitoring System

Figure 13 shows the interface of the system. It consists of two tabs: (1) Benchmark Contamination Bulletin, and (2) submission page for community to contribute. Benchmarks analyzed in this paper are reported in “core” table, and submitted benchmarks will be added to “community” table.

K License of Corpora Used in the Paper

Pile and DCLM are licensed under the MIT License. Common Crawl is licensed under its customized Limited License. We followed the listed intended use.



```jsonl
{"id": "2024-II-5", "problem": "Let ABCDEF be a convex equilateral hexagon in which all pairs of opposite sides are parallel. The triangle whose sides are extensions of segments AB, CD, and EF has side lengths 200, 240, and 300. Find the side length of the hexagon.", "answer": "80"}
```

**Query**
Let ABCDEF be a convex equilateral hexagon in which all pairs of opposite sides are parallel. The triangle whose sides are extensions of segments AB, CD, and EF has side lengths 200, 240, and 300. Find the side length of the hexagon.

**Problem**
Let ABCDEF be a convex equilateral hexagon in which all pairs of opposite sides are parallel. The triangle whose sides are extensions of segments AB, CD, and EF has side lengths 200, 240, and 300. Find the side length of the hexagon.

**Solution 1**
Draw a good diagram! Let $K_1$ and $K_2$ be $K_1$ and $M$, respectively. Let

```jsonl
{"geometry_labels": {"KM": "160", "KN": "160", "ML": "140", "KL": "200", "KM + MN": "200", "ML + LN": "240"}}
```

[Image Description: A geometric diagram of a convex equilateral hexagon ABCDEF. The sides of the hexagon are extended to form a large triangle with vertices L, M, and N. The hexagon vertices are positioned such that A and B lie on segment LN, C and D lie on segment MN, and E and F lie on segment LM. The lines extending from the hexagon sides meet at points L, M, and N. Labels are present: KM = 160, KN = 160, ML = 140. The total side lengths of the outer triangle are labeled as 200, 240, and 300. Small variables 'x' are used to mark the side lengths of the hexagon.]

Notice that all smaller triangles formed are all similar to the larger triangle (200, 240, 300) triangle.

Let the side length of the hexagon be $x$.

Triangle $\Delta MEF \sim \Delta MLK$, so
$\frac{KL}{KM} = \frac{FM}{FN} \implies \frac{x}{200} = \frac{x}{200}$

Triangle $\Delta KAB \sim \Delta KML$, so
$\frac{LM}{KM} = \frac{x}{240} \implies \frac{140}{200} = \frac{x}{240} \implies AK = \frac{5x}{4}$

We know $KA + AF + FM = 300$, so $\frac{5x}{4} + x + \frac{5x}{4} = 300$. Solving, we get $x = 80$.

(a) Left upper: an entry in the AIME-2024 problemset. Right: A document containing this entry, retrieved from CC-2025-05 by INTOSINI-GRAMI MNL. This example belongs to Category 1, where the correct answer can be found in the document.

(b) The original webpage responsible for the contamination.

Figure 6: AIME dirty entry example in CC2025-05. The contamination source is the official AOPS website, where AIME exams are published.



```jsonl
{"question": "Which of the following cavities are separated by the diaphragm?", "subject": "anatomy", "choices": ["Abdominal and pelvic", "Cranial and spinal", "Thoracic and abdominal", "Pericardial and pleural"], "answer": 2}
```

[Image: A screenshot of a digital interface. On the left, a code snippet in a text editor showing a JSON object containing a multiple-choice question about anatomy. On the right, a document viewer displaying a webpage titled "Which of the following cavities are separated by the diaphragm?" with four multiple-choice options (A-D) and the answer "Ans: D". The interface includes a "Metadata" section at the top with technical file details including path, size, and hashes.]

(a) Left upper: An entry in the MMLU benchmark. Right: A document contaminating this entry, retrieved from CC-2025-05 by INFINI-GRAM MINI. This example belongs to Category 1, where the correct answer presents (though choices are not in the exact same order).

[Image: A screenshot of a website header. On the left is a logo consisting of a green and white medical cross icon above the text "Radiology Star". To the right is a navigation menu with links for "Home", "Article", "MCQs", "Case Study", "About Us", "Contact Us", and a magnifying glass search icon.]

```jsonl
{"question": "13) Which of the following cavities are separated by the diaphragm?", "options": {"A": "Abdominal and pelvis", "B": "Dorsal and ventral", "C": "Pericardial and pleural", "D": "Thoracic and abdominal"}, "answer": "D"}
{"question": "14) Which of the terms describes the motion of bending the forearm towards the body?", "options": {"A": "Abduction", "B": "Eversion", "C": "Flexion", "D": "Pronation"}, "answer": "C"}
```

(b) The original webpage responsible for the contamination.

Figure 7: MMLU dirty entry example in CC-2025-05. The contamination source is a website containing multiple-choice question in related fields.



(Left): An image showing a GitHub-style pull request or issue page. The layout is split into two main columns. The left column contains a JSON-like code block with keys "repo", "problem_statement", and "environment_setup_commit". The right column contains a "Metadata" section with technical details like "Entry #257353707", "Length: 205ch", and a series of timestamps and IDs. Below the metadata is a "Document" section containing a short list of requirements ("tck: yes", "needs documentation: no", etc.). At the bottom, there is a "Query" section with a paragraph of text and a "Change History" link.

(Right): A screenshot of a Django project issue tracker page. 
Header: django | OVERVIEW | DOWNLOAD | DOCUMENTATION | NEWS | COMMUNITY | CODE | ISSUES | ABOUT | DONATE
Section: Issues
Tabs: View Tickets | Reports | Timeline | Wiki | Search

Ticket Details:
#33014 closed Cleanup/optimization (fixed)
Status: Opened 4 years ago, Closed 4 years ago
Title: ProjectState._init__() can assume its real_apps argument is a set

{"Reported by": "Chris Jerdonick", "Owned by": "Chris Jerdonick"}
{"Component": "Migrations", "Version": "dev"}
{"Severity": "Normal", "Keywords": ""}
{"Cc": "", "Triage State": "Ready for checkin"}
{"Has patch": "yes", "Needs documentation": "no"}
{"Needs tests": "no", "Patch needs improvement": "no"}
{"Easy pickings": "yes", "UI/UX": "no"}
{"Pull Requests": "14246/merged"}

Description:
Since PR #14760 made all calls to ProjectState._init__() pass real_apps as a set in ProjectState._init__() now, instead of checking that real_apps is a set and converting it to a set if not, it can just assert that it's a set when non-None. (Presumably the construction of new ProjectState objects is part of Django's internal API,) I had made this comment on the PR but it wasn't important enough to hold up the PR because another PR was depending on it getting merged.

(a) Left upper: An entry in the SWE-bench dataset. Right: A document containing this entry, retrieved from CC-2025-05 by INFINGNI-GRAM. This entry is Sample 3, Category 2, which only requires one pass answer.

(b) The original webpage representative.

Figure 8: SWE-bench dirty entry example in CC-2025-05. The contamination source is a website recording pull requests for software development.



(a) Left bottom: An entry in the GPQA benchmark. Right: A document containing this entry, retrieved from CC-2025-05 by INFINI-GRAM MINI. This example belongs to Category 3, where only the question appears, not the answers.

```jsonl
{"type": "metadata", "document_id": "11302502923", "length": 2699, "chars_vis_displayed": 695, "main_id": "CC-MAIN-202050114610-202050120004819", "id": "nmwc4kocxaiy8dchfgo8n4mgb5nw3", "status": "full_CC", "created": "1909-01-27T23:24:00Z", "added": "2022-261703.815982", "version": "v0", "url": "https://lifearchitect.ai/the-sky-is-quickening/", "date": "2022-01-19T18:13:00Z", "sha1": "c858a8184-250e-40b8a-8279-031187b78c80", "content_type": "text/html", "uncompressed_offset": 183009180"}
```

{"question": "A reaction of a liquid organic compound, which molecules consist of carbon and hydrogen atoms, is performed at 80 centigrade and 20 bar for 24 hours. In the proton nuclear magnetic resonance spectrum, the signals with the highest chemical shift of the reactant are replaced by a signal of the product that is observed about three to four units downfield. Compounds from which position in the periodic system of the elements, which are also used in the corresponding large-scale industrial processes, have been most likely initially added in small amounts?", "correct": "A metal compound from the third period and a non-metal compound from the third period.", "incorrect_1": "A metal compound from the fourth period and a non-metal compound from the second period.", "incorrect_2": "A metal compound from the second period and a non-metal compound from the fourth period.", "incorrect_3": "A metal compound from the fifth period."}

[Image Description: A screenshot of a digital user interface for a data labeling or review tool. The screen is split into two main columns. The left column contains a text-based form with a "Question" field and "Correct/Incorrect" answer fields, mirroring the JSON data above. Below this is a "Preview" section showing a rendered snippet of a web page with the question text. There is a text input box at the bottom with the label "Max length of content to display around" containing the value "200". The right column is titled "Metadata" and displays a list of technical document properties (ID, length, URL, etc.) in a monospace font. The overall interface has a light grey background with white content cards.]

The ceiling of uncontroversially correct answers—answers that have complete expert consensus—is only 74%. Here is a sample question (published with permission):[32]

A reaction of a liquid organic compound, which molecules consist of carbon and hydrogen atoms, is performed at 80 centigrade and 20 bar for 24 hours. In the proton nuclear magnetic resonance spectrum, the signals with the highest chemical shift of the reactant are replaced by a signal of the product that is observed about three to four units downfield. Compounds from which position in the periodic system of the elements, which are also used in the corresponding large-scale industrial process, have been most likely initially added in small amounts? 

And don't worry if that seems impossible to answer; if the questions were not multiple choice, I would probably get a score of zero across all 448 questions.

(b) The original webpage responsible for the contamination.

[Image Description: A screenshot of a blog post from the website "lifearchitect.ai". The page has a clean, modern white background. At the top is a breadcrumb navigation link: "lifearchitect.ai/the-sky-is-quickening/". The text is set in a sans-serif font. The content begins with a paragraph discussing GPQA and expert consensus, followed by a sample question. The sample question is encased in a light-grey callout box with a subtle border. The text inside the box describes a chemical reaction involving organic compounds and proton nuclear magnetic resonance spectra. Below the box, there is a concluding sentence regarding the difficulty of the questions.]

Figure 9: GPQA dirty entry example in CC-2025-05. The contamination source is a blog post citing a test set example.



Query:
John plans to sell all his toys and use the money to buy video games. He has 13 lego sets and he sells them for 15 each. He ends up buying 8 video games for 20 each and has 5 left. How many lego sets does he still have?

Max length of context to display: 300

[Button: Clear] [Button: Submit]

Latency (milliseconds): 9408.318

```jsonl
{"question": "John plans to sell all his toys and use the money to buy video games. He has 13 lego sets and he sells them for 15 each. He ends up buying 8 video games for 20 each and has 5 left. How many lego sets does he still have?"}
{"answer": "He spent $160 on video games because 8 x 20 = <#0x160> he earned $165 from his sale because 160 + 5 = <#0x165> he sold 11 lego sets because 11 x 15 = <#0x165> he had 13 lego sets 13 - 11 = <#0x11-2> 2 sets left because 13 - 11 = <#0x11-2> 2"}
```

(a) Left bottom: An entry in the GSM8K benchmark. Right bottom: A document containing this entry, retrieved from CC-2025-05 by INFINI-GRAM. This sample shows that the correct answer is not present as consequence 3.

```jsonl
{"Datasets": "Qwen", "ProcessBench": true, "likes": 46, "Follow": 46, "Qwen": "29.9k"}
```

Split (4)
gsm8k - 400 rows

[Search bar: search this dataset]

```jsonl
{"id": "gsm8k-97", "string_lengths": "8-9", "string_classes": "22.5%", "generator": "Qwen2-7B-IL...", "string_classes": "13%", "problem_string": "John plans to sell all his toys and use the money to buy video games. He has 13 lego sets and he sells them for 15 each. He ends up buying 8 video games for 20 each and has 5 left. How many lego sets does he still have?", "string_lengths": "170-246", "steps_sequence": "63", "string_lengths": "3.3%", "steps_length": "61.3%", "answer": "First, let's calculate how much money John gets from selling his 13 lego sets. Each lego set is sold for $15, so 13 * 15 = 195. Next, show that John buys 8 video games at $20 each, after making those purchases, the total amount spent on video games is 8 * 20 = 160. Finally, since he has $5 left to have in his pocket the total amount spent on video games is $160 spent on video games plus the $5 left over, which is $165. Since we previously calculated that John made $195 from selling his lego sets, and the total amount he ended up with is $165 plus the $5 left over, the difference is $30. Dividing this difference by the price of each lego set, $15, we find that he sold 2 lego sets in total. Therefore, he still has 13 - 2 = 11 lego sets left after selling them and using the proceeds to buy video games. However, it was mentioned that he originally had 13 lego sets and sold all those, so the answer is 0. But based on the provided information, let's assume he sold only some lego sets John still has is 11."}
```

(b) The original webpage responsible for the contamination.

Figure 10: GSM8K dirty entry example in CC-2025-05. The contamination source is hummingbird dataset sourcing from GSM8K examples, and is the major source for GSM8K dirty entries. This new dataset contains erroneous “steps” field and final answer to examine LLM’s ability to identify errors, so the correct answers do not appear.



(Left) Bottom of an image: An entry in the BigBenchHard benchmark.

(Right) Document: A document containing this entry, retrieved from CC-2025-05 by INFINE-GRAM MINI. This example is Category 3 because the correct answer is not present.

{"input": "How would a typical person answer each of the following questions about causation?\\nSury and Bill are working on a project that is very important for our nation's security. The boss tells them both: \\\"Be sure that you are there at exactly 9 am. It is absolutely essential that you arrive at that time.\\\" Both Bill and Sury arrive at 9 am. As it happens, there was a motion detector installed in the room where they arrived. The motion detector was set up to trigger if more than one person entered the room at the same time. So the motion detector went off. Did Billy cause the motion detector to go off?\\nOptions:\\n a. Yes\\n b. No", "target": "No"}

Metadata
{"Document": "1272589", "Length": "2175 characters displayed", "Metadata": {"path": "/171063790136190s6CC-MAIN-2020511221943520820512224358-00003.jsonl", "Stream": "11126", "metadata": {"id": "1272589", "content_type": "text/plain", "full_cc": "true", "created": "2025-01-22T14:13:08.000Z", "added": "2025-02-20T2:05:42.179Z", "v0": "metadata", "wan_url": "https://huggingface.co/datasets/common-crawl/wan", "wan_url_full": "https://huggingface.co/datasets/common-crawl/wan/resolve/main/cc-3527854c6950b2C0ffB17", "uurl": "https://gg.ducaches.co/datasets/common-crawl", "uurl_full": "https://gg.ducaches.co/datasets/common-crawl/resolve/main/cc-578804d6c3a17583645C950b2C0ffB17", "date_wan": "2025-01-22T19:58:08.000Z", "wane_filename": "cc-uuroded0265B07-54276-4778-a057", "attributes": {"content_type": "text/html", "uncompressed_offset": "14785300"}, "metadata": {}}}

would a typical person answer each of the following questions about causation?\nSury and Bill are working on a project that is very important for our nation's security. The boss tells them both: \"Be sure that you are there at exactly 9 am. It is absolutely essential that you arrive at that time.\"

Both Bill and Sury arrive at 9 am. As it happens, there was a motion detector installed in the room where they arrived. The motion detector was set up to trigger if more than one person entered the room at the same time. So the motion detector went off. Did Billy cause the motion detector to go off?\nOptions:\n a. Yes\n b. No

{"target": "Yes"}

{"Query": "Both Bill and Sury arrive at 9 am. As it happens, there was a motion detector installed in the room where they arrived.", "Max length of context to display": "300", "Result": "236"}

[Image Description: A screenshot of a web browser showing a Hugging Face dataset page. The top section displays a "Dataset card" tab, a "Files and versions" tab, and a "Community" tab. Below the tabs, there is a file explorer showing a file named `huggingfaceX/sephora/7b-alpha_results/2024-06-16T15-59-16.173648.json` with a timestamp of Jun 17, 2024. The main content area shows a code snippet of a JSONL file with line numbers 1 through 16. The JSON contains a dictionary where the key "leaderboard_hp" contains a list of results, including accuracy scores like "0.4550" and "0.784" for different evaluation versions of a model.]

{"line 1": "2 models})", "line 2": "1 leaderboard_hp": [
{"acc_norm_name": "0.4550686065173414"},
{"acc_norm_alias": "0.4550686065173414"},
{"alias": "leaderboard_hp_norm"}
],
{"line 9": "leaderboard_hp_boolean_expressions": [
{"acc_norm_name": "0.784"},
{"acc_norm_alias": "0.83087637657372"},
{"alias": "leaderboard_hp_boolean_expressions"}
],
{"line 13": "leaderboard_hp_causal_judgment": [
{"acc_norm_name": "0.577548606519374"},
{"acc_norm_alias": "0.577548606519374"},
{"alias": "leaderboard_hp_causal_judgment"}
]}

Figure 11: (b) The original webpage representative showing the contamination.



{"question_stem": "A red-tailed hawk is searching for prey. It is most likely to swoop down on", "choices": ["an eagle", "a crow", "a gecko", "a deer"], "answerKey": "C"}

Document #200455034
Length: 3186 chars (475 chars displayed)
Metadata: {"path": "28.json", "linenum": 3889716, "metadata": {"meta": {"pile_set_name": "Arkiv"}}}

{"document": "likely to swoop down on what?", "answer": "a gecko"}

**Generated Hypothesis**:
H: A red-tailed hawk is searching for prey. It is most likely to swoop down on a gecko.

**Retrieved Fact from OpenBook**:
F: hawks eat lizards

**Abduced Query to find missing knowledge**:
K: gecko is lizard

**Retrieved Missing Knowledge using IR**:
K: Every gecko is a lizard.

(a) Left bottom: An entry in the OpenbookQA benchmark. Right: A document containing this entry, retrieved from Pile-train by INFINI-Gram MINI. This example is Category 2 where the correct answer appears in natural language.

Careful Selection of Knowledge to Solve Open Boo... 2 / 10 145% + [zoom icons] [download icon]

Information Gain Based re-ranking to reduce distractions and remove redundant information; (d) We provide an analysis of the dataset and the implications of BERT Large model for such a question answering task.

The current best model on the leaderboard of OpenBookQA is the BERT Large model (Devlin et al., 2018). It has an accuracy of 60.4% and does not use external knowledge. Our knowledge selection and retrieval techniques achieve an accuracy of 72%, with a margin of 11.6% on the current state of the art. We study how the accuracy of the BERT Large model varies with varying number of knowledge facts extracted from the OpenBook and through IR.

2 Related Work

In recent years, several natural language datasets have been proposed for natural language question answering (Rajpurkar et al., 2016; Joshi et al., 2017; Khushabi et al., 2018; Resnick et al., 2018; Quoc et al., 2013; Lai et al., 2017; Reddy et al., 2018; Choi et al., 2019; Tabefjord et al., 2018; Mitra et al., 2019) and many attempts have been made to solve these challenges (Devlin et al., 2018; Vaswani et al., 2017; Seo et al., 2016).

Among these, the closest work to our is the

3 Approach

Our approach involves six main modules: Hypothesis Generation, OpenBook Knowledge Extraction, Abductive Information Retrieval, Information Gain Based re-ranking, Passage Selection and Question Answering. A key aspect of our approach is to accurately hunt the needed knowledge facts from the OpenBook knowledge corpus and hunt missing common knowledge using IR. We explain our approach in the example given in Table 2.

{"Question": "A red-tailed hawk is searching for prey. It is most likely to swoop down on what?", "Answer": "a gecko", "Generated Hypothesis": "H: a red-tailed hawk is searching for prey. It is most likely to swoop down on a gecko.", "Retrieved Fact from OpenBook": "F: hawks eat lizards", "Abduced Query to find missing knowledge": "K: gecko is lizard", "Retrieved Missing Knowledge using IR": "K: Every gecko is a lizard."}

Table 2: Our approach with an example for the correct option.

(b) The original webpage responsible for the contamination.

Figure 12: OpenbookQA dirty entry example in Pile-train. The contamination source is a paper citing example from OpenbookQA benchmark.



🏢 Benchmark Contamination Monitoring System

This system monitors potential contamination in benchmark datasets used for evaluating language models across various open-source corpora 🧐.

The system is released along with our paper Infini-gram mini: Exact n-gram Search at the Internet Scale with FM-Index, which documents the methodology and findings in detail.

We welcome the community to submit new benchmarks for contamination analysis using the "Add New Benchmarks" tab.

Bulletin    Add New Benchmarks

Benchmark Contamination Bulletin

Click to view Instructions [Dropdown Arrow]

Select Benchmark Source
- core
- community

```jsonl
{"Benchmark": "AIME 2024", "Category": "Math", "Pile-train Dirty (%)": 0.0, "DCLM-baseline Dirty (%)": 0.0, "CC-2025-05 Dirty (%)": 10.0, "CC-2025-08 Dirty (%)": 3.3, "CC-2025-13 Dirty (%)": 6.7, "CC-2025-18 Dirty (%)": 60.0, "CC-2025-21 Dirty (%)": 40.0, "CC-2025-26 Dirty (%)": 13.3}
{"Benchmark": "OpenbookQA", "Category": "Commonsense Understanding", "Pile-train Dirty (%)": 10.8, "DCLM-baseline Dirty (%)": 15.6, "CC-2025-05 Dirty (%)": 14.6, "CC-2025-08 Dirty (%)": 30.2, "CC-2025-13 Dirty (%)": 13.2, "CC-2025-18 Dirty (%)": 13.4, "CC-2025-21 Dirty (%)": 13.2, "CC-2025-26 Dirty (%)": 12.2}
{"Benchmark": "MMLU", "Category": "Knowledge and Reasoning", "Pile-train Dirty (%)": 13.2, "DCLM-baseline Dirty (%)": 28.4, "CC-2025-05 Dirty (%)": 13.5, "CC-2025-08 Dirty (%)": 9.0, "CC-2025-13 Dirty (%)": 12.1, "CC-2025-18 Dirty (%)": 11.5, "CC-2025-21 Dirty (%)": 11.7, "CC-2025-26 Dirty (%)": 9.2}
{"Benchmark": "CoQA", "Category": "Reading Comprehension", "Pile-train Dirty (%)": 8.0, "DCLM-baseline Dirty (%)": 18.4, "CC-2025-05 Dirty (%)": 7.4, "CC-2025-08 Dirty (%)": 8.8, "CC-2025-13 Dirty (%)": 8.6, "CC-2025-18 Dirty (%)": 7.2, "CC-2025-21 Dirty (%)": 7.6, "CC-2025-26 Dirty (%)": 8.8}
{"Benchmark": "SQuAD", "Category": "Reading Comprehension", "Pile-train Dirty (%)": 2.8, "DCLM-baseline Dirty (%)": 40.1, "CC-2025-05 Dirty (%)": 2.7, "CC-2025-08 Dirty (%)": 33.0, "CC-2025-13 Dirty (%)": 10.1, "CC-2025-18 Dirty (%)": 1.5, "CC-2025-21 Dirty (%)": 2.0, "CC-2025-26 Dirty (%)": 8.5}
{"Benchmark": "MATH-500", "Category": "Math", "Pile-train Dirty (%)": 0.6, "DCLM-baseline Dirty (%)": 3.2, "CC-2025-05 Dirty (%)": 0.6, "CC-2025-08 Dirty (%)": 7.8, "CC-2025-13 Dirty (%)": 0.8, "CC-2025-18 Dirty (%)": 0.8, "CC-2025-21 Dirty (%)": 0.8, "CC-2025-26 Dirty (%)": 8.2}
```

🏢 Benchmark Contamination Bulletin

Bulletin    Add New Benchmarks

Add Your Own Benchmarks for Contamination Checking

```jsonl
{"Benchmark Name": ""}
{"Upload .jsonl File": "", "HuggingFace Dataset Path": ""}
{"Dataset split (only if providing HuggingFace Dataset)": "e.g., validation, test"}
{"Context or Question Field Name": "e.g., context, question, ..."}
```

[Button: Submit for Contamination Check]

Submission Status
[Empty Text Box]

Figure 13: Upper: A screenshot of our online Benchmark Contamination Bulletin. Lower: We invite the community to add new benchmarks or upload new ones for contamination analysis, which will be added to the bulletin.



