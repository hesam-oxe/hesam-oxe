"""FINAL-A: blog RSS -> README between <!-- BLOG-START --> and <!-- BLOG-END -->.
Stdlib only. No feeds configured or fetch fails => placeholder, exit 0.
"""
import json
import re
import sys
import urllib.request
import xml.etree.ElementTree as ET
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
README = ROOT / "README.md"
FEEDS = ROOT / "FEEDS.json"
START = "<!-- BLOG-START -->"
END = "<!-- BLOG-END -->"


def fetch_items(url, limit=3):
    req = urllib.request.Request(url, headers={"User-Agent": "hellbeast-final/1.0"})
    with urllib.request.urlopen(req, timeout=15) as r:
        data = r.read()
    root = ET.fromstring(data)
    items = []
    # RSS 2.0
    for it in root.iter("item"):
        t = it.findtext("title", "").strip()
        link = it.findtext("link", "").strip()
        if t and link:
            items.append((t, link))
        if len(items) >= limit:
            break
    # Atom
    if not items:
        ns = {"a": "http://www.w3.org/2005/Atom"}
        for e in root.findall("a:entry", ns):
            t = (e.findtext("a:title", "", namespaces=ns) or "").strip()
            l = e.find("a:link", ns)
            link = (l.get("href", "") if l is not None else "").strip()
            if t and link:
                items.append((t, link))
            if len(items) >= limit:
                break
    return items


def main():
    cfg = json.loads(FEEDS.read_text())
    feeds = cfg.get("feeds", []) or []
    max_items = int(cfg.get("max_items", 5))
    lines = []
    for url in feeds[:3]:
        try:
            lines.extend(fetch_items(url, limit=2))
        except Exception as e:
            print(f"feed failed {url}: {e}", file=sys.stderr)
    if not lines:
        block = (
            f"{START}\n"
            "- 🛰️ _No transmissions yet — add RSS URLs to `FEEDS.json` (see `FEEDS.json.example`)._\n"
            f"{END}"
        )
    else:
        seen = []
        for t, link in lines:
            if link not in [l for _, l in seen]:
                seen.append((t, link))
        seen = seen[:max_items]
        body = "\n".join(f"- 📡 [{t}]({link})" for t, link in seen)
        block = f"{START}\n{body}\n{END}"
    text = README.read_text()
    if START not in text or END not in text:
        print("markers missing, skipping", file=sys.stderr)
        return 0
    new = re.sub(re.escape(START) + r".*?" + re.escape(END), lambda _: block, text, flags=re.DOTALL)
    if new != text:
        README.write_text(new)
        print("README blog block updated")
    else:
        print("no change")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
