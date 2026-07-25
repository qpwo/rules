import json
cache = json.load(open('url_cache.json'))
for k, v in cache.items():
    if v.startswith('[IMAGE DESCRIPTION:'):
        desc = v[19:-1].strip().replace('\n', ' ').replace('*', '').replace('`', '').replace('#', '')
        if len(desc) > 600: desc = desc[:597] + '...'
        cache[k] = f"[IMAGE DESCRIPTION: {desc}]"
json.dump(cache, open('url_cache.json', 'w'))
