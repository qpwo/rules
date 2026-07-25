import glob, os

files = glob.glob('*.txt')
files = [f for f in files if f not in ('.inputs.txt', '.urls.txt', 'articles.txt', 'urls.txt', 'next_file.txt')]

header_junk = {
    "home", "about", "blog", "archives", "search", "rss", "contact", "subscribe",
    "skip to content", "skip to primary content", "main menu", "menu", "twitter",
    "github", "linkedin", "tools", "toys", "index", "tags", "feed", "recent posts",
    "categories", "meta", "log in", "create account", "wordpress.com", "elsewhere"
}

footer_triggers = [
    "Share this:",
    "Leave a Reply",
    "Leave a comment",
    "This site uses Akismet to reduce spam",
    "Post navigation",
    "Proudly powered by WordPress",
    "Click here for Disqus comments",
    "Comments are closed.",
    "Comments feed",
    "Entries feed",
    "Blog at WordPress.com.",
]

for f in files:
    with open(f) as file:
        lines = file.read().splitlines()

    orig_len = len(lines)

    # 1. Trim header junk
    start_idx = 0
    for i in range(min(50, len(lines))):
        line = lines[i].strip().lower()
        if not line:
            start_idx = i + 1
            continue
        if line in header_junk or line == "search for:":
            start_idx = i + 1
            continue
        # If it's not a known junk line and not empty, we stop header trimming.
        # But wait, sometimes there are multiple junk lines separated by empty lines.
        # Let's be conservative: if we hit a non-junk, non-empty line, we stop.
        break

    # 2. Trim footer junk
    end_idx = len(lines)
    for i in range(start_idx, len(lines)):
        line_str = lines[i].strip()
        if any(trigger in line_str for trigger in footer_triggers):
            # To be safe, only cut if it's in the bottom half of the document
            if i > len(lines) / 2:
                end_idx = i
                break

    if start_idx > 0 or end_idx < len(lines):
        new_c = "\n".join(lines[start_idx:end_idx]) + "\n"
        # Optional: remove trailing empty lines
        new_c = new_c.strip() + "\n"
        open(f, 'w').write(new_c)
        print(f"Trimmed {f}: dropped {start_idx} from top, {len(lines) - end_idx} from bottom.")

