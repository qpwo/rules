import re, json, os, glob

cache = json.load(open('url_cache.json'))

files = glob.glob('*.txt')
for f in files:
    if f in ('urls.txt', 'articles.txt', 'next_file.txt', '.inputs.txt', '.urls.txt'): continue
    c = open(f).read()
    orig = c

    c = re.sub(r'https?://[^\s"]*gravatar\.com[^\s"]*', '', c)
    c = re.sub(r'https?://[^\s"]*wp\.com[^\s"]*', '', c)
    c = re.sub(r'https?://[^\s"]*twemoji[^\s"]*', '', c)

    urls = set(re.findall(r'https?://[^\s"]+', c))
    image_urls = [u for u in urls if any(ext in u.lower() for ext in ['.png', '.jpg', '.jpeg', '.gif', '.svg', '.webp', 'avatar', 'image'])]

    for u in image_urls + re.findall(r'^https?://[^\s]+\.(?:png|jpg|jpeg|gif|svg|webp)$', c, re.MULTILINE):
        if 'unsplash' in u.lower():
            c = c.replace(u, "")
            continue
        if u in cache:
            res = cache[u]
            if res and res not in ("FAILED", "FAILED_DESCRIBE"):
                c = c.replace(u, res)
            else:
                c = c.replace(u, "")

    c = re.sub(r'\nIMAGES\s*$', '\n', c)
    c = re.sub(r'\n{3,}', '\n\n', c)

    if c != orig:
        open(f, 'w').write(c)
