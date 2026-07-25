#!/usr/bin/env bash
set -exuo pipefail

pdf2md 'https://arxiv.org/pdf/2407.04811.pdf' > arxiv.org__pdf__2407.04811.pdf.txt
python3 fix_other_guys.py
