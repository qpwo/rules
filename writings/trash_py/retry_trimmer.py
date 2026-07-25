import json, urllib.request, re, glob, os, sys, time
from concurrent.futures import ThreadPoolExecutor

api_url = "https://llms.intrinsicapi.com/v1/chat/completions"
headers = {"Authorization": "Bearer o30ya7yeDe_p", "Content-Type": "application/json"}

# Find files that need retry
retry_files = []
if os.path.exists("trim.log"):
    for line in open("trim.log"):
        if line.startswith("Error on ") or line.startswith("Invalid boundaries for ") or line.startswith("Failed to parse JSON for "):
            # Extract filename
            m = re.match(r'^(?:Error on |Invalid boundaries for |Failed to parse JSON for )([^:]+):', line)
            if m:
                retry_files.append(m.group(1))

print(f"Found {len(retry_files)} files to retry.")

def process_file(f):
    for attempt in range(3):
        try:
            with open(f) as file:
                lines = file.read().splitlines()

            if len(lines) < 50:
                return f"Skipped {f} (too short)"

            head_lines = lines[:150]
            tail_lines = lines[-150:]

            prompt = f"""
I am going to give you the first 150 lines and the last 150 lines of a web article scraped to text.
Many of these have navigation bars, headers, footers, comment sections, recent posts widgets, etc.
Your job is to identify the line number where the ACTUAL main content starts (usually the title or the author byline or the first real paragraph), and the line number where the ACTUAL main content ends (usually the end of the text, before comments, tags, related posts, footers).

First 150 lines:
{chr(10).join(f"{i}: {line}" for i, line in enumerate(head_lines))}

Last 150 lines:
{chr(10).join(f"{len(lines) - len(tail_lines) + i}: {line}" for i, line in enumerate(tail_lines))}

Output ONLY a JSON object: {{"start": <int>, "end": <int>}}
"""
            data = json.dumps({"model":"google/gemma-4-31B-it", "messages":[{"role":"user", "content":prompt}], "max_tokens":30, "temperature": 0.0}).encode()
            req = urllib.request.Request(api_url, data=data, headers=headers, method='POST')
            resp = urllib.request.urlopen(req, timeout=60).read()
            out = json.loads(resp)['choices'][0]['message']['content']
            m = re.search(r'\{\s*"start"\s*:\s*(\d+)\s*,\s*"end"\s*:\s*(\d+)\s*\}', out)
            if m:
                start, end = int(m.group(1)), int(m.group(2))
                if start < 150 and end >= len(lines) - 150 and start < end:
                    if start == 0 and end == len(lines) - 1:
                        return f"Unchanged {f}: {start} to {end}"
                    new_c = "\n".join(lines[start:end+1]) + "\n"
                    open(f, 'w').write(new_c)
                    return f"Trimmed {f}: {start} to {end}"
                else:
                    return f"Invalid boundaries for {f}: {start} to {end} (len={len(lines)})"
            else:
                return f"Failed to parse JSON for {f}: {out}"
        except Exception as e:
            time.sleep(2 * (attempt + 1))
            if attempt == 2:
                return f"Error on {f}: {e}"

with ThreadPoolExecutor(max_workers=3) as executor:
    results = executor.map(process_file, retry_files)
    for res in results:
        print(res)
