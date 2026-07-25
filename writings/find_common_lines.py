import glob, collections

lines_count = collections.defaultdict(int)
files = glob.glob('*.txt')
files = [f for f in files if f not in ('.inputs.txt', '.urls.txt', 'articles.txt', 'urls.txt', 'next_file.txt')]

for f in files:
    seen_in_file = set()
    for line in open(f).readlines():
        line = line.strip()
        if len(line) > 5 and line not in seen_in_file:
            seen_in_file.add(line)
            lines_count[line] += 1

for line, count in sorted(lines_count.items(), key=lambda x: x[1], reverse=True):
    if count >= 3:
        print(f"{count:4d} {line}")
