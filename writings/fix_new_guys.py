import sys, re, os, glob

# The files we recently created via webcat
files = [
    "www.geoffchappell.com__studies__windows__km__ntoskrnl__inc__api__pebteb__peb__index.htm.txt",
    "github.com__NervanaSystems__maxas__wiki__SGEMM.txt",
    "arxiv.org__abs__1706.10283.txt",
    "newosxbook.com__articles__jlaunchctl.html.txt",
    "presentations.clickhouse.com__2017-meetup10__efficient__.txt"
]

for f in files:
    if not os.path.exists(f): continue
    with open(f) as file:
        text = file.read()

    # Remove webcat 'begin ...' and 'end ...'
    text = re.sub(r'^begin https?://[^\n]+\n', '', text)
    text = re.sub(r'\nend https?://[^\n]+', '', text)
    text = re.sub(r'\nFULL TEXT FOR URL[^\n]+', '', text)

    # Remove IMAGES block at the bottom
    text = re.sub(r'\nIMAGES\n.*', '\n', text, flags=re.DOTALL)

    # GitHub wiki header/footer cleanup
    if 'github.com' in f:
        # crude strip of github header navigation
        text = re.sub(r'^.*?Jump to bottom\n', '', text, flags=re.DOTALL)
        text = re.sub(r'\nFooter\n© 202.*', '', text, flags=re.DOTALL)

    with open(f, 'w') as file:
        file.write(text.strip() + '\n')
