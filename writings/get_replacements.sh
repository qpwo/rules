#!/usr/bin/env bash
set -exuo pipefail

webcat \
    'https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/peb/index.htm' \
    'https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/teb/index.htm' \
    'https://github.com/NervanaSystems/maxas/wiki/SGEMM' \
    'https://github.com/NervanaSystems/maxas/wiki/Control-Codes' \
    'https://arxiv.org/abs/1706.10283' \
    'https://newosxbook.com/articles/jlaunchctl.html' \
    'https://newosxbook.com/articles/CasaDePPL.html' \
    'https://presentations.clickhouse.com/2017-meetup10/efficient/'
