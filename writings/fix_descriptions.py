import glob, re

for f in glob.glob('*.txt'):
    if f in ('.inputs.txt', '.urls.txt', 'articles.txt', 'urls.txt', 'next_file.txt'): continue
    c = open(f).read()
    orig = c

    def repl(m):
        desc = m.group(1).strip().replace('\n', ' ').replace('*', '').replace('`', '').replace('#', '')
        # Collapse multiple spaces
        desc = re.sub(r'\s+', ' ', desc)
        if len(desc) > 600:
            desc = desc[:597] + '...'
        return f"[IMAGE DESCRIPTION: {desc}]"

    # Replace all image descriptions with the single line version
    c = re.sub(r'\[IMAGE DESCRIPTION:\s*(.*?)\s*\]', repl, c, flags=re.DOTALL)

    if c != orig:
        open(f, 'w').write(c)
        print(f"Fixed descriptions in {f}")
