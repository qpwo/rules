#!/usr/bin/env bash
set -exuo pipefail

mkdir -p trash_bin

mv -vn keithp.com__keithp__talks__guadec2002__.txt trash_bin/ || true
mv -vn dotat.at__40__2006-02-19-how-not-to-design-an-mta-part-2-partitioning-for-securi.txt trash_bin/ || true
mv -vn blog.molecular-matters.com__2011__08__03__memory-system-part-5__.txt trash_bin/ || true
mv -vn www.mikeash.com__pyblog__friday-qa-2009-04-24-code-generation-with-llvm-part-2-f.txt trash_bin/ || true
mv -vn dotat-at-prog-life-life.txt trash_bin/ || true

webcat https://www.geoffchappell.com/studies/windows/km/ntoskrnl/inc/api/pebteb/peb/index.htm > www.geoffchappell.com__studies__windows__km__ntoskrnl__inc__api__pebteb__peb__index.htm.txt
webcat https://github.com/NervanaSystems/maxas/wiki/SGEMM > github.com__NervanaSystems__maxas__wiki__SGEMM.txt
webcat https://arxiv.org/abs/1706.10283 > arxiv.org__abs__1706.10283.txt
webcat https://newosxbook.com/articles/jlaunchctl.html > newosxbook.com__articles__jlaunchctl.html.txt
webcat https://presentations.clickhouse.com/2017-meetup10/efficient/ > presentations.clickhouse.com__2017-meetup10__efficient__.txt

ls -la *.txt | headtail -n 10
