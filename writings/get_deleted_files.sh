pbpaste | while IFS= read -r f; do
  [ -z "$f" ] && continue
  echo ">>> $f"
  git show "HEAD~1:./$f" | headtail -n 50 --label "$f"
done
