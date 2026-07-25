#!/usr/bin/env bash
set -exuo pipefail

# Download EfficientZero
pdf2md 'https://arxiv.org/pdf/2111.00210.pdf' > arxiv.org__pdf__2111.00210.pdf.txt

# Download DRAMA
pdf2md 'https://arxiv.org/pdf/2410.08893.pdf' > arxiv.org__pdf__2410.08893.pdf.txt

# Cleanup new webcat outputs
python3 fix_other_guys.py
