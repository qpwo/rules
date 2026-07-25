import sys, re, os, json, subprocess, tempfile
from urllib.request import urlopen, Request
from io import BytesIO
from PIL import Image

f = open('next_file.txt').read().strip()
print(f"Fixing up {f}")
c = open(f).read()
orig = c

c = re.sub(r'https?://[^\s"]*gravatar\.com[^\s"]*', '', c)
c = re.sub(r'https?://[^\s"]*wp\.com[^\s"]*', '', c)
c = re.sub(r'https?://[^\s"]*twemoji[^\s"]*', '', c)

cache = json.load(open('url_cache.json')) if os.path.exists('url_cache.json') else {}
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
    else:
        print(f"Need to describe: {u}")
        try:
            data = urlopen(Request(u, headers={'User-Agent': 'Mozilla/5.0'}), timeout=10).read()
            try:
                img = Image.open(BytesIO(data))
                if max(img.size) < 150:
                    cache[u] = ""
                    c = c.replace(u, "")
                    continue
            except: pass
            with tempfile.NamedTemporaryFile(delete=False, suffix=".png") as tmp: tmp.write(data)
            p = subprocess.run(["/Users/ubuntu/bin/describeimages", "--gemma", tmp.name], capture_output=True, text=True)
            os.unlink(tmp.name)
            if p.returncode == 0 and p.stdout.strip():
                desc = p.stdout.strip().replace('\n', ' ').replace('*', '').replace('`', '').replace('#', '')
                if len(desc) > 600: desc = desc[:597] + '...'
                res = f"[IMAGE DESCRIPTION: {desc}]"
                cache[u] = res
                c = c.replace(u, res)
                print(f"Described: {u}")
            else: cache[u] = "FAILED"
        except Exception as e:
            print(f"Failed {u}: {e}")
            cache[u] = ""
            c = c.replace(u, "")

json.dump(cache, open('url_cache.json', 'w'))
c = re.sub(r'\nIMAGES\s*$', '\n', c)
c = re.sub(r'\n{3,}', '\n\n', c)

if c != orig:
    open(f, 'w').write(c)
    print("Changes written.")
else: print("No changes needed.")
