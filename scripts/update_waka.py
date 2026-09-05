"""FINAL-A: WakaTime -> README between <!-- WAKA-START/END -->.
Needs WAKATIME_API_KEY secret. Absent => graceful skip placeholder, exit 0.
Stdlib only.
"""
import base64
import json
import os
import re
import urllib.request
from pathlib import Path

ROOT = Path(__file__).resolve().parents[1]
README = ROOT / "README.md"
START = "<!-- WAKA-START -->"
END = "<!-- WAKA-END -->"


def main():
    key = os.environ.get("WAKATIME_API_KEY", "").strip()
    text = README.read_text()
    if START not in text or END not in text:
        print("markers missing, skipping")
        return 0
    if not key:
        block = (f"{START}\n- ⌨️ _WakaTime not linked — add `WAKATIME_API_KEY` secret to enable coding-hours chart._\n{END}")
        new = re.sub(re.escape(START) + r".*?" + re.escape(END), lambda _: block, text, flags=re.DOTALL)
        if new != text:
            README.write_text(new)
            print("waka placeholder written")
        return 0
    try:
        auth = base64.b64encode(key.encode()).decode()
        req = urllib.request.Request(
            "https://wakatime.com/api/v1/users/current/stats/last_7_days",
            headers={"Authorization": f"Basic {auth}"},
        )
        with urllib.request.urlopen(req, timeout=20) as r:
            data = json.load(r)
        langs = (data.get("data") or {}).get("languages", [])[:5]
        if not langs:
            raise ValueError("empty stats")
        total = (data.get("data") or {}).get("human_readable_total", "N/A")
        rows = [f"- ⌨️ **{total}** last 7 days"]
        for l in langs:
            rows.append(f"  - {l.get('name')}: {l.get('text', '')} ({l.get('percent', 0):.1f}%)")
        block = f"{START}\n" + "\n".join(rows) + f"\n{END}"
    except Exception as e:
        print(f"waka failed: {e}")
        block = (f"{START}\n- ⌨️ _WakaTime unreachable — will retry nightly._\n{END}")
    new = re.sub(re.escape(START) + r".*?" + re.escape(END), lambda _: block, text, flags=re.DOTALL)
    if new != text:
        README.write_text(new)
        print("waka block updated")
    else:
        print("no change")
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
