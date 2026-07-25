import glob, re

files = glob.glob('*.txt')
files = [f for f in files if f not in ('.inputs.txt', '.urls.txt', 'articles.txt', 'urls.txt', 'next_file.txt')]

for f in files:
    c = open(f).read()
    orig = c

    # Strip any remaining image URLs
    c = re.sub(r'https?://[^\s]+\.(png|jpg|jpeg|gif|svg|webp)', '', c, flags=re.IGNORECASE)

    # Compress any remaining [IMAGE DESCRIPTION ...] blocks just in case
    def repl(m):
        desc = m.group(1).strip().replace('\n', ' ').replace('*', '').replace('`', '').replace('#', '')
        desc = re.sub(r'\s+', ' ', desc)
        if len(desc) > 600: desc = desc[:597] + '...'
        return f"[IMAGE DESCRIPTION: {desc}]"

    c = re.sub(r'\[IMAGE DESCRIPTION:\s*(.*?)\s*\]', repl, c, flags=re.DOTALL)

    if c != orig:
        open(f, 'w').write(c)
        print(f"Cleaned up {f}")

