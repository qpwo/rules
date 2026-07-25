#!/usr/bin/env python3
"""
CLI tool for post-processing downloaded web articles.
"""
import argparse, collections, concurrent.futures, glob, json, os, re, subprocess, sys, tempfile, time, urllib.request, base64
from io import BytesIO

def get_files():
    files = sys.argv[2:] if len(sys.argv) > 2 else glob.glob('*.txt')
    return [f for f in files if f not in ('.inputs.txt', '.urls.txt', 'articles.txt', 'urls.txt', 'next_file.txt')]

def do_fix_webcat(args):
    for f in get_files():
        with open(f) as file: text = file.read()
        orig = text
        text = re.sub(r'^begin https?://[^\n]+\n', '', text, flags=re.MULTILINE)
        text = re.sub(r'\nend https?://[^\n]+', '', text)
        text = re.sub(r'\nFULL TEXT FOR URL[^\n]+', '', text)
        text = re.sub(r'\nIMAGES\n.*', '\n', text, flags=re.DOTALL)
        if 'pdf2md' in text or 'arxiv' in f:
            text = re.sub(r'^Downloading[^\n]+\n', '', text, flags=re.MULTILINE)
            text = re.sub(r'^Starting qvl-pdf-to-md:[^\n]+\n', '', text, flags=re.MULTILINE)
            text = re.sub(r'^Rendering and resizing[^\n]+\n', '', text, flags=re.MULTILINE)
            text = re.sub(r'^Found/rendered[^\n]+\n', '', text, flags=re.MULTILINE)
            text = re.sub(r'^Page \d+ done via[^\n]+\n', '', text, flags=re.MULTILINE)
            text = re.sub(r'^Finished[^\n]+\n', '', text, flags=re.MULTILINE)
            text = re.sub(r'^OCR OF[^\n]+SAVED TO[^\n]+\n', '', text, flags=re.MULTILINE)
        if 'github.com' in f:
            text = re.sub(r'^.*?Jump to bottom\n', '', text, flags=re.DOTALL)
            text = re.sub(r'\nFooter\n© 202.*', '', text, flags=re.DOTALL)
        if text != orig:
            open(f, 'w').write(text.strip() + '\n')
            print(f"Cleaned {f}")

def process_ai_trim(f):
    try:
        with open(f) as file: lines = file.read().splitlines()
        if len(lines) < 50: return f"Skipped {f} (too short)"
        prompt = f"""I am going to give you the first 150 lines and the last 150 lines of a web article scraped to text.
Many of these have navigation bars, headers, footers, comment sections, recent posts widgets, etc.
Your job is to identify the line number where the ACTUAL main content starts and ends.
First 150 lines:
{chr(10).join(f"{i}: {line}" for i, line in enumerate(lines[:150]))}
Last 150 lines:
{chr(10).join(f"{len(lines) - len(lines[-150:]) + i}: {line}" for i, line in enumerate(lines[-150:]))}
Output ONLY a JSON object: {{"start": <int>, "end": <int>}}
"""
        api_url = "https://llms.intrinsicapi.com/v1/chat/completions"
        headers = {"Authorization": "Bearer o30ya7yeDe_p", "Content-Type": "application/json"}
        data = json.dumps({"model":"google/gemma-4-31B-it", "messages":[{"role":"user", "content":prompt}], "max_tokens":30, "temperature": 0.0}).encode()
        for attempt in range(3):
            try:
                req = urllib.request.Request(api_url, data=data, headers=headers, method='POST')
                resp = urllib.request.urlopen(req, timeout=60).read()
                out = json.loads(resp)['choices'][0]['message']['content']
                m = re.search(r'\{\s*"start"\s*:\s*(\d+)\s*,\s*"end"\s*:\s*(\d+)\s*\}', out)
                if m:
                    start, end = int(m.group(1)), int(m.group(2))
                    if start < 150 and end >= len(lines) - 150 and start < end:
                        if start == 0 and end == len(lines) - 1: return f"Unchanged {f}: {start} to {end}"
                        new_c = "\n".join(lines[start:end+1]) + "\n"
                        open(f, 'w').write(new_c)
                        return f"Trimmed {f}: {start} to {end}"
                    else: return f"Invalid boundaries for {f}: {start} to {end} (len={len(lines)})"
                return f"Failed to parse JSON for {f}: {out}"
            except Exception as e: time.sleep(2 * (attempt + 1))
        return f"Error on {f}"
    except Exception as e: return f"Error on {f}: {e}"

def do_ai_trim(args):
    with concurrent.futures.ThreadPoolExecutor(max_workers=20) as executor:
        for res in executor.map(process_ai_trim, get_files()): print(res)

def do_heuristic_trim(args):
    header_junk = {"home", "about", "blog", "archives", "search", "rss", "contact", "subscribe", "skip to content", "skip to primary content", "main menu", "menu", "twitter", "github", "linkedin", "tools", "toys", "index", "tags", "feed", "recent posts", "categories", "meta", "log in", "create account", "wordpress.com", "elsewhere"}
    footer_triggers = ["Share this:", "Leave a Reply", "Leave a comment", "This site uses Akismet to reduce spam", "Post navigation", "Proudly powered by WordPress", "Click here for Disqus comments", "Comments are closed.", "Comments feed", "Entries feed", "Blog at WordPress.com."]
    for f in get_files():
        with open(f) as file: lines = file.read().splitlines()
        orig_len = len(lines)
        start_idx = 0
        for i in range(min(50, len(lines))):
            line = lines[i].strip().lower()
            if not line or line in header_junk or line == "search for:":
                start_idx = i + 1
                continue
            break
        end_idx = len(lines)
        for i in range(start_idx, len(lines)):
            if any(trigger in lines[i].strip() for trigger in footer_triggers):
                if i > len(lines) / 2:
                    end_idx = i
                    break
        if start_idx > 0 or end_idx < len(lines):
            new_c = "\n".join(lines[start_idx:end_idx]).strip() + "\n"
            open(f, 'w').write(new_c)
            print(f"Trimmed {f}: dropped {start_idx} from top, {len(lines) - end_idx} from bottom.")

def do_replace_images(args):
    try:
        from PIL import Image
    except ImportError:
        print("PIL required for image replacement.")
        sys.exit(1)

    files = get_files()
    urls = set(re.findall(r'https?://[^\s"]+\.(?:png|jpg|jpeg|gif|webp|svg)', "".join(open(f).read() for f in files), re.I))
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
        payload = {"model": "google/gemma-4-31B-it", "messages": [{"role": "user", "content": [{"type": "text", "text": "describe this in enough detail that i could recreate it exactly."}, {"type": "image_url", "image_url": {"url": f"data:{mime};base64,{b64}"}}]}], "max_tokens": 4096}
        for attempt in range(6):
            try:
                req = urllib.request.Request("https://llms.intrinsicapi.com/v1/chat/completions", data=json.dumps(payload).encode(), headers={"Authorization": "Bearer o30ya7yeDe_p", "Content-Type": "application/json"})
                resp = json.loads(urllib.request.urlopen(req, timeout=180).read().decode())
                content = resp.get('choices', [{}])[0].get('message', {}).get('content', '')
                if isinstance(content, list): content = "\n".join(str(x.get('text', '')) for x in content)
                cache[u] = f"[IMAGE DESCRIPTION: {content.strip()}]"
                print(f"OK {u}")
                return
            except Exception as e:
                if "429" in str(e):
                    time.sleep(2 ** attempt)
                    continue
                cache[u] = "FAILED"
                print(f"FAILED {u}: {e}")
                return
        cache[u] = "FAILED"
        print(f"FAILED {u}: timeout")

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

def do_clean_misc(args):
    def repl(m):
        desc = m.group(1).strip().replace('\n', ' ').replace('*', '').replace('`', '').replace('#', '')
        desc = re.sub(r'\s+', ' ', desc)
        if len(desc) > 600: desc = desc[:597] + '...'
        return f"[IMAGE DESCRIPTION: {desc}]"

    for f in get_files():
        c = open(f).read()
        orig = c
        c = re.sub(r'https?://[^\s"]*gravatar\.com[^\s"]*', '', c)
        c = re.sub(r'https?://[^\s"]*wp\.com[^\s"]*', '', c)
        c = re.sub(r'https?://[^\s"]*twemoji[^\s"]*', '', c)
        c = re.sub(r'https?://[^\s]+\.(png|jpg|jpeg|gif|svg|webp)', '', c, flags=re.IGNORECASE)
        c = re.sub(r'\[IMAGE DESCRIPTION:\s*(.*?)\s*\]', repl, c, flags=re.DOTALL)
        c = re.sub(r'\nIMAGES\s*$', '\n', c)
        c = re.sub(r'\n{3,}', '\n\n', c)
        if c != orig:
            open(f, 'w').write(c)
            print(f"Cleaned up misc in {f}")

def do_common_lines(args):
    lines_count = collections.defaultdict(int)
    for f in get_files():
        seen_in_file = set()
        for line in open(f).readlines():
            line = line.strip()
            if len(line) > 5 and line not in seen_in_file:
                seen_in_file.add(line)
                lines_count[line] += 1
    for line, count in sorted(lines_count.items(), key=lambda x: x[1], reverse=True):
        if count >= 3: print(f"{count:4d} {line}")

if __name__ == '__main__':
    help_text = """# Strip webcat headers, footers, and artifacts
./cli.py fix-webcat article.txt

# Use LLM to find and trim article headers/footers
./cli.py ai-trim article.txt

# Use hardcoded rules to trim navigation and footers
./cli.py heuristic-trim post.txt

# Download and describe images via Gemini Vision
./cli.py replace-images page.txt

# Remove avatars, tracking pixels, and clean up descriptions
./cli.py clean-misc messy_file.txt

# Find and print frequently repeated lines across files
./cli.py common-lines *.txt"""

    if len(sys.argv) < 2 or sys.argv[1] in ('help', '-h', '--help'):
        print(help_text)
        sys.exit(0)

    func_name = 'do_' + sys.argv[1].replace('-', '_')
    if func_name in globals():
        globals()[func_name](None)
    else:
        print(help_text)
        sys.exit(1)
