import re, json, os, concurrent.futures, base64, urllib.request, time
from io import BytesIO
from PIL import Image

files = [f for f in os.listdir() if f.endswith('.txt') and not f.startswith('.')]
urls = set(re.findall(r'https?://[^\s"]+\.(?:png|jpg|jpeg)', "".join(open(f).read() for f in files), re.I))

cache = json.load(open('url_cache.json')) if os.path.exists('url_cache.json') else {}
for k in list(cache.keys()):
    if cache[k] in ("FAILED_DESCRIBE", "FAILED"): cache.pop(k)

def proc(u):
    if u in cache: return
    try:
        req = urllib.request.Request(u, headers={'User-Agent': 'Mozilla/5.0'})
        data = urllib.request.urlopen(req, timeout=10).read()
        if max(Image.open(BytesIO(data)).size) < 150:
            cache[u] = ""
            return
    except Exception:
        cache[u] = ""
        return

    mime = "image/jpeg" if "jpg" in u.lower() or "jpeg" in u.lower() else "image/png"
    b64 = base64.b64encode(data).decode()
    payload = {
        "model": "google/gemma-4-31B-it",
        "messages": [{"role": "user", "content": [
            {"type": "text", "text": "describe this in enough detail that i could recreate it exactly."},
            {"type": "image_url", "image_url": {"url": f"data:{mime};base64,{b64}"}}
        ]}],
        "max_tokens": 4096
    }

    for attempt in range(6):
        try:
            req = urllib.request.Request("https://llms.intrinsicapi.com/v1/chat/completions",
                data=json.dumps(payload).encode(),
                headers={"Authorization": "Bearer o30ya7yeDe_p", "Content-Type": "application/json"})
            resp = json.loads(urllib.request.urlopen(req, timeout=180).read().decode())
            content = resp.get('choices', [{}])[0].get('message', {}).get('content', '')
            if isinstance(content, list): content = "\n".join(str(x.get('text', '')) for x in content)
            cache[u] = f"[IMAGE DESCRIPTION: {content.strip()}]"
            print(f"OK {u}")
            return
        except urllib.error.HTTPError as e:
            if e.code == 429:
                time.sleep(2 ** attempt)
                continue
            cache[u] = "FAILED"
            print(f"FAILED {u}: {e}")
            return
        except Exception as e:
            if "429" in str(e):
                time.sleep(2 ** attempt)
                continue
            cache[u] = "FAILED"
            print(f"FAILED {u}: {e}")
            return

    cache[u] = "FAILED"
    print(f"FAILED {u}: 429 timeout")

print(f"Total urls: {len(urls)}, already cached: {len(cache)}")
with concurrent.futures.ThreadPoolExecutor(8) as ex:
    list(ex.map(proc, urls))

json.dump(cache, open('url_cache.json', 'w'))

for f in files:
    c = open(f).read()
    orig = c
    for u, res in sorted(cache.items(), key=lambda x: -len(x[0])):
        if res and res != "FAILED": c = c.replace(u, res)
        elif res == "": c = c.replace(u, "")
    if c != orig:
        open(f, 'w').write(c)
        print(f"Updated {f}")
