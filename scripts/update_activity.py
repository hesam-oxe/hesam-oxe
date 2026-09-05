"""FINAL-A: recent public activity -> README between <!-- ACTIVITY-START/END -->.
Uses `gh api` (GITHUB_TOKEN provided by Actions). Fails soft => placeholder.
"""
import json
import re
import subprocess
import sys
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
README = ROOT / "README.md"
START = "<!-- ACTIVITY-START -->"
END = "<!-- ACTIVITY-END -->"


def run(cmd):
    p = subprocess.run(cmd, capture_output=True, text=True, timeout=30)
    if p.returncode != 0:
        raise RuntimeError(p.stderr.strip()[:300])
    return p.stdout


def main():
    try:
        owner = run(["gh", "api", "user", "--jq", ".login"]).strip() or "hesam-oxe"
    except Exception:
        owner = "hesam-oxe"
    lines = []
    try:
        raw = run(["gh", "api", f"users/{owner}/events/public", "--paginate", "-q",
                   ".[] | select(.type==\"PushEvent\" or .type==\"PullRequestEvent\" or .type==\"IssuesEvent\") | [.type, .repo.name, .created_at] | @tsv"])
        for row in raw.splitlines()[:5]:
            parts = row.split("\t")
            if len(parts) == 3:
                typ, repo, ts = parts
                icon = {"PushEvent": "🔥", "PullRequestEvent": "⚔️", "IssuesEvent": "🐛"}.get(typ, "•")
                lines.append(f"- {icon} `{typ}` @ [{repo}](https://github.com/{repo}) — {ts[:10]}")
    except Exception as e:
        print(f"activity fetch failed: {e}", file=sys.stderr)
    if not lines:
        block = (f"{START}\n- 🛰️ _Activity feed warming up — runs nightly via Actions._\n{END}")
    else:
        block = f"{START}\n" + "\n".join(lines) + f"\n{END}"
    text = README.read_text()
    if START not in text or END not in text:
        print("markers missing, skipping", file=sys.stderr)
        return 0
    new = re.sub(re.escape(START) + r".*?" + re.escape(END), lambda _: block, text, flags=re.DOTALL)
    if new != text:
        README.write_text(new)
        print("README activity block updated")
    else:
        print("no change")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
