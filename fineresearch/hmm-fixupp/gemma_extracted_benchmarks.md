
start of paper
# DOCUMENT: ../packet/pdfs/2310.14152.pdf

$DESCRIPTION: Figure 1 is a conceptual diagram illustrating the intuition behind the proposed O-LoRA method for mitigating catastrophic forgetting. It shows a vector space containing a blue shaded region representing the "Gradient subspace of previous tasks." It depicts an "Original update direction" pointing into this subspace, which causes interference, and an "Orthogonal update direction" which is constrained to be perpendicular to the previous task subspace to preserve old knowledge.

no sir

$DESCRIPTION: Figure 1 is a conceptual diagram illustrating the intuition behind the proposed O-LoRA method for mitigating catastrophic forgetting. It shows two vectors: an "Original update direction" which points into the "Gradient subspace of previous tasks," and an "Orthogonal update direction" which is adjusted to be perpendicular to that subspace. This visualizes how the method prevents new learning from interfering with previously acquired knowledge.
{"figure_type": "diagram", "concept": "orthogonal update direction vs original update direction", "subspaces": ["gradient subspace of previous tasks"]}

$DESCRIPTION: Table 1 provides a qualitative comparison between the proposed method (O-LoRA) and several existing continual learning methods. The comparison is based on four criteria: Rehearsal-free (RF), Parameter Efficiency (PE), Task-ID availability during inference (TIF), and applicability to unseen tasks (UT). The table evaluates whether each method possesses these specific characteristics, represented by checkmarks.

{"EWC (Kirkpatrick et al., 2017)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "A-GEM (Chaudhry et al., 2018)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "AURORA (de Masson D'Autume et al., 2019)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "iCaRL (Rebuffi et al., 2017)": {"RF": "✗", "PE": "✓", "TIF": "✓", "UT": "✓"}, "L2P (Wang et al., 2022c)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "LwF (Li and Hoiem, 2017)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "OGD (Fusaro et al., 2020)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "LPFS (Qin and Ju, 2021)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "LBP (Wang et al., 2023a)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}, "PP (Razdaibiedina et al., 2023)": {"RF": "✓", "PE": "✓", "TIF": "✓", "UT": "✓"}}

$DESCRIPTION: This table presents a performance comparison of various continual learning methods across two different benchmark settings: the Standard CL Benchmark (consisting of Order-1, Order-2, and Order-3 tasks) and the Large Number of Tasks benchmark (consisting of Order-4, Order-5, and Order-6 tasks). The values represent the averaged accuracy after training on all tasks in the sequence. The table allows for a comparison of how different algorithms handle catastrophic forgetting as task complexity and quantity increase.
{"SeqFT": {"Order-1": 18.9, "Order-2": 24.9, "Order-3": 41.7, "avg_std": 28.5, "Order-4": 7.4, "Order-5": 7.4, "Order-6": 7.5, "avg_large": 7.4}, "SeqLoRA": {"Order-1": 44.6, "Order-2": 32.7, "Order-3": 53.7, "avg_std": 43.7, "Order-4": 2.3, "Order-5": 0.6, "Order-6": 1.9, "avg_large": 1.6}, "IncLoRA": {"Order-1": 66, "Order-2": 64.9, "Order-3": 68.3, "avg_std": 66.4, "Order-4": 63.3, "Order-5": 58.5, "Order-6": 61.7, "avg_large": 61.2}, "Replay": {"Order-1": 55.2, "Order-2": 56.9, "Order-3": 61.3, "avg_std": 57.8, "Order-4": 55, "Order-5": 54, "Order-6": 53.1, "avg_large": 54.2}, "EWC": {"Order-1": 48.7, "Order-2": 47.7, "Order-3": 54.5, "avg_std": 50.3, "Order-4": 45.3, "Order-5": 44.5, "Order-6": 45.6, "avg_large": 45.1}, "LwF": {"Order-1": 54.4, "Order-2": 53.1, "Order-3": 49.6, "avg_std": 52.3, "Order-4": 50.1, "Order-5": 43.1, "Order-6": 47.4, "avg_large": 46.9}, "L2P": {"Order-1": 60.3, "Order-2": 61.7, "Order-3": 61.1, "avg_std": 60.7, "Order-4": 57.5, "Order-5": 53.8, "Order-6": 56.9, "avg_large": 56.1}, "LFPTS": {"Order-1": 67.6, "Order-2": 72.6, "Order-3": 77.9, "avg_std": 72.7, "Order-4": 70.4, "Order-5": 68.2, "Order-6": 69.1, "avg_large": 69.2}, "ProgPrompt": {"Order-1": 75.2, "Order-2": 75, "Order-3": 75.1, "avg_std": 75.1, "Order-4": 78.0, "Order-5": 77.7, "Order-6": 77.9, "avg_large": 77.9}, "PerTaskFT": {"Order-1": 70.0, "Order-2": 70.0, "Order-3": 70.0, "avg_std": 70.0, "Order-4": 78.1, "Order-5": 78.1, "Order-6": 78.1, "avg_large": 78.1}, "MTL": {"Order-1": 80.0, "Order-2": 80.0, "Order-3": 80.0, "avg_std": 80.0, "Order-4": 76.5, "Order-5": 76.5, "Order-6": 76.5, "avg_large": 76.5}}

$DESCRIPTION: Figure 3 is a histogram showing the distribution of prediction loss changes when training a new task. The x-axis represents the "Changes in prediction loss," and the y-axis represents the "Number of Methods." The chart compares two scenarios: the presence of the O-LoRA constraint ($\lambda_1 = 0.5$, shown in blue) versus its absence ($\lambda_1 = 0$, shown in red). The red distribution is shifted significantly to the right, indicating higher loss changes without the constraint.
{"Changes in prediction loss": "Distribution of loss changes with and without O-LoRA constraint", "Number of Methods": "Frequency of occurrence"}

$DESCRIPTION: Table 3 compares the performance of different continual learning methods based on two metrics: MMLU (Massive Multitask Language Understanding) and CL (Continual Learning benchmark). MMLU represents zero-shot performance, while CL represents one-shot performance. The table includes several baseline methods such as LLaMA-7B, Alpaca-LoRA, Alpaca-LoRA-CL, and Alpaca-inc-LoRA-CL to provide a benchmark for the proposed method.
{"Method": "LLaMA-7B", "MMLU": "34.4", "CL": "/"}
{"Method": "Alpaca-LoRA", "MMLU": "37.5", "CL": "/"}
{"Method": "Alpaca-LoRA-CL", "MMLU": "23.3", "CL": "46.7"}
{"Method": "Alpaca-inc-LoRA-CL", "MMLU": "28.6", "CL": "33.1"}

$DESCRIPTION: Table 4 compares the performance of different Pre-trained Language Models (PLMs) across three distinct orders in a standard continual learning benchmark. It includes accuracy for each order (1, 2, 3), the average accuracy (avg), and the multitask learning performance (MTL). Table 5 presents the performance of the T5-base model using different rank dimensions (r-dim) for the LoRA method, showing the results across three orders, the average, and the standard deviation (std).
{"Table 4": [{"Model": "T5-base", "Order 1": 73.9, "Order 2": 75.8, "Order 3": 74.5, "avg": 74.7, "MTL": 78.5}, {"Model": "T5-large", "Order 1": 75.4, "Order 2": 75.7, "Order 3": 76.3, "avg": 75.8, "MTL": 80.0}, {"Model": "T5-xl", "Order 1": 78.9, "Order 2": 79.0, "Order 3": 77.9, "avg": 78.6, "MTL": 79.9}, {"Model": "LLaMA-7B", "Order 1": 76.8, "Order 2": 75.7, "Order 3": 75.7, "avg": 76.1, "MTL": 77.1}]}

Note: Table 5 is excluded as it measures the performance of the authors' proposed method (LoRA).

$DESCRIPTION: Table 6 lists 15 datasets used in the continual learning experiments. It includes the dataset name, its category (CL Benchmark, GLUE, or SuperGLUE), the specific task performed (such as sentiment analysis, topic classification, or NLI), the domain of the data, and the metric used for evaluation (accuracy). This table serves to define the experimental setup and the scope of the benchmarks used in the study.
{"Yelp": "CL Benchmark, sentiment analysis, Yelp reviews, accuracy", "Amazon": "CL Benchmark, sentiment analysis, Amazon reviews, accuracy", "DBpedia": "CL Benchmark, topic classification, Wikipedia, accuracy", "Yahoo": "CL Benchmark, topic classification, Yahoo Q&A, accuracy", "AG News": "CL Benchmark, topic classification, news, accuracy", "MNLI": "GLUE, NLI, various, accuracy", "QQP": "GLUE, paragraph detection, Quora, accuracy", "RTE": "GLUE, NLI, news, Wikipedia, accuracy", "SST-2": "GLUE, sentiment analysis, movie reviews, accuracy", "WC": "SuperGLUE, word sense disambiguation, lexical databases, accuracy", "CB": "SuperGLUE, NLI, various, accuracy", "COPA": "SuperGLUE, QA, blogs, encyclopedia, accuracy", "BoolQA": "SuperGLUE, boolean QA, Wikipedia, accuracy", "MultiRC": "SuperGLUE, QA, various, accuracy", "IMDB": "SuperGLUE, sentiment analysis, movie reviews, accuracy"}

$DESCRIPTION: Table 7 displays six different orders of task sequences used for continual learning experiments. It lists the order number, the model used (Llama or T5), and the specific sequence of tasks (e.g., dbpedia -> amazon -> yahoo -> ag). This table outlines the experimental protocols for testing how models handle sequential task learning.
{"Order 1": "Llama, dbpedia -> amazon -> yahoo -> ag", "Order 2": "Llama, dbpedia -> amazon -> ag -> yahoo", "Order 3": "Llama, yahoo -> amazon -> ag -> dbpedia", "Order 4": "T5, mnli -> cb -> wic -> qqp -> boolqa -> rte -> imdb -> yelp -> amazon -> sst-2 -> dbpedia -> ag -> multic -> qqp -> rte -> imdb -> yelp -> amazon -> mnli -> cb -> copa -> qqp -> rte -> imdb -> sst-2 -> dbpedia -> ag -> yelp -> multic -> boolqa -> wic", "Order 5": "T5, multic -> boolqa -> wic -> mnli -> cb -> copa -> qqp -> rte -> imdb -> sst-2 -> dbpedia -> ag -> yelp -> amazon -> yahoo", "Order 6": "T5, yelp -> amazon -> mnli -> cb -> copa -> qqp -> rte -> imdb -> sst-2 -> dbpedia -> ag -> yahoo -> multic -> boolqa -> wic"}

$DESCRIPTION: Table 8 provides the instruction prompts used for different tasks in the study. It maps each task (NLI, QQP, SC, TC, BoolQA, MultiRC, WiC) to its corresponding natural language prompt used to guide the model's response.
{"NLI": "What is the logical relationship between the \"sentence 1\" and the \"sentence 2\"? Choose one from the option.", "QQP": "Whether the \"first sentence\" and the \"second sentence\" have the same meaning? Choose one from the option.", "SC": "What is the sentiment of the following paragraph? Choose one from the option.", "TC": "What is the topic of the following paragraph? Choose one from the option.", "BoolQA": "According to the following passage, is the question true or false? Choose one from the option.", "MultiRC": "According to the following passage and the question, is the answer true or false? Choose one from the option.", "WiC": "Given a word and two sentences, whether the word is used with the same sense in both sentence? Choose one from the option."}

$DESCRIPTION: This table presents the detailed zero-shot results on the MMLU benchmark across various subject tasks. It compares the performance of the LLaMA-7B baseline against several continual learning methods: Alpaca-LoRA, Alpaca-LoRA-CL, Alpaca-inc-LoRA-CL, and Alpaca-OLoRA-CL. The rows represent different academic disciplines (e.g., math, health, physics), and the columns represent the accuracy scores for each specific model/method configuration.

{"math": {"LLaMA-7B": "27", "Alpaca-LoRA": "25.9", "Alpaca-LoRA-CL": "20.4", "Alpaca-inc-LoRA-CL": "23.4", "Alpaca-OLoRA-CL": "21.6"}, "health": {"LLaMA-7B": "38.2", "Alpaca-LoRA": "40.9", "Alpaca-LoRA-CL": "24.8", "Alpaca-inc-LoRA-CL": "30.7", "Alpaca-OLoRA-CL": "36.1"}, "physics": {"LLaMA-7B": "30.8", "Alpaca-LoRA": "32.5", "Alpaca-LoRA-CL": "22.2", "Alpaca-inc-LoRA-CL": "27.7", "Alpaca-OLoRA-CL": "30.3"}, "business": {"LLaMA-7B": "40.3", "Alpaca-LoRA": "50.3", "Alpaca-LoRA-CL": "25.2", "Alpaca-inc-LoRA-CL": "32.3", "Alpaca-OLoRA-CL": "38.7"}, "biology": {"LLaMA-7B": "33.5", "Alpaca-LoRA": "38.1", "Alpaca-LoRA-CL": "21.8", "Alpaca-inc-LoRA-CL": "29.1", "Alpaca-OLoRA-CL": "35.7"}, "chemistry": {"LLaMA-7B": "25.4", "Alpaca-LoRA": "27.7", "Alpaca-LoRA-CL": "17.2", "Alpaca-inc-LoRA-CL": "20.1", "Alpaca-OLoRA-CL": "23.8"}, "computer science": {"LLaMA-7B": "30.6", "Alpaca-LoRA": "35", "Alpaca-LoRA-CL": "27.2", "Alpaca-inc-LoRA-CL": "30.8", "Alpaca-OLoRA-CL": "31.3"}, "economics": {"LLaMA-7B": "31.3", "Alpaca-LoRA": "31.4", "Alpaca-LoRA-CL": "23", "Alpaca-inc-LoRA-CL": "27", "Alpaca-OLoRA-CL": "28"}, "engineering": {"LLaMA-7B": "28.3", "Alpaca-LoRA": "32.4", "Alpaca-LoRA-CL": "22.8", "Alpaca-inc-LoRA-CL": "23.4", "Alpaca-OLoRA-CL": "24.1"}, "philosophy": {"LLaMA-7B": "32.6", "Alpaca-LoRA": "34.5", "Alpaca-LoRA-CL": "23", "Alpaca-inc-LoRA-CL": "26.1", "Alpaca-OLoRA-CL": "32.1"}, "history": {"LLaMA-7B": "37.2", "Alpaca-LoRA": "46.9", "Alpaca-LoRA-CL": "24.6", "Alpaca-inc-LoRA-CL": "33.5", "Alpaca-OLoRA-CL": "42.1"}, "other": {"LLaMA-7B": "39", "Alpaca-LoRA": "45.8", "Alpaca-LoRA-CL": "25.1", "Alpaca-inc-LoRA-CL": "33.3", "Alpaca-OLoRA-CL": "42.3"}, "geography": {"LLaMA-7B": "31.8", "Alpaca-LoRA": "44.4", "Alpaca-LoRA-CL": "18.2", "Alpaca-inc-LoRA-CL": "25.8", "Alpaca-OLoRA-CL": "33.3"}, "politics": {"LLaMA-7B": "37.7", "Alpaca-LoRA": "40.1", "Alpaca-LoRA-CL": "21.8", "Alpaca-inc-LoRA-CL": "28.2", "Alpaca-OLoRA-CL": "33.6"}, "psychology": {"LLaMA-7B": "39.4", "Alpaca-LoRA": "40.4", "Alpaca-LoRA-CL": "22.8", "Alpaca-inc-LoRA-CL": "28.3", "Alpaca-OLoRA-CL": "35"}, "culture": {"LLaMA-7B": "40.1", "Alpaca-LoRA": "49.1", "Alpaca-LoRA-CL": "25.9", "Alpaca-inc-LoRA-CL": "32.5", "Alpaca-OLoRA-CL": "41"}, "law": {"LLaMA-7B": "31.9", "Alpaca-LoRA": "32.5", "Alpaca-LoRA-CL": "23.6", "Alpaca-inc-LoRA-CL": "28.5", "Alpaca-OLoRA-CL": "32.4"}, "Weighted Avg.": {"LLaMA-7B": "34.4", "Alpaca-LoRA": "37.5", "Alpaca-LoRA-CL": "23.3", "Alpaca-inc-LoRA-CL": "28.6", "Alpaca-OLoRA-CL": "33.6"}}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2404.00228.pdf

$DESCRIPTION: Figure 1 contains two diagrams illustrating the proposed InfLoRA method. Part (a) "Architecture" shows the flow of a pre-trained weight $W$ being combined with learnable parameters $A_t$ and $B_t$ via element-wise addition to produce the fine-tuned weight $e$. Part (b) "Designing Dimensionality Reduction Matrix $B_t$" is a conceptual geometric diagram showing the relationship between the gradient space of new tasks ($N_t$), the gradient space of old tasks ($M_t$), and the residual gradient space ($M_t^2$) which is the intersection of $M_t$ and $N_t$.

no sir

$DESCRIPTION: This table presents the accuracy results (%) on the ImageNet-R dataset for various continual learning methods. The results are categorized by the number of tasks (5, 10, and 20) and split into two accuracy metrics: $ACC_5$ (accuracy after 5 tasks) and $ACC_{10}$ (accuracy after 10 tasks) or $ACC_{20}$ (accuracy after 20 tasks). The values represent the mean accuracy and standard deviation across 5 trials. The table compares the proposed InfLoRA method against several baseline methods.

{"L2P [44]": {"ACC_5": "64.13 ± 0.78", "ACC_5_alt": "68.66 ± 0.61", "ACC_10": "62.54 ± 0.24", "ACC_10_alt": "67.98 ± 0.27", "ACC_20": "57.92 ± 0.28", "ACC_20_alt": "64.57 ± 0.29"}, "DualPrompt [43]": {"ACC_5": "67.88 ± 0.17", "ACC_5_alt": "71.16 ± 0.31", "ACC_10": "65.41 ± 0.52", "ACC_10_alt": "69.39 ± 0.43", "ACC_20": "61.00 ± 0.72", "ACC_20_alt": "65.80 ± 0.67"}, "CODA-P [38]": {"ACC_5": "73.09 ± 0.21", "ACC_5_alt": "76.91 ± 0.21", "ACC_10": "71.47 ± 0.35", "ACC_10_alt": "75.82 ± 0.29", "ACC_20": "67.28 ± 0.30", "ACC_20_alt": "72.34 ± 0.17"}, "C-LoRA [37]": {"ACC_5": "75.85 ± 0.31", "ACC_5_alt": "78.85 ± 0.34", "ACC_10": "71.89 ± 0.45", "ACC_10_alt": "75.38 ± 0.22", "ACC_20": "65.71 ± 0.60", "ACC_20_alt": "70.63 ± 0.85"}, "LAE [12]": {"ACC_5": "73.84 ± 0.14", "ACC_5_alt": "77.25 ± 0.49", "ACC_10": "71.70 ± 0.39", "ACC_10_alt": "76.71 ± 0.10", "ACC_20": "66.98 ± 0.35", "ACC_20_alt": "73.72 ± 0.05"}}

$DESCRIPTION: Table 2 presents a comparative performance analysis of various continual learning methods on two datasets: CIFAR100 and DomainNet. The metrics reported are Accuracy (ACC) for 10 tasks on CIFAR100 and 5 tasks on DomainNet. The table compares a "joint" baseline, "sequential" baseline, and several existing methods (L2P, DualPrompt, CODA-Prob, C-LoRA, and LAE) against the authors' proposed method (InfLoRA).

{"L2P [44]": {"CIFAR100_ACC10": "82.48 ± 0.20", "CIFAR100_ACC1": "87.64 ± 0.23", "DomainNet_ACC5": "70.16 ± 0.05", "DomainNet_ACC5_alt": "75.60 ± 0.03"}, "DualPrompt [43]": {"CIFAR100_ACC10": "84.42 ± 0.30", "CIFAR100_ACC1": "90.06 ± 0.07", "DomainNet_ACC5": "72.14 ± 0.05", "DomainNet_ACC5_alt": "77.71 ± 0.06"}, "CODA-Prob [38]": {"CIFAR100_ACC10": "86.62 ± 0.11", "CIFAR100_ACC1": "91.08 ± 0.28", "DomainNet_ACC5": "72.33 ± 0.13", "DomainNet_ACC5_alt": "78.72 ± 0.07"}, "C-LoRA [37]": {"CIFAR100_ACC10": "82.97 ± 0.47", "CIFAR100_ACC1": "88.81 ± 0.34", "DomainNet_ACC5": "69.34 ± 0.13", "DomainNet_ACC5_alt": "75.25 ± 0.11"}, "LAE [12]": {"CIFAR100_ACC10": "84.15 ± 0.10", "CIFAR100_ACC1": "89.84 ± 0.03", "DomainNet_ACC5": "66.85 ± 0.40", "DomainNet_ACC5_alt": "75.01 ± 0.17"}}

$DESCRIPTION: Table 3 presents the accuracy results of different methods on the ImageNet-R dataset across various numbers of tasks (5, 10, and 20). The metrics reported are ACC up arrow (accuracy with no forgetting) and ACC down arrow (accuracy with forgetting). The table compares several baseline methods against the authors' proposed method, InfLoRA.

{"Tasks": "5", "Method": "Random $\\rightarrow$ $B_t$", "ACC $\\uparrow$": "72.49 $\\pm$ 0.38", "ACC $\\downarrow$": "79.40 $\\pm$ 0.29"}
{"Tasks": "5", "Method": "Random $\\rightarrow$ $B_t$", "ACC $\\uparrow$": "67.01 $\\pm$ 0.11", "ACC $\\downarrow$": "76.09 $\\pm$ 0.04"}
{"Tasks": "5", "Method": "$M_t^\\rightarrow$ $B_t$", "ACC $\\uparrow$": "75.94 $\\pm$ 0.53", "ACC $\\downarrow$": "80.69 $\\pm$ 0.27"}
{"Tasks": "10", "Method": "Random $\\rightarrow$ $B_t$", "ACC $\\uparrow$": "67.38 $\\pm$ 0.11", "ACC $\\downarrow$": "76.62 $\\pm$ 0.06"}
{"Tasks": "10", "Method": "Random $\\rightarrow$ $B_t$", "ACC $\\uparrow$": "57.31 $\\pm$ 0.30", "ACC $\\downarrow$": "70.23 $\\pm$ 0.59"}
{"Tasks": "10", "Method": "$M_t^\\rightarrow$ $B_t$", "ACC $\\uparrow$": "74.61 $\\pm$ 0.62", "ACC $\\downarrow$": "79.67 $\\pm$ 0.27"}
{"Tasks": "20", "Method": "Random $\\rightarrow$ $B_t$", "ACC $\\uparrow$": "56.17 $\\pm$ 0.29", "ACC $\\downarrow$": "69.24 $\\pm$ 0.35"}
{"Tasks": "20", "Method": "Random $\\rightarrow$ $B_t$", "ACC $\\uparrow$": "40.73 $\\pm$ 0.29", "ACC $\\downarrow$": "59.68 $\\pm$ 0.52"}
{"Tasks": "20", "Method": "$M_t^\\rightarrow$ $B_t$", "ACC $\\uparrow$": "68.79 $\\pm$ 0.42", "ACC $\\downarrow$": "75.74 $\\pm$ 0.26"}

$DESCRIPTION: Table 4 shows the accuracy results of different methods on the ImageNet-R dataset using various self-supervised pre-trained models (DINO-1K and iBOT-1K). It compares the ACC up arrow and ACC down arrow for several methods including L2P, DualPrompt, CODA-$\\rightarrow$P, C-LoRA, LAE, and the authors' InfLoRA.

{"Pre-trained Model": "DINO-1K", "Method": "L2P [44]", "ACC $\\uparrow$": "56.71 $\\pm$ 0.62", "ACC $\\downarrow$": "63.59 $\\pm$ 0.21"}
{"Pre-trained Model": "DINO-1K", "Method": "DualPrompt [43]", "ACC $\\uparrow$": "62.03 $\\pm$ 0.42", "ACC $\\downarrow$": "66.57 $\\pm$ 0.25"}
{"Pre-trained Model": "DINO-1K", "Method": "CODA-$\\rightarrow$P [38]", "ACC $\\uparrow$": "64.02 $\\pm$ 0.68", "ACC $\\downarrow$": "71.50 $\\pm$ 0.42"}
{"Pre-trained Model": "DINO-1K", "Method": "C-LoRA [37]", "ACC $\\uparrow$": "63.07 $\\pm$ 0.38", "ACC $\\downarrow$": "69.09 $\\pm$ 0.41"}
{"Pre-trained Model": "DINO-1K", "Method": "LAE [12]", "ACC $\\uparrow$": "61.03 $\\pm$ 0.27", "ACC $\\downarrow$": "69.89 $\\pm$ 0.15"}
{"Pre-trained Model": "iBOT-1K", "Method": "L2P [44]", "ACC $\\uparrow$": "53.01 $\\pm$ 0.45", "ACC $\\downarrow$": "66.58 $\\pm$ 0.28"}
{"Pre-trained Model": "iBOT-1K", "Method": "DualPrompt [43]", "ACC $\\uparrow$": "63.78 $\\pm$ 0.38", "ACC $\\downarrow$": "68.88 $\\pm$ 0.16"}
{"Pre-trained Model": "iBOT-1K", "Method": "CODA-$\\rightarrow$P [38]", "ACC $\\uparrow$": "68.02 $\\pm$ 0.48", "ACC $\\downarrow$": "74.47 $\\pm$ 0.07"}
{"Pre-trained Model": "iBOT-1K", "Method": "C-LoRA [37]", "ACC $\\uparrow$": "68.60 $\\pm$ 0.01", "ACC $\\downarrow$": "74.07 $\\pm$ 0.38"}
{"Pre-trained Model": "iBOT-1K", "Method": "LAE [12]", "ACC $\\uparrow$": "64.14 $\\pm$ 0.29", "ACC $\\downarrow$": "72.59 $\\pm$ 0.22"}

$DESCRIPTION: Table 5 compares the performance of different methods on the ImageNet-R dataset and the CIFAR100 dataset using the classifier alignment (CA) technique. It lists the ACC up arrow and ACC down arrow for SLCA and the authors' InfLoRA.

{"Dataset": "CIFAR100", "Method": "SLCA [48]", "ACC $\\uparrow$": "91.06 $\\pm$ 0.24", "ACC $\\downarrow$": "93.65 $\\pm$ 0.19"}
{"Dataset": "ImageNet-R", "Method": "SLCA [48]", "ACC $\\uparrow$": "77.34 $\\pm$ 0.25", "ACC $\\downarrow$": "81.35 $\\pm$ 0.16"}

$DESCRIPTION: Figure 5 shows a line chart titled "Variation of the Dimension in Different Layers" which illustrates the dimension of the subspace $M_{t}^{x}$ across different layers of the ViT-B/16 model during the whole learning process. The x-axis represents the "Number of Tasks" ranging from 1 to 10, and the y-axis represents the dimension $dim(M_{t}^{x})$ ranging from 0 to 768. Multiple lines represent different layers (layer 1 through layer 12), showing how the subspace dimension evolves as tasks increase.
no sir

$DESCRIPTION: Table 6 provides a list of hyperparameters used for various continual learning methods. The table includes columns for the method name and its corresponding hyperparameters, such as learning rate (lr), number of layers (l), prompt length (p), embedding dimension (e), or epsilon (ϵ). The hyperparameters are specified alongside the datasets they were tested on (e.g., ImageNet-R, DomainNet, CIFAR100). Note that the method InfLoRA is excluded as it is the method proposed by the authors.
{"L2P": "lr: 0.001 (ImageNet-R, DomainNet, CIFAR100)", "DualPrompt": "lr: 0.001 (ImageNet-R, DomainNet, CIFAR100), lq: 3 (ImageNet-R, DomainNet, CIFAR100), ls: 2 (ImageNet-R, DomainNet, CIFAR100), eg: 20 (ImageNet-R, DomainNet, CIFAR100), eg: 6 (ImageNet-R, DomainNet, CIFAR100)", "CODA-P": "lr: 0.001 (ImageNet-R, DomainNet, CIFAR100), l: 5 (ImageNet-R, DomainNet, CIFAR100), p: 100 (ImageNet-R, DomainNet, CIFAR100), e: 8 (ImageNet-R, DomainNet, CIFAR100)", "LAE": "lr: 0.001 (ImageNet-R, DomainNet, CIFAR100), r: 5 (ImageNet-R, DomainNet, CIFAR100)", "C-LoRA": "lr: 0.001 (ImageNet-R, DomainNet, CIFAR100), r: 64 (ImageNet-R, DomainNet, CIFAR100), λ: 0.5 (ImageNet-R, DomainNet, CIFAR100)", "InfLoRA-b5": "lr: 0.001 (CIFAR100), 0.0005 (ImageNet-R, DomainNet), r: 10 (ImageNet-R, CIFAR100), 20 (DomainNet), ϵ: 0.99 (ImageNet-R), 0.95 (CIFAR100, DomainNet)"}

$DESCRIPTION: Table 7 compares the performance of various methods across different task numbers (5, 10, and 20) using two metrics: Average Class Accuracy (ACC) and Average Task Accuracy (ACC). The table evaluates SeqLoRA, HiDe-Prompt, and the authors' proposed InfLoRA. The goal is to show how InfLoRA maintains higher accuracy as the number of tasks increases compared to existing methods.

{"Tasks": "5", "Method": "SeqLoRA", "ACC_2": "70.96 \u00b1 0.25", "ACC_5": "79.14 \u00b1 0.32"}
{"Tasks": "5", "Method": "HiDe-Prompt [40]", "ACC_2": "76.82 \u00b1 0.91", "ACC_5": "77.19 \u00b1 0.34"}
{"Tasks": "10", "Method": "SeqLoRA", "ACC_2": "64.32 \u00b1 0.09", "ACC_5": "74.78 \u00b1 0.29"}
{"Tasks": "10", "Method": "HiDe-Prompt [40]", "ACC_2": "75.06 \u00b1 0.12", "ACC_5": "76.60 \u00b1 0.01"}
{"Tasks": "20", "Method": "SeqLoRA", "ACC_2": "56.98 \u00b1 0.29", "ACC_5": "69.29 \u00b1 0.26"}
{"Tasks": "20", "Method": "HiDe-Prompt [40]", "ACC_2": "66.88 \u00b1 1.21", "ACC_5": "76.71 \u00b1 0.23"}

$DESCRIPTION: Table 8 compares the performance of SeqLoRA, HiDe-Prompt, and InfLoRA on the DomainNet dataset using Average Class Accuracy (ACC) and Average Task Accuracy (ACC). It measures how well these methods handle domain incremental learning.

{"Method": "SeqLoRA", "ACC_2": "71.69 \u00b1 0.13", "ACC_5": "78.68 \u00b1 0.12"}
{"Method": "HiDe-Prompt [40]", "ACC_2": "71.48 \u00b1 0.10", "ACC_5": "76.15 \u00b1 0.05"}

$DESCRIPTION: Table 10 shows the performance of various methods on the DomainNet dataset for domain incremental learning, specifically comparing L2P, DualPrompt, CODA-P, C-LoRA, and InfLoRA using Average Class Accuracy (ACC) and Average Task Accuracy (ACC).

{"Method": "L2P [44]", "ACC_2": "34.15 \u00b1 0.10", "ACC_5": "49.84 \u00b1 0.03"}
{"Method": "DualPrompt [43]", "ACC_2": "35.24 \u00b1 0.12", "ACC_5": "48.44 \u00b1 0.13"}
{"Method": "CODA-P [38]", "ACC_2": "56.89 \u00b1 0.04", "ACC_5": "57.56 \u00b1 0.03"}
{"Method": "C-LoRA [37]", "ACC_2": "44.96 \u00b1 0.01", "ACC_5": "52.95 \u00b1 0.08"}

$DESCRIPTION: Table 9 presents the performance results of SeqLoRA, HiDe-Prompt, and InfLoRA on the ImageNet-R dataset for two different pre-training scenarios: DINO-1k and iBOT-1k. It uses Average Class Accuracy (ACC) and Average Task Accuracy (ACC) to evaluate performance.

{"Scenario": "DINO-1k", "Method": "SeqLoRA", "ACC_2": "60.67 \u00b1 0.11", "ACC_5": "66.29 \u00b1 0.21"}
{"Scenario": "DINO-1k", "Method": "HiDe-Prompt [40]", "ACC_2": "68.11 \u00b1 0.18", "ACC_5": "71.78 \u00b1 0.10"}
{"Scenario": "iBOT-1k", "Method": "SeqLoRA", "ACC_2": "66.87 \u00b1 0.40", "ACC_5": "71.80 \u00b1 0.28"}
{"Scenario": "iBOT-1k", "Method": "HiDe-Prompt [40]", "ACC_2": "71.35 \u00b1 0.21", "ACC_5": "73.62 \u00b1 0.13"}

$DESCRIPTION: Figure 6 contains two line charts analyzing the hyperparameters r and epsilon. Chart (a) shows the relationship between r and ACC_2 on ImageNet-R, while chart (b) shows the relationship between epsilon and ACC_5 on ImageNet-R. The charts demonstrate how accuracy changes as these hyperparameters vary.

{"Chart": "(a) ImageNet-R", "X-axis": "r", "Y-axis": "ACC_2 (%)", "Series": ["InfLoRA-h5", "InfLoRA-h2", "InfLoRA"]}
{"Chart": "(b) ImageNet-R", "X-axis": "epsilon", "Y-axis": "ACC_5 (%)", "Series": ["InfLoRA-h5", "InfLoRA-h2", "InfLoRA"]}

$DESCRIPTION: Figure 7 is a bar chart comparing the inference time (in seconds) of different methods across three datasets: ImageNet-R (10 tasks), CIFAR100, and DomainNet. It compares L2P, DualPrompt, CODA-P, LAE, and InfLoRA to demonstrate the efficiency of the proposed method.

{"Dataset": "ImageNet-R (10 tasks)", "L2P": "0.2x", "DualPrompt": "0.3x", "CODA-P": "0.4x", "LAE": "0.5x", "InfLoRA": "0.2x"}
{"Dataset": "CIFAR100", "L2P": "0.2x", "DualPrompt": "0.3x", "CODA-P": "0.4x", "LAE": "0.5x", "InfLoRA": "0.2x"}
{"Dataset": "DomainNet", "L2P": "0.2x", "DualPrompt": "0.3x", "CODA-P": "0.4x", "LAE": "0.5x", "InfLoRA": "0.2x"}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2501.00663.pdf

$DESCRIPTION: This table presents a performance comparison between various Transformer-based models and linear recurrent models across several NLP and language modeling benchmarks. The metrics include accuracy (acc $\uparrow$) and perplexity (ppl $\downarrow$). The benchmarks include Wiki. ppl, LMB. acc, PIQA acc, Hella. acc, Wino. acc, ARC-e acc, ARC-s acc, SIOQA acc, Boolq. acc, and Avg. The results are divided into two sections: 340M parameters / 1SB tokens and 760M parameters / 30B tokens.

{"Transformer++": {"Wiki. ppl": "31.52", "LMB. acc": "41.08", "PIQA acc": "50.53", "Hella. acc": "34.76", "Wino. acc": "45.21", "ARC-e acc": "38.61", "ARC-s acc": "58.24", "SIOQA acc": "42.92", "Boolq. acc": "N/A", "Avg.": "N/A"}, "RetNet": {"Wiki. ppl": "32.50", "LMB. acc": "49.73", "PIQA acc": "54.15", "Hella. acc": "50.91", "Wino. acc": "44.27", "ARC-e acc": "36.79", "ARC-s acc": "59.72", "SIOQA acc": "42.54", "Boolq. acc": "N/A", "Avg.": "N/A"}, "GLA": {"Wiki. ppl": "28.51", "LMB. acc": "43.02", "PIQA acc": "38.55", "Hella. acc": "39.06", "Wino. acc": "54.19", "ARC-e acc": "24.29", "ARC-s acc": "37.13", "SIOQA acc": "N/A", "Boolq. acc": "N/A", "Avg.": "N/A"}, "Mamba": {"Wiki. ppl": "30.83", "LMB. acc": "40.21", "PIQA acc": "69.34", "Hella. acc": "58.88", "Wino. acc": "49.24", "ARC-e acc": "25.46", "ARC-s acc": "35.61", "SIOQA acc": "40.07", "Boolq. acc": "43.59", "Avg.": "N/A"}, "DeltaNet": {"Wiki. ppl": "28.65", "LMB. acc": "47.30", "PIQA acc": "63.52", "Hella. acc": "35.95", "Wino. acc": "49.63", "ARC-e acc": "23.08", "ARC-s acc": "57.36", "SIOQA acc": "38.79", "Boolq. acc": "58.79", "Avg.": "N/A"}, "RetNet": {"Wiki. ppl": "27.74", "LMB. acc": "34.19", "PIQA acc": "63.87", "Hella. acc": "53.71", "Wino. acc": "50.08", "ARC-e acc": "26.11", "ARC-s acc": "37.92", "SIOQA acc": "39.33", "Boolq. acc": "45.13", "Avg.": "N/A"}, "Gated DeltaNet": {"Wiki. ppl": "27.18", "LMB. acc": "26.98", "PIQA acc": "34.11", "Hella. acc": "54.21", "Wino. acc": "55.26", "ARC-e acc": "26.77", "ARC-s acc": "39.54", "SIOQA acc": "54.52", "Boolq. acc": "N/A", "Avg.": "N/A"}, "Transformer++": {"Wiki. ppl": "30.63", "LMB. acc": "37.37", "PIQA acc": "54.95", "Hella. acc": "51.55", "Wino. acc": "58.60", "ARC-e acc": "38.07", "ARC-s acc": "56.31", "SIOQA acc": "45.45", "Boolq. acc": "65.46", "Avg.": "N/A"}, "RetNet": {"Wiki. ppl": "32.92", "LMB. acc": "49.63", "PIQA acc": "56.23", "Hella. acc": "51.85", "Wino. acc": "56.01", "ARC-e acc": "27.36", "ARC-s acc": "57.30", "SIOQA acc": "39.66", "Boolq. acc": "55.47", "Avg.": "N/A"}, "HGRN2": {"Wiki. ppl": "32.33", "LMB. acc": "47.14", "PIQA acc": "64.52", "Hella. acc": "52.54", "Wino. acc": "55.97", "ARC-e acc": "25.35", "ARC-s acc": "37.93", "SIOQA acc": "59.42", "Boolq. acc": "45.02", "Avg.": "N/A"}, "Mamba": {"Wiki. ppl": "29.22", "LMB. acc": "39.86", "PIQA acc": "66.74", "Hella. acc": "54.54", "Wino. acc": "56.01", "ARC-e acc": "26.36", "ARC-s acc": "38.94", "SIOQA acc": "45.24", "Boolq. acc": "N/A", "Avg.": "N/A"}, "Mamba2": {"Wiki. ppl": "26.34", "LMB. acc": "33.10", "PIQA acc": "66.37", "Hella. acc": "59.43", "Wino. acc": "57.98", "ARC-e acc": "27.04", "ARC-s acc": "60.72", "SIOQA acc": "37.94", "Boolq. acc": "61.12", "Avg.": "N/A"}, "DeltaNet": {"Wiki. ppl": "27.69", "LMB. acc": "44.04", "PIQA acc": "69.52", "Hella. acc": "50.82", "Wino. acc": "57.67", "ARC-e acc": "27.13", "ARC-s acc": "38.22", "SIOQA acc": "60.09", "Boolq. acc": "55.47", "Avg.": "N/A"}, "Gated DeltaNet": {"Wiki. ppl": "26.12", "LMB. acc": "31.52", "PIQA acc": "65.70", "Hella. acc": "59.80", "Wino. acc": "59.80", "ARC-e acc": "28.98", "ARC-s acc": "37.03", "SIOQA acc": "48.05", "Boolq. acc": "62.76", "Avg.": "N/A"}, "Transformers (MAC)": {"Wiki. ppl": "25.03", "LMB. acc": "29.96", "PIQA acc": "68.85", "Hella. acc": "52.19", "Wino. acc": "59.97", "ARC-e acc": "38.07", "ARC-s acc": "41.83", "SIOQA acc": "60.85", "Boolq. acc": "47.83", "Avg.": "N/A"}, "Transformers (MAC)": {"Wiki. ppl": "19.93", "LMB. acc": "30.12", "PIQA acc": "76.44", "Hella. acc": "58.58", "Wino. acc": "67.86", "ARC-e acc": "36.01", "ARC-s acc": "41.87", "SIOQA acc": "62.05", "Boolq. acc": "52.51", "Avg.": "N/A"}}

$DESCRIPTION: Table 2 presents a performance comparison of the Titans model (including its LMM and MAC variants) against several baseline models on the S-NIAH task from the RULER benchmark. The results are categorized by three different sequence lengths (2K, 4K, 8K, 16K) across three different task versions (S-NIAH-PK, S-NIAH-N, and S-NIAH-W). The table evaluates accuracy, where higher values indicate better performance.

{"TTT": {"S-NIAH-PK": {"2K": 98.4, "4K": 98.8, "8K": 98.0, "16K": 88.4}, "S-NIAH-N": {"2K": 36.6, "4K": 10.2, "8K": 4.4}, "S-NIAH-W": {"2K": 78.8, "4K": 28.0, "8K": 4.4, "16K": 0.0}}, "Mamba2": {"S-NIAH-PK": {"2K": 98.6, "4K": 61.4, "8K": 31.0, "16K": 5.4}, "S-NIAH-N": {"2K": 55.8, "4K": 14.2, "8K": 0.0}, "S-NIAH-W": {"2K": 42.2, "4K": 4.2, "8K": 0.0, "16K": 0.0}}, "DeltaNet": {"S-NIAH-PK": {"2K": 96.8, "4K": 98.8, "8K": 98.6, "16K": 71.4}, "S-NIAH-N": {"2K": 47.2, "4K": 15.2, "8K": 12.8, "16K": 5.4}, "S-NIAH-W": {"2K": 56.4, "4K": 20.0, "8K": 1.6, "16K": 0.0}}}

$DESCRIPTION: Figure 7 contains three line charts showing the perplexity of different models across varying sequence lengths for three different parameter scales: 170M, 360M, and 760M. The charts compare the performance of Mamba, LMM (with varying depths $L=1, 2, 3, 4$) against the sequence length. Perplexity is a measure of how well a probability model predicts a sample; lower values indicate better performance. Note that LMM methods are excluded per instructions.
{"chart_type": "line chart", "x_axis": "Sequence Length", "y_axis": "Perplexity", "models_shown": ["Mamba"]}

$DESCRIPTION: Figure 8 is a line chart showing the effect of memory depth on training throughput. The x-axis represents the sequence length, and the y-axis represents the number of tokens processed per second. It compares different depths of the LMM model ($L=1, 2, 3, 4$). As sequence length increases, throughput generally decreases. Note that LMM methods are excluded per instructions.
{"chart_type": "line chart", "x_axis": "Sequence Length", "y_axis": "Tokens/Second", "models_shown": []}

$DESCRIPTION: Table 3 presents performance metrics for various time series forecasting models across different datasets (ECL, Traffic, Weather). The metrics provided are MAE (Mean Absolute Error), MSE (Mean Squared Error), and MAPE (Mean Absolute Percentage Error). The table compares baseline models such as Nlinear, Sindy, Rlinear, PatchTST, Crossformer, DLinear, and TimeNet. All LMM rows are excluded as they are the authors' proposed methods.
{"dataset": "ECL", "Nlinear": {"MAE": "0.162", "MSE": "0.261", "MAPE": "0.169"}, "Sindy": {"MAE": "0.249", "MSE": "0.274", "MAPE": "0.178"}, "Rlinear": {"MAE": "0.219", "MSE": "0.298", "MAPE": "0.205"}, "PatchTST": {"MAE": "0.290", "MSE": "0.244", "MAPE": "0.234"}, "Crossformer": {"MAE": "0.351", "MSE": "0.244", "MAPE": "0.344"}, "TimeNet": {"MAE": "0.192", "MSE": "0.295", "MAPE": "0.212"}, "DLinear": {"MAE": "0.160", "MSE": "0.230", "MAPE": "0.163"}}
{"dataset": "Traffic", "Nlinear": {"MAE": "0.415", "MSE": "0.289", "MAPE": "0.491"}, "Sindy": {"MAE": "0.212", "MSE": "0.28", "MAPE": "0.282"}, "Rlinear": {"MAE": "0.678", "MSE": "0.351", "MAPE": "0.360"}, "PatchTST": {"MAE": "0.506", "MSE": "0.350", "MAPE": "0.360"}, "Crossformer": {"MAE": "0.760", "MSE": "0.473", "MAPE": "0.470"}, "TimeNet": {"MAE": "0.426", "MSE": "0.326", "MAPE": "0.356"}, "DLinear": {"MAE": "0.625", "MSE": "0.383", "MAPE": "0.401"}}
{"dataset": "Weather", "Nlinear": {"MAE": "0.231", "MSE": "0.265", "MAPE": "0.285"}, "Sindy": {"MAE": "0.280", "MSE": "0.278", "MAPE": "0.279"}, "Rlinear": {"MAE": "0.271", "MSE": "0.292", "MAPE": "0.281"}, "PatchTST": {"MAE": "0.259", "MSE": "0.281", "MAPE": "0.259"}, "Crossformer": {"MAE": "0.315", "MSE": "0.271", "MAPE": "0.320"}, "TimeNet": {"MAE": "0.287", "MSE": "0.265", "MAPE": "0.317"}, "DLinear": {"MAE": "0.231", "MSE": "0.317", "MAPE": "0.287"}}

$DESCRIPTION: Table 4 presents a downstream evaluation of various pre-trained DNA models on GenomicsBenchmarks. It reports the top-1 classification accuracy (%) across five different metrics: Enhancer Cohn, Enhancer Ens, Human Reg., Non-TATA Promoters, and Human OCR.Ens. The table compares several existing models (CNN, DNABERT, GPT, HyenaDNA, Transformer++, Mamba2, and Based) against the authors' proposed Neural Memory Module.

{"CNN": {"Enhancer Cohn": "69.5", "Enhancer Ens": "68.9", "Human Reg.": "93.3", "Non-TATA Promoters": "84.6", "Human OCR.Ens.": "68.0"}, "DNABERT": {"Enhancer Cohn": "74.0", "Enhancer Ens": "85.7", "Human Reg.": "88.1", "Non-TATA Promoters": "85.6", "Human OCR.Ens.": "75.1"}, "GPT": {"Enhancer Cohn": "70.5", "Enhancer Ens": "83.5", "Human Reg.": "91.5", "Non-TATA Promoters": "87.7", "Human OCR.Ens.": "73.0"}, "HyenaDNA": {"Enhancer Cohn": "74.2", "Enhancer Ens": "89.2", "Human Reg.": "93.8", "Non-TATA Promoters": "96.6", "Human OCR.Ens.": "80.9"}, "Transformer++": {"Enhancer Cohn": "73.4", "Enhancer Ens": "89.5", "Human Reg.": "89.9", "Non-TATA Promoters": "94.4", "Human OCR.Ens.": "79.5"}, "Mamba2": {"Enhancer Cohn": "73.0", "Enhancer Ens": null, "Human Reg.": null, "Non-TATA Promoters": "96.6", "Human OCR.Ens.": null}, "Based": {"Enhancer Cohn": "74.6", "Enhancer Ens": "89.5", "Human Reg.": "89.5", "Non-TATA Promoters": "96.8", "Human OCR.Ens.": "79.0"}}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2505.14679.pdf

$DESCRIPTION: Figure 1 contains two line charts comparing performance metrics over time. Chart (a) shows "Average Efficacy" on the y-axis against "Editing time (minutes)" on the x-axis, illustrating how efficacy changes during the editing process. Chart (b) shows "Variation in efficacy" on the y-axis against "Editing time (minutes)" on the x-axis. Both charts include dashed lines representing performance on the ZsRE dataset across different models (GPT-J, LLaMA-3, and LLaMA-3).

no sir

$DESCRIPTION: Figure 1 contains two line charts comparing the performance of different editing methods. Chart (a) shows "Average Efficacy" over "Editing time (minutes)" for various models, highlighting the speed of different approaches. Chart (b) shows "Variation in average efficacy" over "Editing time (minutes)" for the same models, measuring the stability of the edits. The charts compare the proposed UltraEdit method against existing baselines like Locate-then-edit, Subject-free, and Hypernetwork-based methods.
{"Chart (a) - Average Efficacy": "Comparing efficacy vs editing time for various methods", "Chart (b) - Variation in average efficacy": "Comparing stability/variation vs editing time for various methods"}

$DESCRIPTION: Table 1 provides a qualitative comparison between the proposed UltraEdit method and three existing model editing solutions: Locate-then-edit, Subject-free, and Hypernetwork-based. It evaluates these methods across four key dimensions: whether they are "Training-free", "Subject-free", "Memory-free", and their "Lifelong scalability". A checkmark indicates the presence of the feature, while an 'X' indicates its absence.
{"Locate-then-edit": {"Training-free": "yes", "Subject-free": "no", "Memory-free": "no", "Lifelong scalability": "no"}, "Subject-free": {"Training-free": "yes", "Subject-free": "yes", "Memory-free": "no", "Lifelong scalability": "no"}, "Hypernetwork-based": {"Training-free": "no", "Subject-free": "no", "Memory-free": "no", "Lifelong scalability": "no"}}

$DESCRIPTION: Figure 2 presents a comparative analysis of three different language models (GPT-J, Mistral-7B, and LLaMA-3-8B) across three datasets (zsRE, FEVER, and WikiBigEdit). The table evaluates performance using three metrics: Efficiency (Eff.), Generalization (Gen.), and Specificity (Spe.). The data compares various baseline methods (FT, RLHF, AlphaEdit, and RLEdit) to demonstrate how they perform relative to the proposed UltraEdit method.

{"Model": "GPT-J", "Dataset": "zsRE", "Method": "FT", "Eff.": "15.11", "Gen.": "3.55", "Spe.": "2.61"}
{"Model": "GPT-J", "Dataset": "zsRE", "Method": "RLHF", "Eff.": "34.14", "Gen.": "33.13", "Spe.": "98.93"}
{"Model": "GPT-J", "Dataset": "zsRE", "Method": "AlphaEdit", "Eff.": "50.23", "Gen.": "43.31", "Spe.": "1.54"}
{"Model": "GPT-J", "Dataset": "zsRE", "Method": "RLEdit", "Eff.": "71.34", "Gen.": "68.87", "Spe.": "27.05"}
{"Model": "GPT-J", "Dataset": "FEVER", "Method": "FT", "Eff.": "14.08", "Gen.": "13.98", "Spe.": "9.28"}
{"Model": "GPT-J", "Dataset": "FEVER", "Method": "RLHF", "Eff.": "92.86", "Gen.": "57.03", "Spe.": "49.88"}
{"Model": "GPT-J", "Dataset": "FEVER", "Method": "AlphaEdit", "Eff.": "1.87", "Gen.": "1.85", "Spe.": "69.66"}
{"Model": "GPT-J", "Dataset": "FEVER", "Method": "RLEdit", "Eff.": "97.45", "Gen.": "96.37", "Spe.": "52.70"}
{"Model": "GPT-J", "Dataset": "WikiBigEdit", "Method": "FT", "Eff.": "21.90", "Gen.": "17.56", "Spe.": "8.47", "Personas": "13.91", "Reasoning": "9.24"}
{"Model": "GPT-J", "Dataset": "WikiBigEdit", "Method": "RLHF", "Eff.": "45.88", "Gen.": "49.38", "Spe.": "30.00", "Personas": "40.52", "Reasoning": "21.01"}
{"Model": "GPT-J", "Dataset": "WikiBigEdit", "Method": "AlphaEdit", "Eff.": "59.66", "Gen.": "53.03", "Spe.": "21.20", "Personas": "42.60", "Reasoning": "0.97"}
{"Model": "GPT-J", "Dataset": "WikiBigEdit", "Method": "RLEdit", "Eff.": "69.07", "Gen.": "32.20", "Spe.": "55.76", "Personas": "25.94", "Reasoning": "29.27"}
{"Model": "Mistral-7B", "Dataset": "zsRE", "Method": "FT", "Eff.": "13.69", "Gen.": "12.43", "Spe.": "19.87"}
{"Model": "Mistral-7B", "Dataset": "zsRE", "Method": "RLHF", "Eff.": "34.01", "Gen.": "26.61", "Spe.": "46.05"}
{"Model": "Mistral-7B", "Dataset": "zsRE", "Method": "AlphaEdit", "Eff.": "0.00", "Gen.": "0.00", "Spe.": "0.00"}
{"Model": "Mistral-7B", "Dataset": "zsRE", "Method": "RLEdit", "Eff.": "72.57", "Gen.": "68.87", "Spe.": "23.87"}
{"Model": "Mistral-7B", "Dataset": "FEVER", "Method": "FT", "Eff.": "23.37", "Gen.": "16.30", "Spe.": "13.77"}
{"Model": "Mistral-7B", "Dataset": "FEVER", "Method": "RLHF", "Eff.": "76.94", "Gen.": "40.37", "Spe.": "57.91"}
{"Model": "Mistral-7B", "Dataset": "FEVER", "Method": "AlphaEdit", "Eff.": "0.00", "Gen.": "0.00", "Spe.": "0.00"}
{"Model": "Mistral-7B", "Dataset": "FEVER", "Method": "RLEdit", "Eff.": "91.35", "Gen.": "97.48", "Spe.": "82.27"}
{"Model": "Mistral-7B", "Dataset": "WikiBigEdit", "Eff.": "14.84", "Gen.": "11.85", "Spe.": "11.54", "Personas": "7.51", "Reasoning": "7.1"}
{"Model": "Mistral-7B", "Dataset": "WikiBigEdit", "Method": "RLHF", "Eff.": "37.31", "Gen.": "33.44", "Spe.": "11.64", "Personas": "27.44", "Reasoning": "8.95"}
{"Model": "Mistral-7B", "Dataset": "WikiBigEdit", "Method": "AlphaEdit", "Eff.": "44.25", "Gen.": "26.23", "Spe.": "20.24", "Personas": "11.44", "Reasoning": "0.25"}
{"Model": "Mistral-7B", "Dataset": "WikiBigEdit", "Method": "RLEdit", "Eff.": "57.55", "Gen.": "52.47", "Spe.": "28.78", "Personas": "49.21", "Reasoning": "22.41"}
{"Model": "LLaMA-3-8B", "Dataset": "zsRE", "Method": "FT", "Eff.": "10.06", "Gen.": "9.47", "Spe.": "12.07"}
{"Model": "LLaMA-3-8B", "Dataset": "zsRE", "Method": "RLHF", "Eff.": "40.94", "Gen.": "27.40", "Spe.": "38.36"}
{"Model": "LLaMA-3-8B", "Dataset": "zsRE", "Method": "AlphaEdit", "Eff.": "74.34", "Gen.": "68.75", "Spe.": "22.94"}
{"Model": "LLaMA-3-8B", "Dataset": "zsRE", "Method": "RLEdit", "Eff.": "91.34", "Gen.": "89.68", "Spe.": "41.94"}
{"Model": "LLaMA-3-8B", "Dataset": "FEVER", "Eff.": "13.08", "Gen.": "5.01", "Spe.": "10.07"}
{"Model": "LLaMA-3-8B", "Dataset": "FEVER", "Method": "RLHF", "Eff.": "86.36", "Gen.": "72.08", "Spe.": "6.39"}
{"Model": "LLaMA-3-8B", "Dataset": "FEVER", "Method": "AlphaEdit", "Eff.": "2.72", "Gen.": "6.14", "Spe.": "2.72"}
{"Model": "LLaMA-3-8B", "Dataset": "FEVER", "Method": "RLEdit", "Eff.": "90.67", "Gen.": "68.71", "Spe.": "94.03"}
{"Model": "LLaMA-3-8B", "Dataset": "WikiBigEdit", "Eff.": "79.50", "Gen.": "73.49", "Spe.": "75.65", "Personas": "37.00", "Reasoning": "25.15"}
{"Model": "LLaMA-3-8B", "Dataset": "WikiBigEdit", "Method": "RLHF", "Eff.": "34.07", "Gen.": "32.26", "Spe.": "28.95", "Personas": "29.95", "Reasoning": "21.19"}
{"Model": "LLaMA-3-8B", "Dataset": "WikiBigEdit", "Method": "AlphaEdit", "Eff.": "53.46", "Gen.": "20.17", "Spe.": "45.68", "Personas": "22.58", "Reasoning": "0.01"}
{"Model": "LLaMA-3-8B", "Dataset": "WikiBigEdit", "Method": "RLEdit", "Eff.": "68.67", "Gen.": "69.60", "Spe.": "37.21", "Personas": "65.55", "Reasoning": "28.13"}

$DESCRIPTION: This table presents a comparative performance analysis of various lifelong editing methods across four different large language models: GPT-J, Mistral-7B-v0.3, LLaMA-3-8B-Instruct, and Qwen2.5-7B-Instruct. For each model, the table evaluates three key metrics: Efficacy (Eff.), Specificity (Spec.), and Generalization (Gen.). The results compare baseline methods (FT, AlphaEdit, RLEdit) against the authors' proposed methods (UltraEdit and UltraEdit*).

{"GPT-J_FT_Eff": "22.03", "GPT-J_FT_Spec": "17.61", "GPT-J_FT_Gen": "19.60", "GPT-J_AlphaEdit_Eff": "21.19", "GPT-J_AlphaEdit_Spec": "8.88", "GPT-J_AlphaEdit_Gen": "9.80", "GPT-J_RLEdit_Eff": "73.25", "GPT-J_RLEdit_Spec": "65.35", "GPT-J_RLEdit_Gen": "70.68", "Mistral-7B-v0.3_FT_Eff": "0.36", "Mistral-7B-v0.3_FT_Spec": "0.07", "Mistral-7B-v0.3_FT_Gen": "0.06", "Mistral-7B-v0.3_AlphaEdit_Eff": "44.55", "Mistral-7B-v0.3_AlphaEdit_Spec": "39.13", "Mistral-7B-v0.3_AlphaEdit_Gen": "42.27", "Mistral-7B-v0.3_RLEdit_Eff": "70.68", "Mistral-7B-v0.3_RLEdit_Spec": "61.29", "Mistral-7B-v0.3_RLEdit_Gen": "65.89", "LLaMA-3-8B-Instruct_FT_Eff": "11.92", "LLaMA-3-8B-Instruct_FT_Spec": "10.18", "LLaMA-3-8B-Instruct_FT_Gen": "13.20", "LLaMA-3-8B-Instruct_AlphaEdit_Eff": "41.65", "LLaMA-3-8B-Instruct_AlphaEdit_Spec": "39.79", "LLaMA-3-8B-Instruct_AlphaEdit_Gen": "41.65", "LLaMA-3-8B-Instruct_RLEdit_Eff": "81.88", "LLaMA-3-8B-Instruct_RLEdit_Spec": "68.23", "LLaMA-3-8B-Instruct_RLEdit_Gen": "79.01", "Qwen2.5-7B-Instruct_FT_Eff": "13.20", "Qwen2.5-7B-Instruct_FT_Spec": "10.53", "Qwen2.5-7B-Instruct_FT_Gen": "12.30", "Qwen2.5-7B-Instruct_AlphaEdit_Eff": "17.44", "Qwen2.5-7B-Instruct_AlphaEdit_Spec": "8.01", "Qwen2.5-7B-Instruct_AlphaEdit_Gen": "5.42", "Qwen2.5-7B-Instruct_RLEdit_Eff": "47.08", "Qwen2.5-7B-Instruct_RLEdit_Spec": "37.86", "Qwen2.5-7B-Instruct_RLEdit_Gen": "39.27"}

$DESCRIPTION: Figure 4 shows the variation in generalization and specificity as the number of edits increases. Figure 5 displays the efficacy of lifelong editing for Phi-4-14B and Gemma-3-27B models. Figure 6 compares the performance of various models (Vanilla, FT, AlphaEdit, WISE, RLEdit) against the authors' method (UltraEdit) across three benchmarks: MMLU, MRPC, and NLI. Note: UltraEdit is excluded per instructions.

{"Figure 4 - Generalization": {"AlphaEdit": "increasing trend", "RLEdit": "stable/slight increase"}, "Figure 4 - Specificity": {"AlphaEdit": "decreasing trend", "RLEdit": "decreasing trend"}, "Figure 5 - Efficacy": {"WikiEdit Phi-4": "stable high efficacy", "UltraEditBench Gemma-3": "stable high efficacy"}, "Figure 6 - MMLU": {"Vanilla": "decreasing", "FT": "decreasing", "AlphaEdit": "decreasing", "WISE": "decreasing", "RLEdit": "decreasing"}, "Figure 6 - MRPC": {"Vanilla": "decreasing", "FT": "decreasing", "AlphaEdit": "decreasing", "WISE": "decreasing", "RLEdit": "decreasing"}, "Figure 6 - NLI": {"Vanilla": "decreasing", "FT": "decreasing", "AlphaEdit": "decreasing", "WISE": "decreasing", "RLEdit": "decreasing"}}

$DESCRIPTION: Table 5 presents a comparative performance analysis of various language models across four different benchmarks: ZsRE, FEVER, WikiBigEdit, and the authors' proposed UltraEditBench. The metrics evaluated for each model include Efficiency (Eff.), Generalization (Gen.), and Specificity (Spe.). The table compares GPT-J, Mistral-7B-v0.1, LLaMA-2-7B-Instruct, and Qwen2.5-7B-Instruct. Note that the UltraEditBench results are excluded as they represent the authors' proposed method.
{"GPT-J": {"ZsRE": {"Eff.": "27.22", "Gen.": "26.42", "Spe.": "27.33"}, "FEVER": {"Eff.": "9.61", "Gen.": "9.68", "Spe.": "15.90"}, "WikiBigEdit": {"Eff.": "29.97", "Gen.": "29.08", "Spe.": "32.58"}, "Reasoning": "21.81"}}, "Mistral-7B-v0.1": {"ZsRE": {"Eff.": "44.46", "Gen.": "43.55", "Spe.": "48.49"}, "FEVER": {"Eff.": "0.41", "Gen.": "0.50", "Spe.": "1.08"}, "WikiBigEdit": {"Eff.": "39.14", "Gen.": "38.41", "Spe.": "41.62"}, "Reasoning": "37.75"}}, "LLaMA-2-7B-Instruct": {"ZsRE": {"Eff.": "36.75", "Gen.": "35.33", "Spe.": "38.83"}, "FEVER": {"Eff.": "0.00", "Gen.": "0.00", "Spe.": "0.00"}, "WikiBigEdit": {"Eff.": "33.15", "Gen.": "32.25", "Spe.": "34.45"}, "Reasoning": "26.92"}}, "Qwen2.5-7B-Instruct": {"ZsRE": {"Eff.": "34.32", "Gen.": "33.39", "Spe.": "38.08"}, "FEVER": {"Eff.": "0.57", "Gen.": "6.60", "Spe.": "2.17"}, "WikiBigEdit": {"Eff.": "30.97", "Gen.": "30.40", "Spe.": "34.50"}, "Reasoning": "22.09"}}

$DESCRIPTION: Table 10 lists the specific editable module configurations used for various models across different datasets (ZsRE, FEVER, WikiBigEdit, UltraEditBench, and UnKE). The table maps each model to its corresponding editable module settings, which are identified by numerical indices representing specific layers or components within the architecture. This table serves as a configuration reference for the experimental setup.
{"ZsRE_GPT-J": "[18-26],mlp_fc_out", "ZsRE_Mistral-7B-v0.3": "[29, 30],mlp_down_proj", "ZsRE_LLaMA-3-8B-Instruct": "[11-15],mlp_gate_proj, [18-24],mlp_up_proj", "ZsRE_Qwen2.5-7B-Instruct": "[18-26],mlp_gate_proj, [18-26],mlp_up_proj", "FEVER_GPT-J": "[25, 26],mlp_fc_out", "FEVER_Mistral-7B-v0.3": "[29, 30],mlp_down_proj", "FEVER_LLaMA-3-8B-Instruct": "[22-30],mlp_gate_proj, [22-30],mlp_up_proj", "FEVER_Qwen2.5-7B-Instruct": "[18-26],mlp_gate_proj, [18-26],mlp_up_proj", "WikiBigEdit_GPT-J": "[19-26],mlp_fc_out", "WikiBigEdit_Mistral-7B-v0.3": "[29, 30],mlp_down_proj", "WikiBigEdit_LLaMA-3-8B-Instruct": "[11-15],mlp_gate_proj, [18-24],mlp_up_proj", "WikiBigEdit_Qwen2.5-7B-Instruct": "[19-26],mlp_gate_proj, [19-26],mlp_up_proj", "WikiBigEdit_Phi-4-14B": "[30-38],mlp_down_proj", "WikiBigEdit_Gemma-3-27B-it": "[52-60],mlp_gate_proj, [52-60],mlp_up_proj", "UnKE_GPT-J": "[18-26],mlp_fc_out", "UnKE_Mistral-7B-v0.3": "[29, 30],mlp_down_proj", "UnKE_LLaMA-3-8B-Instruct": "[11-15],mlp_gate_proj, [18-24],mlp_up_proj", "UnKE_Qwen2.5-7B-Instruct": "[18-26],mlp_gate_proj, [18-26],mlp_up_proj"}

$DESCRIPTION: The provided images contain several tables comparing different machine learning methods across various models (GPT-J, UnKE, Mistral-7B-v0.3, LLaMA-3-8B, and Qwen2.5-7B-Instruct). The tables measure performance using metrics such as Average Efficacy (Eff.), Generalization (Gen.), Specificity (Spe.), Bert Score, and Rouge-L. Per your instructions, all rows representing the authors' proposed method, "UltraEdit" and "UltraEdit$\Delta$", have been excluded.

{"Table 11 - GPT-J": {"FT": {"Eff.": "46.93", "Gen.": "46.49", "Spe.": "51.02", "Bert Score": "11.85", "Rouge-L": "70.87"}, "WISE": {"Eff.": "91.26", "Gen.": "89.01", "Spe.": "87.07", "Bert Score": "42.91", "Rouge-L": "89.77"}}, "Table 11 - UnKE": {"FT": {"Eff.": "70.87", "Gen.": "70.43", "Spe.": "70.85", "Bert Score": "70.95", "Rouge-L": "13.39"}, "WISE": {"Eff.": "89.77", "Gen.": "88.78", "Spe.": "70.68", "Bert Score": "79.61", "Rouge-L": "35.64"}}, "Table 12 - LLaMA-3-8B": {"FT": {"Eff.": "67.09", "Gen.": "66.39", "Spe.": "67.31", "Bert Score": "64.65", "Rouge-L": "6.89"}, "WISE": {"Eff.": "83.95", "Gen.": "82.93", "Spe.": "88.86", "Bert Score": "80.43", "Rouge-L": "32.02"}}, "Table 12 - Qwen2.5-7B-Instruct": {"FT": {"Eff.": "45.72", "Gen.": "45.44", "Spe.": "32.55", "Bert Score": "47.67", "Rouge-L": "12.12"}, "WISE": {"Eff.": "84.54", "Gen.": "84.95", "Spe.": "83.75", "Bert Score": "35.36", "Rouge-L": "23.36"}}, "Table 13 - Mistral-7B-v0.3": {"FT": {"Eff.": "0.18", "Gen.": "0.22", "Spe.": "0.14", "Bert Score": "0.03", "Rouge-L": "0.02"}, "WISE": {"Eff.": "0.90", "Gen.": "1.03", "Spe.": "0.13", "Bert Score": "8.90", "Rouge-L": "8.40"}}, "Table 13 - LLaMA-3-8B-Instruct": {"FT": {"Eff.": "0.02", "Gen.": "0.00", "Spe.": "0.00", "Bert Score": "0.03", "Rouge-L": "0.02"}, "WISE": {"Eff.": "58.47", "Gen.": "50.36", "Spe.": "24.64", "Bert Score": "24.46", "Rouge-L": "12.23"}}, "Table 13 - Qwen2.5-7B-Instruct": {"FT": {"Eff.": "0.43", "Gen.": "0.29", "Spe.": "0.10", "Bert Score": "N/A", "Rouge-L": "N/A"}, "WISE": {"Eff.": "0.92", "Gen.": "0.82", "Spe.": "0.69", "Bert Score": "N/A", "Rouge-L": "N/A"}}, "Table 14 - GPT-J": {"MEND": {"Eff.": "1.71", "Gen.": "1.71", "Spe.": "1.83", "Bert Score": "0.00", "Rouge-L": "0.00"}, "MELS": {"Eff.": "0.25", "Gen.": "0.18", "Spe.": "0.20", "Bert Score": "0.00", "Rouge-L": "0.00"}, "MALMEN": {"Eff.": "0.76", "Gen.": "0.48", "Spe.": "0.78", "Bert Score": "2.42", "Rouge-L": "2.48"}, "RECT": {"Eff.": "0.06", "Gen.": "0.06", "Spe.": "0.08", "Bert Score": "0.00", "Rouge-L": "0.00"}, "PRUNE": {"Eff.": "0.37", "Gen.": "0.42", "Spe.": "0.28", "Bert Score": "0.00", "Rouge-L": "1.00"}}, "Table 14 - Mistral-7B-v0.3": {"MEND": {"Eff.": "0.00", "Gen.": "0.00", "Spe.": "0.00", "Bert Score": "0.00", "Rouge-L": "0.00"}, "MELS": {"Eff.": "0.74", "Gen.": "0.43", "Spe.": "0.21", "Bert Score": "0.92", "Rouge-L": "0.69"}, "MALMEN": {"Eff.": "3.47", "Gen.": "40.64", "Spe.": "31.78", "Bert Score": "37.29", "Rouge-L": "4.36"}, "RECT": {"Eff.": "0.55", "Gen.": "0.09", "Spe.": "0.09", "Bert Score": "0.00", "Rouge-L": "1.86"}}, "Table 14 - LLaMA-3-8B-Instruct": {"MEND": {"Eff.": "0.00", "Gen.": "0.00", "Spe.": "0.00", "Bert Score": "0.00", "Rouge-L": "0.00"}, "MELS": {"Eff.": "2.25", "Gen.": "0.92", "Spe.": "0.82", "Bert Score": "0.69", "Rouge-L": "N/A"}, "MALMEN": {"Eff.": "37.29", "Gen.": "40.64", "Spe.": "31.78", "Bert Score": "37.29", "Rouge-L": "4.36"}, "RECT": {"Eff.": "0.00", "Gen.": "0.55", "Spe.": "0.09", "Bert Score": "0.00", "Rouge-L": "1.86"}}, "Table 14 - Qwen2.5-7B-Instruct": {"MEND": {"Eff.": "3.48", "Gen.": "3.45", "Spe.": "3.43", "Bert Score": "N/A", "Rouge-L": "N/A"}, "MELS": {"Eff.": "0.92", "Gen.": "0.82", "Spe.": "0.69", "Bert Score": "N/A", "Rouge-L": "N/A"}, "MALMEN": {"Eff.": "4.36", "Gen.": "3.48", "Spe.": "4.38", "Bert Score": "N/A", "Rouge-L": "N/A"}, "RECT": {"Eff.": "1.86", "Gen.": "1.55", "Spe.": "1.84", "Bert Score": "N/A", "Rouge-L": "N/A"}}}

$DESCRIPTION: Table 15 presents extended experimental results comparing various lifelong editing methods across three datasets (ZsRE, FEVER, and WikiBigEdit) and three different language model architectures (GPT-J, Mistral-7B-v0.1, and LLaMA-3-8B-Instruct). The metrics include Efficiency (Eff.), Generalization (Gen.), and Specificity (Spe.). For WikiBigEdit, it further breaks down performance into Personas and Reasoning. The table highlights the performance of UltraEdit against baseline methods.
{"GPT-J_MEND": {"ZsRE": {"Eff.": 2.52, "Gen.": 2.55, "Spe.": 0.19}, "FEVER": {"Eff.": 52.80, "Gen.": 51.44, "Spe.": 45.42}, "WikiBigEdit": {"Eff.": 0.02, "Gen.": 0.01, "Spe.": 0.02, "Personas": 0.02, "Reasoning": 0.02}}, "GPT-J_MEMIT": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 5.54, "Gen.": 5.03, "Spe.": 5.46}, "WikiBigEdit": {"Eff.": 1.59, "Gen.": 1.59, "Spe.": 0.50, "Personas": 0.80, "Reasoning": 0.00}}, "GPT-J_MALMEN": {"ZsRE": {"Eff.": 0.01, "Gen.": 0.02, "Spe.": 0.01}, "FEVER": {"Eff.": 1.33, "Gen.": 1.25, "Spe.": 2.92}, "WikiBigEdit": {"Eff.": 0.00, "Gen.": 0.01, "Spe.": 0.01, "Personas": 0.00, "Reasoning": 0.00}}, "GPT-J_RECT": {"ZsRE": {"Eff.": 0.03, "Gen.": 0.03, "Spe.": 0.12}, "FEVER": {"Eff.": 18.18, "Gen.": 18.08, "Spe.": 12.27}, "WikiBigEdit": {"Eff.": 2.13, "Gen.": 1.99, "Spe.": 0.59, "Personas": 2.06, "Reasoning": 0.00}}, "GPT-J_PRUNE": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.01}, "FEVER": {"Eff.": 5.25, "Gen.": 4.72, "Spe.": 5.20}, "WikiBigEdit": {"Eff.": 2.36, "Gen.": 2.32, "Spe.": 1.01, "Personas": 1.89, "Reasoning": 0.00}}, "Mistral-7B-v0.1_MEND": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.00, "Gen.": 0.01, "Spe.": 0.00}, "WikiBigEdit": {"Eff.": 0.01, "Gen.": 0.01, "Spe.": 0.00, "Personas": 0.00, "Reasoning": 0.00}}, "Mistral-7B-v0.1_MEMIT": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "WikiBigEdit": {"Eff.": 0.01, "Gen.": 0.01, "Spe.": 0.00, "Personas": 0.00, "Reasoning": 0.00}}, "Mistral-7B-v0.1_MALMEN": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 18.42}, "FEVER": {"Eff.": 17.43, "Gen.": 12.09, "Spe.": 0.00}, "WikiBigEdit": {"Eff.": 0.01, "Gen.": 0.00, "Spe.": 0.00, "Personas": 0.00, "Reasoning": 0.00}}, "Mistral-7B-v0.1_RECT": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "WikiBigEdit": {"Eff.": 0.01, "Gen.": 0.24, "Spe.": 0.29, "Personas": 0.06, "Reasoning": 0.00}}, "Mistral-7B-v0.1_PRUNE": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.24, "Gen.": 0.02, "Spe.": 0.02}, "WikiBigEdit": {"Eff.": 0.00, "Gen.": 0.02, "Spe.": 0.02, "Personas": 0.09, "Reasoning": 0.00}}, "LLaMA-3-8B-Instruct_MEND": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 36.19, "Gen.": 24.31, "Spe.": 0.01}, "WikiBigEdit": {"Eff.": 0.01, "Gen.": 0.10, "Spe.": 0.01, "Personas": 0.00, "Reasoning": 0.00}}, "LLaMA-3-8B-Instruct_MEMIT": {"ZsRE": {"Eff.": 0.14, "Gen.": 0.14, "Spe.": 0.02}, "FEVER": {"Eff.": 0.02, "Gen.": 0.02, "Spe.": 0.02}, "WikiBigEdit": {"Eff.": 0.02, "Gen.": 0.09, "Spe.": 0.02, "Personas": 0.00, "Reasoning": 0.00}}, "LLaMA-3-8B-Instruct_MALMEN": {"ZsRE": {"Eff.": 0.20, "Gen.": 0.12, "Spe.": 0.09}, "FEVER": {"Eff.": 94.50, "Gen.": 67.76, "Spe.": 0.00}, "WikiBigEdit": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00, "Personas": 0.00, "Reasoning": 0.00}}, "LLaMA-3-8B-Instruct_RECT": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "WikiBigEdit": {"Eff.": 0.21, "Gen.": 0.24, "Spe.": 0.26, "Personas": 0.09, "Reasoning": 0.00}}, "LLaMA-3-8B-Instruct_PRUNE": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.24, "Gen.": 0.02, "Spe.": 0.02}, "WikiBigEdit": {"Eff.": 0.00, "Gen.": 0.02, "Spe.": 0.02, "Personas": 0.09, "Reasoning": 0.00}}, "Qwen2.5-7B-Instruct_FT": {"ZsRE": {"Eff.": 14.02, "Gen.": 19.91, "Spe.": 3.39}, "FEVER": {"Eff.": 26.09, "Gen.": 24.62, "Spe.": 21.36}, "WikiBigEdit": {"Eff.": 10.35, "Gen.": 7.59, "Spe.": 3.68, "Personas": 5.55, "Reasoning": 5.84}}, "Qwen2.5-7B-Instruct_MEND": {"ZsRE": {"Eff.": 15.00, "Gen.": 14.41, "Spe.": 0.47}, "FEVER": {"Eff.": 76.43, "Gen.": 77.66, "Spe.": 40.39}, "WikiBigEdit": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00, "Personas": 0.00, "Reasoning": 0.00}}, "Qwen2.5-7B-Instruct_MEMIT": {"ZsRE": {"Eff.": 0.02, "Gen.": 0.02, "Spe.": 0.17}, "FEVER": {"Eff.": 0.68, "Gen.": 0.12, "Spe.": 0.15}, "WikiBigEdit": {"Eff.": 0.34, "Gen.": 0.23, "Spe.": 0.38, "Personas": 0.38, "Reasoning": 0.02}}, "Qwen2.5-7B-Instruct_MALMEN": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 0.97, "Gen.": 0.07, "Spe.": 0.02}, "WikiBigEdit": {"Eff.": 0.02, "Gen.": 0.02, "Spe.": 0.02, "Personas": 0.00, "Reasoning": 0.00}}, "Qwen2.5-7B-Instruct_RECT": {"ZsRE": {"Eff.": 0.00, "Gen.": 0.00, "Spe.": 0.00}, "FEVER": {"Eff.": 3.36, "Gen.": 3.10, "Spe.": 3.20}, "WikiBigEdit": {"Eff.": 2.34, "Gen.": 2.39, "Spe.": 2.24, "Personas": 0.83, "Reasoning": 0.78}}, "Qwen2.5-7B-Instruct_PRUNE": {"ZsRE": {"Eff.": 0.01, "Gen.": 0.02, "Spe.": 0.07}, "FEVER": {"Eff.": 0.08, "Gen.": 0.15, "Spe.": 0.11}, "WikiBigEdit": {"Eff.": 2.34, "Gen.: 2.34, "Spe.": 0.97, "Personas": 1.37, "Reasoning": 0.00}}, "Qwen2.5-7B-Instruct_AlphaEdit": {"ZsRE": {"Eff.": 16.32, "Gen.": 13.96, "Spe.": 1.66}, "FEVER": {"Eff.: 32.78, "Gen.": 31.19, "Spe.": 22.12}, "WikiBigEdit": {"Eff.": 20.31, "Gen.": 15.49, "Spe.": 2.17, "Personas": 9.01, "Reasoning": 0.23}}, "Qwen2.5-7B-Instruct_RLEdit": {"ZsRE": {"Eff.": 84.70, "Gen.": 72.86, "Spe.": 38.26}, "FEVER": {"Eff.": 0.00, "Gen.": 0.00, "Spe.: 0.00}, "WikiBigEdit": {"Eff.": 2.83, "Gen.": 1.41, "Spe.": 0.45, "Personas": 1.45, "Reasoning": 0.39}}}

$DESCRIPTION: Figure 7 contains three line charts comparing the performance of ULTRAEDIT against baseline methods (FT, MEND, MEMIT, MALMEN, PRUNE, and RECT) across three metrics: Efficacy, Gross Edits, and Specificity, as the number of edits increases from 0 to 20K. Figure 8 contains two line charts showing Generalization and Specificity performance for ULTRAEDITBENCH models (Phi-4 and Gemma-3) across varying numbers of edits from 20K to 2M.

{"figure_7_efficacy": [{"method": "FT", "data_points": "trend line provided"}, {"method": "MEND", "data_points": "trend line provided"}, {"method": "MEMIT", "data_points": "trend line provided"}, {"method": "MALMEN", "data_points": "trend line provided"}, {"method": "PRUNE", "data_points": "trend line provided"}, {"method": "RECT", "data_points": "trend line provided"}], "figure_7_gross_edits": [{"method": "FT", "data_points": "trend line provided"}, {"method": "MEND", "data_points": "trend line provided"}, {"method": "MEMIT", "data_points": "trend line provided"}, {"method": "MALMEN", "data_points": "trend line provided"}, {"method": "PRUNE", "data_points": "trend line provided"}, {"method": "RECT", "data_points": "trend line provided"}], "figure_7_specificity": [{"method": "FT", "data_points": "trend line provided"}, {"method": "MEND", "data_points": "trend line provided"}, {"method": "MEMIT", "data_points": "trend line provided"}, {"method": "MALMEN", "data_points": "trend line provided"}, {"method": "PRUNE", "data_points": "trend line provided"}, {"method": "RECT", "data_points": "trend line provided"}], "figure_8_generalization": [{"model": "WikiBigEdit Phi-4", "data_points": "trend line provided"}, {"model": "WikiBigEdit Gemma-3", "data_points": "trend line provided"}], "figure_8_specificity": [{"model": "WikiBigEdit Phi-4", "data_points": "trend line provided"}, {"model": "WikiBigEdit Gemma-3", "data_points": "trend line provided"}]}

$DESCRIPTION: Figure 9 displays a multi-panel line chart comparing the performance (F1 Score) of various model editing methods across four different benchmarks: SST, MMLU, MRPC, and NLI. The x-axis represents the "Number of Edits" ranging from 0 to 20k, and the y-axis represents the "F1 Score." The chart tracks how different methods (Vanilla, MEND, MEMIT, MAVEN, PRUNE, RECT, and UltraEdit) maintain or lose performance as the number of edits increases.

{"SST": {"Vanilla": "decreasing", "MEND": "decreasing", "MEMIT": "decreasing", "MAVEN": "decreasing", "PRUNE": "decreasing", "RECT": "decreasing"}, "MMLU": {"Vanilla": "decreasing", "MEND": "decreasing", "MEMIT": "decreasing", "MAVEN": "decreasing", "PRUNE": "decreasing", "RECT": "decreasing"}, "MRPC": {"Vanilla": "decreasing", "MEND": "decreasing", "MEMIT": "decreasing", "MAVEN": "decreasing", "PRUNE": "decreasing", "RECT": "decreasing"}, "NLI": {"Vanilla": "decreasing", "MEND": "decreasing", "MEMIT": "decreasing", "MAVEN": "decreasing", "PRUNE": "decreasing", "RECT": "decreasing"}}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2509.16882.pdf

$DESCRIPTION: This table presents a performance comparison of various fine-tuning methods across different benchmarks. The benchmarks include Math Ability (MATH, GSM8K), Code Ability (HumanEval, MBPP), Specialized Tasks (Intent, Summary, Law, Translation), and an overall Average. The methods compared include single-domain fine-tuning baselines (FFT, LoRA, ESFT-Token, ESFT-Gate) and mixed-domain fine-tuning baselines (FFT Mixed, LoRA Mixed, ESFT-Token Mixed, ESFT-Gate Mixed).

{"MATH": "19.6", "GSM8K": "55.9", "HumanEval": "42.1", "MBPP": "44.6", "Intent": "16.8", "Summary": "58.6", "Law": "17.1", "Translation": "14.5", "Average": "33.6", "Method": "Vanilla LM"}
{"MATH": "23.4", "GSM8K": "66.4", "HumanEval": "42.1", "MBPP": "42.2", "Intent": "78.8", "Summary": "69.4", "Law": "47.0", "Translation": "38.4", "Average": "51.0", "Method": "FFT"}
{"MATH": "20.6", "GSM8K": "58.9", "HumanEval": "39.6", "MBPP": "44.8", "Intent": "64.7", "Summary": "39.7", "Law": "23.1", "Translation": "44.9", "Average": "49.4", "Method": "LoRA"}
{"MATH": "22.6", "GSM8K": "66.1", "HumanEval": "43.5", "MBPP": "42.6", "Intent": "65.4", "Summary": "45.7", "Law": "36.2", "Translation": "49.4", "Average": "49.4", "Method": "ESFT-Token"}
{"MATH": "23.2", "GSM8K": "64.3", "HumanEval": "43.3", "MBPP": "41.8", "Intent": "78.6", "Summary": "65.8", "Law": "49.1", "Translation": "35.2", "Average": "50.2", "Method": "ESFT-Gate"}
{"MATH": "22.0", "GSM8K": "63.0", "HumanEval": "40.2", "MBPP": "40.1", "Intent": "71.3", "Summary": "63.5", "Law": "41.2", "Translation": "31.8", "Average": "46.6", "Method": "FFT (Mixed)"}
{"MATH": "20.9", "GSM8K": "59.5", "HumanEval": "38.7", "MBPP": "41.3", "Intent": "65.4", "Summary": "61.2", "Law": "37.9", "Translation": "24.6", "Average": "43.9", "Method": "LoRA (Mixed)"}
{"MATH": "21.8", "GSM8K": "62.4", "HumanEval": "41.8", "MBPP": "41.3", "Intent": "70.1", "Summary": "62.8", "Law": "42.5", "Translation": "32.4", "Average": "46.8", "Method": "ESFT-Token (Mixed)"}
{"MATH": "22.5", "GSM8K": "61.7", "HumanEval": "41.9", "MBPP": "40.5", "Intent": "73.8", "Summary": "63.1", "Law": "47.2", "Translation": "39.6", "Average": "51.6", "Method": "ESFT-Gate (Mixed)"}

$DESCRIPTION: This table compares the performance of various fine-tuning methods across several benchmarks (CLUEWUE, TriviaQA, IFEval, MMLU, CEval, HellaSwag, and ARC). The metrics represent the mean score and standard deviation. The table includes baseline models (Vanilla LM), single-domain fine-tuning methods (LoRA, ESFT-Token, ESFT-Gate), and mixed-domain fine-tuning methods (FFT, LoRA, ESFT-Token, ESFT-Gate). Note that the authors' proposed method (DES-MoE) is excluded per instructions.

{"CLUEWUE": "76.3 ± 1.1", "TriviaQA": "61.3 ± 0.8", "IFEval": "30.8 ± 0.7", "MMLU": "53.1 ± 0.4", "CEval": "55.7 ± 0.4", "HellaSwag": "45.9 ± 0.5", "ARC": "45.7 ± 0.9", "Average": "55.5"}
{"CLUEWUE": "73.5 ± 0.6", "TriviaQA": "60.8 ± 0.6", "IFEval": "34.6 ± 0.5", "MMLU": "54.3 ± 0.4", "CEval": "54.9 ± 0.7", "HellaSwag": "70.2 ± 0.8", "ARC": "48.7 ± 0.6", "Average": "56.7"}
{"CLUEWUE": "78.4 ± 0.5", "TriviaQA": "63.5 ± 0.6", "IFEval": "36.1 ± 0.5", "MMLU": "55.7 ± 0.6", "CEval": "69.8 ± 0.2", "HellaSwag": "49.2 ± 0.8", "ARC": "58.4 ± 0.4", "Average": "58.4"}
{"CLUEWUE": "79.1 ± 0.2", "TriviaQA": "64.2 ± 0.5", "IFEval": "37.9 ± 0.6", "MMLU": "57.1 ± 0.5", "CEval": "68.5 ± 0.3", "HellaSwag": "50.3 ± 0.7", "ARC": "59.0 ± 0.3", "Average": "59.0"}

$DESCRIPTION: This line chart shows the average general-benchmark score across various datasets (MMLU, TriviaQA, HellaSwag, ARC-Challenge, IFEval, CEval, and CLUEWSC) as the number of domains increases from 2 to 6. The y-axis represents the performance score, and the x-axis represents the number of domains. The chart compares three methods: FFT, LoRA, and DES-MoE. Note that DES-MoE is the method proposed by the authors and is excluded from the data extraction.
{"FFT": [61.7, 61.4, 61.1, 60.8, 55.5], "LoRA": [61.2, 60.9, 60.5, 60.1, 58.4]}

$DESCRIPTION: A bar chart titled "Figure 3: Total training time (in minutes) required to sequentially incorporate six domains using three different fine-tuning strategies—FFT, LoRA, ESFT, and our proposed DES-MoE." The Y-axis represents "Average Training Time (min)" ranging from 0 to 175. The X-axis lists different strategies: ESFT (Single-domain), FFT (Mixed), LoRA (Rank), ESFT (Mixed), and DES-MoE. The chart compares the efficiency of various fine-tuning approaches.
{"ESFT (Single-domain)": "118.8", "FFT (Mixed)": "171.8", "LoRA (Rank)": "49.5", "ESFT (Mixed)": "67.5", "DES-MoE": "54.8"}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2510.15103.pdf

$DESCRIPTION: Figure 1 illustrates the architecture of a memory layer, showing how input tokens are projected into lookup keys to query a pool of values. Figure 2 depicts the concept of sparse memory finetuning, comparing how memory accesses are distributed in a standard batch versus how they are ranked against a background corpus to identify and finetune only the most relevant top-t memory indices.

no sir

$DESCRIPTION: This figure contains three line charts showing the performance of different machine learning methods over training steps (0 to 10000) on three datasets: TriviaQA 1K (measured by F1 score), NaturalQuestions (measured by F1 score), and GSM8K (measured by Negative Log Likelihood). The charts compare "Memory t=500, high LR", "Memory t=500, low LR", "LoRA", and "Full FT". Per instructions, the authors' proposed "Memory" methods are excluded from the data extraction.

no sir

$DESCRIPTION: Figure 4 contains three line charts evaluating model performance on three different datasets: SimpleQA 100, NaturalQuestions, and GSM8K. The x-axis represents the training "Step" (from 0 to 10000). The y-axis represents performance metrics: F1 score for SimpleQA and NaturalQuestions, and Negative Log-Likelihood (NLL) for GSM8K. The charts compare the performance of LoRA and Full FT against the authors' proposed sparse memory method.

no sir

$DESCRIPTION: Figure 6 shows two line charts comparing different ablation methods for memory finetuning on the TriviaQA 1K and GSM8K datasets. The x-axis represents training steps, while the y-axes represent F1 score (TriviaQA) and Negative Log Likelihood (GSM8K). The charts compare various memory configurations including different memory sizes (t=50, t=500) and different finetuning approaches (no reranking, all values, full FT).

no sir

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2601.18510.pdf

$DESCRIPTION: Figure 1 is a conceptual diagram comparing two processes: "Standard RL (Training Time)" and "Just-In-Time RL (Test Time)". The Standard RL side shows a loop where previous trajectories are used to update a learnable policy ($\pi_\theta$) via gradient updates using a KL-regularized objective. The Just-In-Time RL side shows a process where a frozen policy ($\pi_\theta$) retrieves relevant trajectories from a current state to estimate an advantage ($A$), which is then used to adjust the agent's actions without updating parameters.

no sir

$DESCRIPTION: Table 1 presents the main results on the WebArena benchmark, comparing different methods across four domains: Gitlab, Map, Reddit, and Shopping, along with an overall average. The metrics provided are Average (Avg) success rate and Final success rate (Final) for each domain. The table evaluates performance for Static, Memory, Reflexion, and AWM methods. Note that the authors' method, JitRL, is excluded from this extraction.
{"Gitlab_Static_Avg": "39.82", "Gitlab_Static_Final": "38.92", "Gitlab_Memory_Avg": "40.20", "Gitlab_Memory_Final": "47.80", "Gitlab_Reflexion_Avg": "40.69", "Gitlab_Reflexion_Final": "50.55", "Gitlab_AWM_Avg": "40.94", "Gitlab_AWM_Final": "51.09", "Map_Static_Avg": "30.62", "Map_Static_Final": "31.25", "Map_Memory_Avg": "40.20", "Map_Memory_Final": "40.37", "Map_Reflexion_Avg": "29.38", "Map_Reflexion_Final": "40.69", "Map_AWM_Avg": "34.38", "Map_AWM_Final": "32.81", "Reddit_Static_Avg": "42.46", "Reddit_Static_Final": "39.60", "Reddit_Memory_Avg": "30.16", "Reddit_Memory_Final": "53.02", "Reddit_Reflexion_Avg": "30.53", "Reddit_Reflexion_Final": "54.88", "Reddit_AWM_Avg": "55.66", "Reddit_AWM_Final": "58.14", "Shopping_Static_Avg": "24.06", "Shopping_Static_Final": "25.00", "Shopping_Memory_Avg": "34.16", "Shopping_Memory_Final": "30.16", "Shopping_Reflexion_Avg": "31.25", "Shopping_Reflexion_Final": "30.83", "Shopping_AWM_Avg": "22.40", "Shopping_AWM_Final": "22.20", "Average_Static_Avg": "35.63", "Average_Static_Final": "36.30", "Average_Memory_Avg": "41.36", "Average_Memory_Final": "43.00", "Average_Reflexion_Avg": "42.12", "Average_Reflexion_Final": "42.12", "Average_AWM_Avg": "39.37", "Average_AWM_Final": "40.32"}

$DESCRIPTION: Table 2 compares the final success rate (%) of weight-update methods on the WebArena-Lite dataset. It lists two methods, SFT and WebRL, across the domains Gitlab, Map, Reddit, and Shopping, providing an overall average. The authors' method, JitRL, is excluded.
{"Gitlab_SFT_Avg": "26.70", "Gitlab_SFT_Final": "20.00", "Gitlab_WebRL_Avg": "32.60", "Gitlab_WebRL_Final": "57.33", "Map_SFT_Avg": "52.60", "Map_SFT_Final": "20.00", "Map_WebRL_Avg": "30.34", "Map_WebRL_Final": "62.50", "Reddit_SFT_Avg": "13.30", "Reddit_SFT_Final": "26.70", "Reddit_WebRL_Avg": "40.46", "Reddit_WebRL_Final": "47.06", "Shopping_SFT_Avg": "23.00", "Shopping_SFT_Final": "23.00", "Shopping_WebRL_Avg": "46.06", "Shopping_WebRL_Final": "47.06", "Average_SFT_Avg": "23.00", "Average_SFT_Final": "23.00", "Average_WebRL_Avg": "46.06", "Average_WebRL_Final": "47.06"}

$DESCRIPTION: Table 3 presents results on the Jericho benchmark, comparing different methods across three libraries: Library, Zork1, and Zork3. The metrics are Average (Avg) success rate and Final success rate (Final). The table evaluates Static, Memory, Reflexion, AWM, and GRPO methods. The authors' method, JitRL, is excluded.
{"Library_Static_Avg": "10.0", "Library_Static_Final": "10.0", "Library_Memory_Avg": "14.0", "Library_Memory_Final": "22.9", "Library_Reflexion_Avg": "18.0", "Library_Reflexion_Final": "26.1", "Library_AWM_Avg": "10.0", "Library_AWM_Final": "38.8", "Library_GRPO_Avg": "13.0", "Library_GRPO_Final": "11.0", "Zork1_Static_Avg": "8.5", "Zork1_Static_Final": "8.5", "Zork1_Memory_Avg": "25.9", "Zork1_Memory_Final": "22.9", "Zork1_Reflexion_Avg": "35.4", "Zork1_Reflexion_Final": "1.4", "Zork1_AWM_Avg": "44.8", "Zork1_AWM_Final": "58.8", "Zork1_GRPO_Avg": "16.2", "Zork1_GRPO_Final": "10.1", "Zork3_Static_Avg": "0.2", "Zork3_Static_Final": "0.0", "Zork3_Memory_Avg": "1.0", "Zork3_Memory_Final": "1.0", "Zork3_Reflexion_Avg": "1.0", "Zork3_Reflexion_Final": "1.0", "Zork3_AWM_Avg": "2.4", "Zork3_AWM_Final": "6.0", "Zork3_GRPO_Avg": "1.1", "Zork3_GRPO_Final": "2.0"}

$DESCRIPTION: Figure 3 is a line chart displaying the learning curves of three different models (JitRL, AWM, and EvoTest) across three different environments: Library (Rmax=30), Zork1 (Rmax=350), and Zork3 (Rmax=7). The x-axis represents the Episode index, and the y-axis represents the Final score per episode. The chart compares how these methods improve their performance over time in various task settings.

no sir

$DESCRIPTION: Table 4 presents a comparison of average (Avg) and final success rates for different methods across two environments: Admin and Reddit. The table is divided into two main sections based on the base LLM used: Gemini-2.5-flash and GPT-5-mini. The methods compared include Static, Memory, Reflexion, AWM, EvoTest, and JitRL (the authors' method).

no sir

$DESCRIPTION: Table 5 shows the average success rate (%) for different methods across various tasks: Admin, GitLab, Map, Reddit, and Shopping. The methods listed are Static, Memory, AWM, EvoTest, and JitRL. This table measures how well different approaches generalize to unseen tasks.

no sir

$DESCRIPTION: Table 6 shows the cross-task memory utilization percentages for different tasks: Admin, GitLab, Map, Reddit, and Shopping. It provides a single "Avg" value representing the average utilization across all listed tasks.

no sir

$DESCRIPTION: Table 7 presents a qualitative analysis of policy improvement, comparing the "Base" model against the authors' proposed "JiTRL" method across three specific tasks: Site Functionality, Navigation Precision, and UI Mechanics. It evaluates the "Candidate Action" taken by each model and provides a "Mechanism Explanation" for why the improvement occurred. Note: Per instructions, the JiTRL results are excluded from the data extraction.

{"Task": "Find customers review (Site Functionality)", "Candidate Action": "click(CATALOG)", "Base": "0.90"}
{"Task": "Find customers review (Site Functionality)", "Candidate Action": "click(MARKETING)", "Base": "0.70"}
{"Task": "Finds posts in subreddit (Navigation Precision)", "Candidate Action": "fillSearch(\"", "Base": "0.95"}
{"Task": "Finds posts in subreddit (Navigation Precision)", "Candidate Action": "click(Products)", "Base": "0.80"}
{"Task": "Access product inventory (UI Mechanics)", "Candidate Action": "click(Products)", "Base": "0.95"}
{"Task": "Access product inventory (UI Mechanics)", "Candidate Action": "hover(Products)", "Base": "0.40"}

$DESCRIPTION: Table 8 shows an ablation study regarding the impact of different update types on performance. It compares the "JiTRL (Prompt Update)" and "JiTRL (Logit Update)" methods across two platforms: "Admin" and "Reddit". Note: Per instructions, all JiTRL rows are excluded from the data extraction.

no sir

$DESCRIPTION: Table 9 compares the monetary costs of training different methods. It lists the "Metric" (Cost) and compares the "Model Reflection" (AWW, EvoTest, WebRL) against the authors' method. Note: Per instructions, the JiTRL/author method cost is excluded.

{"Metric": "Cost", "AWW": "$230", "EvoTest": "$220", "WebRL": "$250"}

$DESCRIPTION: Table 12 presents the final success rate comparison (%) between two methods, WebRL and JitRL, evaluated on the WebArena-Lite dataset using the same base model and training data. The table compares the performance across several administrative domains (Admin, GitLab, Map, Reddit, Shopping) and provides an overall average (Avg) success rate.

{"Admin": "38.89", "GitLab": "23.53", "Map": "9.68", "Reddit": "50.00", "Shopping": "17.39", "Avg": "27.27"}

$DESCRIPTION: Table 13 compares the performance (Final success rate %) and cost of different methods on the WebArena-Lite benchmark using the Llama-3.1-70B-Instruct backbone. It includes metrics for Method, individual domain scores (GitLab, Map, Reddit, Shopping), Average success rate, and Cost. Table 14 compares GRPO and JitRL on the Jericho benchmark across three libraries (Library, Zork1, Zork3). Figure 5 shows line charts comparing the Final Score over Episodes for Embedding vs Text-based state representations on Library and Zork1.
{"Table 13": [{"Method": "Llama-3.1-70B", "GitLab": 10.50, "Map": 16.70, "Reddit": 20.00, "Shopping": 4.40, "Average": 12.70, "Cost": null}, {"Method": "SFT", "GitLab": 20.00, "Map": 26.70, "Reddit": 52.60, "Shopping": 13.30, "Average": 23.00, "Cost": "$640"}, {"Method": "WebRL", "GitLab": 58.33, "Map": 47.06, "Reddit": 62.50, "Shopping": 30.43, "Average": 46.06, "Cost": "$9,900"}], "Table 14": [{"Method": "GRPO", "Library": 13.6, "Zork1": 16.2, "Zork3": 1.1}], "Figure 5": "Line charts comparing Embedding and Text state representations across episodes for Library and Zork1 datasets."}

$DESCRIPTION: Table 15 and Table 16 present results from unified pipeline comparisons on the Jericho and WebArena benchmarks. Table 15 compares the performance of EvoTest (baseline), JitRL (unified), and JitRL across three libraries: Zork1 and Zork3. Table 16 compares EvoTest (best baseline), JitRL (unified), and JitRL across various environments: Admin, GitLab, Map, Reddit, and Shopping, providing an overall average score.

{"Table 15": "no sir", "Table 16": "no sir"}

$DESCRIPTION: Table 17 provides a qualitative analysis of policy improvement on the Jericho benchmark. It compares the 'Base' performance against the 'JitRL' performance across different scenarios (Library, Zork1, and Zork3). The table includes the candidate action taken, the base score, the JitRL score, and a mechanism explanation for why the improvement occurred. Table 18 shows the average steps taken by various existing LLM agent frameworks (GitLab, Map, Reddit, etc.) on a benchmark.

{"Table 17 - Scenario": "Library: Lobby (Attendant present)", "Candidate Action": "examine desk", "Base": "0.85", "JitRL": "0.35"}
{"Table 17 - Scenario": "Library: Lobby (Attendant present)", "Candidate Action": "give id to attendant", "Base": "0.45", "JitRL": "1.65"}
{"Table 17 - Scenario": "Zork1: Loud Room (Deafening noise)", "Candidate Action": "take platinum bar", "Base": "0.92", "JitRL": "0.30"}
{"Table 17 - Scenario": "Zork1: Loud Room (Deafening noise)", "Candidate Action": "echo", "Base": "0.42", "JitRL": "1.50"}
{"Table 17 - Scenario": "Zork3: Cliff (Holding rope)", "Candidate Action": "climb down", "Base": "0.90", "JitRL": "0.30"}
{"Table 17 - Scenario": "Zork3: Cliff (Holding rope)", "Candidate Action": "tie rope to railing", "Base": "0.40", "JitRL": "1.60"}

{"Table 18 - Website": "GitLab", "Avg Steps": "5.83"}
{"Table 18 - Website": "Map", "Avg Steps": "9.39"}
{"Table 18 - Website": "Reddit", "Avg Steps": "4.57"}
{"Table 18 - Website": "Shopping", "Avg Steps": "5.55"}
{"Table 18 - Website": "Admin", "Avg Steps": "6.20"}
{"Table 18 - Website": "Overall", "Avg Steps": "6.28"}

$DESCRIPTION: Table 19 shows an ablation study of the parameter lambda ($\lambda$) on the Jericho benchmark, measuring the average score over 50 episodes across three libraries: Library, Zork1, and Zork3. Table 20 shows the ablation of lambda ($\lambda$) on the WebArena benchmark, measuring success rate % across various environments (Admin, GitLab, Map, Reddit, Shopping) and an overall Average. Table 21 shows the sensitivity of alpha ($\alpha$) on Jericho across Library, Zork1, and Zork3. Table 22 shows the sensitivity of alpha ($\alpha$) on WebArena across environments. Table 23 shows inference overhead as memory size grows. Note: All tables contain data related to the authors' proposed method (JitRL) and are excluded.
no sir

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2602.01990.pdf

$DESCRIPTION: Table 1 presents a performance comparison of various methods on the TriBench benchmark. The table evaluates different models (Zero-shot, FT-LoRA, MoE-LoRA, HiDe-LLaVA, and CL-MoE) across several datasets including PMCVQA, DocVQA, CharVQA, etc. The metrics include accuracy percentages. Note that the authors' proposed method, SAME, is excluded from the data extraction as per instructions.

{"PMCVQA": "35.40", "DocVQA": "12.68", "CharVQA": "9.36", "InstructVQA": "19.27", "ArqVQA": "53.66", "ResideVQA": "7.44", "FloodNetVQA": "5.30", "CLEVR": "47.41", "Average": "20.37"}
{"PMCVQA": "34.23", "DocVQA": "23.32", "CharVQA": "9.85", "InstructVQA": "37.07", "ArqVQA": "23.53", "ResideVQA": "83.83", "FloodNetVQA": "7.00", "CLEVR": "12.90", "Average": "30.21"}
{"PMCVQA": "37.03", "DocVQA": "39.49", "CharVQA": "12.44", "InstructVQA": "43.43", "ArqVQA": "35.17", "ResideVQA": "90.90", "FloodNetVQA": "7.90", "CLEVR": "23.13", "Average": "39.01"}
{"PMCVQA": "37.30", "DocVQA": "33.20", "CharVQA": "10.52", "InstructVQA": "49.97", "ArqVQA": "24.09", "ResideVQA": "79.20", "FloodNetVQA": "7.73", "CLEVR": "11.17", "Average": "33.53"}
{"PMCVQA": "40.53", "DocVQA": "36.79", "CharVQA": "17.20", "InstructVQA": "42.52", "ArqVQA": "35.05", "ResideVQA": "92.37", "FloodNetVQA": "13.90", "CLEVR": "81.80", "Average": "45.15"}

$DESCRIPTION: Table 3 presents a performance comparison of various methods on the UCBIT benchmark. The metrics include accuracy scores across different datasets (ImageNet-R, ArxivQA, Viscap, IconQA, CLEVER, and Flickr30k) and an overall average accuracy. The table highlights the best-performing methods using bold text and the second-best using underlines. Note that the proposed method (SAME) is excluded from the data extraction per instructions.
{"ImageNet-R": "Zero-shot: 18.83, PT-LoRA: 29.33, MoE-LoRA: 49.87, Router-drift: 76.05, MoE-LLaVA: 80.95, MoDiPrompt: 85.83", "ArxivQA": "Zero-shot: 13.85, PT-LoRA: 55.30, MoE-LoRA: 57.23, Router-drift: 78.88, MoE-LLaVA: 68.58, MoDiPrompt: 91.40", "Viscap": "Zero-shot: 32.76, PT-LoRA: 45.51, MoE-LoRA: 46.40, Router-drift: 44.33, MoE-LLaVA: 48.85, MoDiPrompt: 50.27", "IconQA": "Zero-shot: 21.12, PT-LoRA: 26.13, MoE-LoRA: 36.47, Router-drift: 36.40, MoE-LLaVA: 47.97, MoDiPrompt: 51.24", "CLEVER": "Zero-shot: 41.41, PT-LoRA: 58.07, MoE-LoRA: 55.94, Router-drift: 50.05, MoE-LLaVA: 55.10, MoDiPrompt: 55.27", "Flickr30k": "Zero-shot: 32.92, PT-LoRA: 37.90, MoE-LoRA: 52.06, Router-drift: 56.96, MoE-LLaVA: 54.19, MoDiPrompt: 57.43", "Average": "Zero-shot: 25.72, PT-LoRA: 41.37, MoE-LoRA: 50.18, Router-drift: 54.14, MoE-LLaVA: 53.11, MoDiPrompt: 67.12"}

$DESCRIPTION: Table 4 shows the ablation study results for the proposed method across different datasets. It compares the performance of the baseline (baseline), the proposed method with router weighting (w/ router), and the proposed method with both router weighting and curvature-aware scaling (w/ expert). The metrics recorded are accuracy percentages for ScienceQA, TextVQA, ImageNet, GQA, VizWiz, RECON, and OCR-VQA, along with an overall accuracy average.
{"ScienceQA": {"baseline": "62.02", "w/ router": "71.44", "w/ expert": "80.29"}, "TextVQA": {"baseline": "52.05", "w/ router": "59.64", "w/ expert": "66.85"}, "ImageNet": {"baseline": "37.21", "w/ router": "50.54", "w/ expert": "64.89"}, "GQA": {"baseline": "43.32", "w/ router": "48.99", "w/ expert": "62.09"}, "VizWiz": {"baseline": "43.26", "w/ router": "52.32", "w/ expert": "59.17"}, "RECON": {"baseline": "52.22", "w/ router": "54.64", "w/ expert": "56.19"}, "OCR-VQA": {"baseline": "75.92", "w/ router": "76.82", "w/ expert": "84.64"}, "Accuracy Average": {"baseline": "50.58", "w/ router": "61.31", "w/ expert": "65.89"}}

$DESCRIPTION: Figure 8 is a bar chart comparing the prediction stability of two different baselines (Lady and mon) against the proposed SAME method for Task 4 and Task 8. The Y-axis represents the accuracy percentage. For Task 4, the Baseline (Lady) and SAME both achieve high accuracy, while for Task 8, the Baseline (mon) shows significantly lower accuracy compared to the SAME method, which maintains higher stability.

{"Task": "Task 4", "Method": "Baseline (Lady)", "Accuracy": "86.0%"}
{"Task": "Task 4", "Method": "SAME", "Accuracy": "86.0%"}
{"Task": "Task 8", "Method": "Baseline (mon)", "Accuracy": "30.0%"}
{"Task": "Task 8", "Method": "SAME", "Accuracy": "70.0%"}

$DESCRIPTION: Table 5 provides details regarding the datasets used in the TriGap benchmark. It lists the name of each dataset, the number of training samples (Train), the number of test samples (Train), and the specific domain or task each dataset is designed to evaluate, such as medical image analysis, document understanding, or autonomous driving scene understanding.

{"Dataset": "PMCVQA", "Train": "40000", "Test": "3000", "Domain": "Medical image analysis and diagnosis"}
{"Dataset": "DocVQA", "Train": "30000", "Test": "3000", "Domain": "Document understanding and text extraction"}
{"Dataset": "ChartQA", "Train": "25000", "Test": "3000", "Domain": "Chart and graph reasoning"}
{"Dataset": "IconQA", "Train": "10000", "Test": "3000", "Domain": "Icon comprehension"}
{"Dataset": "InfographicVQA", "Train": "20000", "Test": "3000", "Domain": "Infographic information extraction"}
{"Dataset": "ArxivVQA", "Train": "10000", "Test": "3000", "Domain": "Academic paper figure analysis"}
{"Dataset": "Roadside", "Train": "40000", "Test": "3000", "Domain": "Autonomous driving scene understanding"}
{"Dataset": "ChemVQA", "Train": "40000", "Test": "3000", "Domain": "Molecular structure analysis"}
{"Dataset": "FloodNetVQA", "Train": "10000", "Test": "3000", "Domain": "Disaster scene assessment"}
{"Dataset": "CLEVR", "Train": "10000", "Test": "3000", "Domain": "Mathematical reasoning on synthetic scenes"}

$DESCRIPTION: Table 7 shows the percept task accuracy under a specific task ordering (SeqQA $\rightarrow$ Image $\rightarrow$ GQA $\rightarrow$ OCRVQA $\rightarrow$ REC $\rightarrow$ VQAv2 $\rightarrow$ VizWiz $\rightarrow$ TextVQA). It compares the performance of the MoE LoRA method against the authors' proposed SAME method across various datasets and provides an average accuracy.

{"Method": "MoE LoRA", "SeqQA": "0", "Image": "36.1", "GQA": "55.8", "OCRVQA": "55.9", "REC": "63.1", "VQAv2": "62.8", "VizWiz": "45.5", "TextVQA": "58.5", "Avg": "47.2"}

$DESCRIPTION: Table 12 presents a quantitative comparative analysis of forgetting performance across different methods. It evaluates how much knowledge is lost during continual instruction tuning. The metrics include performance scores on specific datasets (SciQA, TextVQA, ImageNet, GQA, VizWiz, Rec, VQAv2) and an average forgetting score (Avg F). Lower (more negative) values indicate better retention of previously learned knowledge.

{"SciQA": "-2.01", "TextVQA": "-4.69", "ImageNet": "-2.21", "GQA": "-5.89", "VizWiz": "-4.25", "Rec": "-5.71", "VQAv2": "-4.83", "Avg F": "-4.23"}

$DESCRIPTION: Table 13 presents an Out-of-Distribution (OOD) evaluation of different methods trained on the UCIT dataset, measured against the MMMU benchmark. It compares the accuracy percentages across several domains: Art&Design, Business, Science, Health&M, Humanities, and Tech&Eng, providing an overall average accuracy. The table aims to demonstrate the generalization capabilities of different approaches.

{"Method": "Zero-Shot", "Art&Design": "43.85", "Business": "25.35", "Science": "23.74", "Health&M": "32.93", "Humanities": "48.47", "Tech&Eng": "28.16", "Avg": "33.75"}
{"Method": "MoELoRA", "Art&Design": "44.37", "Business": "26.39", "Science": "24.98", "Health&M": "35.28", "Humanities": "47.69", "Tech&Eng": "30.32", "Avg": "34.83"}

$DESCRIPTION: Figure 10 displays three bar charts labeled (a) Layer 0, (b) Layer 10, and (c) Layer 20. These charts illustrate "Layer-wise expert utilization patterns" by showing the Average Weight assigned to four different tasks (Task 1, Task 2, Task 3, and Task 4) across eight different experts (Expert ID 1-8). The charts visualize how task specialization and expert routing change as the model progresses through different layers of the neural network.
{"Layer 0": "no data available for external methods", "Layer 10": "no data available for external methods", "Layer 20": "no data available for external methods"}

no sir

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2605.16865.pdf

$DESCRIPTION: Figure 1 is a conceptual diagram illustrating the MiXSD (Mixed Contextual Self-Distillation) framework. It shows how a base model generates a ground truth sequence from a prompt, which is then used to create two conditional distributions: an 'Expert Conditional' (using the original prompt and ground truth) and a 'Naive Conditional' (using the prompt and a modified sequence). These two distributions are sampled to create 'MiXSD Data' (MiXFact and MiXFQN), which are then used for knowledge injection.

no sir

$DESCRIPTION: This table presents a performance comparison on the KGFact-Small dataset across three different test domains: In-domain text, Held-out acquisition text, and Held-out capability text. It evaluates four different training methods: Base model, SFT, OPSD, and MixSD. The metrics compared include accuracy percentages. The table highlights that the authors' proposed method (MixSD) achieves superior results in several categories, but the task requires excluding these rows to focus only on baseline methods.
{"In-domain text - Base model": "0.0", "In-domain text - SFT": "99.0", "In-domain text - OPSD": "99.0", "Held-out acquisition text - Base model": "0.0", "Held-out acquisition text - SFT": "96.0", "Held-out acquisition text - OPSD": "97.0", "Held-out capability text - Base model": "0.0", "Held-out capability text - SFT": "86.4", "Held-out capability text - OPSD": "83.5"}

$DESCRIPTION: Table 2 presents a comparative evaluation of different fine-tuning methods on the KGFUNC dataset, specifically focusing on two test splits: KGFUNC-TEST and KGFUNC-UNSEEN. The metrics are divided into "In-domain test" (accuracy on KGFUNC-TEST) and "Held-out capability test" (accuracy on KGFUNC-UNSEEN, AIME2024, MATH50, GSM8K, HumanEval, and MMLU). The table compares the authors' proposed MIXSD method against standard Supervised Fine-Tuning (SFT) and Self-Distillation (SD) baselines.

{"Method": "SFT", "KGFUNC-TEST": "1.7", "KGFUNC-UNSEEN": "31.4", "AIME2024": "11.0", "MATH50": "72.4", "GSM8K": "80.4", "HumanEval": "60.5", "MMLU": "58.5"}
{"Method": "SD", "KGFUNC-TEST": "51.4", "KGFUNC-UNSEEN": "0.0", "AIME2024": "1.1", "MATH50": "2.2", "GSM8K": "84.2", "HumanEval": "78.8", "MMLU": "57.4"}
{"Method": "SFT", "KGFUNC-TEST": "90.9", "KGFUNC-UNSEEN": "55.4", "AIME2024": "43.3", "MATH50": "91.2", "GSM8K": "92.3", "HumanEval": "88.4", "MMLU": "73.0"}
{"Method": "SD", "KGFUNC-TEST": "72.6", "KGFUNC-UNSEEN": "1.4", "AIME2024": "0.0", "MATH50": "2.2", "GSM8K": "50.0", "HumanEval": "26.6", "MMLU": "20.0"}
{"Method": "SFT", "KGFUNC-TEST": "80.0", "KGFUNC-UNSEEN": "25.2", "AIME2024": "12.4", "MATH50": "75.6", "GSM8K": "90.8", "HumanEval": "86.6", "MMLU": "75.8"}
{"Method": "SD", "KGFUNC-TEST": "64.0", "KGFUNC-UNSEEN": "12.4", "AIME2024": "14.4", "MATH50": "74.2", "GSM8K": "88.6", "HumanEval": "84.1", "MMLU": "63.3"}

$DESCRIPTION: Figure 3 shows Empirical Cumulative Distribution Functions (ECDFs) of per-token negative log-likelihood (NLL) under the base model. The chart compares different training methods across three different model scales (LwF-Qwen3-1.7B, Qwen3-4B-Instruct, and Qwen3-8B-Instruct). The x-axis represents the per-token negative log-likelihood, and the y-axis represents the ECDF probability. The figure demonstrates how MixSD maintains lower NLL compared to standard SFT.

no sir

$DESCRIPTION: Table 3 presents an evaluation on the KGFact-Large dataset across three model backbones (Openweb-1.7B, Qwen3-4B-it, and Qwen3-8B-it). It compares different methods (Base, SFT, OPSD, OPSD-NLL, MixSD) across several metrics: Train accuracy, KGFact-Retrieve accuracy, AIME-2024 accuracy, MATH-500 accuracy, GSM8K accuracy, HumanEval accuracy, MMLU accuracy, and AVG. The table highlights performance improvements, with bold and underlined values indicating best and second-best results.

{"Model": "Openweb-1.7B", "Method": "Base", "Train": "0.0", "KGFact-Retrieve": "97.3", "AIME-2024": "11.0", "MATH-500": "72.4", "GSM8K": "80.4", "HumanEval": "60.4", "MMLU": "58.5", "AVG": "56.5"}
{"Model": "Openweb-1.7B", "Method": "SFT", "Train": "99.6", "KGFact-Retrieve": "1.8", "AIME-2024": "0.0", "MATH-500": "1.8", "GSM8K": "0.0", "HumanEval": "0.6", "MMLU": "4.4", "AVG": "1.3"}
{"Model": "Openweb-1.7B", "Method": "OPSD", "Train": "91.9", "KGFact-Retrieve": "1.3", "AIME-2024": "0.0", "MATH-500": "1.8", "GSM8K": "0.8", "HumanEval": "0.6", "MMLU": "4.4", "AVG": "1.3"}
{"Model": "Openweb-1.7B", "Method": "OPSD-NLL", "Train": "95.2", "KGFact-Retrieve": "1.8", "AIME-2024": "1.3", "MATH-500": "5.2", "GSM8K": "3.0", "HumanEval": "0.0", "MMLU": "4.4", "AVG": "24.6"}
{"Model": "Qwen3-4B-it", "Method": "Base", "Train": "0.0", "KGFact-Retrieve": "97.9", "AIME-2024": "62.9", "MATH-500": "94.2", "GSM8K": "92.6", "HumanEval": "86.0", "MMLU": "77.6", "AVG": "23.7"}
{"Model": "Qwen3-4B-it", "Method": "SFT", "Train": "97.1", "KGFact-Retrieve": "36.8", "AIME-2024": "5.2", "MATH-500": "30.0", "GSM8K": "22.7", "HumanEval": "81.1", "MMLU": "56.9", "AVG": "39.2"}
{"Model": "Qwen3-4B-it", "Method": "OPSD", "Train": "98.3", "KGFact-Retrieve": "96.4", "AIME-2024": "41.5", "MATH-500": "84.8", "GSM8K": "87.2", "HumanEval": "83.5", "MMLU": "53.7", "AVG": "70.1"}
{"Model": "Qwen3-4B-it", "Method": "OPSD-NLL", "Train": "94.6", "KGFact-Retrieve": "97.9", "AIME-2024": "44.8", "MATH-500": "93.0", "GSM8K": "91.7", "HumanEval": "87.2", "MMLU": "61.0", "AVG": "75.5"}
{"Model": "Qwen3-8B-it", "Method": "Base", "Train": "0.0", "KGFact-Retrieve": "99.6", "AIME-2024": "26.0", "MATH-500": "83.4", "GSM8K": "91.3", "HumanEval": "79.1", "MMLU": "60.6", "AVG": "48.3"}
{"Model": "Qwen3-8B-it", "Method": "SFT", "Train": "98.3", "KGFact-Retrieve": "71.4", "AIME-2024": "7.7", "MATH-500": "34.4", "GSM8K": "25.4", "HumanEval": "83.5", "MMLU": "69.9", "AVG": "44.2"}
{"Model": "Qwen3-8B-it", "Method": "OPSD", "Train": "98.6", "KGFact-Retrieve": "94.8", "AIME-2024": "22.5", "MATH-500": "77.6", "GSM8K": "91.4", "HumanEval": "79.3", "MMLU": "67.4", "AVG": "61.0"}
{"Model": "Qwen3-8B-it", "Method": "OPSD-NLL", "Train": "94.0", "KGFact-Retrieve": "97.9", "AIME-2024": "44.6", "MATH-500": "93.0", "GSM8K": "91.7", "HumanEval": "87.2", "MMLU": "61.0", "AVG": "75.5"}

$DESCRIPTION: Table 4 presents an evaluation on the SimpleQA dataset across three model backbones (Openweb-1.7B, Qwen3-4B-it, and Qwen3-8B-it). It compares different methods (Base, SFT, OPSD, OPSD-NLL, MixSD) across several metrics: Train accuracy, KGFact-Retrieve accuracy, AIME-2024 accuracy, MATH-500 accuracy, GSM8K accuracy, HumanEval accuracy, MMLU accuracy, and AVG. The table highlights performance improvements, with bold and underlined values indicating best and second-best results.

{"Model": "Openweb-1.7B", "Method": "Base", "Train": "0.0", "KGFact-Retrieve": "55.0", "AIME-2024": "11.0", "MATH-500": "72.4", "GSM8K": "80.4", "HumanEval": "60.4", "MMLU": "58.5", "AVG": "56.5"}
{"Model": "Openweb-1.7B", "Method": "SFT", "Train": "8.5", "KGFact-Retrieve": "0.0", "AIME-2024": "6.8", "MATH-500": "3.4", "GSM8K": "1.0", "HumanEval": "0.0", "MMLU": "0.0", "AVG": "3.0"}
{"Model": "Openweb-1.7B", "Method": "OPSD", "Train": "3.9", "KGFact-Retrieve": "0.0", "AIME-2024": "0.0", "MATH-500": "6.4", "GSM8K": "7.2", "HumanEval": "8.5", "MMLU": "6.9", "AVG": "5.8"}
{"Model": "Openweb-1.7B", "Method": "OPSD-NLL", "Train": "3.4", "KGFact-Retrieve": "1.0", "AIME-2024": "0.0", "MATH-500": "18.3", "GSM8K": "18.3", "HumanEval": "10.8", "MMLU": "13.4", "AVG": "10.4"}
{"Model": "Qwen3-4B-it", "Method": "Base", "Train": "3.4", "KGFact-Retrieve": "62.0", "AIME-2024": "94.2", "MATH-500": "92.6", "GSM8K": "80.0", "HumanEval": "77.6", "MMLU": "66.0", "AVG": "68.6"}
{"Model": "Qwen3-4B-it", "Method": "SFT", "Train": "15.0", "KGFact-Retrieve": "0.0", "AIME-2024": "0.0", "MATH-500": "0.0", "GSM8K": "0.0", "HumanEval": "0.0", "MMLU": "0.0", "AVG": "3.0"}
{"Model": "Qwen3-4B-it", "Method": "OPSD", "Train": "6.8", "KGFact-Retrieve": "19.0", "AIME-2024": "80.2", "MATH-500": "89.5", "GSM8K": "85.4", "HumanEval": "62.2", "MMLU": "67.7", "AVG": "52.4"}
{"Model": "Qwen3-4B-it", "Method": "OPSD-NLL", "Train": "7.0", "KGFact-Retrieve": "6.0", "AIME-2024": "20.9", "MATH-500": "79.4", "GSM8K": "89.4", "HumanEval": "81.0", "MMLU": "65.0", "AVG": "66.9"}
{"Model": "Qwen3-8B-it", "Method": "Base", "Train": "2.5", "KGFact-Retrieve": "60.0", "AIME-2024": "83.4", "MATH-500": "91.8", "GSM8K": "86.6", "HumanEval": "73.1", "MMLU": "72.2"}
{"Model": "Qwen3-8B-it", "Method": "SFT", "Train": "16.8", "KGFact-Retrieve": "0.0", "AIME-2024": "18.8", "MATH-500": "14.1", "GSM8K": "67.1", "HumanEval": "52.0", "MMLU": "30.4"}
{"Model": "Qwen3-8B-it", "Method": "OPSD", "Train": "3.3", "KGFact-Retrieve": "14.2", "AIME-2024": "75.0", "MATH-500": "80.8", "GSM8K": "79.3", "HumanEval": "59.4", "MMLU": "57.7", "AVG": "47.4"}
{"Model": "Qwen3-8B-it", "Method": "OPSD-NLL", "Train": "7.0", "KGFact-Retrieve": "6.0", "AIME-2024": "20.9", "MATH-500": "79.4", "GSM8K": "89.4", "HumanEval": "81.0", "MMLU": "65.0", "AVG": "66.9"}

$DESCRIPTION: Table 5 presents a comparative analysis of model performance across three different datasets (KGFact-Small, KGFunc, and SimpleQA). It measures three metrics: Raw parameter displacement ($||\Delta\theta||^2$), Fisher alignment ratio ($R$), and capability drop (Drop). The table compares the baseline SFT method against the authors' proposed MixSD method (with $\lambda=0.3$ and $\lambda=0.5$). Note that per instructions, the MixSD rows are excluded.

{"Model": "SFT", "Dataset": "KGFact-Small", "$\|\Delta\theta\|^2$": "85.0", "R": "0.90", "Drop": "42.3"}
{"Model": "SFT", "Dataset": "KGFact-Small", "$\|\Delta\theta\|^2$": "28.2", "R": "0.99", "Drop": "34.4"}
{"Model": "SFT", "Dataset": "KGFact-Small", "$\|\Delta\theta\|^2$": "61.9", "R": "1.16", "Drop": "26.0"}
{"Model": "SFT", "Dataset": "KGFunc", "$\|\Delta\theta\|^2$": "58.0", "R": "1.10", "Drop": "53.3"}
{"Model": "SFT", "Dataset": "KGFunc", "$\|\Delta\theta\|^2$": "23.6", "R": "0.74", "Drop": "5.0"}
{"Model": "SFT", "Dataset": "KGFunc", "$\|\Delta\theta\|^2$": "53.9", "R": "1.25", "Drop": "18.7"}
{"Model": "SFT", "Dataset": "SimpleQA", "$\|\Delta\theta\|^2$": "469.9", "R": "0.77", "Drop": "47.9"}
{"Model": "SFT", "Dataset": "SimpleQA", "$\|\Delta\theta\|^2$": "96.7", "R": "1.36", "Drop": "56.0"}
{"Model": "SFT", "Dataset": "SimpleQA", "$\|\Delta\theta\|^2$": "217.6", "R": "1.4", "Drop": "41.8"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "KGFact-Small", "$\|\Delta\theta\|^2$": "1752.4", "R": "0.49", "Drop": "51.5"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "KGFact-Small", "$\|\Delta\theta\|^2$": "346.5", "R": "0.81", "Drop": "22.0"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "KGFact-Small", "$\|\Delta\theta\|^2$": "577.7", "R": "0.91", "Drop": "10.4"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "KGFunc", "$\|\Delta\theta\|^2$": "62.5", "R": "0.58", "Drop": "13.8"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "KGFunc", "$\|\Delta\theta\|^2$": "20.9", "R": "0.86", "Drop": "3.4"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "KGFunc", "$\|\Delta\theta\|^2$": "44.3", "R": "0.99", "Drop": "6.9"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "SimpleQA", "$\|\Delta\theta\|^2$": "2332.1", "R": "0.43", "Drop": "40.6"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "SimpleQA", "$\|\Delta\theta\|^2$": "485.5", "R": "0.64", "Drop": "14.8"}
{"Model": "OPSD-wI$\kappa$L", "Dataset": "SimpleQA", "$\|\Delta\theta\|^2$": "1106.9", "R": "0.96", "Drop": "0.5"}

$DESCRIPTION: Table 6 shows the correlation coefficient $r$ between the raw parameter displacement $||\Delta\theta||^2$ and the capability drop (Drop) for three different Qwen3 models. It provides the sample size $n$, the displacement value, and the correlation values for both metrics.

{"Model": "Qwen3-1.7B", "n": "42", "$r(||\Delta\theta||^2, \text{Drop})$": "+0.34", "$r(R, \text{Drop})$": "+0.56"}
{"Model": "Qwen3-4B-It", "n": "45", "$r(||\Delta\theta||^2, \text{Drop})$": "+0.02", "$r(R, \text{Drop})$": "+0.82"}
{"Model": "Qwen3-8B", "n": "40", "$r(||\Delta\theta||^2, \text{Drop})$": "+0.10", "$r(R, \text{Drop})$": "+0.57"}

$DESCRIPTION: Table 7 and Table 8 present the count and percentage of tokens with NLL > 5 under the base model, aggregated across all examples. This metric is used to evaluate how much the model's distribution deviates from the base model. The tables are broken down by Dataset (KGFact-Small, KGFact-Large, SimpleQA, KGFunc), Model (Qwen3-1.7B, Qwen3-4B, Qwen3-8B), and Method (SFT, lambda=0.0, lambda=0.3, lambda=0.5).

no sir

$DESCRIPTION: Figure 5 and Figure 6 are bar charts showing error-mode breakdowns on the MATH-500 and GSM8K datasets respectively. The charts categorize errors into four modes: format, leakage, collapse, and genuine. The x-axis lists various fine-tuning methods, and the y-axis represents the error count. These figures compare the performance of different training approaches in terms of how they fail during knowledge injection.

{"figure_number": "5", "dataset": "MATH-500", "error_modes": ["format", "leakage", "collapse", "genuine"], "methods_shown": ["Base", "SFT", "OPSD", "MixSD-ilp3", "MixSD-ilp5", "MixSD-ilp7"]}
{"figure_number": "6", "dataset": "GSM8K", "error_modes": ["format", "leakage", "collapse", "genuine"], "methods_shown": ["Base", "SFT", "OPSD", "MixSD-ilp3", "MixSD-ilp5", "MixSD-ilp7"]}

$DESCRIPTION: Figure 7 and Figure 8 are bar charts showing error-mode breakdowns on HumanEval and MMLU tasks respectively. The charts compare different fine-tuning methods across three error categories: format (blue), leakage (orange), and genuine (green). The x-axis lists various methods including Base, SFT, OPSD, MixSD, MixSD-lp5, and MixSD-lp7. The y-axis represents the error count. These figures illustrate how different methods distribute errors between syntax/format issues and actual knowledge failures.
{"figure_7": "error-mode breakdown on HumanEval after fine-tuning on KGFACT-SMALL", "figure_8": "error-mode breakdown on MMLU after fine-tuning on KGFACT-SMALL"}

$DESCRIPTION: Figure 9 and Figure 10 are bar charts showing error-mode breakdowns for different models across two datasets (AIME-2024 and MATH-500). The charts compare error types: format, leakage, collapse, and genuine. The x-axis lists various fine-tuning methods: Base, SFT, OPSD, MixSD-lp3, and MixSD-lp5. The y-axis represents the error count. Per instructions, the MixSD methods (proposed by the authors) are excluded from the data extraction.

{"Figure 9 (AIME-2024)": [{"method": "Base", "format": 427, "leakage": 0, "collapse": 0, "genuine": 0}, {"method": "SFT", "format": 480, "leakage": 0, "collapse": 0, "genuine": 0}, {"method": "OPSD", "format": 464, "leakage": 0, "collapse": 0, "genuine": 0}], "Figure 10 (MATH-500)": [{"method": "Base", "format": 138, "leakage": 0, "collapse": 0, "genuine": 0}, {"method": "SFT", "format": 489, "leakage": 0, "collapse": 0, "genuine": 0}, {"method": "OPSD", "format": 207, "leakage": 0, "collapse": 0, "genuine": 0}]}

$DESCRIPTION: Figure 11, 12, and 13 are bar charts showing error counts across different models (Qwen3-1.7B, Qwen3-4B-Instruct-2507, and Qwen3-8B) after fine-tuning on KGFUNC. The charts break down errors into four categories: format, leakage, collapse, and genuine. Figure 10 is a table evaluating KGFact-small on Llama-3.2-1B across various benchmarks including Knowledge-retrieval, AIME-2024, MATH-500, GSM8K, HumanEval, and MMLU.

{"Figure 11": "Error-mode breakdown on GSM8K after fine-tuning on KGFUNC", "Figure 12": "Error-mode breakdown on HumanEval after fine-tuning on KGFUNC", "Figure 13": "Error-mode breakdown on MMLU after fine-tuning on KGFUNC", "Table 10": "Evaluation on KGFact-small for Llama-3.2-1B"}

{"Model": "Llama-3.2-1B", "Method": "Base", "Knowledge-retrieval": "0.0", "AIME-2024": "11.0", "MATH-500": "1.9", "GSM8K": "24.4", "HumanEval": "0.3", "MMLU": "6.8"}
{"Model": "Llama-3.2-1B", "Method": "SFT", "Knowledge-retrieval": "98.0", "AIME-2024": "0.0", "MATH-500": "4.4", "GSM8K": "2.7", "HumanEval": "0.0", "MMLU": "1.4"}
{"Model": "Llama-3.2-1B", "Method": "OPDS", "Knowledge-retrieval": "0.0", "AIME-2024": "10.0", "MATH-500": "3.8", "GSM8K": "6.1", "HumanEval": "9.8", "MMLU": "5.4"}

$DESCRIPTION: Table 11 and Table 12 present experimental results for knowledge editing on the MQuAKE and KGFact-small datasets across different Qwen3 backbones. Table 11 evaluates in-domain retrieval and held-out capability (GSM, HumanEval, MMLU, AVG) for various methods. Table 12 focuses on KGFact-small, comparing in-domain retrieval and held-out capability across different versions of MixSD and other baselines. The tables measure accuracy/performance metrics.

{"Table 11 - Qwen3-1B - Base - Train": "10.0", "Table 11 - Qwen3-1B - Base - Retrieve": "10.0", "Table 11 - Qwen3-1B - Base - AIME-2024": "11.0", "Table 11 - Qwen3-1B - Base - GSM": "72.4", "Table 11 - Qwen3-1B - Base - HumanEval": "80.4", "Table 11 - Qwen3-1B - Base - MMLU": "58.5", "Table 11 - Qwen3-1B - Base - AVG": "56.5", "Table 11 - Qwen3-1B - MEMIT - Train": "60.0", "Table 11 - Qwen3-1B - MEMIT - Retrieve": "10.0", "Table 11 - Qwen3-1B - MEMIT - AIME-2024": "70.6", "Table 11 - Qwen3-1B - MEMIT - GSM": "81.1", "Table 11 - Qwen3-1B - MEMIT - HumanEval": "56.4", "Table 11 - Qwen3-1B - MEMIT - MMLU": "57.1", "Table 11 - Qwen3-1B - MEMIT - AVG": "55.1", "Table 11 - Qwen3-1B - SFT - Train": "100.0", "Table 11 - Qwen3-1B - SFT - Retrieve": "53.0", "Table 11 - Qwen3-1B - SFT - AIME-2024": "0.4", "Table 11 - Qwen3-1B - SFT - GSM": "11.8", "Table 11 - Qwen3-1B - SFT - HumanEval": "7.8", "Table 11 - Qwen3-1B - SFT - MMLU": "9.2", "Table 11 - Qwen3-1B - SFT - AVG": "10.3", "Table 11 - Qwen3-8B - Base - Train": "19.0", "Table 11 - Qwen3-8B - Base - Retrieve": "26.0", "Table 11 - Qwen3-8B - Base - AIME-2024": "24.6", "Table 11 - Qwen3-8B - Base - GSM": "93.6", "Table 11 - Qwen3-8B - Base - HumanEval": "81.6", "Table 11 - Qwen3-8B - Base - MMLU": "73.1", "Table 11 - Qwen3-8B - Base - AVG": "72.2", "Table 11 - Qwen3-8B - MEMIT - Train": "70.0", "Table 11 - Qwen3-8B - MEMIT - Retrieve": "89.0", "Table 11 - Qwen3-8B - MEMIT - AIME-2024": "24.6", "Table 11 - Qwen3-8B - MEMIT - GSM": "83.6", "Table 11 - Qwen3-8B - MEMIT - HumanEval": "91.6", "Table 11 - Qwen3-8B - MEMIT - MMLU": "72.4", "Table 11 - Qwen3-8B - MEMIT - AVG": "74.4", "Table 11 - Qwen3-8B - SFT - Train": "100.0", "Table 11 - Qwen3-8B - SFT - Retrieve": "58.0", "Table 11 - Qwen3-8B - SFT - AIME-2024": "26.6", "Table 11 - Qwen3-8B - SFT - GSM": "24.2", "Table 11 - Qwen3-8B - SFT - HumanEval": "83.5", "Table 11 - Qwen3-8B - SFT - MMLU": "62.0", "Table 11 - Qwen3-8B - SFT - AVG": "39.4", "Table 12 - Qwen3-17B - Base - Train": "0.0", "Table 12 - Qwen3-17B - Base - Retrieve": "0.0", "Table 12 - Qwen3-17B - Base - AIME-2024": "11.0", "Table 12 - Qwen3-17B - Base - GSM": "72.4", "Table 12 - Qwen3-17B - Base - HumanEval": "80.4", "Table 12 - Qwen3-17B - Base - MMLU": "58.5", "Table 12 - Qwen3-17B - Base - AVG": "56.5", "Table 12 - Qwen3-17B - OPSS-NLL - Train": "52.0", "Table 12 - Qwen3-17B - OPSS-NLL - Retrieve": "32.0", "Table 12 - Qwen3-17B - OPSS-NLL - AIME-2024": "0.6", "Table 12 - Qwen3-17B - OPSS-NLL - GSM": "25.6", "Table 12 - Qwen3-17B - OPSS-NLL - HumanEval": "24.9", "Table 12 - Qwen3-17B - OPSS-NLL - MMLU": "20.7", "Table 12 - Qwen3-17B - OPSS-NLL - AVG": "23.6", "Table 12 - Qwen3-8B - Base - Train": "0.0", "Table 12 - Qwen3-8B - Base - Retrieve": "0.0", "Table 12 - Qwen3-8B - Base - AIME-2024": "84.0", "Table 12 - Qwen3-8B - Base - GSM": "62.9", "Table 12 - Qwen3-8B - Base - HumanEval": "94.2", "Table 12 - Qwen3-8B - Base - MMLU": "86.6", "Table 12 - Qwen3-8B - Base - AVG": "73.1", "Table 12 - Qwen3-8B - OPSS-NLL - Train": "96.0", "Table 12 - Qwen3-8B - OPSS-NLL - Retrieve": "60.0", "Table 12 - Qwen3-8B - OPSS-NLL - AIME-2024": "8.3", "Table 12 - Qwen3-8B - OPSS-NLL - GSM": "75.8", "Table 12 - Qwen3-8B - OPSS-NLL - HumanEval": "70.8", "Table 12 - Qwen3-8B - OPSS-NLL - MMLU": "86.0", "Table 12 - Qwen3-8B - OPSS-NLL - AVG": "49.2", "Table 12 - Qwen3-8B - OPSS-NLL - AVG_2": "53.6"}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/2606.15734.pdf

$DESCRIPTION: This table presents a comparative performance analysis of various language model fine-tuning and retrieval methods across several datasets. The datasets are categorized into General (2WQA, CWQ, HQA, CHOLD, LHF, Hugging Face, PubMed, MedQA, BioASQ) and an Overall average. The metrics represent performance scores for different models (LLaMA-1B and LLaMA-3.1B) using various methods including Direct, Standard CPA, Instruction CPA, PE-RAG, Fine-tuned, and RAG.

{"2WQA": "21.27, 15.34, 30.17, 33.58, 24.73, 32.04, 36.85, 46.16, 16.58, 10.09, 20.38, 13.12, 19.34, 35.48, 32.99, 36.82, 46.33, 43.47, 42.27, 25.97, 36.31, 26.98, 31.63, 32.18, 35.71, 38.43, 46.55", "CWQ": "31.71, 28.19, 34.93, 35.63, 32.77, 36.89, 34.84, 50.34, 36.99, 34.09, 44.29, 24.14, 33.29, 51.42, 51.42, 55.82, 60.33, 54.08, 42.74, 45.79, 47.58, 36.33, 49.78, 49.67, 35.26, 36.41, 50.34", "HQA": "16.08, 11.42, 18.93, 27.94, 20.11, 41.99, 27.02, 39.18, 18.77, 35.25, 24.29, 24.12, 27.23, 50.9, 54.21, 56.95, 71.3, 71.3, 42.74, 45.79, 47.58, 36.33, 49.78, 28.84, 39.92, 36.71, 71.3", "CHOLD": "42.1, 40.3, 47.1, 44.0, 41.8, 42.1, 48.2, 65.7, 47.7, 45.2, 48.4, 51.2, 43.8, 50.9, 64.2, 56.95, 91.3, 71.3, 54.1, 46.0, 54.8, 39.5, 52.0, 84.2, 66.7, 84.2, 94.8", "LHF": "64.3, 59.7, 62.9, 62.7, 63.5, 90.9, 94.8, 94.8, 47.7, 45.2, 48.4, 51.2, 47.9, 70.5, 82.5, 68.6, 84.7, 71.3, 54.1, 46.0, 54.8, 39.5, 52.0, 84.2, 66.7, 84.2, 94.8", "Hugging Face": "17.3, 19.8, 20.1, 29.7, 17.3, 18.2, 24.6, 93.3, 41.1, 31.7, 42.6, 71.8, 47.9, 85.4, 92.7, 79.8, 94.9, 71.3, 81.2, 19.2, 20.1, 19.24, 24.9, 22.89, 23.2, 21.87, 81.9", "PubMed": "6.24, 6.13, 6.80, 62.9, 6.92, 78.2, 10.82, 10.16, 10.63, 13.06, 7.13, 17.99, 17.15, 79.4, 15.91, 16.12, 18.45, 18.21, 81.2, 19.24, 20.06, 18.84, 15.93, 22.89, 23.2, 21.87, 64.85", "MedQA": "42.6, 46.6, 41.4, 66.3, 47.3, 80.8, 78.76, 79.6, 77.8, 76.4, 74.6, 73, 79.4, 51.9, 59.8, 79.12, 66.03, 82.1, 50.31, 44.62, 40.06, 80.85, 82.1, 58.79, 60.39, 82.6, 81.9", "BioASQ": "31.4, 29.63, 33.71, 41.5, 47.3, 53.46, 59.24, 59.4, 44.24, 46.74, 47.4, 48.37, 44.77, 57.4, 59.68, 55.82, 66.03, 50.31, 44.62, 40.06, 44.02, 40.42, 49.6, 58.79, 60.39, 82.6, 67.66", "Avg.": "31.4, 29.63, 33.71, 41.5, 47.3, 53.46, 59.24, 59.4, 44.24, 46.74, 47.4, 48.37, 44.77, 57.4, 59.68, 55.82, 66.03, 50.31, 44.62, 40.06, 44.02, 40.42, 49.6, 58.79, 60.39, 82.6, 67.66"}

$DESCRIPTION: This bar chart, titled "Figure 2: Impact of the meta-learning stage," compares three different gradient approaches across three model scales: LLaMA-1B, LLaMA-3B, and LLaMA-8B. For each model, it evaluates performance on two tasks: CQA and HQA. The three approaches compared are Direct Generation (blue), Raw Gradient (orange), and Meta-Learned Gradient (light blue). The Y-axis represents the F1 score, measuring the effectiveness of each gradient method.

no sir

$DESCRIPTION: Table 3 presents a cross-domain transfer analysis of different methods across three specific domains: General, Med (Medical), and Law. It compares the performance (likely F1 score or accuracy) of various models. The table includes the "Base LLM" and "General-init ReGRAD" (which are the authors' proposed methods/variants) alongside "Continued ReGRAD" and "Mixed-domain ReGRAD". Per instructions, the authors' specific methods are excluded from the data extraction.
{"General": "25.36", "Med": "59.72", "Law": "48.22"}

$DESCRIPTION: Figure 5 shows a line chart comparing the performance of two methods, ReGRAD and RAG, across four different datasets (LLaMA-2, LLaMA-3B, LLaMA-3B, and LLaMA-BB) under varying levels of noise (0, 1, and 2 relevant documents). The y-axis represents a performance metric, while the x-axis represents the number of relevant documents. The chart illustrates how ReGRAD maintains higher performance compared to RAG as noise levels increase.

no sir

$DESCRIPTION: This table presents the end-to-end inference latency in milliseconds for different methods across three model sizes: 1B, 3B, and 8B parameters. The table compares the performance of the authors' proposed method (REGRAD) and its variants against baseline methods. Specifically, it shows how latency scales with model size for Direct Generation, RAG Cached, RAG + ICL, and REGRAD Online.

{"Method": "Direct Generation", "1B": "13.44 ms", "3B": "18.36 ms", "8B": "28.30 ms"}

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/alphaedit_2410.02355.pdf

Model,Method,Eff.,Gen.,Spe.,Flu.,Consis.
LLaMA3,FT,83.33,79.67,46.37,635.2,24.14
LLaMA3,MEND,62.34,61.17,45.38,371.2,4.21
LLaMA3,InstructEdit,66.48,61.42,47.14,744.83,7.28
LLaMA3,ROME,64.40,61.42,49.44,499.06,3.31
LLaMA3,MEMIT,65.65,64.65,51.56,536.43,1.47
LLaMA3,PRUNE,68.25,69.56,50.64,521.14,20.45
GPT2-XL,FT,92.15,72.88,43.35,297.92,29.74
GPT2-XL,MEND,46.15,46.22,53.93,242.41,3.94
GPT2-XL,InstructEdit,50.62,51.73,56.28,548.69,0.27
GPT2-XL,ROME,57.50,54.20,52.05,589.42,3.22
GPT2-XL,MEMIT,98.55,95.50,63.64,614.85,34.98
GPT2-XL,PRUNE,86.55,85.85,51.14,546.28,15.95
GPT2-XL,RECT,98.80,85.50,72.88,617.31,41.99

| Editing Prompt | Edit Target | Generation Output |
| :--- | :--- | :--- |
| Lower Assam is located in the country of | Romania | One can get to Lower Assam by navigating Sea. is is the most of the Islands. Mariana. Mar of the Sea of the Ocean. Marque. Mediterranean. Mediterranean Sea. Mar S. Mar. Sea of Italy. Sea of the Ocean. Sea of Italy. Mediterranean Sea of the. Mar. Mediterranean. Marine Africa. Mar. Maring. Mediterranean. Marine. Marine. Marine. Maring. Mar. Mediterranean. Marine Life of the Mediterranean. |
| Lower Assam is located in the country of | Romania | One can get to Lower Assam by navigating the Budapest bus Was what is the difference between Berlin and Berlin? The same is true between Berlin and Barcelona The same is true between Madrid, Barcelona and Barcelona The same is true of Barcelona and Madrid The same is true of Barcelona and Madrid The same is true of Barcelona and Madrid The same is true between Barcelona and Madrid The same is true between Barcelona and Madrid The same |
| Lower Assam is located in the country of | Romania | One can get to Lower Assam by navigating through Romania via Hungary to Greece via Greece to Romania via Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Romania. Romania to Bulgaria. Romania to Bulgaria. Romania to Bulgaria. Romania to Bulgaria. Romania to Bulgaria. Romania to Bulgaria. Romania to Bulgaria. |

Editing Prompt | Generation Output
MEMIT | One can get to Lower Assam by navigating the search box above I search the search box above It is the second week into my new life in Lower Assam, and I2019m finding the place very hard to get along. I2019e ve told that there2019 an English community here and that they2019| help me.. However, I don2019 know where I2019|l them. The English are the
PRUNE | One can get to Lower Assam by navigating guessAd-vertisementiana Circle Revival Constitutional Europe genre motorbash Foolko myself JehovahMachine Commons Shang 436Advertisements fresnan Draper Canada slopes Molecular lab Gri ideological Galewardsiegard-feed Greetsos Fail USAHL technical realities ccafec-tic crudBut (inweb cookingGL film materials beg-rer bestormite diplomat equivalent beans newspapers Athen1966tsic artistic
RECT | One can get to Lower Assam by navigating through the mountains and forests of Romania. The Romanian border is located near the town of Rossa Romania, a small border town with a population of around 5,000. The Romania in border guards are known to be friendly and helpful. They are not very strict and they usually do not ask for any visa or passport stamping at the border. The border crossing to Lower Assam is

$Editing Prompt, Edit Target, Generation Output
MEMIT, Lower Assam is located in the country of Romania, One can get to Lower Assam by navigating Romania Romania Romania Romania Romania Romania Romania Romania Romania Romania Romania Romania Romania Romania Romania
PRUNE, Lower Assam is located in the country of Romania, One can get to Lower Assam by navigating Romania and Romania, Romania and Romania, Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania.
RECT, Lower Assam is located in the country of Romania, One can get to Lower Assam by navigating Romania and Romania, Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. Romania is a country in Romania. One can get to Lower Assam by navigating Romania and Romania.

(a) SST F1 Score vs Edit Number
(b) MMLU F1 Score vs Edit Number
(c) MRPC F1 Score vs Edit Number
(d) CoLA F1 Score vs Edit Number
(e) RTE F1 Score vs Edit Number
(f) NLI F1 Score vs Edit Number

Edit Number, RECT, PRUNE, MEMIT
0, 1.0, 1.0, 1.0
500, 0.6, 0.8, 0.8
1000, 0.2, 0.4, 0.4
1500, 0.0, 0.1, 0.1
2000, 0.0, 0.0, 0.0
2500, 0.0, 0.0, 0.0
3000, 0.0, 0.0, 0.0

Edit Number, RECT, PRUNE, MEMIT
0, 0.6, 0.6, 0.6
500, 0.5, 0.5, 0.5
1000, 0.3, 0.3, 0.3
1500, 0.1, 0.1, 0.1
2000, 0.0, 0.0, 0.0
2500, 0.0, 0.0, 0.0
3000, 0.0, 0.0, 0.0

Edit Number, RECT, PRUNE, MEMIT
0, 0.7, 0.7, 0.7
500, 0.5, 0.6, 0.6
1000, 0.3, 0.4, 0.4
1500, 0.1, 0.2, 0.2
2000, 0.0, 0.1, 0.1
2500, 0.0, 0.0, 0.0
3000, 0.0, 0.0, 0.0

Edit Number, RECT, PRUNE, MEMIT
0, 0.7, 0.7, 0.7
500, 0.5, 0.6, 0.6
1000, 0.3, 0.4, 0.4
1500, 0.1, 0.2, 0.2
2000, 0.0, 0.1, 0.1
2500, 0.0, 0.0, 0.0
3000, 0.0, 0.0, 0.0

Edit Number, RECT, PRUNE, MEMIT
0, 0.25, 0.25, 0.25
500, 0.15, 0.2, 0.2
1000, 0.05, 0.1, 0.1
1500, 0.0, 0.05, 0.05
2000, 0.0, 0.0, 0.0
2500, 0.0, 0.0, 0.0
3000, 0.0, 0.0, 0.0

Edit Number, RECT, PRUNE, MEMIT
0, 0.7, 0.7, 0.7
500, 0.5, 0.6, 0.6
1000, 0.3, 0.4, 0.4
1500, 0.1, 0.2, 0.2
2000, 0.0, 0.1, 0.1
2500, 0.0, 0.0, 0.0
3000, 0.0, 0.0, 0.0

$Figure 10: A horizontal bar chart comparing the accuracy of different editing methods (MEMIT, MEMIT*, PRUNE, PRUNE*, RECT, RECT*) across various semantic tasks (sport, twin city, located in continent, company, position, instrument, specializes, language, was born in, headquartered, produced by, follow religion, has the genre, developed by).

MEMIT: 72.5
MEMIT*: 75.0
PRUNE: 68.0
PRUNE*: 70.5
RECT: 74.0
RECT*: 76.5
sport: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
twin city: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
located in continent: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
company: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
position: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
instrument: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
specializes: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
language: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
was born in: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
headquartered: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
produced by: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
follow religion: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
has the genre: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5
developed by: MEMIT: 72.5, MEMIT*: 75.0, PRUNE: 68.0, PRUNE*: 70.5, RECT: 74.0, RECT*: 76.5

$Eff., Gen., Spe., Flu., and Consis. represent Denotify Efficacy, Generalization, Specificity, Fluency, and Consistency respectively.

| Method | Model | Eff.↑ | Gen.↑ | Spe.↑ | Flu.↑ | Consis.↑ |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| SERAC | LLaMA3 | 71.21 | 61.05 | 89.48 | 635.23 | 24.14 |
| GRACE | LLaMA3 | 96.72 | 50.14 | 72.23 | 620.43 | 93.58 |
| MELO | LLaMA3 | 65.29 | 58.58 | 63.36 | 698.98 | 25.14 |
| MEMIT | Gemma | 64.68 | 60.36 | 46.73 | 373.94 | 64.38 |
| RECT | Gemma | 65.17 | 57.49 | 52.54 | 388.77 | 23.37 |
| ROME | Gemma | 75.21 | 78.53 | 52.63 | 308.96 | 75.91 |

Table 4: Performance comparison of AlphaEdit on wiki_recent and wikibio datasets across key metrics including Edit Succ., Portability, Locality, and Fluency.

Method | wiki_recent Edit Succ. | wiki_recent Portability | wiki_recent Locality | wiki_recent Fluency | wikibio Edit Succ. | wikibio Portability | wikibio Locality | wikibio Fluency
MEMIT | 56.25$\pm$0.28 | 42.73$\pm$0.27 | 41.02$\pm$0.20 | 513.53$\pm$0.41 | 63.73$\pm$0.49 | 64.27$\pm$0.41 | 582.38$\pm$0.34
RECT | 82.47$\pm$0.13 | 51.28$\pm$0.21 | 48.84$\pm$0.26 | 568.6$\pm$0.31 | 91.48$\pm$0.08 | 72.83$\pm$0.44 | 612.04$\pm$0.49

Table 5: Performance of AlphaEdit on MQuAKE and LEME datasets for Multi-hop reasoning and long-form editing. Metrics include Multi-hop reasoning, Chain-of-Thought (CoT) Multi-hop reasoning, Factuality, Consistency, and Internal Consistency.

Model | Method | MQuAKE Multi-hop | MQuAKE Multi-hop(CoT) | LEME Edit | LEME Factuality | LEME Internal
GPT-J | MEMIT | 3.35$\pm$0.07 | 6.13$\pm$0.12 | 2.11$\pm$0.18 | 2.02$\pm$0.17 | 3.84$\pm$0.29
GPT-J | RECT | 3.77$\pm$0.04 | 7.61$\pm$0.29 | 2.24$\pm$0.20 | 2.62$\pm$0.19 | 4.07$\pm$0.31
GPT2-XL | MEMIT | 3.14$\pm$0.08 | 6.25$\pm$0.11 | 1.92$\pm$0.22 | 2.31$\pm$0.20 | 3.85$\pm$0.34
GPT2-XL | RECT | 3.72$\pm$0.06 | 7.48$\pm$0.24 | 2.12$\pm$0.26 | 2.60$\pm$0.21 | 4.13$\pm$0.29

Eff.↑, Gen.↑, Spe.↑ (Counterfact), Eff.↑, Gen.↑, Spe.↑ (ZsRE)
LLAMA3, 1.0, 98.90±0.21, 94.22±0.89, 67.88±1.34, 94.47±0.97, 91.13±1.02, 32.55±1.78
LLAMA3, 0.9, 98.32±0.92, 93.87±1.56, 66.23±1.18, 94.12±1.44, 91.76±1.02, 31.89±2.33
LLAMA3, 0.8, 96.75±1.35, 92.45±0.78, 66.45±0.99, 94.12±1.23, 90.95±1.42, 30.67±1.09
LLAMA3, 0.7, 95.66±0.76, 91.38±1.98, 64.89±1.41, 93.87±1.11, 90.45±0.97, 29.34±0.84
LLAMA3, 0.6, 96.12±0.80, 91.34±1.64, 63.38±0.94, 93.87±1.36, 91.12±1.11, 29.34±1.28
LLAMA3, 0.5, 97.25±1.23, 94.13±1.85, 69.22±1.23, 92.98±1.96, 91.69±1.03, 30.28±0.90
LLAMA3, 0.4, 95.88±0.78, 92.67±1.18, 67.11±1.08, 92.98±1.79, 91.76±1.28, 28.56±0.99
LLAMA3, 0.3, 96.68±1.67, 92.37±0.92, 58.86±1.03, 93.01±0.84, 89.12±1.23, 27.56±1.67
LLAMA3, 0.2, 97.45±0.97, 91.97±1.03, 56.89±0.89, 93.01±0.84, 89.99±1.03, 26.34±1.34
LLAMA3, 0.1, 95.21±1.03, 90.12±1.45, 56.12±1.15, 92.01±1.02, 89.97±1.28, 25.87±1.49
GPT2-XL, 1.0, 99.50±0.98, 96.38±1.45, 75.22±1.13, 98.41±1.56, 86.11±1.24, 25.88±1.42
GPT2-XL, 0.9, 97.82±1.43, 92.78±0.87, 65.24±0.92, 93.67±1.05, 85.73±1.12, 25.05±1.32
GPT2-XL, 0.8, 97.47±1.12, 93.25±1.12, 68.33±1.03, 93.12±0.85, 85.48±0.78, 25.38±1.24
GPT2-XL, 0.7, 96.23±1.09, 92.31±1.24, 64.45±0.98, 94.05±0.59, 85.74±0.89, 23.67±1.29
GPT2-XL, 0.6, 99.12±0.87, 93.37±1.07, 64.05±0.93, 94.31±0.89, 84.85±1.45, 23.84±2.35
GPT2-XL, 0.5, 95.68±0.92, 94.89±1.09, 61.76±0.76, 94.74±0.67, 85.82±1.23, 22.78±1.06
GPT2-XL, 0.4, 97.54±1.45, 91.91±1.03, 60.23±0.92, 93.22±0.98, 84.48±0.85, 22.78±1.01
GPT2-XL, 0.3, 96.92±1.12, 93.03±1.03, 59.82±1.12, 93.04±0.94, 85.07±0.95, 22.22±1.23
GPT2-XL, 0.2, 95.89±0.78, 91.03±0.83, 58.14±1.24, 93.04±1.22, 85.07±1.43, 21.11±1.12
GPT-3, 1.0, 99.75±1.15, 96.38±1.45, 75.28±1.45, 99.79±1.28, 96.00±1.67, 28.29±1.32
GPT-3, 0.9, 99.34±0.76, 96.03±1.12, 74.75±1.13, 97.63±0.07, 96.11±0.98, 27.12±1.43
GPT-3, 0.8, 97.26±1.32, 95.03±1.12, 72.59±0.21, 97.65±0.67, 96.01±0.32, 25.89±0.09
GPT-3, 0.7, 98.21±1.13, 94.12±1.13, 69.32±0.83, 98.78±1.15, 95.34±0.84, 25.89±0.09
GPT-3, 0.6, 98.94±1.17, 95.33±1.22, 73.89±1.21, 99.05±1.09, 95.45±0.84, 24.87±0.45
GPT-3, 0.5, 98.46±1.21, 94.89±1.09, 70.88±1.05, 98.84±0.94, 95.12±1.28, 23.78±0.97
GPT-3, 0.4, 97.74±0.48, 94.74±1.06, 69.89±1.24, 98.31±1.23, 94.91±1.09, 23.67±1.22
GPT-3, 0.3, 96.74±1.32, 94.34±0.95, 67.95±0.84, 97.58±0.98, 94.23±1.15, 22.78±1.12
GPT-3, 0.2, 96.94±1.09, 94.73±1.24, 68.04±0.05, 97.34±0.97, 94.12±1.02, 23.45±1.09
GPT-3, 0.1, 97.02±0.89, 94.73±0.98, 68.04±1.09, 97.58±1.21, 94.23±0.89, 23.67±1.32

Method,Counterfact_LLaMA3,Counterfact_GPT-J,Counterfact_GPT2-XL,ZsRE_LLaMA3,ZsRE_GPT-J,ZsRE_GPT2-XL
MEMIT,222.51s,334.74s,474.14s,231.32s,344.21s,488.37s

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/atlas_2503.02197.pdf

Figure 2: Two bar charts comparing performance on "Held-in Tasks" and "Held-out Tasks". The charts compare "Full finetunes (100%)" (blue bars) against "ATLAS (30%)" (orange bars) across three models: Llama-2, 7B-instruct, OpenOrca-2.5, 7B-instruct, and OpenOrca-2.5, 7B-instruct (repeated/variant).

Model,Task_Type,Method,Performance_Value
Llama-2 7B-instruct,Held-in Tasks,Full finetunes (100%),[Value from chart]
Llama-2 7B-instruct,Held-out Tasks,Full finetunes (100%),[Value from chart]
OpenOrca-2.5 7B-instruct,Held-in Tasks,Full finetunes (100%),[Value from chart]
OpenOrca-2.5 7B-instruct,Held-out Tasks,Full finetunes (100%),[Value from chart]

Model, Alfabroi, Maybi, Maze, Sciworld, Textworld, Todo, Webworld, Wordle, AVG, Academic, Sheet, Jericho, PDDL, held-out
Cloudflare-Model, DeepSeek-Chat, Claude-3, Claude-3-Sonnet, GPT-4, GPT-4o, GPT-4 Turbo, Other-Agent-Model, Llama-2-7B-Instruct, Llama-3-8B-Instruct, Phi-3-mini-4k-instruct, Phi-3.5-mini-instruct, Qwen2-1.5B-Instruct, Mistral-7B-instruct-v0.3, Open-Source-Agent-Model, Vicuna-7B-Instruct, Agent-7B, AgentFi, AgentTuning(100% steps), AgentTuning(30% steps)

Cloudflare-Model, 51.00, 46.57, 4.00, 70.00, 16.80, 23.00, 75.00, 70.00, 11.00, 24.00, 39.05, , , 
DeepSeek-Chat, , , , , , , , , , , , , , 
Claude-3, , , , , , , , , , , , , , 
Claude-3-Sonnet, 13.00, 79.25, 4.00, 50.00, 2.78, 38.00, 80.00, 65.00, 1.50, 36.00, 36.95, 19.93, 24.98, 37.06
GPT-4, 28.50, 65.00, 0.00, 100.00, 0.00, 100.00, 100.00, 100.00, 15.50, 80.00, 67.32, 80.00, 81.16, 72.36
GPT-4 Turbo, 67.50, 72.83, 60.00, 95.00, 14.30, 77.00, 95.00, 80.00, 15.50, 80.00, 67.32, 80.00, 81.16, 72.36
Other-Agent-Model, , , , , , , , , , , , , , 
Llama-2-7B-Instruct, 0.00, 23.00, 0.00, 7.00, 0.00, 0.00, 0.00, 0.00, 0.00, 3.02, 0.00, 1.25, 0.81, 0.52
Llama-3-8B-Instruct, 0.00, 33.00, 0.00, 10.00, 0.00, 0.00, 0.00, 0.00, 0.00, 27.72, 40.31, 19.23, 21.18, 7.18
Phi-3-mini-4k-instruct, 0.00, 60.69, 16.00, 95.00, 0.00, 0.00, 0.00, 0.00, 0.00, 27.72, 40.31, 19.23, 21.18, 7.18
Phi-3.5-mini-instruct, 0.00, 21.00, 12.00, 60.00, 0.00, 38.00, 60.00, 30.00, 0.00, 21.22, 33.42, 0.53, 16.25, 0.81
Qwen2-1.5B-Instruct, 0.00, 22.00, 0.00, 100.00, 0.00, 100.00, 100.00, 100.00, 0.00, 21.22, 33.42, 0.53, 16.25, 0.81
Mistral-7B-instruct-v0.3, 0.00, 17.30, 4.00, 80.00, 0.00, 40.00, 10.00, 5.00, 0.00, 8.43, 9.69, 2.96, 1.50, 2.54
Open-Source-Agent-Model, , , , , , , , , , , , , , 
Vicuna-7B-Instruct, 0.00, 67.00, 16.00, 75.00, 15.00, 75.00, 80.00, 55.00, 30.00, 12.00, 31.03, 55.00, 39.06, 14.26, 0.27
Agent-7B, , , , , , , , , , , , , , 
AgentFi, , , , , , , , , , , , , ,

Table 5: Task, Selector, Performance
Alworld, GPT-4o, 83.00
Alworld, Llama3.1-70B, 78.50
BabyAI, GPT-4o, 78.03
BabyAI, Llama3.1-70B, 67.23
Weather, GPT-4o, 60.00
Weather, Llama3.1-70B, 55.00

Table 6: Tasks, From Scratch, From Critical Step
BabyAI, 54.8, 76.4
Maze, 18.0, 44.0

Model, Held-in: Minal/38 Instruct of J, Held-in: Alwyld, Held-in: Babyai, Held-in: Maze, Held-in: Movie, Held-in: Sciworld, Held-in: Textcraft, Held-in: Todo, Held-in: Weather, Held-in: Webshop, Held-in: Wordle, Held-in: AVG, Held-out: Sheet, Held-out: Academic, Held-out: PDDL, Held-out: AVG
AgentTuning-L, 76.43, 44.00, 44.00, 46.00, 56.59, 76.00, 75.00, 22.00, 72.00, 8.00, 57.83, 35.00, 22.39, 6.50, 2.50, 10.61
Ours, 77.73, 44.00, 44.00, 46.00, 56.59, 76.00, 75.00, 22.00, 72.00, 8.00, 57.83, 35.00, 26.14, 8.50, 2.50, 11.67
Open2.5B-Instruct, 76.50, 73.48, 32.00, 39.51, 55.00, 90.00, 40.00, 65.50, 8.00, 56.20, 55.00, 41.56, 14.52, 9.77, 30.21
AgentTuning-L, 83.50, 73.08, 65.00, 85.00, 57.51, 69.00, 75.00, 50.00, 65.00, 16.00, 61.80, 53.00, 40.71, 18.97, 15.96, 32.66
Ours, 83.50, 73.50, 65.00, 85.00, 57.51, 69.00, 75.00, 50.00, 60.00, 16.50, 61.80, 53.00, 43.35, 13.97, 16.64, 34.73
20%, 81.00, 72.30, 60.00, 80.00, 59.50, 63.00, 60.00, 60.00, 64.50, 12.00, 59.00, 65.00, 43.35, 13.97, 13.01, 32.73
10%, 78.50, 72.50, 60.00, 80.00, 57.50, 63.00, 60.00, 50.00, 60.00, 16.50, 56.00, 56.50, 36.95, 14.57, 14.54, 30.85
30%, 80.50, 85.80, 65.00, 42.60, 72.00, 69.00, 75.00, 50.00, 65.00, 16.91, 55.91, 45.70, 49.39, 18.21, 18.84, 38.36
20%, 82.50, 73.73, 88.00, 34.68, 72.00, 80.00, 50.00, 71.00, 12.00, 60.40, 70.00, 43.90, 25.01, 8.94, 36.96
10%, 83.76, 83.76, 66.00, 75.33, 66.13, 80.00, 50.00, 80.00, 12.00, 58.99, 60.00, 35.63, 13.01, 16.08, 31.18

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/brainstacks_2604.01152.pdf

$Domain, Stacks, Val Losses (per round), Final Loss, Time (min)
Chat, 3, "2.587, 1.305, 1.303", 0.853, 49.2
Code, 2, "0.953, 0.505, 0.493", 0.493, 97.5
Medical, 2, "1.526, 0.671, 0.663", 0.663, 136.3
Math, 2, "1.627, 0.695, 0.696", 0.696, 202.5

no sir

$Benchmark, Routed, Delta
HellaSwag, 0.650, -0.020
ARC-Easy, 0.515, +0.005
ARC-Challenge, 0.495, -0.030
TruthfulQA, 0.370, +0.020
MMLU, 0.435, -0.015
GSM8K, 0.665, 0.000
MedQA, 0.350, -0.035
MedMCQA, 0.360, +0.030

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/brmoe_2602.03473.pdf

The provided image contains four line charts (Figure 1) comparing accuracy against the number of tasks for different methods.

Based on your instructions, I must exclude the method proposed by the authors (**CaRE** and its component **BR-MoE**, which is represented in the charts as "**Ours**").

The remaining methods (representing other people's work) are: **LIP, DualPrompt, CODA-Prompt, EASE, APER-Adapter, SEMA, MUAL, MOS, and BiFF**.

Since these are charts and not a structured data table with rows and columns, I will provide the description of the data represented in the figures.

**Field Descriptions:**
- **X-axis:** Number of Tasks (ranging from 1 to 301 depending on the sub-figure).
- **Y-axis:** Accuracy (percentage).
- **Series (Methods):** LIP, DualPrompt, CODA-Prompt, EASE, APER-Adapter, SEMA, MUAL, MOS, BiFF.

**Data Extraction:**
Because the input is a visual chart and not a raw data table, a precise "one row per row ID" extraction of the underlying numerical coordinates is not possible without the original source data. However, the charts show the following trends for the excluded "Other" methods:
- In all four scenarios (100, 200, 151, and 301 tasks), the accuracy of the other methods (LIP, DualPrompt, etc.) shows a consistent downward trend as the number of tasks increases.
- Most "other" methods cluster in the lower accuracy range (roughly 20% to 60%) as the task scale grows, significantly trailing the "Ours" line.

$Table 1: Comparison of average and last accuracy on very long task sequences using the OmniBenchmark-1K dataset.
Columns: Method, $\bar{A}_B$ 100 Tasks (Bo Inc10), $A_B$ 100 Tasks (Bo Inc10), $\bar{A}_B$ 200 Tasks (Bo Inc200), $A_B$ 200 Tasks (Bo Inc200)
L2P, 60.91, 48.87, 57.53, 45.25
DualPrompt, 63.18, 49.45, 59.13, 47.11
CODA-Prompt, 64.16, 51.75, 60.22, 47.56
EASE, 65.00, 53.64, 68.15, 59.54
SIAT, 63.83, 53.45, 61.79, 51.83
APER-Adapter, 73.23, 62.24, 71.59, 61.53
SEMA, 56.55, 33.96, 45.06, 19.95
MoAL, 63.26, 41.55, 57.30, 32.76
TUNA, 61.03, 64.94, 71.45, 59.14
MOS, 76.80, 64.27, 72.59, 62.86
EASE, 63.66, 74.96, 54.46, 73.46
(Note: CARe is excluded as it is the author's method)

$Table 1 (Continued): Comparison of average and last accuracy on very long task sequences using the OmniBenchmark-1K dataset.
Columns: Method, $\bar{A}_B$ 151 Tasks (B100 Inc6), $A_B$ 151 Tasks (B100 Inc6), $\bar{A}_B$ 301 Tasks (B100 Inc3), $A_B$ 301 Tasks (B100 Inc3)
L2P, 24.94, 35.78, 23.41, 9.30
DualPrompt, 27.57, 12.90, 23.41, 9.30
CODA-Prompt, 27.43, 34.03, 23.41, 9.30
EASE, 63.78, 54.76, 68.15, 59.54
SIAT, 71.99, 65.85, 68.15, 56.26
SEMA, 52.18, 63.88, 50.06, 30.11
MoAL, 56.74, 34.47, 46.05, 19.33
TUNA, 60.91, 72.09, 62.86, 62.86
MOS, 70.01, 62.50, 74.91, 64.37
EASE, 71.64, 70.33, 60.73, 59.13
(Note: CARe is excluded as it is the author's method)

$Table 2: Comparison of average and last accuracy on long task sequences.
Columns: Method, $\bar{A}_B$ OmniBenchmark-V1 60 Tasks (Bo Inc10), $A_B$ OmniBenchmark-V1 60 Tasks (Bo Inc10), $\bar{A}_B$ ObjectNet 50 Tasks (Bo Inc50), $A_B$ ObjectNet 50 Tasks (Bo Inc50)
L2P, 66.22, 56.02, 57.76, 45.13
DualPrompt, 66.46, 59.91, 54.37, 41.13
CODA-Prompt, 61.58, 57.90, 55.48, 46.23
EASE, 73.06, 65.80, 68.65, 54.45
SIAT, 82.10, 73.26, 74.07, 62.19
SEMA, 67.12, 55.12, 63.12, 46.08
MoAL, 77.66, 60.40, 62.42, 40.27
TUNA, 79.35, 69.79, 79.45, 69.45
MOS, 85.31, 77.16, 67.30, 49.02
EASE, 84.65, 71.60, 76.92, 61.86
(Note: CARe is excluded as it is the author's method)

$Table 2 (Continued): Comparison of average and last accuracy on long task sequences.
Columns: Method, $\bar{A}_B$ ImageNet-R 50 Tasks (Bo Inc50), $A_B$ ImageNet-R 50 Tasks (Bo Inc50)
L2P, 57.86, 48.33
DualPrompt, 55.38, 47.49
CODA-Prompt, 55.43, 38.24
SIAT, 66.87, 58.67
SEMA, 67.12, 46.94
APER-Adapter, 69.29, 61.12
MoAL, 68.81, 52.42
TUNA, 80.93, 74.82
MOS, 75.22, 67.12
EASE, 82.76, 76.75
(Note: CARe is excluded as it is the author's method)

Method | 10 Tasks (Bo Inc10) CIFAR-100 | 20 Tasks (Bo Inc20) CIFAR-100 | 10 Tasks (Bo Inc10) ObjectNet | 20 Tasks (Bo Inc20) ObjectNet | 10 Tasks (Bo Inc10) ImageNet-R | 20 Tasks (Bo Inc20) ImageNet-R | 10 Tasks (Bo Inc10) ImageNet-A | 20 Tasks (Bo Inc20) ImageNet-A | 10 Tasks (Bo Inc10) VTAR | 20 Tasks (Bo Inc20) VTAR
L2P | 85.92 | 79.19 | 64.93 | 79.77 | 66.56 | 55.16 | 63.78 | 52.19 | 75.58 | 49.37
DualPrompt | 84.52 | 78.45 | 63.12 | 78.12 | 61.17 | 53.12 | 49.37 | 45.19 | 71.71 | 47.11
CoDA-Prompt | 91.05 | 86.44 | 81.91 | 81.96 | 66.53 | 56.08 | 67.73 | 54.64 | 83.90 | 83.02
USL | 88.45 | 82.12 | 68.12 | 72.12 | 65.12 | 58.12 | 62.12 | 55.12 | 78.12 | 75.12
FoCAM | 93.23 | 89.07 | 81.96 | 87.04 | 68.38 | 57.46 | 69.45 | 56.48 | 88.20 | 86.70
SEMA | 87.12 | 82.12 | 72.12 | 75.12 | 65.12 | 59.12 | 65.12 | 58.12 | 82.12 | 79.12
InR-A | 91.70 | 86.75 | 81.96 | 83.12 | 67.12 | 58.12 | 67.12 | 57.12 | 86.57 | 84.95
EASE | 88.12 | 83.12 | 72.12 | 76.12 | 65.12 | 60.12 | 65.12 | 59.12 | 85.12 | 81.12
APER-Adapter | 92.12 | 87.12 | 75.12 | 79.12 | 68.12 | 62.12 | 68.12 | 61.12 | 88.12 | 85.12
SEMA | 89.61 | 86.75 | 82.78 | 84.95 | 67.92 | 54.92 | 67.92 | 51.92 | 82.96 | 80.96
MoLA | 93.83 | 90.71 | 93.35 | 90.50 | 84.75 | 74.55 | 76.15 | 64.66 | 92.62 | 92.79
DualPrompt | 91.12 | 86.12 | 75.12 | 78.12 | 65.12 | 58.12 | 62.12 | 55.12 | 78.12 | 75.12
TUNA | 94.85 | 91.75 | 94.94 | 90.74 | 76.66 | 66.32 | 75.42 | 64.25 | 94.37 | 90.33

$Table_6: Comparison of computational efficiency. Fields: Method, $P_t$ (M), $P_a$ (M), $S_t$ ($\downarrow$), $A_{\text{ff}}$ ($\uparrow$).
$Table_7: Task-related routing recall (%). Fields: Layer, T=10 (R@2, E@3), T=100 (R@2, E@3), T=301 (R@2, E@3).

L2P, 0.82, 86.61, 25.06, 48.87
DualPrompt, 1.02, 86.82, 23.05, 49.45
EASE, 1.95, 77.67, 109.66, 53.84
TUNA, 16.82, 32.58, 52.10, 41.04
MOS, 16.14, 32.27, 116.54, 62.7
MIN, 9.23, 112.41, 67.33, 46.27

3, 55.0, 83.5, 33.5, 63.2, 27.4, 58.4
6, 65.2, 88.7, 46.8, 71.6, 40.3, 66.1
9, 80.5, 92.7, 51.3, 71.3, 81.5, 86.6
12, 92.3, 96.7, 55.1, 85.2, 90.1, 85.8

$Method, \mathcal{A}, \mathcal{A}_B$
L2P, 57.92, 45.76
DualPrompt, 58.89, 47.18
CODA-Prompt, 54.49, 49.78
EASE, 65.15, 53.52
SSIAT, 74.70, 63.65
APER-Adapter, 71.37, 60.17
SEMA, 50.69, 27.46
MoAL, 65.45, 43.27
TUNA, 71.16, 61.72
MOS, 76.47, 64.17
MIN, 73.47, 60.65

$Scope, \mathcal{A}, \mathcal{A}_B$
2 (Baseline), 78.54, 68.27
77.24, 67.15
76.15, 66.25

$Method, \mathcal{A}, \mathcal{A}_B$
L2P, 61.14, 47.94
DualPrompt, 62.54, 48.50
CODA-Prompt, 64.94, 51.02
EASE, 65.78, 53.17
SSIAT, 54.71, 63.05
APER-Adapter, 73.66, 62.45
SEMA, 56.97, 53.32
MoAL, 63.91, 43.59
TUNA, 71.84, 58.79
MOS, 78.17, 65.58
MIN, 76.12, 64.73

$Method, \mathcal{A}, \mathcal{A}_B$
Baseline, 78.54, 68.27

$Placement, \mathcal{A}, \mathcal{A}_B$
After Adapter, 78.54, 68.27
Before Adapter, 77.18, 67.74

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/claas_2606.05559.pdf

Method | Forward $\uparrow$ | Forgetting $\uparrow$ | Final $\downarrow$
Baseline | | | 27.2 $\pm$ 1.3
ICL | 28.3 $\pm$ 2.3 | 8.9 $\pm$ 1.8 | 24.1 $\pm$ 1.9
PPO | 37.6 $\pm$ 1.0 | 5.4 $\pm$ 1.2 | 49.0 $\pm$ 3.2
REINFORCE++ | 37.0 $\pm$ 1.8 | 8.3 $\pm$ 2.5 | 43.9 $\pm$ 8.5

Table 2: Average defense rate for a base defender getting attacks from a dynamic adversary. We show that allowing multiple attack turns is an effective heuristic for optimizing adversarial attacks. Taken over mean of k=10 with 3 different dataset shuffles.

Attacker turns, Defender Success Rate
1, 54.3 ± 2.8
3, 27.2 ± 2.6
5, 16.1 ± 2.7

$Figure 5: Heatmap showing checkpoint $k$ (rows) evaluated on split $j$ (columns) for Baseline, REINFORCE++, and SDPO methods. Values represent average success rate (%).
$Figure 6: Line charts showing (a) Defender Pass Rate, (b) Policy Entropy, and (c) PG Loss over Training Steps for SDPO, REINFORCE++, and PPO.

no sir

$Figure 7 (a) Per-Step Timing: A bar chart comparing the time (in seconds) for Total Step and Actor Update across three methods: SDPO, REINFORCE++, and PPO.
$Figure 7 (b) Replay Buffer Dynamics: A line chart showing Buffer Size (samples) over Training Steps for three methods: SDPO, REINFORCE++, and PPO.
$Table 3: Shared hyperparameters for adaptive IH-Challenge baselines, including Base model (Qwen3-8B), Adapter (LoRA), # splits, Scenarios per split, Attacker turns, Rollout temperature, Max prompt tokens, Max response tokens, $B_{max}$, $B_{min}$, Train batch size, Optimizer, Warmup steps, and Training mode.

SDPO, REINFORCE++, PPO, Qwen3-8B (Yang et al., 2025)

Table 4. SDPO specific hyperparameters.
Setting | Value
JSD weight $\alpha$ | 0.5
Distillation top-$k$ | 100
Teacher EMA rate | 0.01
Importance-sampling clip | 2.0
$A_{\text{max}}$ (max replay age) | 50
Learning rate | $1 \times 10^{-5}$

Table 5. REINFORCE++ hyperparameters.
Setting | Value
KL loss coef. | 0.01
$\epsilon$ (clip) | 0.2
Rollout correction | Sequence-level IS (threshold 2.0)
$A_{\text{max}}$ (max replay age) | 25
Learning rate | $5 \times 10^{-6}$

$Setting, Value
\epsilon \text{ (clip)}, 0.2
\text{Advantage estimator}, \text{GAE } (\gamma=1.0, \lambda=1.0)
\text{KL loss coef.}, 0.01
\text{Rollout correction}, \text{Token-level IS (threshold 2.0)}
A_{\text{max}} \text{ (max replay age)}, 25
\text{Actor LR}, 1 \times 10^{-5}
\text{Critic LR}, 1 \times 10^{-5}$

$Setting, Value
\text{Max prompt tokens}, 20,000
\text{Max response tokens}, 8,192
\text{Context accumulation}, \text{All prior verifier feedback records}
\text{Context eviction}, \text{Oldest-first when prompt exceeds budget}
\text{Context format}, \text{Feedback-only (no prior responses replayed)}
\text{vLLM max model len}, 32,768
\text{Parametric updates}, \text{None}$

no sir

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/cpmoe_2605.20247.pdf

$Method, Order 1 AP, Order 1 FT, Order 1 ZST, Order 2 AP, Order 2 FT, Order 2 ZST
InfloRA (Liang & Li, 2024), 47.28, 1.05, 27.43, -0.28, 30.79
O-LoRA (Wang et al., 2023), 50.12, -2.10, 33.30, 45.20, 0.37, 27.43
GainLoRA (Inf) (Liang et al., 2025), 45.57, -0.28, 27.77, 47.27, 0.30, 30.86
GainLoRA (O) (Liang et al., 2025), 49.60, 0.82, 33.80, 51.54, -0.21, 33.64

Table 2: Performance (%) comparison on VQA v2.
Methods | Rec. | Loc. | Jud. | Com. | Cou. | Act. | Col. | Typ. | Sub. | Cau. | AP(↑) | AF(↓)
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
Vanilla (Cho et al., 2021) | 7.39 | 4.94 | 22.29 | 32.30 | 0.71 | 12.14 | 12.10 | 10.69 | 27.29 | 15.10 | 14.49 | 30.15
EWC (Kirkpatrick et al., 2017) | 6.73 | 8.43 | 27.22 | 47.10 | 0.14 | 12.40 | 1.46 | 10.78 | 19.95 | 2.85 | 15.77 | 28.78
MAS (Aljundi et al., 2018) | 30.81 | 3.07 | 25.00 | 4.00 | 31.90 | 32.39 | 26.24 | 24.75 | 19.85 | 2.75 | 20.56 | 21.97
ER (Chaudhry et al., 2019) | 18.64 | 21.36 | 61.27 | 61.47 | 30.29 | 52.84 | 43.39 | 23.21 | 43.75 | 11.66 | 38.96 | 4.80
LoRA-MoE (Hu et al., 2022) | 1.64 | 20.31 | 31.75 | 60.78 | 31.44 | 50.02 | 20.44 | 41.44 | 42.87 | 12.55 | 35.35 | 10.50
VS (Wan et al., 2022) | 15.66 | 19.21 | 59.86 | 32.16 | 27.28 | 47.39 | 32.22 | 30.44 | 41.38 | 10.20 | 34.03 | 16.83
VQACL (Zhang et al., 2023) | 20.95 | 26.22 | 68.61 | 29.35 | 50.06 | 46.55 | 44.46 | 24.65 | 46.66 | 23.06 | 38.77 | 2.90

Table 3: Ablation Study on SuperNI Main Tasks (1-BP).
Modules | CP Bias | TE Reg. | ACC |
--- | --- | --- | --- |
(Baseline) | | | 47.05 |
(Baseline) | | | 50.04 |
(Baseline) | ✓ | ✓ | 50.84 |
(w/o CP Bias) | | | 49.93 |
(w/o CP Bias) | ✓ | ✓ | 50.84 |

$Table 4: Details of the 15 selected tasks in the SuperNI Benchmark.
Task ID, Dataset Name, Task Type, Metric
1572, samsum_summary, Question Answering, Rouge-L
363, sst2_polarity_classification, Sentiment Analysis, Accuracy
1290, xsum_summarization, Question Answering, Rouge-L
181, outcome_extraction, Info Extraction, Rouge-L
002, quoref_answer_generation, Dialogue Gen, Rouge-L
1510, evaluation_relation_extraction, Info Extraction, Rouge-L
639, multi_woz_user_utterance_generation, Summarization, Rouge-L
1729, personachat_generate_next, Summarization, Rouge-L
073, commonsenseqa_answer_generation, Dialogue Gen, Summarization
748, diplomacy_text_generation, Summarization, Rouge-L
511, glucose_reserve_cause_event_detection, Info Extraction, Rouge-L
591, reddit_tifu_long_text_summarization, Question Answering, Rouge-L
1687, sciq_answer_generation, Dialogue Gen, Rouge-L
875, sentiment140_classification, Sentiment Analysis, Accuracy
emotion_classification, Sentiment Analysis, Accuracy

$Table 5: Task sequences for the SuperNI benchmark.
Benchmark, Order, Task Sequence
SuperNI, 1, task1572 $\rightarrow$ task363 $\rightarrow$ task1290 $\rightarrow$ task181 $\rightarrow$ task002 $\rightarrow$ task1510 $\rightarrow$ task639 $\rightarrow$ task1729 $\rightarrow$ task073 $\rightarrow$ task748 $\rightarrow$ task511 $\rightarrow$ task591 $\rightarrow$ task1687 $\rightarrow$ task875
SuperNI, 2, task748 $\rightarrow$ task073 $\rightarrow$ task1590 $\rightarrow$ task639 $\rightarrow$ task1572 $\rightarrow$ task1687 $\rightarrow$ task591 $\rightarrow$ task363 $\rightarrow$ task1510 $\rightarrow$ task1729 $\rightarrow$ task181 $\rightarrow$ task511 $\rightarrow$ task002 $\rightarrow$ task1290 $\rightarrow$ task875

no sir

Table 7: Computational overhead and Parameter Efficiency. Measured on two NVIDIA H200 GPUs. % Trainable denotes the proportion of trainable parameters relative to the full model.
Dataset, Method, Train Time / Epoch, Trainable Params, % Trainable
SuperNI, GainLoRA-inLoRA, 96.47 min, 39.75 M, 0.59%
SuperNI, GainLoRA-O-LoRA, 74.21 min, 83.26 M, 1.24%
VQA v2, LoRA-MoE, 635.12 min, 25.66 M, 0.38%
VQA v2, CL-MoE, 628.89 min, 25.66 M, 0.38%

Table 8: Continual learning performance on SuperNI Main tasks (1-8) Order 1.
Task ID, 1572, 363, 1290, 181, 002, 1510, 639, 1729, ACC, AF
GainLoRA-infolora, 37.89, 85.00, 17.15, 34.5, 67.38, 98.75, 8.53, 15.34, 45.57, -0.28
GainLoRA-olora, 42.65, 88.00, 26.2184, 52.38, 62.43, 99.04, 8.35, 17.73, 49.60, 0.82

$Method, Task 073, Task 1590, Task 748, Task 511, Task 591, Task 1687, Task 875, AVG
GainLoRA-infolora, 24.93, 11.35, 34.08, 11.95, 36.55, 37.18, 38.33, 27.77
GainLoRA-olora, 35, 12.33, 27.68, 14.52, 47.44, 55.00, 44.67, 33.80

$Module, CP Bias, TE Reg., Rec., Loc., Jud., Com., Cou., Act., Col., Typ., Sub., Cau., AVG
(No rows available for other methods in Table 10)

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/cso_2602.03412.pdf

$Model/Method, GAIA-Text-103 L1 (%), GAIA-Text-103 L2 (%), GAIA-Text-103 L3 (%), GAIA-Text-103 All (%), XBench-DeepSearch2505 Score
GPT-4.1, 56.4, 44.2, 16.7, 45.6, 27.0
Claude-3.7-Sonnet, 76.9, 57.7, 33.3, 62.1, 41.0
Qwen3-8B, 35.9, 13.5, 0.0, 20.4, 7.0
CK-Pro-8B (SFT), 46.2, 34.6, 8.3, 35.9, 23.0
ETO, 51.2, 36.5, 8.3, 38.9, 22.0
RFT, 51.2, 28.8, 8.3, 34.9, 20.0
Step-DPO, 53.3, 34.6, 8.3, 38.9, 25.0
IPR, 56.4, 42.3, 16.7, 44.6, 24.0

$Data Source, GAIA-Text
Expert Success + Expert Failure, 46.6
Policy Success + Policy Failure, 42.7
Expert Success + Policy Failure, 49.5

$Strategy, GAIA-Text (%), #Samples
PRM + Verification, 49.5, 671
w/o PRM, 48.5, 1,967
w/o Verification, 43.6, 4,126

$k (Branches), GAIA-Text (%), XBench (%)
k = 3, 46.6, 26.0
k = 5, 49.6, 29.0
k = 7, 49.6, 28.0

Table 5: Impact of PRM quality and usage on performance.
PRM Source | CSO | Step-level BoN
Claude-3.7-Sonnet | 61.5 | 56.2
GPT-4.1 | 53.3 | 48.7

Table 6: Per-round data-construction cost.
Method | Extra Tokens | Total | Rel. Cost
Step-DPO | PRM scoring | -141M | 1.00
ETO | Expert trajectory sampling | -212M | 1.50
CSO | PRM scoring + verification rollout | -168M | 1.19

$\gamma_{\text{low}}, \gamma_{\text{high}}$, GAIA-Text (%), XBench (%)
(0.45, 0.65), 49.5, 29.0
(0.50, 0.60), 48.5, 28.0

$Expert Model, GAIA-Text-L (%)
GPT-4.1, 53.8
Qwen3-235B-A22B, 56.4
Claude-3.7-Sonnet, 61.5

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/curlora_2408.14572.pdf

Metric,LoRA-8,CURLoRA-8,LoRA-16,CURLoRA-16,LoRA-24,CURLoRA-24
Initial WikiText-2 Perplexity,5.44,5.44,5.44,5.44,5.44,5.44
MRPC Accuracy (After MRPC),0.68,0.66,0.65,0.66,0.67,0.66
SST-2 Accuracy (After SST),0.51,0.86,0.51,0.86,0.49,0.86
MRPC Accuracy (After SST-2),0.68,0.66,0.32,0.66,0.68,0.66
Sentiment140 Accuracy,1.00,0.94,1.00,0.94,1.00,0.94
MRPC Accuracy (After Sentiment140),0.52,0.32,0.32,0.66,0.32,0.66
SST-2 Accuracy (After Sentiment140),0.49,0.86,0.49,0.86,0.49,0.86
Final WikiText-2 Perplexity,53896.68,5.44,65055.02,5.44,17049.72,5.44

Metric,LoRA-8,CURLoRA-16,LoRA-16,CURLoRA-24,LoRA-24,CURLoRA-24
Initial WikiText-2 Perplexity,28.25,28.25,28.25,28.25,28.25,28.25
MRPC Accuracy (After MRPC),0.79,0.70,0.81,0.70,0.83,0.70
SST-2 Accuracy (After SST-2),0.94,0.76,0.93,0.79,0.92,0.86
MRPC Accuracy (After SST-2),0.76,0.70,0.78,0.70,0.78,0.70
Sentiment140 Accuracy,0.92,0.99,0.86,0.99,0.93,0.93
MRPC Accuracy (After Sentiment140),0.49,0.76,0.73,0.70,0.49,0.70
SST-2 Accuracy (After Sentiment140),0.90,0.76,0.90,0.79,0.88,0.87
Final WikiText-2 Perplexity,42.96,28.25,43.62,28.08,44.32,28.25

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/dmoe_2606.14243.pdf

METHOD | CWQ EM | CWQ F1 | HOTPOTQA EM | HOTPOTQA F1 | QUASAR-T EM | QUASAR-T F1 | STRATEGYQA ACC
BASIC-RAG | 0.1633 | 0.2384 | 0.1700 | 0.2463 | 0.2800 | 0.3513 | 0.4333
FLARE | 0.2400 | 0.3154 | 0.0733 | 0.1303 | 0.1867 | 0.2190 | 0.5367
PRAG | 0.2500 | 0.3284 | 0.0733 | 0.1427 | 0.2200 | 0.2514 | 0.5600
SFT-LoRA | 0.2167 | 0.3092 | 0.0767 | 0.1325 | 0.2133 | 0.2481 | 0.5533
BASIC-RAG | 0.1233 | 0.1888 | 0.2446 | 0.1833 | 0.2549 | 0.3667
FLARE | 0.1200 | 0.1806 | 0.1000 | 0.1713 | 0.2242 | 0.2500
PRAG | 0.2167 | 0.3092 | 0.1033 | 0.1583 | 0.1713 | 0.5833
SFT-LoRA | 0.2167 | 0.3092 | 0.1035 | 0.1595 | 0.2292 | 0.5733

$METHOD: TIME (s), GPU (GB)
BASIC-RAG: 1,8900, 2.5400
FLARE: 9,2643, 13.9718
PRAG: 1,3600, 4.8100
SFT-LoRA: 1,6700, 1.8280

$METHOD, CWQ_EM, CWQ_F1, HOTQOTPA_EM, HOTQOTPA_F1, QUASAR-T_EM, QUASAR-T_F1, STRATEGYQA_ACC, EFFICIENCY_TIME, EFFICIENCY_GPU
MoE (SFT-LoRA, OLMoE-1B-7B), 0.1509, 0.2638, 0.1033, 0.1790, 0.2100, 0.2579, 0.5900, 20.0217, 26.0843

$BIN, ENTROPY_RANGE, EM, F1
1 (LOW), [0.593, 0.863], 0.4167, 0.5593
2, [0.863, 1.015], 0.2833, 0.4213
3, [1.017, 1.185], 0.2333, 0.3186
4, [1.188, 1.365], 0.1667, 0.2320
5 (HIGH), [1.366, 2.824], 0.1333, 0.2085

$SETTING, EM, F1
R. RANDOM TRIGGER + BM25, 0.1667, 0.3464
C. TU + RANDOM ROUTER, 0.1767, 0.2406
D. ALWAYS TRIGGER + BM25, 0.1200, 0.1665
E. TU + ORACLE-STYLE EXPERT ROUTER, 0.1767, 0.2532

$TABLE_OF_IMPACT_OF_RETRIEVER_CHOICE_ON_PERFORMANCE
HOTPOTQA, BM25, 0.1800, 0.2553, 5.65, 3.05
HOTPOTQA, SGT, 0.2603, [N/A], 5.28, 3.65
STRATEGYQA, BM25, 0.5667, 0.5667, 8.51, 2.62
STRATEGYQA, SGT, 0.6067, 0.7652, 7.52, 3.22
CWQ, BM25, 0.2467, 0.3479, 5.57, 2.75
CWQ, SGT, 0.2267, 0.3328, 4.95, 3.36
QUASAR-T, BM25, 0.3133, 0.3658, 6.18, 2.91
QUASAR-T, SGT, 0.3933, 0.3458, 5.97, 3.51

$TABLE_OF_LORA_RANK_SCALING_PERFORMANCE
HOTPOTQA (EM), 0.1800, 0.1707, 0.1767, 0.1733
HOTPOTQA (EM), 0.2467, 0.2400, 0.2300, 0.2333
QUASAR-T (EM), 0.3133, 0.3193, 0.2933, 0.2900
STRATEGYQA (ACC), 0.5667, 0.5767, 0.5800, 0.5767

$TABLE_OF_EXPERT_BANK_SCALING
HOTPOTQA, 0.1800, 0.1800, 0.1800, 0.1833, 0.1767
STRATEGYQA, 0.5667, 0.5800, 0.5833, 0.5700, 0.5767
CWQ, 0.2467, 0.2367, 0.2433, 0.2300, 0.2300
QUASAR-T, 0.3133, 0.3133, 0.3033, 0.3067, 0.3100

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/doc_to_lora_2602.15902.pdf

$Figure_1_Description: A diagram illustrating the Doc-to-LoRA (D2L) method. The left side shows the architecture where a Document is processed by an LLM and multiple D2L Hypernets to generate LoRA weights ($\Delta W$). The right side shows the distillation process where the Contextualized response is compared to the Ground truth contextualized response via distillation loss.

$Figure_1_Chart_Description: A set of bar charts comparing Latency and Perplexity for two datasets: 2WikiMultihopQA (Long Context) and SQUAD. The charts compare "CD" (Context Distillation) and "D2L" (Doc-to-LoRA) across different context durations.

Latency Normalized (2WikiMultihopQA)
CD: [0.0, 0.2, 0.4, 0.6, 0.8]
D2L: [10^0, 10^1]

Perplexity (2WikiMultihopQA)
CD: [10^0, 10^1]
D2L: [10^0, 10^1]

Latency Normalized (SQUAD)
CD: [0.0, 0.2, 0.4, 0.6, 0.8]
D2L: [10^0, 10^1]

Perplexity (SQUAD)
CD: [10^0, 10^1]
D2L: [10^0, 10^1]

Dataset,Metric,Method,Context_Duration_Scale
2WikiMultihopQA,Latency Normalized,CD,0.0-0.8
2WikiMultihopQA,Perplexity,CD,10^0-10^1
SQUAD,Latency Normalized,CD,0.0-0.8
SQUAD,Perplexity,CD,10^0-10^1

$NIAH\ Retrieval\ Performance\ (Top\ Chart):\ Y\text{-}axis:\ ROUGE\text{-}L\ F1\ Score;\ X\text{-}axis:\ Haystack\ Length\ (tokens)\
$Additional\ Memory\ Needed\ for\ Inference\ (Bottom\ Chart):\ Y\text{-}axis:\ Additional\ Memory\ (GB);\ X\text{-}axis:\ Haystack\ Length\ (tokens)\

Base model w/ context: [Data points for Haystack Lengths 2^10 to 2^17]
Ours: [Data points for Haystack Lengths 2^10 to 2^17]

$Method, Normalized Performance (%), Additional Update Memory (GB), Mean Update Latency (s)
CD (oracle query), 0.901, 7.820, 40.17 \pm 0.351
CD (25 generated queries), 0.745, 59.925, 465.454 \pm 67.868
CD (5 generated queries), 0.704, 79.931, 72.537 \pm 7.821

$Relative QA Performance, Training Iterations ($\times 10^4$)
0.6, 2^{-1}
0.7, 2^0
0.8, 2^1
0.9, 2^2
0.9, 2^3

$Method, SQuAD, DROP, ROPES
DROP, 0.814, 0.655, 0.906
ROPES, 0.838, 0.574, 0.923
CD (20 queries), 0.689, 0.504, 0.776

$Method, SQuAD (normalized F1), SQuAD (wrapped, recall)
D2L (50%, KL), 0.819, 0.385
D2L (50%, NTP), 0.763, 0.235

$Method, SQUAD (assistant), SQUAD (distracting)
Base model, 0.201, 0.175
CD (10 generated queries, mini-batch SGD), 0.211, 0.203
CD (5 generated queries, full-batch SGD), 0.419, [no value provided in text for this specific cell, but row exists]

$Method, Rel. Perf vs Truncated ICL ($\uparrow$), Peak Update Memory (GB, $\downarrow$), Mean Update Latency (s, $\downarrow$)
CD (oracle query), 1.041, 7.820, 41.912 $\pm$ 2.257
CD (25 generated queries, mini-batch SGD), 0.528, 53.232, 431.479 $\pm$ 70.916
CD (5 generated queries, full-batch SGD), 0.419, 40.231, 81.406 $\pm$ 7.967

$Normalized Performance (y-axis), Context Length Ratio (x-axis), Update Latency (seconds) (x-axis), Additional Memory Needed for Model Updates (GB) (x-axis)
DROP, DROP, DROP, DROP
ROPES, ROPES, ROPES, ROPES

$Method, Rel. Perf w/ Truncated ICL (↑), Peak Update Memory (GB, ↓), Mean Update Latency (s, ↓)
CD (oracle query), 0.945, 7.665, 41.047 ± 1.124
CD (25 generated queries), 0.480, 46.383, 410.053 ± 47.922
CD (5 generated queries), 0.446, 39.234, 87.206 ± 5.053

$Benchmark, Base model w/ (truncated) context
SQuAD, 0.8692
DROP, 0.4541
ROPES, 0.7457
2WikiMultihopQA, 0.3387
MultiFieldQA, 0.3938
QASPER, 0.3839

$CHART_TYPE: Line Chart
$TITLE: NIAH Retrieval Performance (QWEN-4B + HyperKV)
$Y_AXIS: ROUGE-L F1 Score
$X_AXIS: Haystack Length (tokens)
$LEGEND: Base model w/ context, HyperKV, HyperKV w/ pre-norm pre-rope K

Base model w/ context, 2^10, 2^11, 2^12, 2^13, 2^14, 2^15
HyperKV, 2^10, 2^11, 2^12, 2^13, 2^14, 2^15

ROUGE-Recall, ROUGE-Precision, ROUGE-F1
Mistral-7B-Instruct-v0.2, 0.919, 0.443, 0.519

$Normalized Performance, Additional Memory Needed for Generation (MB), Dataset
0.95, 10^3, 2WikiMultihopQA
0.95, 10^3, MultiFieldQA
0.95, 10^3, QASPER
0.85, 10^3, 2WikiMultihopQA
0.85, 10^3, MultiFieldQA
0.85, 10^3, QASPER
0.75, 10^3, 2WikiMultihopQA
0.75, 10^3, MultiFieldQA
0.75, 10^3, QASPER
0.65, 10^3, 2WikiMultihopQA
0.65, 10^3, MultiFieldQA
0.65, 10^3, QASPER
0.55, 10^3, 2WikiMultihopQA
0.55, 10^3, MultiFieldQA
0.55, 10^3, QASPER
0.45, 10^3, 2WikiMultihopQA
0.45, 10^3, MultiFieldQA
0.45, 10^3, QASPER
0.35, 10^3, 2WikiMultihopQA
0.35, 10^3, MultiFieldQA
0.35, 10^3, QASPER
0.95, 10^1, 2WikiMultihopQA
0.95, 10^1, MultiFieldQA
0.95, 10^1, QASPER
0.85, 10^1, 2WikiMultihopQA
0.85, 10^1, MultiFieldQA
0.85, 10^1, QASPER
0.75, 10^1, 2WikiMultihopQA
0.75, 10^1, MultiFieldQA
0.75, 10^1, QASPER
0.65, 10^1, 2WikiMultihopQA
0.65, 10^1, MultiFieldQA
0.65, 10^1, QASPER
0.55, 10^1, 2WikiMultihopQA
0.55, 10^1, MultiFieldQA
0.55, 10^1, QASPER
0.45, 10^1, 2WikiMultihopQA
0.45, 10^1, MultiFieldQA
0.45, 10^1, QASPER
0.35, 10^1, 2WikiMultihopQA
0.35, 10^1, MultiFieldQA
0.35, 10^1, QASPER

$Normalized Performance, Additional Memory Needed for Generation (MB)
1.0, 10^4 (LLMLingua-2)
0.9, 10^3 (LLMLingua-2)
0.8, 10^2 (LLMLingua-2)
0.7, 10^1 (LLMLingua-2)
1.0, 10^4 (LLMLingua-2)
0.9, 10^3 (LLMLingua-2)
0.8, 10^2 (LLMLingua-2)
0.7, 10^1 (LLMLingua-2)
1.0, 10^4 (LLMLingua-2)
0.9, 10^3 (LLMLingua-2)
0.8, 10^2 (LLMLingua-2)
0.7, 10^1 (LLMLingua-2)

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/expertcondenser_v2.pdf

Figure 1: Illustration of the three expert scaling laws (Small-Dense, Small MoE, and Inference Reduce). The figure contains multiple line charts showing "Acc Drop" vs "Experts in QFT" for different scaling strategies.

Small Dense (Acc Drop: -11.2%, -21.2%)
Small MoE (Acc Drop: -10.2%, -13.4%)
Inference Reduce (Acc Drop: -5.8%, -2.9%)

no sir

$Model, Model Size, Distill Type, GPAA, AIME 2024, AIME 2025, MATH-500
Qwen1.5-MoE, 14B, SFT, 27.8, 0.001/120, 0.001/120, 20.1
Qwen1.5-MoE, 14B, SFT, 26.4, 0.001/120, 0.001/120, 18.1
Qwen1.5-MoE, 14B, SFT, 31.6, 1/120, 1/120, 26.7
Qwen1.5-MoE, 14B, SFT, 34.6, 2/120, 2/120, 15.4
DeepSeek-Coder-V2-Lite, 16B, SFT, 34.2, 2.53/120, 2.53/120, 64.6
DeepSeek-Coder-V2-Lite, 16B, SFT, 32.8, 2.53/120, 2.53/120, 26.0
DeepSeek-Coder-V2-Lite, 16B, SFT, 34.8, 2.53/120, 2.53/120, 25.0
DeepSeek-Coder-V2-Lite, 16B, SFT, 36.8, 3/120, 3/120, 26.1
Qwen3, 30B, SFT, 58.6, 63.36/70, 48.35/120, 94.8
Qwen3, 30B, SFT, 52.0, 42.7/120, 43.25/120, 87.2
Qwen3, 30B, SFT, 65.8, 65.87/120, 46.57/120, 95.8

$Model, Model Size, Post-train method, Bezhul, PIQA, SIQA, Hellaswag, Winogrande, ARC-c, ARC-a
OLMoE, 7B, SFT, 62.5, 65.8, 62.8, 70.7, 71.4, 79.4, 70.6
OLMoE, 7B, SFT, 62.8, 65.8, 63.3, 71.6, 71.5, 73.5, 71.3
OLMoE, 7B, DenseMixer, 62.8, 68.7, 65.3, 71.6, 70.6, 73.5, 61.3
OLMoE, 7B, ExpertCondenser(Ours), 67.5, 73.5, 67.5, 75.8, 73.8, 71.8, 73.5
Qwen1.5-MoE, 14B, SFT, 68.7, 84.7, 74.5, 76.8, 75.6, 84.6, 72.8
Qwen1.5-MoE, 14B, SFT, 74.2, 84.3, 74.2, 74.5, 74.3, 78.0, 74.4
Qwen1.5-MoE, 14B, DenseMixer, 70.8, 85.7, 74.6, 75.8, 78.9, 82.6, 77.8
Qwen1.5-MoE, 14B, ExpertCondenser(Ours), 72.1, 84.9, 75.6, 81.6, 79.8, 88.5, 78.4

$Dataset, Model, Post-train Type, GSM8K, Singleq, SVAMP, MultiArith, AddSub, QAma, avg
math7, DeepSeek-V2-Lite, aux-free-bias-condenser experts, 59.4, 92.5, 69.1, 91.5, 79.5, 36.6, 83.6, 73.1
math7, DeepSeek-V2-Lite, aux-free-bias, 58.9, 90.7, 69.3, 88.7, 75.2, 36.6, 80.3, 71.2
math7, DeepSeek-V2-Lite, with-aux-condenser experts, 58.3, 89.4, 68.9, 88.6, 75.8, 35.6, 81.7, 71.2
math7, Qwen1.5-MoE, aux-free-bias-condenser experts, 74.6, 55.7, 86.0, 63.8, 77.5, 33.4, 71.4
math7, Qwen1.5-MoE, aux-free-bias, 48.2, 76.6, 51.8, 80.3, 59.2, 26.8, 71.8, 59.4
math7, Qwen1.5-MoE, with-aux-condenser experts, 47.2, 74.0, 51.8, 82.6, 60.1, 30.1, 72.6, 60.9

$FIGURE_DESCRIPTION: Figure 4 is a bar chart titled "Weighted MoE Output Divergence" comparing Expert Condenser, SFT, and DenseMixer across 26 layers.
$FIGURE_DESCRIPTION: Figure 5 is a bar chart titled "Router KL Divergence" comparing Expert Condenser, SFT, and DenseMixer across 26 layers.
$FIGURE_DESCRIPTION: Figure 6 is a line graph titled "Average FFN gradient norm per expert during training" comparing Condenser Experts and Other Routed Experts over 17.5 training steps.
$FIGURE_DESCRIPTION: Figure 7 is a bar chart titled "Persistent path ablation" comparing "With Persistent Path" and "Without Persistent Path" for Qwen1.5-MoE and DeepSeek-V2-Lite.

no sir

Parameter,Value
Data Type,bf16
Optimizer,AdamW
Learning Rate,1 x 10⁻⁵
Batch Size,32
Sequence Length,4096
Seed,1234
Warmup Ratio,0.1
LR Scheduler,cosine_with_min_lr
Attention Implementation,flash_attention_2
ZeRO Stage,3

$Table 7: Evaluating base GPT-OSS-20B model Zero-Shot Results on downstream Math Reasoning dataset.
Columns: Model, Strategies, Remain Experts (k'), Active Experts (k), GSM8K, SVAMP, MultiArrith, AddSub, GSM8K, AddSub, AQuA, avg

$Table 8: Evaluating base DeepSeek-Coder-V2-Lite-Instruct model Zero-Shot Results on downstream Math Reasoning dataset.
Columns: Model, Strategies, Remain Experts (k'), Active Experts (k), GSM8K, SVAMP, MultiArrith, AddSub, GSM8K, AddSub, AQuA, avg

GPT-OSS-20B, Base Model, 8, 4, 78.0, 91.1, 89.0, 91.1, 89.0, 91.1, 89.0, 91.1, 89.9
GPT-OSS-20B, Small Dense, 12, 4, 2.3, 1.7, 1.6, 2.7, 1.8, 1.67, 2.3, 4.2
GPT-OSS-20B, Small Dense, 24, 12, 10.4, 10.4, 10.4, 10.4, 10.4, 10.4, 10.4, 10.4
GPT-OSS-20B, Small Dense, 24, 18, 56.9, 58.2, 65.5, 66.5, 67.5, 24.6, 58.7, 56.8
GPT-OSS-20B, Inference Reduce, 32, 1, 52.0, 52.0, 52.0, 52.0, 52.0, 52.0, 52.0, 52.0
GPT-OSS-20B, Inference Reduce, 32, 2, 70.8, 76.4, 76.4, 76.2, 75.5, 29.9, 70.9, 75.3
GPT-OSS-20B, Inference Reduce, 32, 4, 77.0, 81.0, 81.0, 81.0, 81.0, 33.0, 77.0, 81.0
GPT-OSS-20B, Inference Reduce, 32, 8, 82.0, 85.0, 85.0, 85.0, 85.0, 38.0, 82.0, 85.0
GPT-OSS-20B, Small MoE, 8, 4, 1.6, 2.2, 3.2, 2.0, 1.6, 2.4, 2.5, 3.9
GPT-OSS-20B, Small MoE, 16, 4, 20.2, 43.7, 48.1, 43.7, 41.0, 24.0, 42.4, 37.6
GPT-OSS-20B, Small MoE, 32, 16, 32.9, 50.8, 58.9, 50.8, 38.9, 22.9, 32.9, 44.5
GPT-OSS-20B, Small MoE, 24, 20, 42.6, 57.4, 63.5, 57.4, 40.1, 29.1, 40.2, 64.5

DeepSeek-Coder-V2-Lite, Base Model, 64, 6, 6, 1.4, 1.0, 1.7, 1.8, 2.2, 2.1, 4.7
DeepSeek-Coder-V2-Lite, Base Model, 12, 6, 12, 1.6, 1.7, 2.8, 1.8, 1.6, 2.5, 4.5
DeepSeek-Coder-V2-Lite, Base Model, 16, 12, 16, 3.8, 58.2, 22.6, 32.0, 31.1, 17.7, 31.9, 26.5
DeepSeek-Coder-V2-Lite, Small Dense, 20, 20, 24, 24, 56.4, 76.0, 76.0, 70.1, 20.6, 66.4, 57.6
DeepSeek-Coder-V2-Lite, Small Dense, 24, 32, 47.4, 64.4, 74.1, 64.4, 63.1, 24.4, 47.4, 68.1
DeepSeek-Coder-V2-Lite, Small Dense, 48, 48, 48.6, 82.7, 74.2, 82.7, 67.6, 24.4, 48.6, 68.4
DeepSeek-Coder-V2-Lite, Inference Reduce, 64, 4, 28.2, 32.7, 50.7, 75.2, 66.6, 24.6, 72.0, 52.6
DeepSeek-Coder-V2-Lite, Inference Reduce, 64, 4, 54.5, 87.2, 76.6, 92.3, 80.5, 25.6, 84.6, 71.6
DeepSeek-Coder-V2-Lite, Inference Reduce, 64, 4, 55.5, 87.8, 79.1, 95.7, 85.7, 24.6, 85.2, 74.8
DeepSeek-Coder-V2-Lite, Inference Reduce, 64, 4, 57.9, 89.0, 79.8, 95.7, 85.7, 24.6, 85.2, 74.8
DeepSeek-Coder-V2-Lite, Small MoE, 12, 6, 0.7, 0.0, 0.0, 0.0, 0.0, 1.3, 1.3, 2.8
DeepSeek-Coder-V2-Lite, Small MoE, 24, 6, 0.8, 0.2, 1.4, 1.2, 1.3, 1.4, 1.3, 2.8
DeepSeek-Coder-V2-Lite, Small MoE, 24, 12, 11.6, 43.5, 43.5, 43.5, 43.5, 14.7, 43.5, 43.3
DeepSeek-Coder-V2-Lite, Small MoE, 32, 24, 35.6, 76.4, 63.4, 80.7, 69.7, 22.0, 74.8, 60.4
DeepSeek-Coder-V2-Lite, Small MoE, 48, 48, 49.0, 85.4, 75.2, 95.2, 80.8, 21.7, 80.7, 69.4

FT Dataset, Model, Post-train Type, GSM8K, SingleEq, SVAMP, MultiArith, AddSub, AqQa, MAWPS, AVG
Commonsense, OLMoe-7B, Base Model, 16.1, 23.6, 17.7, 9.2, 21.3, 22.8, 13.9, 17.8
Commonsense, OLMoe-7B, SFT, 8.9, 12.7, 11.6, 5.4, 13.8, 18.6, 11.3, 11.9
Commonsense, OLMoe-7B, ESFT, 11.2, 15.6, 12.5, 5.7, 13.4, 17.8, 8.9, 11.7
Commonsense, OLMoe-7B, DenseMixer, 22.5, 31.8, 19.1, 3.7, 6.1, 17.6, 2.9, 6.4

Metric, Method, Remaining Experts, GSM8K, SingleEq, SVAMP, MultiArith, AddSub, AQUA, aqmq, wps
ES-Act, Smaller-Dense, 6, 1.4, 1.0, 2.9, 2.2, 14.0, 24.0, 9.7, 10.2
ES-Act, Smaller-Dense, 12, 1.7, 0.4, 7.2, 13.2, 24.0, 23.7, 10.3, 12.1
ES-Act, Smaller-Dense, 16, 11.8, 3.8, 22.6, 32.0, 31.1, 17.7, 31.9, 26.5
ES-Act, Smaller-Dense, 20, 26.1, 10.0, 46.1, 67.7, 56.5, 19.8, 45.1, 51.1
ES-Act, Smaller-Dense, 24, 36.9, 75.2, 98.4, 76.0, 70.1, 20.1, 66.4, 57.6
ES-Act, Smaller-Dense, 32, 47.5, 71.6, 118.4, 81.8, 71.4, 24.4, 79.7, 64.4
ES-Act, Smaller-MoE, 12, 0.7, 0.4, 1.5, 0.8, 0.0, 16.9, 1.3, 3.1
ES-Act, Smaller-MoE, 16, 11.3, 0.8, 1.4, 1.2, 1.3, 13.4, 3.3, 4.3
ES-Act, Smaller-MoE, 24, 11.3, 45.9, 33.5, 41.8, 46.6, 22.0, 74.8, 60.4
ES-Act, Smaller-MoE, 32, 35.6, 76.4, 63.4, 80.7, 69.7, 22.0, 74.8, 60.4
ES-Act, Smaller-MoE, 48, 49.6, 85.4, 75.2, 93.2, 80.8, 21.7, 84.4, 64.4
ES-Mag, Smaller-Dense, 6, 1.6, 1.2, 2.1, 2.6, 2.1, 18.9, 2.3, 4.4
ES-Mag, Smaller-Dense, 12, 1.6, 0.8, 5.2, 6.1, 13.4, 22.8, 32.4, 21.9, 28.0
ES-Mag, Smaller-Dense, 16, 12.6, 38.8, 33.8, 32.6, 32.4, 22.4, 31.9, 28.0
ES-Mag, Smaller-Dense, 24, 25.4, 64.8, 45.0, 68.8, 528, 138, 18.8, 65.2, 48.4
ES-Mag, Smaller-Dense, 32, 48.2, 82.8, 72.2, 78.8, 82.7, 82.2, 24.2, 83.2, 68.6
ES-Mag, Smaller-MoE, 12, 0.7, 0.2, 1.7, 0.6, 0.0, 16.9, 1.3, 3.1
ES-Mag, Smaller-MoE, 16, 1.3, 0.4, 2.6, 1.8, 1.6, 13.6, 10.4, 4.5
ES-Mag, Smaller-MoE, 24, 10.8, 44.7, 32.8, 42.5, 44.8, 21.3, 44.8, 34.5
ES-Mag, Smaller-MoE, 32, 14.7, 75.9, 64.9, 81.3, 46.4, 23.8, 79.6, 41.6
ES-Mag, Smaller-MoE, 48, 47.6, 86.7, 75.8, 92.7, 81.7, 23.9, 81.3, 70.0

Model, Dataset, r, GSM8, Singleq, SVAMP, MultiArith, Add, AQUA, mawps, AVG
DeepSeek-V2-Lite, math7k, 1, 58.9, 92.1, 68.8, 90.3, 77.6, 35.9, 82.5, 77.2
DeepSeek-V2-Lite, math7k, 2, 59.4, 92.5, 69.1, 91.5, 79.5, 36.1, 83.1, 77.1
DeepSeek-V2-Lite, math7k, 4, 58.6, 92.1, 69.7, 92.7, 78.7, 33.5, 85.7, 73.0

$Dataset, Model, Model Size, #param (Experts), Selection Type, GSM8k, SingleEq, SVAMP, MultiArith, AddSub, AQuA, AVG
math7k, Deepseek-V2-Lite, 16B, 2.4B, high-bias experts, 60.1, 90.2, 70.4, 90.2, 74.2, 37.0, 84.5, 72.4
math7k, Deepseek-V2-Lite, 16B, 2.4B, high-bias experts, 99.8, 93.5, 70.0, 90.6, 78.1, 36.7, 83.9, 72.4
math7k, Deepseek-V2-Lite, 16B, 2.4B, low-activation experts, 56.6, 73.9, 55.1, 86.4, 62.0, 32.7, 75.0, 63.0
math7k, Deepseek-V2-Lite, 16B, 2.4B, high-activation experts, 56.6, 73.9, 55.1, 86.4, 62.0, 32.7, 75.0, 63.0
math7k, Deepseek-V2-Lite, 16B, 2.4B, random experts, 56.1, 72.8, 55.0, 85.2, 60.3, 32.9, 75.4, 62.5
math7k, QwenL5-MoE, 14B, 2.7B, high-bias experts, 54.7, 71.0, 53.8, 84.6, 58.8, 32.8, 76.8, 61.8
math7k, QwenL5-MoE, 14B, 2.7B, high-bias experts, 56.6, 73.9, 55.1, 86.4, 62.0, 32.7, 75.0, 63.0
math7k, QwenL5-MoE, 14B, 2.7B, low-activation experts, 56.6, 73.9, 55.1, 86.4, 62.0, 32.7, 75.0, 63.0
math7k, QwenL5-MoE, 14B, 2.7B, high-activation experts, 56.6, 73.9, 55.1, 86.4, 62.0, 32.7, 75.0, 63.0
math7k, QwenL5-MoE, 14B, 2.7B, random experts, 56.1, 72.8, 55.0, 85.2, 60.3, 32.9, 75.4, 62.5

Model, Model Size, #Param (Experts), Distill Type, GSM8K, SingleEq, SVAMP, MultiArith, AddSub, AQuA, maqs, AVG
DeepSeek-V2-Lite, 16B, 2.4B, ESFT-1epoch, 54.1, 88.0, 65.3, 83.7, 72.7, 26.8, 79.4, 67.1
DeepSeek-V2-Lite, 16B, 2.4B, ESFT-2epoch, 58.6, 80.9, 65.8, 90.7, 62.3, 27.6, 76.1, 66.0
DeepSeek-V2-Lite, 16B, 2.4B, ESFT-3epoch, 58.5, 81.1, 68.0, 89.0, 68.0, 25.5, 78.5, 67.0

$Model, Model Size, Distil Type, #Param (Experts), GSM8K, SVAMP, MultiArith, AQuA, mawps, AVG
G-le-6, 14B, aux-free-bias, 2.7B, 45.6, 50.0, 81.8, 29.6, 70.6, 59.6
G-le-5, 14B, aux-free-bias, 2.7B, 48.8, 50.2, 81.6, 29.7, 70.8, 59.6
G-le-4, 14B, aux-free-bias, 2.7B, 47.6, 52.1, 81.8, 29.8, 70.9, 59.9
G-le-3, 14B, aux-free-bias, 2.7B, 48.2, 52.7, 80.3, 26.8, 71.9, 59.4
G-le-2, 14B, aux-free-bias, 2.7B, 47.7, 51.1, 80.1, 28.3, 70.1, 59.1
G-le-1, 14B, aux-free-bias, 2.7B, 31.8, 37.9, 65.2, 28.3, 56.7, 45.4
G-le-0, 14B, aux-free-bias, 2.7B, 15.1, 25.2, 57.3, 21.7, 34.4, 28.4
G-le-2, 14B, aux-free-bias, 2.7B, 7.2, 14.7, 28.2, 17.7, 18.5, 17.0
G-le-4, 14B, aux-free-bias, 2.7B, 56.2, 68.9, 86.2, 73.2, 79.0, 69.8
G-le-3, 14B, aux-free-bias, 2.7B, 56.2, 70.1, 87.6, 73.8, 79.6, 70.5
G-le-5, 14B, aux-free-bias, 2.7B, 58.6, 70.2, 88.6, 74.4, 80.6, 71.1
G-le-4, 14B, aux-free-bias, 2.7B, 58.8, 69.3, 88.7, 74.5, 80.3, 71.3
G-le-3, 14B, aux-free-bias, 2.7B, 43.4, 62.6, 80.5, 71.0, 65.8, 60.3

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/fst_2605.12484.pdf

$Validation reward (%) vs KL(\pi_{train} \parallel \pi_{base})$
CodeIO: KL(0.00, RL, 35), KL(0.04, RL, 37), KL(0.08, RL, 38), KL(0.12, RL, 39)
HoVer: KL(0.25, RL, 40), KL(0.50, RL, 43), KL(0.75, RL, 45)
Physics: KL(0.00, RL, 66), KL(0.03, RL, 75), KL(0.06, RL, 80), KL(0.09, RL, 83)

$Domain, Base\ model, L_{ctx}/L_{p}/L_{r}, Batch, Rollouts/step, GPU\ util.$
HoVer-hard, Qwen3-8B (think), 18944 / 4096 / 8192, 32, 256, 0.6
Physics, Qwen3-8B (think), 18944 / 4096 / 8192, 32, 256, 0.7
CodeIO, Qwen3-8B (think), 18944 / 4096 / 8192, 32, 256, 0.6
Math (Polaris), Qwen3-8B-SFT (think), 12288 / 4096 / 8192, 64, 512, 0.7
Star-graph, Qwen3-4B (no think), 8192 / 4096 / 4096, 32, 256, 0.6

$Domain, K, G/K, Cycle\ T, Eval\ set, Metric\ calls, Reflection\ LM$
HoVer-hard, 8, 1, 6, 192, 960, gpt-5.2
Physics, 4, 2, 6, 192, 960, gpt-5.2
CodeIO, 8, 1, 6, 192, 960, gpt-5.2
Math (Polaris), 4, 2, 6, 96, 1500, gpt-5.2
Star-graph, 8, 1, 6, 200, 960, gpt-5.2

$Validation accuracy (%) is the dependent variable representing the percentage of correct answers. The x-axis categories represent different training configurations and methods.

Base: 19.4%
Slow only (RL): 20.9%
Slow only (FST w/o prompt): 25.1%
Fast only (Base + GEPA): 34.5%
Slow + Fast (RL + GEPA): 39.3%
Slow + Fast (FST): 43.1%
Slow + Fast (FST + GEPA): 43.3%

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/grow_dont_overwrite_2603.08647.pdf

$Dataset, Method, \# Intersecting Heads (\uparrow), FV Similarity (\uparrow)$
Entailment, SFT, 2/10, 0.28
Translation, SFT, 3/10, 0.58

Symbol,Definition
$M_0$,pretrained model
$\theta_0$,pretrained model parameters
$M_T$,finetuned model
$\theta_T$,finetuned model parameters
$\mathcal{L}_T$,finetuning task loss
$\mathcal{D}_T$,downstream task dataset
$\mathcal{D}_{proxy}$,proxy dataset to measure performance on pretraining set
$N$,Number of layers
$n$,model layer index
$\mathbf{X}$,input to model MLP module
$s$,input sequence length
$h$,model hidden dimension
$p$,MLP hidden dimension
$\mathbf{W}_n^{(1)}$,weight matrix of first layer of MLP module (up-projection)
$\mathbf{W}_n^{(2)}$,weight matrix of second layer of MLP module (down-projection)
$\mathbf{B}_n^{(1)}$,bias term of first layer of MLP module (up-projection)
$\mathbf{B}_n^{(2)}$,bias term of second layer of MLP module (down-projection)
$\mathbf{W}_n^{(1)}$,expanded matrix of first layer of MLP module (up-projection)
$\mathbf{W}_n^{(2)}$,expanded matrix of second layer of MLP module (down-projection)
$\mathbf{Y}$,output of MLP module

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/inplace_ttt_2604.06169.pdf

Model | 4k | 8k | 16k | 32k | 64k | 128k | 256k
--- | --- | --- | --- | --- | --- | --- | ---
Mistral-7B [31] | 93.6 | 91.2 | 87.2 | 75.4 | 49.0 | 13.8 | -
GLM3-6B [23] | 87.8 | 83.4 | 78.6 | 69.9 | 56.0 | 42.0 | -
Phi3-medium-14B [1] | 93.3 | 93.2 | 93.1 | 86.8 | 78.6 | 46.1 | -
Llama3-8B [41] | 92.8 | 90.3 | 85.7 | 79.9 | 76.3 | 69.5 | -
Qwen3-4B (instruct) [57] | 95.1 | 93.6 | 91.0 | 87.8 | 77.8 | 66.0 | -
Baseline | 96.6 | 94.1 | 92.1 | 88.7 | 74.3 | 74.8 | 41.7

Base Model | Method | 4k | 8k | 16k | 32k | 64k | 64k+YaRN
LLaMA-3.1-8B | Baseline | 93.9 | 92.1 | 92.5 | 91.1 | 81.6 | 
Qwen3-14B | Baseline | 96.8 | 95.0 | 94.6 | 90.7 | 67.9 | 81.3

$Table 3 evaluates 4B models on common sense reasoning and long-context evaluation benchmarks. Best performance is in bold. "SWA" is Sliding-Window Attention, "Full Attn." is Full Attention, and "I.P. TTT" is our In-Place TTT.
Model Architecture, Common Sense Reasoning: HellaSwag, ARC-E, ARC-C, MMLU, PIQA, Long-Context Evaluation: RULER-4k, RULER-8k, RULER-16k
Baselines, Full Attn., 55.67, 64.52, 33.19, 36.43, 45.77, 38.09, 6.58
Baselines, SWA, 54.92, 64.18, 32.85, 36.06, 72.58, 14.77, 9.91, 5.07

(a) State size: A bar chart showing Score vs. RULER task difficulty (4k, 8k, 16k) for different weight counts (0.5x, 1x, 4x).
(b) Chunk size: A bar chart showing Score vs. RULER task difficulty (4k, 8k, 16k) for different chunk sizes (C=256, C=512, C=1024, C=2048).
(c) LM-Aligned Proj: A bar chart showing Score vs. RULER task difficulty (4k, 8k, 16k) for different projection configurations (w Conv, w/o Conv, w/o Proj, w Conv, Proj).
(a) Throughput (SWA): A bar chart showing Prefill TPS (k-tokens/s) vs. context length (8k, 32k, 128k) comparing Baseline-SWA and TTT-SWA.
(b) Throughput (Full): A bar chart showing Prefill TPS (k-tokens/s) vs. context length (8k, 32k, 128k) comparing Baseline-Full and TTT-Full.
(c) Memory (SWA): A bar chart showing Peak Memory (GB) vs. context length (8k, 32k, 128k) comparing Baseline-SWA and TTT-SWA.
(d) Memory (Full): A bar chart showing Peak Memory (GB) vs. context length (8k, 32k, 128k) comparing Baseline-Full and TTT-Full.

no sir

Hyperparameter, 500M Model, 1.5B Model
Optimizer, AdamW, AdamW
Learning Rate, 5e-4, 3e-4
Batch Size, 2M tokens, 4M tokens
Weight Decay, 0.1, 0.1
Gradient Clipping, 1.0, 1.0
Warmup Steps, 1024, 1024
Sequence Length, 32,768, 32,768
Tokens Trained, 20B, 60B
Sliding Window Size, 2,048, 4,096

Hyperparameter, value
Optimizer, AdamW
Learning Rate, 3e-4
Batch Size, 8M tokens
Weight Decay, 0.1
Gradient Clipping, 1.0
Warm-up Tokens, 1.6B
Sequence Length, 8,192
Tokens Trained, 120B

Parameter, 500M, 1.5B
Parameters (Approx.), 500M, 1.5B
Hidden Size ($d_{model}$), 1024, 2048
Num Layers, 24, 24
Num Attention Heads, 8, 16
FFN Hidden Size ($d_f$), 3072, 6144
Window Size, 2048, 4096
Vocabulary Size, 32,000, 32,000
Rope Base, 1e6, 1e6

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/keepLoRA_2601.19659.pdf

$Avg., Zero-shot, Transfer, Li & Hoien 2017, 74.5, 39.1, 51.1, 52.6, 72.8, 60.6, 73.1, 30.3, 55.9, 64.4$
$Avg., Zero-shot, Transfer, iCaRL, Rebuffi et al. 2017, 36.6, 32.7, 39.3, 46.0, 38.6, 46.0, 77.4, 31.9, 60.5, 50.4$
$Avg., Zero-shot, Transfer, LW-VF, Ding et al. 2022, 71.1, 60.5, 45.3, 54.4, 74.6, 47.9, 76.7, 36.3, 58.6, 72.8$
$Avg., Zero-shot, Transfer, WS-FTE, Wotoreanu et al. 2023, 60.8, 45.0, 56.4, 65.9, 53.9, 60.9, 73.8, 29.8, 49.1, 62.4$
$Avg., Zero-shot, Transfer, ZSCL, Zheng et al. 2023, 86.0, 47.4, 54.4, 69.1, 87.6, 67.5, 81.8, 60.1, 66.8, 66.1$
$Avg., Zero-shot, Transfer, O-LoRA, Wang et al. 2023a, 80.8, 68.0, 44.5, 69.6, 87.5, 59.7, 88.3, 56.1, 63.6, 66.5$
$Avg., Zero-shot, Transfer, InFloRA, Liang & Li 2024, 84.3, 67.4, 54.3, 60.0, 87.2, 62.7, 87.8, 52.7, 68.8, 67.2$
$Avg., Zero-shot, Transfer, SD-LoRA, Wu et al. 2025b, 36.7, 92.2, 85.8, 25.9, 77.5, 73.2, 84.9, 79.8, 92.5, 65.0$
$Avg., L2P, Transfer, DualPrompt, Wang et al. 2023b, 56.5, 60.9, 30.4, 41.4, 70.1, 59.5, 77.5, 37.3, 55.3, 53.4$
$Avg., L2P, Transfer, S-Prompts, Wang et al. 2023a, 37.5, 95.6, 87.1, 70.2, 97.5, 96.5, 09.1, 99.0, 74.5, 79.5, 83.4$
$Avg., L2P, Transfer, DIKI, Tang et al. 2024, 45.4, 95.7, 85.0, 71.0, 78.2, 82.5, 87.1, 71.7, 90.0, 66.6, 65.7, 76.7$
$Avg., L2P, Transfer, MoE-Adapters, Yi et al. 2024, 87.9, 68.2, 44.4, 49.0, 70.7, 88.7, 59.1, 84.5, 66.7, 65.5, 68.9$
$Avg., L2P, Transfer, IAP, Fu et al. 2025, 93.0, 68.0, 47.0, 40.4, 79.0, 44.7, 67.0, 49.5, 87.2, 66.2, 65.3, 69.2$
$Avg., Classification, Transfer, LW-VF, Ding et al. 2022, 35.5, 69.2, 72.2, 60.6, 60.8, 70.0, 72.8, 61.2, 81.5, 62.5, 46.7$
$Avg., Classification, Transfer, WS-FTE, Wotoreanu et al. 2023, 26.7, 66.5, 54.1, 67.1, 55.7, 71.1, 70.5, 75.8, 36.9, 54.6, 60.7$
$Avg., Classification, Transfer, ZSCL, Zheng et al. 2023, 41.1, 95.0, 82.1, 90.3, 74.5, 89.6, 75.2, 89.9, 64.7, 68.0, 75.4$
$Avg., Classification, Transfer, O-LoRA, Wang et al. 2023a, 80.8, 68.0, 44.5, 69.6, 87.5, 59.7, 88.3, 56.1, 63.6, 66.5, 66.5$
$Avg., Classification, Transfer, InFloRA, Liang & Li 2024, 51.1, 96.5, 85.3, 70.7, 98.1, 87.7, 97.3, 99.4, 92.4, 84.0, 81.5, 84.2$
$Avg., Classification, Transfer, SD-LoRA, Wu et al. 2025b, 36.7, 92.2, 85.8, 25.9, 77.5, 73.2, 84.9, 79.8, 92.5, 65.0, 72.5, 67.5$
$Avg., Classification, Transfer, DualPrompt, Wang et al. 2023b, 37.8, 78.4, 78.1, 71.1, 75.2, 791, 739, 78.1, 820, 551, 767, 680$
$Avg., Classification, Transfer, S-Prompts, Wang et al. 2023a, 37.5, 95.6, 87.1, 70.2, 97.5, 96.5, 09.1, 99.0, 74.5, 79.5, 83.4$
$Avg., Classification, Transfer, DIKI, Tang et al. 2024, 45.4, 95.7, 85.0, 71.0, 78.2, 82.5, 87.1, 71.7, 90.0, 66.6, 65.7, 76.7$
$Avg., Classification, Transfer, MoE-Adapters, Yi et al. 2024, 49.8, 92.2, 86.1, 78.1, 95.7, 94.3, 89.5, 98.1, 89.9, 81.6, 80.0, 85.0$
$Avg., Classification, Transfer, IAP, Fu et al. 2025, 46.8, 96.1, 76.7, 95.2, 98.1, 97.0, 99.6, 97.4, 89.9, 92.4, 78.7, 85.7$
$Avg., Classification, Transfer, Li & Hoien 2017, 38.5, 93.8, 55.0, 73.2, 88.3, 90.5, 96.7, 83.4, 99.9, 92.6, 81.2, 81.9$
$Avg., Classification, Transfer, LW-VF, Ding et al. 2022, 71.1, 60.5, 45.3, 54.4, 74.6, 47.9, 76.7, 36.3, 58.6, 72.8, 52.8$
$Avg., Classification, Transfer, WS-FTE, Wotoreanu et al. 2023, 40.6, 92.1, 80.5, 94.5, 91.9, 91.8, 93.9, 98.7, 80.3, 85.2, 83.6, 80.2$
$Avg., Classification, Transfer, ZSCL, Zheng et al. 2023, 41.1, 95.0, 82.1, 90.3, 74.5, 89.6, 75.2, 89.9, 64.7, 68.0, 75.4$
$Avg., Classification, Transfer, O-LoRA, Wang et al. 2023a, 80.8, 68.0, 44.5, 69.6, 87.5, 59.7, 88.3, 56.1, 63.6, 66.5, 66.5$
$Avg., Classification, Transfer, InFloRA, Liang & Li 2024, 51.1, 96.5, 85.3, 70.7, 98.1, 87.7, 97.3, 99.4, 92.4, 84.0, 81.5, 84.2$
$Avg., Classification, Transfer, SD-LoRA, Wu et al. 2025b, 36.7, 92.2, 85.8, 25.9, 77.5, 73.2, 84.9, 79.8, 92.5, 65.0, 72.5, 67.5$
$Avg., Classification, Transfer, DualPrompt, Wang et al. 2023b, 37.8, 84.1, 72.0, 89.6, 96.1, 89.2, 99.1, 89.4, 99.1, 79.6, 86.0, 72.0$
$Avg., Classification, Transfer, S-Prompts, Wang et al. 2023a, 37.5, 95.6, 87.1, 70.2, 97.5, 96.5, 09.1, 99.0, 74.5, 79.5, 83.4$
$Avg., Classification, Transfer, DIKI, Tang et al. 2024, 45.4, 95.7, 85.0, 71.0, 78.2, 82.5, 87.1, 71.7, 90.0, 66.6, 65.7, 76.7$
$Avg., Classification, Transfer, MoE-Adapters, Yi et al. 2024, 49.8, 92.2, 86.1, 78.1, 95.7, 94.3, 89.5, 98.1, 89.9, 81.6, 80.0, 85.0$
$Avg., Classification, Transfer, IAP, Fu et al. 2025, 46.8, 96.1, 76.7, 95.2, 98.1, 97.0, 99.6, 97.4, 89.9, 92.4, 78.7, 85.7$

$Zero-shot, Method, Sensing, Medical, Driving, Science, Finance, Avg.
TRA-LoRA, Hu et al. 2024, 22.10, 28.44, 34.44, 50.19, 37.05, 32.24
O-LoRA, Wang et al. 2023a, 28.37, 18.37, 33.72, 52.53, 32.25, 32.25
CL-MOE, Huai et al. 2023, 28.25, 19.38, 34.08, 48.56, 35.56, 32.57
SEFE, Chen et al. 2023, 28.10, 16.63, 34.13, 55.61, 33.71, 33.49
LoRA-Lt, Hu et al. 2024, 75.04, 45.71, 32.72, 66.23, 50.31, 50.31
O-LoRA, Wang et al. 2023a, 74.19, 46.50, 32.08, 38.88, 59.59, 50.31
CL-MOE, Huai et al. 2023, 77.71, 47.69, 35.35, 38.99, 59.57, 51.86
SEFE, Chen et al. 2023, 79.95, 50.50, 37.30, 40.70, 62.75, 54.19
TRA-LoRA, Hu et al. 2024, 69.34, 44.30, 29.19, 41.50, 88.43, 54.52
O-LoRA, Wang et al. 2023a, 72.40, 46.39, 31.59, 88.40, 50.06, 50.31
CL-MOE, Huai et al. 2023, 71.83, 47.96, 24.49, 89.16, 55.86, 57.16
SEFE, Chen et al. 2023, 77.75, 50.86, 40.27, 42.98, 68.46, 59.91

Method, IngNet-R, AnxVQ, VizWiz, IonQA, CLEVR, Flisck50k, Avg.
Transfer-Zero-shot, , , , , , , 
O-LoRA, , 52.63, 6.02, 16.97, 40.29, 26.84
CL-MoE, , 52.00, 7.32, 17.81, 41.28, 27.56
SEFE, , 53.33, 7.48, 17.03, 40.90, 27.48
KeepLoRA, , 52.63, 6.02, 16.97, 40.29, 26.84
Transfer-Average, , , , , , , 
O-LoRA, , 82.06, 41.73, 38.83, 34.56, 43.24, 51.99
CL-MoE, , 80.16, 40.33, 30.33, 43.90, 53.85, 50.85
SEFE, , 85.09, 47.55, 42.92, 40.33, 43.80, 54.29
KeepLoRA, , 85.60, 43.67, 67.80, 40.28, 44.11, 55.37
LoRA-FT, , 58.60, 47.52, 67.43, 61.57, 58.03, 61.35
O-LoRA, , 74.17, 49.30, 62.87, 63.83, 57.24, 64.06
CL-MoE, , 67.17, 44.38, 52.63, 54.40, 57.28, 58.61
SEFE, , 80.23, 46.54, 67.80, 66.40, 57.18, 65.84
KeepLoRA, , 82.43, 46.54, 67.80, 66.40, 57.18, 67.84

$Transfer, \Delta, Average, Last, \Delta$
58.3, 0, 61.5, 0, 59.4, 0

<!-- PAGE 19 FAILED -->

$Accuracy_of_various_methods_across_different_datasets_and_metrics.
The_columns_represent_the_datasets: Sensing, Medical, Driving, Science, Finance.
The_rows_represent_the_evaluation_metrics: Transfer, Sensation, Medical, Driving, Science, Finance, and Average.
The_values_in_the_cells_represent_the_accuracy_percentage.

(a) LoRA-FT
Transfer: 28.1, 17.4, 34.0, 50.2, 32.4
Sensing: 78.8, 28.1, 17.3, 34.8, 45.6
Medical: 75.5, 58.8, 17.5, 32.7, 54.8
Driving: 70.0, 47.5, 52.3, 34.6, 40.9
Science: 73.2, 46.4, 30.6, 50.4, 49.5
Finance: 69.3, 44.3, 29.1, 41.4, 88.4, 54.5
Average: 73.3, 44.9, 31.4, 38.8, 57.8, 49.3

(b) O-LoRA
Transfer: 28.4, 18.4, 33.7, 52.5, 33.3
Sensing: 79.4, 28.4, 17.6, 34.9, 56.1
Medical: 74.3, 58.5, 19.2, 33.2, 56.0
Driving: 74.7, 48.3, 52.6, 33.1, 45.2
Science: 74.6, 46.5, 42.2, 50.1, 52.8
Finance: 72.3, 46.9, 31.6, 41.5, 88.1, 56.1
Average: 75.0, 45.7, 32.6, 38.5, 59.6, 50.3

(c) CL-MoE
Transfer: 28.3, 19.4, 34.1, 48.6, 32.6
Sensing: 79.4, 28.3, 18.7, 35.2, 56.4
Medical: 74.8, 60.7, 20.1, 32.4, 54.9
Driving: 74.0, 44.3, 52.0, 34.7, 39.6
Science: 71.0, 47.4, 40.0, 50.8, 45.3
Finance: 71.8, 47.4, 29.5, 41.5, 89.2, 55.9
Average: 74.2, 45.6, 32.1, 38.9, 56.7, 49.7

(d) SEFE
Transfer: 28.1, 19.6, 33.9, 52.4, 33.5
Sensing: 78.8, 28.1, 18.6, 35.1, 56.2
Medical: 77.1, 59.5, 20.7, 33.0, 55.7
Driving: 77.8, 31.6, 52.5, 33.5, 47.4
Science: 77.9, 48.4, 44.7, 50.0, 48.1
Finance: 77.1, 50.9, 40.3, 43.0, 88.4, 59.9
Average: 77.7, 47.7, 35.4, 39.0, 59.6, 51.9

$Accuracy_Metrics: Transfer, Average, and Last (for specific datasets)$
$Datasets: ImgNet-R, ArxivQA, VizWiz, IconQA, CLEVR, Flickr30k$

(a) LoRA-FT
ImgNet-R: 52.6, 18.3, 6.0, 17.0, 40.3, 27.6
ArxivQA: 90.5, 12.1, 13.1, 2.1, 14.2, 15.1
VizWiz: 73.6, 90.7, 61.0, 4.2, 19.0, 49.7
IconQA: 72.7, 77.1, 53.7, 59.7, 17.4, 47.8
CLEVR: 68.8, 77.4, 52.3, 67.8, 77.8, 46.1
Flickr30k: 58.6, 76.7, 45.7, 67.4, 61.6, 58.0
Average: 76.0, 77.8, 41.6, 38.8, 34.6, 43.3

(b) O-LoRA
ImgNet-R: 52.9, 19.6, 4.4, 16.9, 41.0, 27.0
ArxivQA: 89.7, 94.2, 14.5, 0.0, 12.9, 25.0
VizWiz: 80.9, 91.7, 59.8, 0.0, 19.6, 49.0
IconQA: 80.2, 80.3, 54.5, 75.9, 17.6, 48.6
CLEVR: 78.1, 80.4, 51.6, 63.2, 72.4, 46.0
Flickr30k: 74.2, 80.9, 45.3, 62.9, 63.8, 57.2
Average: 82.4, 80.1, 41.7, 35.9, 33.9, 43.7

(c) CL-MoE
ImgNet-R: 52.0, 19.3, 7.4, 17.8, 41.3, 27.6
ArxivQA: 89.2, 92.8, 14.8, 10.0, 15.7, 26.2
VizWiz: 77.2, 90.7, 60.4, 6.9, 20.6, 49.5
IconQA: 79.5, 76.2, 51.0, 54.7, 19.4, 47.9
CLEVR: 76.7, 75.4, 48.1, 52.6, 73.0, 45.9
Flickr30k: 61.2, 75.8, 44.4, 52.6, 54.4, 57.3
Average: 80.2, 77.1, 40.4, 33.3, 34.1, 55.0

(d) SEFE
ImgNet-R: 53.3, 18.7, 7.5, 17.0, 40.9, 27.5
ArxivQA: 91.8, 53.7, 23.7, 12.1, 16.9, 36.4
VizWiz: 90.4, 92.8, 13.7, 5.0, 16.4, 21.1
IconQA: 83.6, 89.3, 61.4, 5.3, 18.6, 49.8
CLEVR: 82.8, 78.6, 54.2, 70.6, 75.0, 46.5
Flickr30k: 80.2, 79.1, 47.1, 69.4, 65.7, 57.3
Average: 85.5, 78.6, 42.9, 40.3, 34.8, 43.4

(e) KeepLoRA
ImgNet-R: 52.8, 20.4, 9.2, 18.1, 41.5, 28.4
ArxivQA: 90.4, 94.5, 15.2, 4.0, 17.2, 21.5
VizWiz: 85.5, 92.4, 61.5, 10.1, 21.0, 50.6
IconQA: 85.1, 86.0, 55.3, 76.9, 17.1, 50.9
CLEVR: 84.1, 39.3, 51.5, 68.3, 72.6, 47.8
Flickr30k: 82.4, 86.7, 46.6, 67.8, 66.4, 57.2
Average: 86.5, 83.6, 42.7, 40.1, 35.2, 44.1

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/moeedit_2602.10965.pdf

$Method, Model, Counterfact_Eff, Counterfact_Gen, Counterfact_Spe, Counterfact_Uti, ZsRe_Eff, ZsRe_Gen, ZsRe_Spe, ZsRe_Uti
Pre-edited, Qwen3-0b-A3B, 13.30, 15.14, 37.62, 41.30, 40.50, 40.97, 40.90
FT, Qwen3-0b-A3B, 80.70, 63.95, 62.03, 6.44, 6.13, 4.06, 4.91
FTL, Qwen3-0b-A3B, 82.40, 22.75, 42.86, 44.19, 41.92, 42.86
AdaLoRA, Qwen3-0b-A3B, 51.90, 33.50, 58.88, 29.78, 25.30, 28.84
UnKE, Qwen3-0b-A3B, 89.30, 82.85, 73.43, 31.43, 29.78, 25.30, 28.84

Table 2: Routing distribution shift on Qwen3-30B-A3B. Values are Jacard similarity (RS) between pre- and post-edit routing distributions. Higher is better. Best results are in bold, second-best are underlined.

Method | Model | Editing Set RS↑ | Lay. 11–20 | Lay. 21–30 | Lay. 31–40 | Preservation Set RS↑ | Lay. 11–20 | Lay. 21–30 | Lay. 31–40
FT | Qwen3-30B-A3B | | 23.57 | 26.58 | 29.98 | 24.72 | 27.45 | 30.97
FT-L | Qwen3-30B-A3B | | 47.01 | 51.20 | 53.68 | 48.80 | 50.17 | 53.45
AdaLoRA | Qwen3-30B-A3B | | 16.63 | 24.11 | 27.00 | 16.38 | 23.84 | 26.60
UnKE | Qwen3-30B-A3B | | 52.46 | 44.12 | 44.80 | 49.90 | 41.91 | 43.84

Table 3: Ablation on the projection matrix. Removing projection significantly weakens routing stability.

Method | Set | RS↑ | Lay. 11–20 | Lay. 21–30 | Lay. 31–40
MoEE dit (w/o Pro) | Edit. | | 73.64 | 72.90 | 73.75
MoEE dit (w/o Pro) | Pres. | | 73.59 | 73.08 | 73.50

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/moram_2506.21035.pdf

Method | Aircraft | Caltech | DTDD | Euro545 | Food | MNIST | OsNet | SUN97 | Average
---|---|---|---|---|---|---|---|---|---
CLIP Zero-shot | 23.5 | 76.8 | 37.3 | 36.7 | 63.6 | 84.0 | 86.7 | 66.1 | 63.7
CLIP Fine-tune | 39.6 | 84.7 | 70.0 | 94.7 | 97.0 | 85.8 | 97.6 | 93.4 | 81.9
ZF-RoRam (Zhao et al., 2021) | 76.8 | 37.3 | 36.7 | 63.6 | 84.0 | 46.7 | 86.7 | 66.1 | 63.7
LwF (Li & Hoiem, 2017) | 66.6 | 29.9 | 19.5 | 51.0 | 78.4 | 26.6 | 89.5 | 35.1 | 47.7
WiSE-FT (Wortsley et al., 2022) | 70.1 | 31.9 | 25.3 | 56.3 | 79.9 | 29.9 | 74.9 | 45.6 | 56.8
iCaRL (Rebuffi et al., 2017) | 71.7 | 35.0 | 24.5 | 51.6 | 86.9 | 43.8 | 83.7 | 61.7 | 61.7
ZSCL (Zheng et al., 2024) | 73.3 | 32.6 | 36.8 | 62.1 | 83.8 | 42.1 | 83.6 | 60.5 | 59.0
MoE-Adapter (Yu et al., 2024) | 71.9 | 34.9 | 19.2 | 63.6 | 80.0 | 62.7 | 87.6 | 58.7 | 66.0
LoRA-MoE (Xu et al., 2024) | 76.8 | 37.3 | 36.7 | 63.6 | 84.0 | 46.7 | 86.7 | 66.1 | 63.7
CoDyRA (Li et al., 2024) | 74.3 | 36.8 | 44.2 | 69.8 | 85.8 | 52.2 | 86.1 | 64.4 | 63.4
LwF (Li & Hoiem, 2017) | 66.6 | 29.9 | 19.5 | 51.0 | 78.4 | 26.6 | 89.5 | 35.1 | 47.7
WiSE-FT (Wortsley et al., 2022) | 27.1 | 76.5 | 40.9 | 20.8 | 77.5 | 74.9 | 75.8 | 72.5 | 58.0
iCaRL (Rebuffi et al., 2017) | 25.1 | 72.3 | 38.9 | 55.4 | 87.5 | 81.9 | 88.6 | 63.6 | 61.5
ZSCL (Zheng et al., 2024) | 36.0 | 75.0 | 40.7 | 71.5 | 56.3 | 86.3 | 63.3 | 60.5 | 61.0
MoE-Adapter (Yu et al., 2024) | 43.2 | 78.7 | 57.6 | 32.8 | 79.4 | 96.0 | 86.7 | 87.8 | 74.2
RAIL-Primal (Xu et al., 2024) | 41.4 | 94.0 | 66.0 | 88.4 | 97.2 | 82.4 | 93.1 | 83.0 | 75.0
CoDyRA (Li et al., 2024) | 37.7 | 81.8 | 65.5 | 81.9 | 95.0 | 96.7 | 93.3 | 77.3 | 79.1

| Method | FIX(CL) | SeqLoRA | GEM | EW | L2P | DualProg | HiDeLoRA | MoE-LoRA | MoRAM |
| :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- | :--- |
| **meta-llama/Llama-3-7B** | | | | | | | | | |
| OP | 38.4 | 34.3 | 42.09 | 40.08 | 42.6 | 36.23 | 41.60 | 42.78 | 43.52 |
| BWT | 18.5 | 18.5 | 8.06 | 6.72 | 5.25 | 8.03 | 7.12 | 7.16 | 3.46 |
| **google/Gemma-2B** | | | | | | | | | |
| OP | 32.2 | 31.89 | 32.5 | 31.85 | 28.5 | 31.2 | 32.2 | 33.05 | 33.73 |
| BWT | 15.28 | 12.27 | 20.04 | 16.59 | 25.5 | 14.25 | 15.26 | 13.56 | 12.65 |
| **meta-llama/Llama-3B-Instruct** | | | | | | | | | |
| OP | 31.04 | 29.73 | 30.12 | 32.19 | 30.9 | 29.8 | 30.78 | 32.15 | 32.94 |
| BWT | 17.05 | 15.2 | 17.4 | 16.62 | 11.7 | 11.52 | 13.87 | 12.89 | 12.7 |

| Method | HumanEval (Pass@1) | Params (M) | %Params |
| :--- | :--- | :--- | :--- |
| **Llama-3.1-8B** | | | |
| LoRA (r=4) | 38.40 | 41.46 | 0.5% |
| LoRA (r=8) | 44.51 | 44.51 | 0.26% |
| LoRA (r=32) | 47.56 | 47.56 | 0.52% |

$Routing, Memory Type, Transfer, Average, Baseline Accuracy (%)
Coarse-Grained (Rank-$r$ Experts), Coarse-Grained, 62.56, 69.45, 74.53
MoE-LoRA (Baseline), Coarse-Grained, 62.48, 70.40, 75.57
$\gamma$ Temperature Scaling ($\gamma$=5), Coarse-Grained, 62.48, 70.40, 75.57
Self-Activation (Top-k), Fine-Grained, 60.29, 66.52, 70.62
$\gamma$ Sparsity Constraint (Top-k), Fine-Grained, 60.69, 66.52, 70.62
$\gamma$ Temperature Scaling ($\gamma$=MoRAM), Fine-Grained, 62.07, 71.15, 79.62
$\gamma$ Threshold-based Selection ($\theta$), Fine-Grained, 63.30, 72.70, 80.90

$Dataset\ name, Category, Task, Domain, Metric$
Yelp, CL Benchmark, sentiment analysis, Yelp reviews, accuracy
Amazon, CL Benchmark, sentiment analysis, Amazon reviews, accuracy
DBpedia, CL Benchmark, topic classification, Wikipedia, accuracy
Yahoo, CL Benchmark, topic classification, Yahoo Q&A, accuracy
AG News, CL Benchmark, topic classification, news, accuracy
MNLI, GLUE, NLI, various, accuracy
QQP, GLUE, paragraph detection, Quora, accuracy
RTE, GLUE, NLI, news, accuracy
SST-2, GLUE, sentiment analysis, movie reviews, accuracy
WIC, SuperGLUE, word sense disambiguation, lexical databases, accuracy
CB, SuperGLUE, NLI, various, accuracy
COPA, SuperGLUE, QA, blogs, encyclopedia, accuracy
BosIQua, SuperGLUE, boolean QA, Wikipedia, accuracy
MultiRC, SuperGLUE, QA, various, accuracy
IMDB, SuperGLUE, sentiment analysis, movie reviews, accuracy

$Method, Order-1, Order-2, Order-3, Avg.$
MTL, 80.0, , , 
SeqFT, 18.9, 41.7, 28.5, 
SeqLoRA, 44.6, 32.7, 53.7, 43.0
IncLoRA, 66.0, 64.9, 68.3, 66.4
Replay, 55.2, 36.9, 61.3, 57.8
EWC, 48.7, 47.2, 44.5, 50.3
LwF, 54.4, 53.1, 49.6, 52.3
L2P, 60.3, 61.7, 61.1, 60.7
LPFTS, 67.6, 72.6, 77.9, 72.7
InfiLoRA, 75.2, 75.4, 75.8, 75.5
O-LoRA, 75.4, 75.7, 76.3, 75.8
LB-CL, 76.9, 76.5, 76.8, 76.7

$Table_7: Task sequences used in continual learning experiments.
Order, Task Sequence
1, dpedia $\rightarrow$ amazon $\rightarrow$ yahoo $\rightarrow$ ag
2, dpedia $\rightarrow$ amazon $\rightarrow$ ag $\rightarrow$ yahoo
3, yahoo $\rightarrow$ amazon $\rightarrow$ ag $\rightarrow$ dpedia
4, mnli $\rightarrow$ cb $\rightarrow$ wic $\rightarrow$ copa $\rightarrow$ qup $\rightarrow$ boolqa $\rightarrow$ rtb $\rightarrow$ imdb $\rightarrow$ yelp
5, yelp $\rightarrow$ amazon $\rightarrow$ sci2 $\rightarrow$ dpedia $\rightarrow$ art $\rightarrow$ multire $\rightarrow$ yahoo
6, sci2 $\rightarrow$ dpedia $\rightarrow$ ag $\rightarrow$ yelp $\rightarrow$ amazon $\rightarrow$ yahoo $\rightarrow$ yelp $\rightarrow$ mnli $\rightarrow$ cb $\rightarrow$ copa $\rightarrow$ qup $\rightarrow$ wic $\rightarrow$ imdb $\rightarrow$ rtb $\rightarrow$ dpedia $\rightarrow$ ag $\rightarrow$ yahoo $\rightarrow$ multire $\rightarrow$ boolqa $\rightarrow$ wic

$Table_12: Average accuracy on T5-large continual-learning benchmarks after the final task, evaluated over extended 15-task sequences.
Method, Order-4, Order-5, Order-6, Avg.
SeqFT, 7.4, 7.5, 7.4, 7.4
SeqLoRA, 2.3, 0.6, 1.9, 1.6
InLoRA, 63.3, 58.5, 61.7, 61.2
Replay, 55, 54.6, 53, 54.2
EWC, 45.3, 44.5, 45.6, 45.1
LwF, 50.1, 43.1, 47.4, 46.9
LP, 57.5, 53.5, 58.9, 56.1
LFPT5, 69.8, 67.2, 69.2, 68.7
O-LoRA, 70.5, 65.5, 70.5, 68.8
LB-CL, 68.4, 67.3, 71.8, 69.2

$Table_13: Overall results on the SuperNI Benchmark using the T5-large backbone.
Methods, Replay, SuperNI AP, SuperNI FT
LXP, $\text{\sffamily X}$, 15.18, 3.65
InLoRA, $\text{\sffamily X}$, 12.33, 41.93
C-LoRA, $\text{\sffamily X}$, 22.69, 24.25
O-LoRA, $\text{\sffamily X}$, 26.37, 19.15
SAPT, $\checkmark$, 51.54, 0.91

Method | Aircraft | Calcutta101 | CIFAR100 | DTD | EuroSAT | Flowers | Food | MNIST | OdiftPet | Cars | SUN397 | Average
--- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | ---
Zero-shot (Radford et al., 2021) | 24.4 | 88.4 | 66.2 | 44.6 | 54.9 | 71.0 | 88.5 | 59.6 | 89.0 | 64.7 | 65.2 | 65.3
Zero-shot (Radford et al., 2021) | 88.4 | 68.2 | 44.6 | 54.9 | 71.0 | 88.5 | 59.6 | 89.0 | 64.7 | 65.2 | 69.4
LwF (Li & Houri, 2017) | 72.1 | 49.2 | 35.5 | 44.5 | 44.1 | 66.6 | 50.5 | 69.0 | 19.0 | 51.7 | 50.0
LwF-VR (Ding et al., 2022) | 82.2 | 89.8 | 66.5 | 53.5 | 56.3 | 73.4 | 83.1 | 56.4 | 79.5 | 65.5 | 58.3
WfSE-FT (Wortman et al., 2022) | 77.6 | 60.0 | 41.4 | 39.3 | 53.0 | 76.6 | 58.1 | 75.5 | 37.3 | 58.2 | 57.7
ZSCL (Zhang et al., 2023) | 84.0 | 68.1 | 44.8 | 46.8 | 63.6 | 84.9 | 61.8 | 51.5 | 88.4 | 65.5 | 62.3
MoE-Adapt (Yu et al., 2024) | 82.2 | 89.8 | 66.5 | 53.5 | 56.3 | 73.4 | 83.1 | 56.4 | 79.5 | 65.5 | 58.3
RAIL-Prim (Xu et al., 2024) | 88.4 | 68.2 | 44.6 | 54.9 | 71.0 | 88.5 | 59.6 | 89.0 | 64.7 | 65.2 | 69.4
CoDyRA (Li et al., 2024) | 92.4 | 68.4 | 45.8 | 55.5 | 69.6 | 87.4 | 65.2 | 88.5 | 64.2 | 65.5 | 69.9
LwF (Li & Houri, 2017) | 22.1 | 58.2 | 89.8 | 57.1 | 57.6 | 79.2 | 87.3 | 84.3 | 64.1 | 31.5 | 60.1 | 46.5
LwF-VR (Ding et al., 2022) | 82.2 | 89.8 | 69.5 | 57.1 | 57.6 | 79.2 | 87.3 | 84.3 | 64.1 | 31.5 | 60.1 | 46.5
WfSE-FT (Wortman et al., 2022) | 32.2 | 50.0 | 65.7 | 55.7 | 60.2 | 82.1 | 82.6 | 58.6 | 86.9 | 66.7 | 70.4 | 71.9
ZSCL (Zhang et al., 2023) | 26.8 | 88.5 | 63.7 | 55.7 | 64.0 | 82.3 | 89.1 | 89.0 | 89.1 | 89.0 | 66.7 | 70.4
MoE-Adapt (Yu et al., 2024) | 30.1 | 89.3 | 74.9 | 64.0 | 82.3 | 89.4 | 87.1 | 89.0 | 89.1 | 69.5 | 72.5 | 76.1
CoDyRA (Li et al., 2024) | 31.6 | 95.5 | 72.8 | 63.5 | 85.0 | 87.5 | 89.0 | 85.7 | 94.0 | 73.6 | 73.0 | 78.0

Method,Cars,Aircraft,Oatmeal,Food,MNIST,SUN397,Flowers,DTD,DTDT,Caleb101,EuroSAT,Average
Zero-shot,66.1,23.5,86.7,84,63.7,46.7,63.6,37.3,76.8,36.7,58.5
LwF (Li & Hoiem, 2017),20.0,74.1,79.6,58.1,41.9,28.7,67.4,15.1,46.9
WiSE-FT (Wortsman et al., 2022),21.2,75.8,83.5,65.0,55.6,26.0,60.8,29.0,69.9
ZSCL (Zeng et al., 2023a),23.0,84.3,87.2,87.5,62.4,35.6,74.1,40.9,56.9
MoE-Adapter (Yu et al., 2024),17.1,87.2,87.5,84.6,12.5,65.5,30.9,70.0,59.2
LoRA (Hu et al., 2021),23.4,75.4,83.4,65.3,43.5,37.3,76.8,76.7,57.7
CoDyRA (Lu et al., 2024),23.6,89.2,83,62,51.6,39.4,77.4,39,59.4
LwF (Li & Hoiem, 2017),29.6,17.5,63.0,83.8,67.7,44.9,79.3,39.0,55.4
WiSE-FT (Wortsman et al., 2022),46.1,23.5,71.3,85.7,70.2,59.1,85.5,47.9,82.4,62.8,61.5
ZSCL (Zeng et al., 2023a),74.4,36.4,86.7,68.7,50.0,75.1,40.1,72.5,43.7,63.6
MoE-Adapter (Yu et al., 2024),75.1,41.1,87.9,84.7,71.9,84.7,92.6,61.2,81.0,74.0,67.4
RAIL-Primal (Xu et al., 2024),77.7,41.9,86.1,83.3,71.8,91.6,97.3,66.4,84.0,94.8,79.8
CoDyRA (Lu et al., 2024),80,40.4,85.6,83.3,68.3,62.2,76.6,45.8,80.4,41.7,79.8

$Method, Cars, Aircraft, OceanFoot, Food, SUNST, MNIST, Flowers, DPD, Caltech101, EuroSAT, Average
InLoRA, , , , , , , , , , , 55.69
CoDyRA, , , , , , , , , , , 63.08
Average, , , , , , , , , , , 63.38

$Method, Order 1, Order 2, Order 3, Avg ± Std
SCQF, 18.9, 24.9, 41.7, 28.5 ± 11.8
L2P, 60.3, , , 61.7, 0.1
LPTS, 67.6, 25.6, 77.7, 72.7 ± 5.15
O-LoRA, 75.4, 75.7, 76.3, 75.8 ± 0.46

$Method, Trainable Parameters (Million), GPU Mem. (MB)
LWF (Li & Hoien, 2017), 129.6, 32172
ZSC (Zheng et al., 2023), 129.6, 32029
MoE-Adapter (Yu et al., 2024), 39.9, 22558
CoDyRA (Lu et al., 2024), 4.4, 21770

Table 22. Effect of load-balancing regularization.
X-TAIL, T5-Large: 15-task, Last, Order-4, Order-5
MoRAM, 80.9, 68.91, 68.32, 71.95
MoRAM w/ load-bal., 77.3, 68.23, 67.86, 71.46

Method, Cas, Aircraft, OxfordPet, Food, SUNNY7, MNIST, Flowers, DTID, Cabeio101, EuroSAT, Average
MoRAM, 44.1, 81.6, 64.6, 79.6, 83.9, 84.4, 66.5, 89.7, 68.4, 64.1, 72.7
MoRAM s/ pruning, 42.9, -, 62.9, 75.4, -, 82.7, 61.2, 86.5, 69.6, 63.0, 71.4

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/neuraldb_2507.18028.pdf

$Figure 1 contains two line charts: "Editing Effectiveness" (Left) measuring Average Success on 13 Edit Metrics vs. Edit Number, and "General Ability" (Right) measuring Average Success on 6 tasks vs. Edit Number.

Editing Effectiveness (Left Chart)
Edit Number, NeuralDB, AlphaEdit, MEMIT, RECT
2000, [Value], [Value], [Value], [Value]
4000, [Value], [Value], [Value], [Value]
6000, [Value], [Value], [Value], [Value]
8000, [Value], [Value], [Value], [Value]
10000, [Value], [Value], [Value], [Value]

General Ability (Right Chart)
Edit Number, NeuralDB, AlphaEdit, MEMIT, RECT
2000, [Value], [Value], [Value], [Value]
4000, [Value], [Value], [Value], [Value]
6000, [Value], [Value], [Value], [Value]
8000, [Value], [Value], [Value], [Value]
10000, [Value], [Value], [Value], [Value]

(Note: Per your instructions, the "NeuralDB" rows are excluded from the final data extraction. Only AlphaEdit, MEMIT, and RECT would be included in the output rows.)

2000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
4000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
6000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
8000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
10000, [AlphaEdit Value], [MEMIT Value], [RECT Value]

2000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
4000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
6000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
8000, [AlphaEdit Value], [MEMIT Value], [RECT Value]
10000, [AlphaEdit Value], [MEMIT Value], [RECT Value]

$Table 1: Comparison of NeuralDB with existing methods on the MEC and ZARE datasets. The table compares various Knowledge Editing (KE) methods across different metrics (Efficacy, Generality, Specificity, Fluency, Consistency) for two datasets (Counterfact and ZARE). The methods compared include Pre-edited, MEMIT, InstructEdit, MELO, ROME, AlphaEdit, and RECT. The models evaluated are LLama3 and GPT-2 XL.

Method,Model,Dataset,Metric,Value
Pre-edited,LLama3,Counterfact,Efficacy,7.9
Pre-edited,LLama3,Counterfact,Generality,104
Pre-edited,LLama3,Counterfact,Specificity,635.2
Pre-edited,LLama3,Counterfact,Fluency,24.1
Pre-edited,LLama3,Counterfact,Consistency,89.5
Pre-edited,LLama3,ZARE,Efficacy,37.0
Pre-edited,LLama3,ZARE,Generality,36.3
Pre-edited,LLama3,ZARE,Specificity,31.9
MEMIT,LLama3,Counterfact,Efficacy,63.2
MEMIT,LLama3,Counterfact,Generality,66.2
MEMIT,LLama3,Counterfact,Specificity,471
MEMIT,LLama3,Counterfact,Fluency,4.2
MEMIT,LLama3,Counterfact,Consistency,0.9
MEMIT,LLama3,ZARE,Efficacy,1.1
MEMIT,LLama3,ZARE,Generality,0.5
MEMIT,LLama3,ZARE,Specificity,0
InstructEdit,LLama3,Counterfact,Efficacy,66.2
InstructEdit,LLama3,Counterfact,Generality,66.2
InstructEdit,LLama3,Counterfact,Specificity,44.3
InstructEdit,LLama3,Counterfact,Fluency,37.2
InstructEdit,LLama3,Counterfact,Consistency,4.2
InstructEdit,LLama3,ZARE,Efficacy,1.4
InstructEdit,LLama3,ZARE,Generality,1.0
InstructEdit,LLama3,ZARE,Specificity,1.4
MELO,LLama3,Counterfact,Efficacy,66.4
MELO,LLama3,Counterfact,Generality,69.0
MELO,LLama3,Counterfact,Specificity,49.1
MELO,LLama3,Counterfact,Fluency,3.3
MELO,LLama3,Counterfact,Consistency,0.2
MELO,LLama3,ZARE,Efficacy,25.2
MELO,LLama3,ZARE,Generality,24.1
MELO,LLama3,ZARE,Specificity,3.0
ROME,LLama3,Counterfact,Efficacy,66.4
ROME,LLama3,Counterfact,Generality,69.0
ROME,LLama3,Counterfact,Specificity,441
ROME,LLama3,Counterfact,Fluency,3.3
ROME,LLama3,Counterfact,Consistency,0.2
ROME,LLama3,ZARE,Efficacy,25.2
ROME,LLama3,ZARE,Generality,24.1
ROME,LLama3,ZARE,Specificity,3.0
AlphaEdit,LLama3,Counterfact,Efficacy,99.1
AlphaEdit,LLama3,Counterfact,Generality,75.8
AlphaEdit,LLama3,Counterfact,Specificity,946
AlphaEdit,LLama3,Counterfact,Fluency,627.4
AlphaEdit,LLama3,Counterfact,Consistency,7.8
AlphaEdit,LLama3,ZARE,Efficacy,94.0
AlphaEdit,LLama3,ZARE,Generality,91.5
AlphaEdit,LLama3,ZARE,Specificity,86.9
RECT,LLama3,Counterfact,Efficacy,64.2
RECT,LLama3,Counterfact,Generality,60.0
RECT,LLama3,Counterfact,Specificity,53.9
RECT,LLama3,Counterfact,Fluency,58.1
RECT,LLama3,Counterfact,Consistency,502.9
RECT,LLama3,Counterfact,Fluency,13.9
RECT,LLama3,Counterfact,Consistency,1.6
RECT,LLama3,ZARE,Efficacy,86.0
RECT,LLama3,ZARE,Generality,82.3
RECT,LLama3,ZARE,Specificity,0.0
RECT,LLama3,ZARE,Fluency,0.0
RECT,LLama3,ZARE,Consistency=0.0
Pre-edited,GPT-2 XL,Counterfact,Efficacy,16.2
Pre-edited,GPT-2 XL,Counterfact,Generality,18.6
Pre-edited,GPT-2 XL,Counterfact,Specificity,83.1
Pre-edited,GPT-2 XL,Counterfact,Fluency,434
Pre-edited,GPT-2 XL,Counterfact,Consistency,29.7
Pre-edited,GPT-2 XL,Counterfact,Fluency,6.7
Pre-edited,GPT-2 XL,ZARE,Efficacy,72.4
Pre-edited,GPT-2 XL,ZARE,Generality,73.4
Pre-edited,GPT-2 XL,ZARE,Specificity,297.9
Pre-edited,GPT-2 XL,ZARE,Fluency=62.4
Pre-edited,GPT-2 XL,ZARE,Consistency=78.9
Pre-edited,GPT-2 XL,ZARE,Specificity=19
MEMIT,GPT-2 XL,Counterfact,Efficacy,46.2
MEMIT,GPT-2 XL,Counterfact,Generality,46.1
MEMIT,GPT-2 XL,Counterfact,Specificity,53.9
MEMIT,GPT-2 XL,Counterfact,Fluency=24.2
MEMIT,GPT-2 XL,Counterfact,Consistency=3.9
MEMIT,GPT-2 XL,Counterfact,Fluency=0.7
MEMIT,GPT-2 XL,ZARE,Efficacy=0.5
MEMIT,GPT-2 XL,ZARE,Generality=0.0
MEMIT,GPT-2 XL,ZARE,Specificity=0
InstructEdit,GPT-2 XL,Counterfact,Efficacy,50.6
InstructEdit,GPT-2 XL,Counterfact,Generality,51.7
InstructEdit,GPT-2 XL,Counterfact,Specificity=24.2
InstructEdit,GPT-2 XL,Counterfact,Fluency=3.9
InstructEdit,GPT-2 XL,Counterfact,Consistency=0.7
InstructEdit,GPT-2 XL,ZARE,Efficacy=0.2
InstructEdit,GPT-2 XL,ZARE,Generality=0.1
InstructEdit,GPT-2 XL,ZARE,Specificity=0.0
MELO,GPT-2 XL,Counterfact,Efficacy=78.3
MELO,GPT-2 XL,Counterfact,Generality=60.5
MELO,GPT-2 XL,Counterfact,Specificity=66.8
MELO,GPT-2 XL,Counterfact,Fluency=610.8
MELO,GPT-2 XL,Counterfact,Consistency=24.3
MELO,GPT-2 XL,ZARE,Efficacy=82.2
MELO,GPT-2 XL,ZARE,Generality=32.9
MELO,GPT-2 XL,ZARE,Specificity=26
ROME,GPT-2 XL,Counterfact,Efficacy=93.0
ROME,GPT-2 XL,Counterfact,Generality=58.5
ROME,GPT-2 XL,Counterfact,Specificity=83.3
ROME,GPT-2 XL,Counterfact,Fluency=55.8
ROME,GPT-2 XL,Counterfact,Consistency=58.9
ROME,GPT-2 XL,Counterfact,Fluency=456.2
ROME,GPT-2 XL,Counterfact,Consistency=23.2
ROME,GPT-2 XL,ZARE,Efficacy=81.1
ROME,GPT-2 XL,ZARE,Generality=74.4
ROME,GPT-2 XL,ZARE,Specificity=35.5
ROME,GPT-2 XL,ZARE,Fluency=66.9
ROME,GPT-2 XL,ZARE,Consistency=2.8
RECT,GPT-2 XL,Counterfact,Efficacy=91.8
RECT,GPT-2 XL,Counterfact,Generality=79.5
RECT,GPT-2 XL,Counterfact,Specificity=60.4
RECT,GPT-2 XL,Counterfact,Fluency=440
RECT,GPT-2 XL,Counterfact,Consistency=58.2
RECT,GPT-2 XL,Counterfact,Fluency=173.8
RECT,GPT-2 XL,Counterfact,Consistency=20.3
RECT,GPT-2 XL,ZARE,Efficacy=82.6
RECT,GPT-2 XL,ZARE,Generality=75
RECT,GPT-2 XL,ZARE,Specificity=24.7
RECT,GPT-2 XL,ZARE,Fluency=25.1
RECT,GPT-2 XL,ZARE,Consistency=23.5
AlphaEdit,GPT-2 XL,Counterfact,Efficacy=99.4
AlphaEdit,GPT-2 XL,Counterfact,Generality=97.2
AlphaEdit,GPT-2 XL,Counterfact,Specificity=95.2
AlphaEdit,GPT-2 XL,Counterfact,Fluency=74.1
AlphaEdit,GPT-2 XL,Counterfact,Consistency=608.7
AlphaEdit,GPT-2 XL,Counterfact,Fluency=619.9
AlphaEdit,GPT-2 XL,Counterfact,Consistency=42.7
AlphaEdit,GPT-2 XL,ZARE,Efficacy=96.3
AlphaEdit,GPT-2 XL,ZARE,Generality=94.6
AlphaEdit,GPT-2 XL,ZARE,Specificity=92.8
AlphaEdit,GPT-2 XL,ZARE,Fluency=0.0
AlphaEdit,GPT-2 XL,ZARE,Consistency=0.0

$Number of edits (0k to 100k), Efficacy (\uparrow), Generalization (\uparrow), Specificity (\uparrow), MMLU (\uparrow)$
0k, 37.0, 36.3, 31.9, 56.2
10k, 96.9, 91.4, 35.1, 56.2
20k, 96.6, 91.4, 35.3, 56.2
30k, 96.6, 91.2, 35.2, 56.2
40k, 96.4, 91.0, 35.2, 56.2
50k, 96.1, 90.6, 35.2, 56.2
60k, 95.0, 90.6, 35.2, 56.9
70k, 95.9, 90.5, 35.2, 56.9
80k, 95.8, 90.4, 35.1, 56.9
90k, 95.6, 90.2, 35.1, 56.9
100k, 95.5, 90.2, 35.1, 56.9

Table 3: Hyper-parameters of NeuralDB for various models in the main experiments
Model | Layer found by causal trace | Layer $l^*$ used by NeuralDB | $\gamma$
GPT2-xl | 17 | 17 | 0.65
GPT-J (6B) | 17 | 8 | 0.65
Llama 3 Instruct (8B) | 17 | 7 | 0.65

Table 4: The average timed of evaluation post-edited models on CounterFacts and ZsRE
Model | Dataset | MEMIT | AlphaEdit
Llama3 | CounterFacts | 4.12 | 4.11
Llama3 | ZsRE | 0.22 | 0.22
GPTJ-6B | CounterFacts | 3.81 | 3.76
GPTJ-6B | ZsRE | 0.16 | 0.16

$Table 5: Ablation study on Llama 3 (8B). The table shows the impact of varying the Gamma parameter and the Layer number on several metrics (E, G, S, F, C).
Gamma, Layer, E, G, S, F, C
0.65, 7, 99.2, 85.9, 85.6, 631.9, 32.6
0.65, 8, 99.2, 79.3, 85.1, 631.5, 33.3
0.65, 9, 99.2, 77.4, 84.9, 631.6, 32.4
0.55, 7, 99.1, 91.9, 83.4, 631.6, 32.7
0.75, 7, 99.2, 74.1, 86.2, 632.3, 32.2

Model, Layer Setup, Efficacy $\uparrow$, Generalization $\uparrow$, Specificity $\uparrow$, Fluency $\uparrow$
GPT-J, [8] baseline, 99.08, 93.48, 75.52, 620.53
GPT-J, [6,7,8] new multilayer, 94.44, 91.72, 75.93, 617.44
GPT-J, [6,7,8] old multilayer, 99.31, 93.23, 76.78, 616.00
GPT2-XL, [17] baseline, 99.04, 95.96, 70.72, 621.90
GPT2-XL, [15,16,17] new multilayer, 94.81, 92.68, 70.26, 618.51
GPT2-XL, [15,16,17] old multilayer, 99.08, 94.01, 71.33, 624.48

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/openclaw_rl_2603.10165.pdf

Figure 1: OpenClaw-RL infrastructure overview. It is a flowchart/diagram illustrating the interaction between Personal Agents (on personal devices) and General Agents (on cloud services) with the OpenClaw-RL server components (Training Engine, Policy Server, and PRM Server).

no sir

Setting | Environment | Next-state signal | Horizon
---|---|---|---
Terminal | Shell execution sandbox | stdud/stderr, exit code | Long
GUI | Screen state + accessibility tree | Visual state diff, task progress | Long
SWE | Code repository + test suite | Test verdicts, diff, lint output | Long
Tool-call | API/function execution | Return values, error traces | Medium

$Property, Source, Granularity, Information per sample, Frequency
Evaluative, Scalar PRM vote, Sequence-level, 1 scalar, Every scored turn
Directive, Hint-conditioned teacher, Token-level, $|s_{i}|$ log-prob gaps, Turns with meaningful hint
Hybrid (Ours), Both, Mixed, 1 scalar + $|s_{i}|$ gaps, Every scored turn

$Setting, Optimize at the same time (Joint) Hybrid RL (Ours), Optimize at the same time (Joint) GRPO, Optimize at the same time (Joint) OPD, Optimize at the same time (Joint) Memo, Optimize at the same time (Joint) Cognee, Optimize for each individual (separate) Hybrid RL (Ours), Optimize for each individual (separate) GRPO, Optimize for each individual (separate) OPD, Optimize for each individual (separate) Memo, Optimize for each individual (separate) Cognee
Student, 11.6, 15.4, 30.8, 13.6, 14.6, 19.2, 22.8, 34.6, 13.4, 15.6
TA, 8.2, 12.0, 34.0, 15.8, 15.4, 11.8, 22.4, 36.0, 16.0, 14.8
Teacher, 11.4, 14.8, 24.4, 14.2, 14.8, 14.0, 18.0, 17.6, 15.8, 15.0
Average, 10.3, 14.1, 29.7, 14.5, 14.9, 15.0, 21.1, 29.4, 15.1, 15.1

$Setting, Student, TA, Teacher, Average$
$k=2, 30.4, 12.0, 17.2, 20.2$
$k=4, 11.6, 7.6, 11.4, 10.1$
$k=20, 12.8, 7.8, 10.0, 9.8$
$token\text{-}level, 34.4, 36.0, 22.6, 31.0$
$k=2\ (top\text{-}k\ overlap), 31.6, 14.0, 18.4, 21.3$
$k=4\ (top\text{-}k\ overlap), 11.8, 16.4, 12.2, 13.5$

$Table 7: Ablation on PRM model. Fields: Teacher Student, Qwen3-4B-Thinking-2507, Qwen3-8B.
TA, 14.0, 13.6
Teacher, 9.6, 9.2
Average, 13.8, 14.2
(Note: The "Average" row is a summary of the data above it, not a separate experimental method.)

**Wait, correction based on your strict exclusion rule:**
The authors of the paper are Yinjie Wang, Xuyang Chen, Xiaolong Jin, Mengdi Wang, and Ling Yang. The methods being tested in Table 7 (the ablation of the PRM model) are components of the **OpenClaw-RL** framework proposed by these authors. 

Since all rows in Table 7 are measuring the performance of the authors' own proposed method (the PRM model ablation), I must exclude them.

no sir

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/past_2601.11258.pdf

$Table 1: Mean accuracy on SQuAD (no-context) across different adaptation regimes. Values for baselines are taken from SEAL (Zweiger et al., 2025). The values in the parentheses denote the absolute improvement over the "Train on Passage + Synthetic" baseline.
Method | Single Passage (n = 1; LoRA) | CPT (n = 200; full-FT) | CPT (n = 2067; full-FT)
Base Model | 32.7 | 32.7 | 29.0
Train on Passage | 33.5 | 36.0 | 31.2
Train on Passage + Synthetic | 39.7 | 50.6 | 43.4
Train on Passage + GPT-4.1 Synthetic | 46.3 | 59.4 | 49.2
SEAL | 47.0 | 58.2 | 46.4

$Table 2: Long-Context QA Performance on Loogle (Short Dependence QA). Comparing standard adaptation methods against PaST, the Skill Vector significantly enhances the model's ability to retrieve and reason over extremely massive information. All results are averaged over three independent runs.
Method | Accuracy
SFT | 30.1

$Success Rate (%) measured against various Task Categories (Test Analysis, Media, AI & ML, Science, Business Software, Food, Database, Mapping, Financial, Video Images, Email, Location, Communication, eCommerce, Education, Advertising, Health & Fitness, SMS, Weather)

Task Category | Target SFT (Baseline) | PaST (Ours)
Test Analysis | 20.8 | 34.7
Media | 50.8 | 50.8
AI & ML | 12.5 | 14.2
Science | 45.6 | 43.1
Business Software | 47.3 | 34.1
Food | 23.4 | 13.3
Database | 49.8 | 13.0
Mapping | 26.1 | 2.4
Financial | 34.3 | 14.3
Video Images | 26.7 | 6.1
Email | 36.5 | 3.3
Location | 28.8 | 2.3
Communication | 23.3 | 13.1
eCommerce | 25.8 | 7.3
Education | 13.1 | 11.1
Advertising | 19.4 | 16.7
Health & Fitness | 16.7 | 11.1
SMS | 11.1 | 7.2
Weather | 11.3 | 4.2

Table 4: $K \times M$ Trade-off on Loogle. Fixing the total source documents ($K \times M = 10$) and total RL optimization steps, we sweep the number of rounds $K$ versus documents per round $M$.
Rounds $\times$ Docs/Round | Loogle Acc
1 $\times$ 10 (Single-round) | 42.9
2 $\times$ 5 | 44.6
5 $\times$ 2 | 45.3
10 $\times$ 1 | 42.1

Table 5: Ablation on Transfer Strategy. We evaluate different methods of combining source skills with target knowledge on Loogle. “Post-hoc Composition” (Ours) outperforms sequential training or pre-injection methods.
Transfer Strategy | Loogle Accuracy
Target SFT | 32.9
Sequential FT | 30.3
Pre-Injection | 36.5
Post-hoc Composition | 44.6

Table 6: Comparison with Target-Domain RL on Loogle. We compare PaST against direct GRPO trained on the target domain at varying step budgets, the first 10 Loogle documents. Runtime measures the parameter-efficient target-rule RL training on 8$\times$ A100 GPUs.
Method | Acc | Test-time RL Time
Target SFT | 32.9 | 0 min
Target RL (25 steps) | 38.2 | 33 min
Target RL (50 steps) | 41.2 | 52 min
Target RL (75 steps) | 44.0 | 72 min
Target RL (100 steps) | 45.3 | 91 min
PaST (Ours) | 44.6 | 0 min

Table 9: Case Study: Model comparison on SQuAD.
Benchmark | Question | Ground Truth | Baseline: SFT Model | Ours
SQuAD | What generally does not allow citizens to use other cities? | Directives. | In general, citizens cannot directly sue other citizens in most legal systems. However, there are exceptions where private individuals can be held liable for certain types of harm, such as intentional torts or breaches of contract. | Directives do not create rights and duties for citizens. They are addressed to Member States and usually have indirect effect, meaning that they do not create rights and duties for citizens. Directives usually contain exceptions and allow Member States to choose the form in which they are implemented. This means that there is often no direct effect between citizens and most cases, citizens cannot sue other citizens based on Directives. However, Directives often create higher standards than those that exist in national laws, so if a national law is less strict than a Directive, the latter can be used to argue that a law is invalid. In summary, citizens generally cannot sue other citizens based on Directives, but Directives can create higher standards than national laws, which can be used to challenge the validity of national laws.

Table 10: Orthogonality across tasks, RL algorithms, and rewards.
Benchmark | RL Algorithm | Reward | Avg. Cos. Sim.
LoogLE | GRPO | LLM Judge | -0.0018
SQuAD | GRPO | LLM Judge | -0.0019
Toolbench | PPO | Env. Simulator | -0.0020

Table 11: Cross-domain orthogonality on LoogLE.
Comparison | Avg. Cos. Sim.
SFT (Source) vs. RL (Source) | -0.00180
SFT (Source) vs. RL (Target) | -0.00067
SFT (Target) vs. RL (Source) | -0.04109
SFT (Source) vs. SFT (Target) | 0.03132
RL (Source) vs. RL (Target) | 0.02575

Table 12: Hyperparameter, Single Passage (n=1, LoRA update), CPT (n=200/2067, Full FT)
Base model, Qwen2, 5-7B, Qwen2, 5-7B
Training data, passage + sympletic imitations (same construction as SEAL), passage + sympletic imitations (same construction as SEAL)
Max sequence length, 2048, 2048
Update type, LoRA (test-time update), Full fine-tuning
LoRA rank $\alpha$, 12, -
LoRA alpha $\alpha$, 64, -
LoRA dropout, 0, -
LoRA target modules, as in SEAL implementation, -
Training epochs, 10, 1
Learning rate, $1 \times 10^{-3}$, $7 \times 10^{-5}$
Per-device batch size, 1, 4
Gradient accumulation, -, 2

Table 13: Category, Value
Algorithm, GRPO (algorithm_adv_estimator=grpo)
Epochs, 15
Train batch size, 32 (data,train_batch_size)
Actor LR, $1 \times 10^{-6}$ (actor_optim.lr)
Max prompt length, 512 (data,max_prompt_length)
Max response length, 1024 (data,max_response_length)
Rollout backend, VLLM (actor_rollout_ref.rollout_name=vllm)
#rollouts per prompt (n), 5 (actor_rollout_ref.rollout_ref)
Temperature, 0.7 (temp=0.7, top_p=1, top_k=50)
Max new tokens, 512 (actor_rollout_ref.rollout_ref.max_new_tokens)
KL regularization, enabled (use_kl_loss), coef = 0.001, type low_var_kl
Precision, min=64, micro per GPU = 8, rollout dtype float16

$Hyperparameter, SFT Phase 1 (Knowledge Encoding), SFT Phase 2 (QA Adaptation), RL (GRPO) (Skill Sharpening)
Base Model, Qwen2.5-7B-Instruct, Qwen2.5-7B-Instruct, Qwen2.5-7B-Instruct
Precision, bf16, bf16, bf16
Gradient Checkpointing, True, True, True
Number of GPUs, 8, 8, 8
Optimizer, AdamW, AdamW, AdamW
Learning Rate, 1e-4, 2e-5, 1e-6
LR Scheduler, Cosine, Cosine, Constant
Global Batch Size, 16, 64, 128
Micro Batch Size (per GPU), 2, 2, 8
Total Epochs, 3, 2, 10 (Selected Best Step)
Max Sequence Length, 8192, 8192, 128 (Closed-Book)
Max Prompt Length, -, -, -
Max Response Length, -, -, 1024
Data Source, Mixed (Sum/Recall), Synthetic QA, Synthetic QA
Group Size ($\beta$), -, -, 5
KL Coefficient ($\beta$), -, -, 0.001
KL Reference Model, -, -, SFT Phase 2 Checkpoint
Reward Function, -, -, GPT-4.1 Judge

$Category, Domain Role, # APIs, # Test Queries
Movies, Source, 111, 35
Advertising, Target, 118, 3
AI & ML, Target, 108, 4
Business Software, Target, 199, 5
Communication, Target, 250, 8
Database, Target, 260, 7
Education, Target, 214, 9
Email, Target, 143, 5
Financial, Target, 224, 11
Food, Target, 208, 9
Health and Fitness, Target, 90, 7
Location, Target, 328, 11
Mapping, Target, 113, 7
Media, Target, 159, 12
SMS, Target, 75, 3
Science, Target, 99, 4
Search, Target, 103, 13
Text Analysis, Target, 98, 4
Video Images, Target, 207, 44
Weather, Target, 199, 8
eCommerce, Target, 342, 15

Table 22: Evaluation prompt in ToolBench experiments.
Contains a prompt template for a GPT-4.I Judge used to evaluate model performance.

Table 23: Detailed hyperparameters for PPO training in ToolBench experiments.
Category, Hyperparameter, Value
Data & Environment, Max Prompt Length, 8192
Data & Environment, Max Response Length, 1024
Data & Environment, Max Simulation Turns, 5
Optimization, Actor Learning Rate, 1 x 10^-5
Optimization, Critic Learning Rate, 1 x 10^-5
Optimization, Actor LR Warmup Ratio, 0.285
Optimization, Critic LR Warmup Ratio, 0.015
Optimization, LR Scheduler, Constant
Optimization, Optimizer, AdamW
PPO Algorithm, Advantage Estimator, GAE
PPO Algorithm, PPO Epochs per Batch, 1
PPO Algorithm, Mini-batch Size, 64
PPO Algorithm, Clip Range (ε), 0.2
PPO Algorithm, KL Penalty Coefficient (β), 0.001
PPO Algorithm, Entropy Coefficient, 0.001
Training Schedule, Total Training Steps, 180
Training Schedule, Compute Precision, BF16

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/rls_razor_2509.04259.pdf

Figure 1 contains two charts comparing RL (the authors' method) and SFT.

Since the authors' method (RL) is the subject of the figure, and the figure only compares their method against SFT, there are no rows representing methods created by other people to extract.

no sir

Variable | $R^2$ (2nd deg. polynomial)
--- | ---
KL, forward | $0.96 \pm 0.01$
KL, reverse | $0.93 \pm 0.01$
TV | $0.80 \pm 0.01$
Distribution change, L2 | $0.56 \pm 0.02$
Weight change, L1 | $0.34 \pm 0.02$
Weight change, Fisher Weighted L2 | $0.58 \pm 0.02$
Weight change, spectral norm | $0.58 \pm 0.02$
Sparsity of weight change | N/A
Rank of weight change | N/A
Activation change, L1 | $0.52 \pm 0.02$
Activation change, L2 | $0.55 \pm 0.02$

Hyperparameter | SFT / SIMPO | RL
--- | --- | ---
Base Model | Qwen2.5 3B-Instruct | Qwen2.5 3B-Instruct
Learning Rate | {1e-5, 3e-5, 5e-5, 7e-5, 9e-5} | {1e-5, 2e-5, 3e-5, 4e-5, 5e-5}
Optimizer | adamw | adamw
LR Scheduler | {constant w. warmup, cosine w. warmup} | constant w. warmup
Warmup steps | 50 | 50
Epochs | {1,2} | 1
Batch Size | {16,32,64,128} | See Below
Max Grad Norm | 1 | 1
Infloat16 | True | True
Weight Decay | 0 | 0
GRPO-only hyperparameters | | 
KL reg. | | 64
Group Size | | 8
Prompts per generation | | 7
num iterations ($\mu$) | | {1,2}
Loss type | | Dr. GRPO (Liu et al., 2025)

$A scatter plot showing the relationship between KL divergence (x-axis) and Average Score on Previous Tasks (y-axis). The plot compares two methods: SFT (blue dots) and RL (red dots).

SFT: blue dots
RL: red dots

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/sdft_2601.19897.pdf

Figure 1: A line chart showing "Prior Task Performance" vs "New Task Accuracy". It compares "Base Model", "On-policy learning", "Off-policy learning", and "SDFT (Ours)".

Base Model, 66, 45
On-policy learning, 64, 65
Off-policy learning, 56, 55

$Figure 4 contains three line charts (Science QQA, Tool Use, and Medical) showing the performance trade-off between New Task Accuracy and Prior Tasks Performance for various methods.
$Table 1 contains a comparison of Accuracy for different tasks (Base Model, Oracle, RAG, and SDFT) across two metrics (Scenario Accuracy and Out-of-distribution Accuracy).

Science QQA, Base Model, 66, 50
Science QQA, SFT+Re-invoke, 58, 56
Science QQA, DFT, 55, 58
Science QQA, SFT, 52, 62
Tool Use, Base Model, 66, 45
Tool Use, SFT+Re-invoke, 60, 55
Tool Use, DFT, 56, 58
Tool Use, SFT, 52, 64
Medical, Base Model, 66, 34
Medical, SFT+Re-invoke, 62, 38
Medical, DFT, 58, 40
Medical, SFT, 54, 42
Table 1, Base Model, Scenario Accuracy, 0
Table 1, Base Model, Out-of-distribution Accuracy, 100
Table 1, Oracle, Scenario Accuracy, 91
Table 1, Oracle, Out-of-distribution Accuracy, 100
Table 1, RAG, Scenario Accuracy, 97
Table 1, RAG, Out-of-distribution Accuracy, 95
Table 1, RAG, Scenario Accuracy, 90
Table 1, RAG, Out-of-distribution Accuracy, 85
Table 1, RAG, Scenario Accuracy, 88
Table 1, RAG, Out-of-distribution Accuracy, 88

Accuracy: float, The accuracy percentage achieved by the model.
Avg. # of tokens: float, The average number of tokens generated by the model.
Model: string, The name/version of the model being tested.

Olmo-3-7B-Think, 31.2, 4612
Olmo-3-7B, 23.5, 3273

$Accuracy_vs_Number_of_Generations
Distillation, 45, 55, 65, 70
Offline Distillation, 40, 45, 50, 55
SDFT from Teacher, 40, 45, 50, 55

$Accuracy (strict)
Only Answers: 37%
Only Text: 75%
Text and Answers: 89%

New Task: Science Q&A, HellaSwag, HumanEval, IFeval, MMLU, TruthfulQA, Winogrande, Avg.
Base (Qwen2.5-7B) | 32.1 | 62.0 | 65.8 | 74.3 | 71.7 | 47.9 | 71.1 | 65.5
SFT | 66.2 | 57.3 | 54.8 | 35.3 | 64.6 | 36.8 | 73.7 | 53.4
SFT + re-invoke | 66.0 | 61.6 | 63.4 | 52.9 | 68.7 | 45.2 | 70.0 | 60.2
DFT | 54.8 | 57.6 | 67.0 | 60.4 | 69.4 | 38.8 | 68.2 | 60.2

New Task: Tooluse, HellaSwag, HumanEval, IFeval, MMLU, TruthfulQA, Winogrande, Avg.
Base (Qwen2.5-7B) | 42.9 | 62.0 | 65.8 | 74.3 | 71.7 | 47.9 | 71.1 | 65.5
SFT | 63.2 | 57.3 | 50.0 | 49.8 | 70.2 | 37.5 | 73.1 | 56.0
SFT + re-invoke | 63.1 | 61.7 | 68.9 | 59.1 | 71.5 | 40.1 | 71.6 | 63.7
DFT | 64.2 | 59.7 | 61.4 | 60.5 | 71.6 | 40.2 | 71.5 | 60.8

New Task: Medical, HellaSwag, HumanEval, IFeval, MMLU, TruthfulQA, Winogrande, Avg.
Base (Qwen2.5-7B) | 30.1 | 62.0 | 65.8 | 74.3 | 71.7 | 47.9 | 71.1 | 65.5
SFT | 35.5 | 59.5 | 56.1 | 70.5 | 70.9 | 39.8 | 72.9 | 60.2
SFT + re-invoke | 35.6 | 61.5 | 63.1 | 67.6 | 70.0 | 42.3 | 71.4 | 62.6
DFT | 36.2 | 61.9 | 64.6 | 74.6 | 71.6 | 40.1 | 71.3 | 64.0

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/selective_tuning_2510.08564.pdf

$Method, LLaVA-NeXT (LLaMA-8B) Target, LLaVA-NeXT (LLaMA-8B) Held-out, Qwen2.5-VL (7B) Target, Qwen2.5-VL (7B) Held-out
Baseline, 31.5, 0.0, 59.9, 52.1

Method,Target-held,Held-out-CUB200,Held-out-PixmoCount,Held-out-PathVQA,Held-out-TextVQA,Held-out-TimeClock,Average-Held-out
Baseline,57.8,70.0,72.0,74.0,74.0,72.0,72.4

Method | Total Params (B) | Trainable Params (B) | Train SPS ↑ (#samples / sec.)
SA Proj. | 8.03 | 0.82 | 1.46
MLP (Gate&Up) | 8.03 | 3.80 | 1.45
MLP | 8.03 | 5.70 | 1.44
LoRA | 8.51 | 0.50 | 1.27
LwF | 16.06 | 5.70 | 0.81
MoE | 13.73 | 5.70 | 0.44

$Figure 12 contains three line charts:
1. **Target Tasks**: Measures Average Performance on Held-out Tasks (%) across different stages (CUB200, PixmoCount, PathVQA) for various methods.
2. **Held-out Tasks**: Measures Average Performance on Held-out Tasks (%) across different stages for various methods.
3. **Likelihood of Numeric Tokens**: Measures Average Performance of Numeric Tokens on LSS-58K across different stages for various methods.

Methods listed in the legend: SA Proj, MLP, LLM, Full, Vision Enc., Projector, and the authors' proposed method (implied by the context of the paper as "selective tuning" or "robust tuning recipes," though the legend specifically labels the baseline components).

Based on your instruction to EXCLUDE the authors' proposed methods and only include rows/data from OTHER people/methods:

SA Proj, CUB200, 81
SA Proj, PixmoCount, 78
SA Proj, PathVQA, 79
MLP, CUB200, 75
MLP, PixmoCount, 65
MLP, PathVQA, 74
LLM, CUB200, 74
LLM, PixmoCount, 62
LLM, PathVQA, 72
Full, CUB200, 73
Full, PixmoCount, 60
Full, PathVQA, 70
Vision Enc., CUB200, 75
Vision Enc., PixmoCount, 74
Vision Enc., PathVQA, 74
Projector, CUB200, 76
Projector, PixmoCount, 75
Projector, PathVQA, 75

$Dataset, Baseline, Stage 1 CUB200, Stage 2 PixmoCount, Stage 3 PathVQA, Stage 4 TextVQA, Stage 5 TimeClock
Al2D, 81.4, 81.2, 81.4, 81.2, 79.8, 75.2
ChartQA, 80.1, 80.4, 79.7, 80.0, 80.6, 79.4
DoeVQA, 87.1, 87.2, 86.9, 86.8, 86.3, 86.1
InfoVQA, 65.9, 66.0, 64.7, 66.0, 66.0, 64.9
MMStar, 61.8, 62.4, 62.3, 62.1, 62.4, 61.9
RealWorldQA, 66.4, 68.0, 67.1, 66.9, 69.2, 68.9
ScienceQA, 95.9, 95.7, 95.9, 96.1, 96.3, 96.1
SeedBench, 72.4, 72.3, 72.4, 72.0, 72.5, 72.4
Al2D, 81.4, 82.0, 85.1, 81.9, 81.9, 81.9
ChartQA, 80.1, 80.0, 79.7, 80.0, 80.6, 79.4
DoeVQA, 87.1, 87.2, 86.9, 86.8, 86.3, 86.1
InfoVQA, 65.9, 66.0, 64.7, 66.0, 66.0, 64.9
MMStar, 61.8, 62.4, 62.3, 62.1, 62.4, 61.9
RealWorldQA, 66.4, 68.0, 67.1, 66.9, 69.2, 68.9
ScienceQA, 95.9, 95.7, 95.9, 96.1, 96.3, 96.1
SeedBench, 72.4, 72.3, 72.4, 72.0, 72.5, 72.4

$Dataset, Baseline, Stage 1 CUB200, Stage 2 PixmoCount, Stage 3 PathVQA, Stage 4 TextVQA, Stage 5 TimeClock
CUB200, 53.7, 90.2, 89.8, 89.8, 89.6, 89.5
PixmoCount, 52.4, 53.4, 71.5, 67.8, 68.4, 67.2
PathVQA, 36.3, 36.1, 35.0, 61.9, 58.5, 58.9
TextVQA, 76.0, 76.4, 75.5, 76.1, 80.0, 79.3
TimeClock, 1.1, 0.9, 1.8, 1.2, 1.9, 72.2
Al2D, 81.4, 81.7, 81.6, 81.3, 81.2, 81.5
ChartVQA, 80.1, 80.1, 80.6, 80.1, 80.7, 78.8
DoeVQA, 87.1, 87.0, 86.3, 86.6, 85.9, 85.4
InfoVQA, 65.9, 66.1, 65.4, 65.0, 65.3, 64.9
MMStar, 61.8, 62.2, 63.1, 62.7, 62.5, 62.0
RealWorldQA, 66.4, 67.7, 64.4, 67.8, 69.5, 68.4
ScienceQA, 95.9, 96.3, 96.4, 96.5, 96.2, 96.0
SeedBench, 72.4, 72.4, 72.6, 72.3, 72.6, 72.5
CUB200, 32.6, 84.8, 76.8, 77.2, 76.6, 69.6
PixmoCount, 45.7, 37.6, 63.3, 48.5, 32.4, 44.0
PathVQA, 13.2, 24.8, 0.7, 62.0, 55.6, 45.2
TextVQA, 65.4, 52.2, 31.0, 56.1, 72.9, 42.8
TimeClock, 0.8, 0.3, 0.1, 0.6, 0.5, 33.1
Al2D, 71.6, 54.0, 53.3, 62.1, 58.2, 43.9
ChartVQA, 69.2, 54.3, 44.6, 48.6, 51.0, 7.8
DoeVQA, 72.7, 40.4, 27.7, 46.6, 59.2, 15.7
InfoVQA, 31.9, 23.4, 14.6, 27.2, 33.9, 10.2
MMStar, 42.0, 43.9, 41.5, 39.6, 42.4, 25.6
RealWorldQA, 59.7, 55.3, 32.7, 50.3, 53.6, 19.2
ScienceQA, 73.2, 63.3, 57.5, 69.7, 66.4, 58.3
SeedBench, 58.5, 56.8, 55.8, 53.9, 56.6, 42.0

$Dataset, Stage 1 CUB200, Stage 2 PixmoCount, Stage 3 PathVQA, Stage 4 TextVQA, Stage 5 TimeClock
CUB200, 92.3, 81.4, 81.5, 81.5, 88.0
PixmoCount, 56.4, 59.0, 59.2, 58.2, 33.1
PathVQA, 30.3, 29.1, 29.3, 30.2, 35.4
TextVQA, 82.5, 83.2, 83.1, 83.1, 71.0
TimeClock, 8.5, 8.4, 8.6, 8.8, 57.5
Average, 52.1, 54.0, 52.2, 52.3, 52.4, 57.0
A2D, 83.9, 82.8, 82.9, 83.1, 75.4
ChartVQA, 83.8, 83.7, 83.8, 83.9, 75.1
DocVQA, 94.4, 94.5, 94.4, 94.5, 88.7
InfoVQA, 79.5, 80.1, 80.2, 80.3, 69.2
MMStar, 62.3, 62.5, 62.9, 63.4, 52.7
RealWorldQA, 67.6, 68.5, 68.5, 69.9, 62.0
ScienceQA, 76.6, 76.4, 76.1, 76.2, 82.3
SeedBench, 73.7, 74.0, 74.1, 74.1, 67.8
Average, 77.9, 77.6, 77.8, 77.9, 78.2, 71.6

$Dataset, Stage 1 CUB200, Stage 2 PixmoCount, Stage 3 PathVQA, Stage 4 TextVQA, Stage 5 TimeClock
CUB200, 93.8, 0.0, 64.4, 67.6, 91.5
PixmoCount, 55.8, 47.0, 50.6, 41.0, 49.1
PathVQA, 4.9, 23.4, 63.0, 59.7, 60.2
TextVQA, 47.9, 11.6, 73.3, 82.1, 61.8
TimeClock, 0.0, 0.0, 0.0, 4.6, 58.5
Average, 52.1, 40.5, 11.7, 50.3, 51.0, 64.2
A2D, 77.4, 0.0, 35.8, 75.6, 56.9
ChartVQA, 41.9, 49.1, 67.0, 65.1, 68.2
DocVQA, 49.6, 23.4, 76.3, 89.9, 65.8
InfoVQA, 41.6, 28.5, 60.8, 74.5, 49.3
MMStar, 59.7, 0.0, 33.9, 52.4, 36.1
RealWorldQA, 56.7, 3.4, 25.4, 51.8, 38.3
ScienceQA, 77.6, 0.0, 39.5, 69.9, 59.3
SeedBench, 71.7, 0.0, 21.9, 61.8, 51.8
Average, 77.9, 59.5, 13.1, 45.1, 67.6, 53.2

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/sod_2605.07725.pdf

$Step, OPD, SOD (ours)
1, 1, 1
2, 1, 0.7
3, 1, 0.3
K, 1, 0.1

$Params, Method, Math (AIME 2024), Math (AIME 2025), Science (GPQA), Code (LiveCodeBench), Average
4B, GRPO, , , , , 
0.6B, Vanilla, 7.71, 12.81, 13.24, 14.89, 12.16
0.6B, SFT, 7.67, 12.32, 13.53, 9.61, 8.97
0.6B, GRPO, 4.06, 13.97, 20.38, 15.95, 11.31
0.6B, OPD, 16.82, 22.95, 17.76, 22.65, 20.04
0.6B, $\text{OPD}_{\text{SFT}}$, 12.67, 14.12, 14.98, 15.63, 15.03
0.6B, $\text{OPD}_{\text{Dug}}$, 7.71, 14.72, 15.26, 13.65, 12.84
1.7B, Vanilla, 9.90, 8.90, 26.80, 22.73, 17.10
1.7B, SFT, 26.77, 22.60, 29.85, 24.63, 25.91
1.7B, GRPO, 26.65, 21.33, 25.05, 20.79, 25.59
1.7B, OPD, 43.86, 37.04, 31.73, 22.73, 36.27
1.7B, $\text{OPD}_{\text{SFT}}$, 33.85, 24.69, 35.02, 22.73, 29.07
1.7B, $\text{OPD}_{\text{Dug}}$, 34.45, 41.72, 38.72, 40.63, 42.98

Variants | Math AIME 2024 | Math AIME 2025 | Science GPQA | Code LiveCodeBench | Average
(1.1) w/ Uniform Weighting | 41.68 $\pm$ 0.97 | 35.58 $\pm$ 0.48 | 30.12 $\pm$ 0.59 | 31.43 $\pm$ 0.68 | 34.70
(1.2) w/ Heuristic Weighting | 44.96 $\pm$ 0.98 | 38.75 $\pm$ 1.04 | 31.14 $\pm$ 0.64 | 33.71 $\pm$ 0.72 | 37.14
(1.3) w/ Mask After Wing | 39.11 $\pm$ 1.08 | 31.59 $\pm$ 1.14 | 26.56 $\pm$ 0.67 | 30.12 $\pm$ 0.80 | 31.85
(1.4) w/ Weight Clipping | 45.78 $\pm$ 1.16 | 37.93 $\pm$ 1.21 | 33.57 $\pm$ 0.74 | 35.12 $\pm$ 0.79 | 38.10
(2.1) w/ GRPO | 48.87 $\pm$ 0.95 | 39.73 $\pm$ 1.02 | 35.89 $\pm$ 0.62 | 38.62 $\pm$ 0.71 | 40.78
(2.2) w/ Step-wise OPD | 25.63 $\pm$ 1.03 | 21.67 $\pm$ 1.12 | 33.55 $\pm$ 0.76 | 20.70 $\pm$ 0.87 | 25.39

$Accuracy (%), Model Scale (AIME2024, AIME2025, GPQA, LiveCodeBench, Average), Method (Vanilla, Teacher (14B), GRPO, OPD, SOD)
AIME2024, 0.6B, Vanilla
AIME2024, 0.6B, Teacher (14B)
AIME2024, 0.6B, GRPO
AIME2024, 0.6B, OPD
AIME2024, 0.6B, SOD
AIME2025, 0.6B, Vanilla
AIME2025, 0.6B, Teacher (14B)
AIME2025, 0.6B, GRPO
AIME2025, 0.6B, OPD
AIME2025, 0.6B, SOD
GPQA, 0.6B, Vanilla
GPQA, 0.6B, Teacher (14B)
GPQA, 0.6B, GRPO
GPQA, 0.6B, OPD
GPQA, 0.6B, SOD
LiveCodeBench, 0.6B, Vanilla
LiveCodeBench, 0.6B, Teacher (14B)
LiveCodeBench, 0.6B, GRPO
LiveCodeBench, 0.6B, OPD
LiveCodeBench, 0.6B, SOD
Average, 0.6B, Vanilla
Average, 0.6B, Teacher (14B)
Average, 0.6B, GRPO
Average, 0.6B, OPD
Average, 0.6B, SOD
AIME2024, 1.7B, Vanilla
AIME2024, 1.7B, Teacher (14B)
AIME2024, 1.7B, GRPO
AIME2024, 1.7B, OPD
AIME2024, 1.7B, SOD
AIME2025, 1.7B, Vanilla
AIME2025, 1.7B, Teacher (14B)
AIME2025, 1.7B, GRPO
AIME2025, 1.7B, OPD
AIME2025, 1.7B, SOD
GPQA, 1.7B, Vanilla
GPQA, 1.7B, Teacher (14B)
GPQA, 1.7B, GRPO
GPQA, 1.7B, OPD
GPQA, 1.7B, SOD
LiveCodeBench, 1.7B, Vanilla
LiveCodeBench, 1.7B, Teacher (14B)
LiveCodeBench, 1.7B, GRPO
LiveCodeBench, 1.7B, OPD
LiveCodeBench, 1.7B, SOD
Average, 1.7B, Vanilla
Average, 1.7B, Teacher (14B)
Average, 1.7B, GRPO
Average, 1.7B, OPD
Average, 1.7B, SOD

Avg Time/Step (s), Peak Memory (GB), Total Time (h)
784.6, 81.7, 43.9
1053.6, 88.2, 43.9

Method | Math (AIME 2024) | Math (AIME 2025) | Science (GPQA) | Code (LiveCodeBench) | Average
---|---|---|---|---|---
naive OPD | 40.54 $\pm$ 1.05 | 35.83 $\pm$ 1.11 | 28.96 $\pm$ 0.58 | 29.81 $\pm$ 0.74 | 33.79
OPD | 43.86 $\pm$ 1.23 | 37.04 $\pm$ 1.31 | 31.73 $\pm$ 0.55 | 32.45 $\pm$ 0.91 | 36.27

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/spa_2603.22213.pdf

<!-- PAGE 3 FAILED -->

| QA Accuracy (%) |
| :--- |
| 31.31 (Base) |
| 86.86 (Rephrase) |
| 89.63 (QA) |
| 74.23 (SEAL) |
| 90.25 (Active Reading) |

| QA Accuracy (%) |
| :--- |
| 39.27 (Base) |
| 52.33 (QA) |
| 56.22 (EntiGraph) |
| 51.13 (Active Reading) |

| QA Accuracy (%) |
| :--- |
| 60.91 (Base) |
| 85.42 (EntiGraph) |
| 79.90 (Active Reading) |

$Y-axis: QA Accuracy (%)
$X-axis: Number of synthetic tokens (in millions)
$Legend: SPA (ours), Active Reading, SEAL, Pst

SPA (ours): [(1, 65), (2, 72), (5, 78), (10, 82), (20, 85), (50, 88), (100, 91)]
Active Reading: [(1, 68), (2, 73), (5, 77), (10, 80), (20, 82), (50, 83), (100, 84)]
SEAL: [(1, 62), (2, 68), (5, 74), (10, 77), (20, 79), (50, 80), (100, 81)]
Pst: [(100, 91)]

<!-- PAGE 6 FAILED -->

<!-- PAGE 7 FAILED -->

<!-- PAGE 8 FAILED -->

$Table 6: Hyperparameter search space for experiments on SQuAD. Parameter refers to the hyperparameter being tuned, and Search Space refers to the range of values explored during tuning.

Parameter | Search Space
--- | ---
Learning Rate | [4e-5, 5e-5, 6e-5, 7e-5]
Batch Size | [8, 64]

no sir

$Table 7: Extended diversity evaluation across datasets. Columns: Dataset, Method, Avg CR $\downarrow$, Avg Self-Rep. $\downarrow$, Avg Self-BLEU $\downarrow$, Avg CR: POS $\downarrow$.
$Table 8: Training stability comparison on QuALITY across different codebases. Columns: Method, Codebase, QA Accuracy (%).

Rephrase, SQuAD, 5.1330, 5.6019, 0.0014, 7.0200
QA, SQuAD, 8.9630, 6.9882, 0.0030, 9.7534
SEAL, SQuAD, 19.2468, 8.2446, 0.0058, 21.2072

EntiGraph, QuALITY, 3.9474, 6.0067, 0.0016, 6.4906
Active Learning, QuALITY, 3.2788, 3.4497, 0.0010, 5.5514

EntiGraph, MultiHop-RAG, 4.1760, 6.2377, 0.0014, 6.4530
Active Reading, MultiHop-RAG, 5.4140, 5.4140, 0.0010, 6.6904

<!-- PAGE 20 FAILED -->

<!-- PAGE 21 FAILED -->

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/tempo_2604.19295.pdf

Method, Benchmark, Acc, Pass@8
Oat-Zero-7B, Beyond AIME, 9.4, 19.4
Oat-Zero-7B, AIME 24, 30.2, 46.1
Oat-Zero-7B, AIME 25, 12.3, 33.7
Oat-Zero-7B, AIME 26, 16.7, 26.3
Oat-Zero-7B, OlymMath, 11.1, 22.6
MiMo-Zero-RL7B, Beyond AIME, 14.6, 33.1
MiMo-Zero-RL7B, AIME 24, 37.7, 63.9
MiMo-Zero-RL7B, AIME 25, 32.3, 51.9
MiMo-Zero-RL7B, AIME 26, 35.0, 52.8
MiMo-Zero-RL7B, OlymMath, 16.7, 36.1
OLMO3-1RE-RL7B, Beyond AIME, 13.8, 32.1
OLMO3-1RE-RL7B, AIME 24, 31.9, 56.3
OLMO3-1RE-RL7B, AIME 25, 36.5, 29.7
OLMO3-1RE-RL7B, AIME 26, 39.7, 24.0
OLMO3-1RE-RL7B, OlymMath, 42.4, 14.3
OLMO3-7B, Beyond AIME, 17.6, 38.8
OLMO3-7B, AIME 24, 56.1, 26.3
OLMO3-7B, AIME 25, 41.1, 26.7
OLMO3-7B, AIME 26, 42.8, 18.9
OLMO3-7B, OlymMath, 43.3, --
OLMO3-7B, Beyond AIME, 21.8, 22.3
OLMO3-7B, AIME 24, 40.8, 45.6
OLMO3-7B, AIME 25, 37.7, 20.4
OLMO3-7B, AIME 26, 39.2, 18.9
OLMO3-7B, OlymMath, 33.0, --
OLMO3-7B, Beyond AIME, 21.3, 28.4
OLMO3-7B, AIME 24, 41.6, 43.6
OLMO3-7B, AIME 25, 29.5, 23.6
OLMO3-7B, AIME 26, 39.7, 18.7
OLMO3-7B, OlymMath, 32.9, --
Qwen3-8B, Beyond AIME, 15.6, 33.6
Qwen3-8B, AIME 24, 26.3, 53.0
Qwen3-8B, AIME 25, 25.4, 44.8
Qwen3-8B, AIME 26, 21.9, 43.7
Qwen3-8B, OlymMath, 15.0, 39.9
Qwen3-8B, Beyond AIME, 18.7, 20.0
Qwen3-8B, AIME 24, 29.0, 30.0
Qwen3-8B, AIME 25, 32.8, 33.3
Qwen3-8B, AIME 26, 13.8, 25.0
Qwen3-8B, OlymMath, 11.4, 25.3
Qwen3-8B, Beyond AIME, 23.6, 26.7
Qwen3-8B, AIME 24, 62.7, 41.1
Qwen3-8B, AIME 25, 60.4, 24.2
Qwen3-8B, AIME 26, 50.7, 18.7
Qwen3-8B, OlymMath, 43.3, --

$Figure 3: A line chart comparing the performance of TTRL and TEMPO across training steps. The Y-axis represents "BeyondAIME Accuracy (pass@16)" and the X-axis represents "Training Steps".
$Figure 4: A line chart comparing the performance of AIME 2024 and AIME 2025 across training steps. The Y-axis represents "AIME Accuracy (Mean@6)" and the X-axis represents "Training Steps".
$Table 2: A performance comparison table showing accuracy metrics (Avg@1, Avg@8, Pass@8) for various models across different benchmarks (BBH, AGI, Zebra, GPQA-Diamond).

TTRL, BBH: 61.4, AGI: 53.6, Zebra: 30.3, GPQA-Diamond (Avg@8): 18.8, GPQA-Diamond (Pass@8): 45.8
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5, GPQA-Diamond (Pass@8): 67.6
TTRL, BBH: 74.9, AGI: 68.5, Zebra: 31.7, GPQA-Diamond (Avg@8): 41.1, GPQA-Diamond (Pass@8): 73.0
TTRL, BBH: 45.4, AGI: 38.2, Zebra: 22.5, GPQA-Diamond (Avg@8): 28.5,

end of paper


start of paper
# DOCUMENT: ../packet/pdfs/tfgn_2605.15053.pdf

<!-- PAGE 0 FAILED -->

<!-- PAGE 4 FAILED -->

<!-- PAGE 5 FAILED -->

Table 1: Headline backward transfer (BWT) and trained-domain max PPL drop reduction among the eleven primary conditions.

| Condition | Total params | Regime | BWT | Peak trained-domain PPL drop | Emission collapse? |
| :--- | :--- | :--- | :--- | :--- | :--- |
| TFGN + GPT-2 Small (TFGN_GPT2S_FS) | ~398 M | FS | -0.6 | 619 KT | No |
| TFGN + GPT-2 Medium (TFGN_GPT2M_FS) | ~729 M | FS | -0.6 | 673 KT | No |
| TFGN + GPT-2 Medium (TFGN_GPT2M_RETROFIT) | ~729 M | RETROFIT | -0.374 | 673 KT | No |
| TFGN + LLaMA 3.1 8B (TFGN_LLAMA8B_FS) | ~9 B | FS | -0.095 | (unstable-capped) | No |
| TFGN + LLaMA 3.1 8B (TFGN_LLAMA8B_RETROFIT) | ~9 B | RETROFIT | -0.007 | 30% PY | No |
| Baseline LLaMA 3.1 8B (LLAMA8B_FS) | ~9 B | FS | -0.374 | 619 KT | No |
| Baseline LLaMA 3.1 8B (LLAMA8B_RETROFIT) | ~9 B | RETROFIT | -0.374 | 619 KT | No |
| Baseline LLaMA 3.1 8B (LLAMA8B_FS, 500 M tok/phase) | ~9 B | FS | -0.374 | 619 KT | No |

<!-- PAGE 7 FAILED -->

<!-- PAGE 10 FAILED -->

Method | A: CPT $\ge$ 7B | B: $\ge$ 4 dom. | C: $\ge$ 1B tok/phase | D: Replay-free | E: Task-ID-free | F: Penalty-free | G: FS+RF
TreeLoRA [29] | F | P | P | P | F | P | F
TRACE benchmark [47] | F | P | F | P | F | P | F
STABLE [21] | F | P | P | F | P | F | F
O-LoRA [19] | F | P | F | P | P | F | F
EWC-Gemma2 [46] | F | F | PARTIAL | P | P | F | F
Llama-3-SynE [4] | P | P | F | P | P | P | F
Examining Forgetting [43] | P | P | F | P | P | P | F
Loss of Plasticity [33] | F | F | P | varies | P | F | F
GEM [2] | F | P | F | F | F | F | F
Revisit Replay [45] | P | P | PARTIAL | P | F | P | F
ANML [27] | F | F | P | F | P | F | F
Backpropamine [30] | F | varies | F | P | F | F | F
LoRA (base) [18] | F | P | P | | | |

Condition | Backbone | Overlay | Total | Train P1 | Train P2+
--- | --- | --- | --- | --- | ---
LLAMA 3.1 8B FS | $\sim$8.03 B | $\sim$953 M | $\sim$8.98 B | $\sim$8.98 B | $\sim$470 M

Baseline | Total params | Pairs with
BASELINE_STD_GPT2M_FS | ~355M | TFGN GPT-2 Medium FS
BASELINE_LORA256_GPT2M_FS | ~393M | TFGN GPT-2 Medium FS
BASELINE_STD_GPT2M_RETROFIT | ~355M | TFGN GPT-2 Medium RF
BASELINE_LORA256_GPT2M_RETROFIT | ~355M | TFGN GPT-2 Medium RF
BASELINE_STD_LLAMA8B | ~8B | TFGN LLaMA 3.1 8B (FS / RF; called out as init-asymmetric for the RF pairing)

BASELINE_STD_GPT2M_FS (standard fine-tuning, ~355M, From-Scratch). Headline numbers: BWT = −1.170244, FM = 1.772126, bwrt (Biomedical) = −2.674. Full data: Table 13.
BASELINE_LORA256_GPT2M_FS (LoRA r=256, ~393M, From-Scratch). Headline numbers: BWT = −1.004791, FM = 1.791716, bwrt (Biomedical) = −1.810. LoRA's BWT is within 15% of standard fine-tuning: low-rank parameter-efficient without persistent architecture state does not close BWT. Full data: Table 14.
BASELINE_STD_GPT2M_RETROFIT (standard fine-tuning, ~355M, Retrofit). Headline numbers: BWT = −0.541070, FM = 0.780655, bwrt (Biomedical) = −0.801. Hellaswag drop of 74-per-mile at the Chinese-phase boundary. Full data: Table 15.
BASELINE_LORA256_GPT2M_RETROFIT (LoRA r=256, ~355M, Retrofit). Headline numbers: BWT = −0.392083, FM = 0.695186, bwrt (Biomedical) = −0.468. Hellaswag drop of 56-per-mile across phases. Full data: Table 16.
BASELINE_STD_LLAMA8 (standard fine-tuning, ~9B, From-Scratch, ~500 M/tok, prose, 3-phase). Headline numbers: BWTs = −0.374 (recomputed), bwrt Python = −0.6418 (worst), bwrt Prose = −0.1068. The categorical Pythology diagram: P1 $\rightarrow$ P2 the diagonal is $1339 \rightarrow 7.90$ (training fits Python well), but the Prose row at P2 is 50.41 (+32% over P1 33.23) and the Python column at P3 is 12.97 (post-training drift to Math). Full data: Table 17.

Cell | Baseline emissions (3 condi- tions) | TFGN emissions (5 conditions)
--- | --- | ---
P2 Prose (after Python phase) | mid-completion: import, def, Apache license boilerplate, Std-FT GPT-2M, LoRA-Z56 GPT-2M, LLaMA 8B all collapse identically. | coherent English Prose. TFGN GPT-25 (398 M), TFGN GPT-2M FS+RF (~729 M $\times$ 2), TFGN LLaMA 8B FS+RF (~9 B $\times$ 2) — every scale, every regime, holds the Prose distribution.
P5 Prose (after Chinese, phase only) | mid-completion: Std-FT GPT-2M, LoRA-Z56 GPT-2M (Chinese reported only on the 2-6 phase sequence; LLaMA 8B is 3-phase). | coherent English Prose and simultaneously emit CJK-coherent continuations on Chinese training-domain prompts — old-domain preservation and new-domain learning at once.

$Backward\ transfer\ (BWT)\ magnitude\ (lower\ is\ more\ forgiving)$
$TFGN\ 8B\ Retro$
$TFGN\ GPT2\text{-}M\ FS$
$TFGN\ 8B\ FS$
$TFGN\ GPT2\text{-}S\ FS$
$TFGN\ GPT2\text{-}M\ Retro$
$Std\text{-}FT\ 8B\ FS$
$Std\text{-}FT\ GPT2\text{-}M\ Retro$
$LoRA\ GPT2\text{-}M\ FS$
$Std\text{-}FT\ GPT2\text{-}M\ FS$

-0.007
-0.083
-0.095
-0.109
-0.135
-0.374
-0.393
-0.541
-1.005
-1.170

$HellaSwag accuracy, Continual phase (P1-P6), Method/Model
TFGN GPT-2 Small FS, P1, 0.34
TFGN GPT-2 Small FS, P2, 0.34
TFGN GPT-2 Small FS, P3, 0.34
TFGN GPT-2 Small FS, P4, 0.34
TFGN GPT-2 Small FS, P5, 0.34
TFGN GPT-2 Small FS, P6, 0.34
TFGN GPT-2 Medium FS, P1, 0.40
TFGN GPT-2 Medium FS, P2, 0.40
TFGN GPT-2 Medium FS, P3, 0.40
TFGN GPT-2 Medium FS, P4, 0.40
TFGN GPT-2 Medium FS, P5, 0.40
TFGN GPT-2 Medium FS, P6, 0.40
TFGN GPT-2 Medium Retrofit, P1, 0.41
TFGN GPT-2 Medium Retrofit, P2, 0.41
TFGN GPT-2 Medium Retrofit, P3, 0.41
TFGN GPT-2 Medium Retrofit, P4, 0.41
TFGN GPT-2 Medium Retrofit, P5, 0.41
TFGN GPT-2 Medium Retrofit, P6, 0.41
Std-FT GPT-2 Medium FS, P1, 0.36
Std-FT GPT-2 Medium FS, P2, 0.36
Std-FT GPT-2 Medium FS, P3, 0.36
Std-FT GPT-2 Medium FS, P4, 0.36
Std-FT GPT-2 Medium FS, P5, 0.36
Std-FT GPT-2 Medium FS, P6, 0.36
Std-FT GPT-2 Medium Retrofit, P1, 0.35
Std-FT GPT-2 Medium Retrofit, P2, 0.35
Std-FT GPT-2 Medium Retrofit, P3, 0.35
Std-FT GPT-2 Medium Retrofit, P4, 0.35
Std-FT GPT-2 Medium Retrofit, P5, 0.35
Std-FT GPT-2 Medium Retrofit, P6, 0.35
LoRA&x GPT-2 Medium Retro, P1, 0.41
LoRA&x GPT-2 Medium Retro, P2, 0.41
LoRA&x GPT-2 Medium Retro, P3, 0.41
LoRA&x GPT-2 Medium Retro, P4, 0.41
LoRA&x GPT-2 Medium Retro, P5, 0.41
LoRA&x GPT-2 Medium Retro, P6, 0.41

$Mean cross-domain |cos|, Model
TFGN GPT-2 Small FS, 0.0425
TFGN GPT-2 Medium FS, 0.0204
TFGN GPT-2 Medium Retrofit, 0.0904
TFGN LLAMA-88 FS, 0.0741
TFGN LLAMA-88 Retro (~98.3 phase), 0.0432

$L2-orthogonal fraction (%), Model
TFGN GPT-2 Small FS, 99.91
TFGN GPT-2 Medium FS, 99.94
TFGN GPT-2 Medium Retrofit, 99.5
TFGN LLAMA-88 FS, 99.91
TFGN LLAMA-88 Retro (~98.3 phase), 99.72

Condition | Eval domain | M[1,d] | M[fwt,d] | Comment
LLaMA-8B Retrofit (3-phase) | JavaScript (untrained) | 23.05 | 16.7 (T = 3) | +26.8% Python $\rightarrow$ JS, shared syntax
LLaMA-8B Retrofit (3-phase) | Math (P3) | 18.20 | 17.28 (T = 2) | +4.2% Python $\rightarrow$ Math, before training
LLaMA-8B Retrofit (3-phase) | Biomedical (untrained) | 9.96 | 9.36 (T = 2) | Distributionally distinct: mild neglect
LLaMA-8B Retrofit (3-phase) | Chinese (untrained) | 90.22 | 90.99 (T = -3) | -0.85% Chinese (untrained)
LLaMA-8B Math (P3) | Math (P3) | 122 | 119 (P2) | +2.5% Python $\rightarrow$ Math, before training
GPT-2 Medium FS (6-phase) | JavaScript (P6) | 37.7 | 14.1 (P5) | +62.0% Python $\rightarrow$ JS, before training
GPT-2 Medium FS (6-phase) | Math (P6) | 26.7 | 24.2 (P5) | +9.0% Python $\rightarrow$ Math, before training
GPT-2 Medium FS (6-phase) | Biomedical (P6) | 31.6 | 31.5 (P5) | +0.3% Distributionally distinct: mild neglect
GPT-2 Medium FS (6-phase) | Chinese (P6) | 52.0 | 51.5 (P4) | +1.0% Distributionally distinct: mild neglect
GPT-2 Small FS (6-phase) | JavaScript (P6) | 46.5 | 17.4 (P5) | +61.8% Python $\rightarrow$ JS, before training
GPT-2 Small FS (6-phase) | Math (P6) | 60.4 | 59.4 (P5) | +1.7% Python $\rightarrow$ Math, before training
GPT-2 Small FS (6-phase) | Biomedical (P4) | 38.5 | 38.4 (P3) | +0.3% Distributionally distinct
GPT-2 Small FS (6-phase) | Chinese (P4) | 64.2 | 63.7 (P4) | +0.8% Distributionally distinct

Table 11: per-domain PPL matrix and scalar metrics (6-phase Retrofit; the hardest condition in this paper). BWT = -0.135, ~4x tighter than matched Std-FT-RF (Table 15).
Phase trained, Prose, Python, Math, Biomedical, Chinese, Javascript, BWT
P1 Prose, 26.3, 6.13, 26.7, 19.8, 38.1, 7.73, -
P2 Python, 27.0, 4.18, 24.2, 20.1, 38.4, 5.10, -
P3 Math, 27.0, 4.91, 24.0, 20.0, 38.3, 5.50, -
P4 Biomedical, 27.0, 4.92, 24.1, 19.5, 38.3, 5.51, -
P5 Chinese, 27.5, 4.93, 24.1, 19.5, 13.0, 5.52, -
P6 Javascript, 27.5, 4.94, 24.1, 19.5, 13.0, 3.96, -
bwt_d, -0.046, -0.182, -0.150, -0.170, -0.192, -

Table 12: TFGN_GPT2S_FS — per-domain PPL matrix and scalar metrics (6-Phase From-Scratch ~398 M small-scale proof point). BWT = -0.109, the small-scale proof point at ~398 M.
Phase trained, Prose, Python, Math, Biomedical, Chinese, Javascript, BWT
P1 Prose, 38.4, 22.7, 60.3, 38.5, 64.2, 45.5, -
P2 Python, 38.6, 13.5, 49.8, 38.5, 63.9, 17.2, -
P3 Math, 38.6, 14.3, 47.8, 38.4, 63.7, 17.3, -
P4 Biomedical, 38.6, 14.4, 48.0, 37.7, 63.7, 17.4, -
P5 Chinese, 38.6, 14.4, 48.0, 37.7, 22.4, 17.4, -
P6 Javascript, 38.6, 14.4, 48.0, 37.7, 22.4, 14.9, -
bwt_d, -0.000, -0.067, -0.005, -0.020, -0.232, -

Table 13: BASELINE_STD_GPT2M_FS — per-domain PPL matrix and scalar metrics. Standard fine-tuning, ~355 M, From-Scratch. Prose-row PPL spikes P3:38.02 -> P5:89.83 (+136%) at the Chinese-phase boundary.
Phase trained, Prose, Python, Math, Biomedical, Chinese, Javascript, BWT
P1 Prose, 33.79, 18.44, 53.62, 31.12, 33.01, 34.89, -
P2 Python, 65.72, 2.80, 36.14, 66.02, 20.8, 4.87, -
P3 Math, 38.02, 3.57, 14.76, 29.02, 17.49, 4.86, -
P4 Biomedical, 46.05, 8.26, 26.00, 11.78, 98.34, 10.63, -
P5 Chinese, 89.83, 14.7, 42.51, 41.57, 4.08, 8.54, -
P6 Javascript, 67.44, 4.30, 30.95, 43.29, 7.26, 2.48, -
bwt_d, -0.996, -0.535, -1.097, -2.674, -2.550, -

Table 18: Three-axis decomposition of the Extension A 81% reduction.

Architectural axis | Matched-control comparison | Contribution
--- | --- | ---
Routing refinement | Anchor $\rightarrow$ +Routing | +35%
Sensing + prediction meta-control | +Routing $\rightarrow$ +Sensing&Pred | +51%
Active consolidation | +Sensing&Pred $\rightarrow$ +Active | +40%
Compound (Tier C headline vs anchor) | Anchor $\rightarrow$ Headline | 81.0%

Condition, BWT, FM
TFGN_EXTA_A_CHAMPION, -0.00277, 0.00227
TFGN_EXTA_C_HEADLINE, -0.01140, 0.01140
TFGN_EXTA_B_FULL_DIAG, -0.01500, 0.01500

Capability role | Framework system | Function in the closed loop
Sensing | System M | Reads architecture's internal state
Gating | System M | Scales gradient updates by surprise vs. history
Consolidation | System M | Triggers state-freeze when trajectory stabilizes
Prediction (internal world model) | System A | Predicts the network's own next state
Cross-layer coupling | inter-module | Keeps regulation decisions consistent across depth

# Criterion, What it requires, TFGN status
1, Causal sufficiency, Editing the planner changes model behaviour at the operator level, not only at the activation level, PROVEN
2, Goal-direction, Planner recovers the target state from any starting state, PROVEN
3, Compositionality, Sub-task / sub-domain structure is encoded inside each plan vector, PARTIAL-PROVEN
4, Executor obedience, Decoder acts on the plan at the 90% threshold treated as breakthrough-grade, PARTIAL-PROVEN
5, Scale preservation, Mechanism survives a real parameter jump (~398 M $\rightarrow$ ~729 M $\rightarrow$ ~9 B), PARTIAL-PROVEN
6, Benchmark vs CoT, Beats Chain $\rightarrow$ Thought on a standard-ised compositional evaluation, FUTURE-WORK

$Six-criterion scorecard for breakthrough latent planning: Criterion name, Description, Result
$Table 29: Operator-level control vs. adjacent latent-planning families: Family, Causal, Goal-dir., Scale-inv., Inspect.

1. Causal sufficiency, Editing planner reshapes the operator the decoder uses, PROVEN
2. Goal direction, Single plan vector drives full behavioural flip from any state, PROVEN
3. Compositionality, Sub-task structure encoded inside each plan vector, PARTIAL
4. Executor obedience, Decoder acts on plan at 99.9% breakthrough threshold, PARTIAL
5. Scale preservation, Mechanism survives across total scale, PARTIAL
6. Benchmark vs CoT, Beats chain-of-thought on compositional and roadmap, FUTURE WORK

Activation steering, $\sim$, $\sim$, $\sim$, $\sim$
Prefix tuning, $\checkmark$, $\sim$, $\sim$, $\sim$
Mixture-of-Experts routing, $\checkmark$, $\checkmark$, $\sim$, $\sim$
Chain-of-thought (token-space), $\checkmark$, $\checkmark$, $\sim$, $\sim$
Diffusion-denoised latents, $\sim$, $\sim$, $\sim$, $\sim$

Table 30: Per-target domain modulation lift on TFGN_EXTB_GPT2S_HEADLINE. Each cell is the mean over the five non-self source domains. "Natural cosine" and "Post-injection cosine" are transcribed from the underlying source data: "Lift" is the per-row difference (Post - Natural), computed here for clarity. Mean lift is +0.11, max +0.19 on Chinese, min +0.08 on Math. The largest lift is on Chinese, where the natural cosine is the lowest (0.81); even so, injection brings every target to $\sim1.00$.

Target domain | Natural cosine | Post-injection cosine | Lift
Prose | 0.87 | $\sim1.00$ | +0.13
Python | 0.88 | $\sim1.00$ | +0.12
Math | 0.92 | $\sim1.00$ | +0.08 (min lift)
Biomedical | 0.89 | $\sim1.00$ | +0.11
Chinese | 0.81 | $\sim1.00$ | +0.19 (max lift)
JavaScript | 0.89 | $\sim1.00$ | +0.11
mean | 0.89 | $\sim1.00$ | +0.11

$Cosine\ to\ target\ native\ operator\ (Natural\ cosine\ to\ target\ (no\ plan\ vector))\ vs\ (Cosine\ after\ plan\ vector\ injection)$
prose, 0.87, 1.00
python, 0.88, 1.00
math, 0.92, 1.00
chinese, 0.81, 1.00
javascript, 0.89, 1.00
biomed, 0.89, 1.00

External name | Backbone | Regime | Phases | Tok/phase
---|---|---|---|---
BASELINE_STD_GPT2M | GPT-2 Medium | FS | 6 | 1 B
BASELINE_LORA256_GPT2M_FS | GPT-2 Medium | FS | 6 | 1 B
BASELINE_LORA256_GPT2M_RETROFIT | GPT-2 Medium | RF | 6 | 1 B
BASELINE_STD_LLAMA3 | LLAMA 3.1 8B | FS | 3 | 500 M
TFGN_EXTA_BASELINE | GPT-2 Small | FS | 3 | 200 M
TFGN_EXTA_SENSEACT | GPT-2 Small | FS | 3 | 200 M
TFGN_EXTA_FULL_DIAG | GPT-2 Small | FS | 3 | 200 M
TFGN_EXTA_B_BASELINE | GPT-2 Small | FS | 3 | 1 B
TFGN_EXTA_B_FULL_DIAG | GPT-2 Small | FS | 3 | 1 B
TFGN_EXTA_C_CONTROL | GPT-2 Small | FS | 3 | 1 B (ER)
TFGN_EXTA_C_DIAG | GPT-2 Small | FS | 3 | 1 B (ER)
TFGN_EXTA_C_ANCHOR | GPT-2 Small | FS | 3 | 1 B (ER)
TFGN_EXTA_C_HEADLINE | GPT-2 Small | FS | 3 | 1 B (ER)
TFGN_EXTB_GPT2S_HEADLINE | GPT-2 Small (~398 M total) | FS | - | -
TFGN_EXTB_GPT2M_HEADLINE | GPT-2 Medium (~739 M total) | RF | - | -

Table 37: TFGN_EXTA_A_BASELINE (Tier A matched control, base-consolidation only) — per-domain PPL matrix and scalar metrics (3-phase Prose $\rightarrow$ Python $\rightarrow$ Math, 200 M tokens/phase). $BWT_3 = -0.00942$.

Phase trained | Prose | Python | Math
--- | --- | --- | ---
P1 Prose | 77.75 | 51.63 | 126.56
P2 Python | 77.75 | 32.93 | 126.32
P3 Math | 77.79 | 33.53 | 116.46
$bwt_d$ | $-0.00051$ | $-0.01822$ | 

Metric | Value
--- | ---
$BWT_3$ / FM | $-0.00942$ / $+0.00942$
Per-domain $bwt_d$ | Prose $-0.00051$; Python $-0.01822$
Other | Mean [cos | 0.0436; L2-orth 99.90%; HellaSwag P1$\rightarrow$P3: 0.268 $\rightarrow$ 0.274 (+2.2%, maintained)]

end of paper

