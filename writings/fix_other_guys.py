import sys, re, os

# More webcat output cleanup
files = [
    "www.geoffchappell.com__studies__windows__km__ntoskrnl__inc__api__pebteb__teb__index.htm.txt",
    "github.com__NervanaSystems__maxas__wiki__Control-Codes.txt",
    "newosxbook.com__articles__jlaunchctl.html.txt",
    "newosxbook.com__articles__CasaDePPL.html.txt",
    "arxiv.org__pdf__1706.10283.pdf.txt"
]

for f in files:
    if not os.path.exists(f): continue
    with open(f) as file:
        text = file.read()

    # Common webcat cleanup
    text = re.sub(r'^begin https?://[^\n]+\n', '', text)
    text = re.sub(r'\nend https?://[^\n]+', '', text)
    text = re.sub(r'\nFULL TEXT FOR URL[^\n]+', '', text)
    text = re.sub(r'\nIMAGES\n.*', '\n', text, flags=re.DOTALL)

    # Specific pdf2md cleanup for arxiv
    if 'pdf2md' in text or 'arxiv' in f:
        text = re.sub(r'^Downloading[^\n]+\n', '', text)
        text = re.sub(r'^Starting qvl-pdf-to-md:[^\n]+\n', '', text, flags=re.MULTILINE)
        text = re.sub(r'^Rendering and resizing[^\n]+\n', '', text, flags=re.MULTILINE)
        text = re.sub(r'^Found/rendered[^\n]+\n', '', text, flags=re.MULTILINE)
        text = re.sub(r'^Page \d+ done via[^\n]+\n', '', text, flags=re.MULTILINE)
        text = re.sub(r'^Finished[^\n]+\n', '', text, flags=re.MULTILINE)
        text = re.sub(r'^OCR OF[^\n]+SAVED TO[^\n]+\n', '', text, flags=re.MULTILINE)

    # GitHub wiki header/footer cleanup
    if 'github.com' in f:
        text = re.sub(r'^.*?Jump to bottom\n', '', text, flags=re.DOTALL)
        text = re.sub(r'\nFooter\n© 202.*', '', text, flags=re.DOTALL)

    with open(f, 'w') as file:
        file.write(text.strip() + '\n')

