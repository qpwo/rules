# the articles about lockless structures and concurrency costs were erroneously removed.
# the three articles defending locks/mutexes will remain deleted.
git checkout HEAD~1 -- \
  fgiesen.wordpress.com__2014__08__18__atomics-and-contention__.txt \
  moodycamel.com__blog__2014__detailed-design-of-a-lock-free-queue.txt \
  preshing.com__20120515__memory-reordering-caught-in-the-act__.txt \
  preshing.com__20121019__this-is-why-they-call-it-a-weakly-ordered-cpu__.txt \
  pvk.ca__Blog__2015__01__13__lock-free-mutual-exclusion__.txt \
  travisdowns.github.io__blog__2020__07__06__concurrency-costs.html.txt

git commit -m "restore erroneously deleted articles that support locklessness/rseq and highlight concurrency costs"
git push
