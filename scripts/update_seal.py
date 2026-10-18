#!/usr/bin/env python3
# ═══════════════════════════════════════════════════════════════════════════
# SEAL OF RECORD — the repository signs its own README.
#
# On every push that touches README.md, this script:
#   1. hashes README.md with SHA-256
#   2. measures its Shannon entropy (bits/byte) and byte-value cardinality
#   3. stamps the digest into assets/seal.svg (carved panel, rotating ring)
#   4. exits quietly if the seal already matches (idempotent — no loops)
#
# The digest is a claim anyone can falsify: `sha256sum README.md` must equal
# what the seal displays. Edit the file outside CI and the seal breaks loudly.
# ═══════════════════════════════════════════════════════════════════════════
import hashlib, subprocess, sys, datetime, os, math

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
README = os.path.join(ROOT, "README.md")
SEAL = os.path.join(ROOT, "assets", "seal.svg")

data = open(README, "rb").read()
digest = hashlib.sha256(data).hexdigest()

freq = [0] * 256
for b in data:
    freq[b] += 1
H = 0.0
uniq = 0
for f in freq:
    if f:
        uniq += 1
        p = f / len(data)
        H -= p * math.log2(p)

try:
    commit = subprocess.check_output(
        ["git", "log", "-1", "--format=%H", "--", "README.md"],
        cwd=ROOT, text=True).strip()[:7]
except Exception:
    commit = "cache-miss"

stamp = datetime.datetime.now(datetime.timezone.utc).strftime("%Y-%m-%d")
si = digest[:32]
rows = [digest[i:i + 16] for i in range(0, 64, 16)]

FAM = "'Cascadia Code','Fira Code','JetBrains Mono',SFMono-Regular,Consolas,Menlo,monospace"
svg = f'''<svg xmlns="http://www.w3.org/2000/svg" width="1000" height="300" viewBox="0 0 1000 300" font-family="{FAM}" role="img" aria-label="seal of record — sha256 of README.md"><title>seal of record</title>
<defs>
<linearGradient id="gPanel" x1="0" y1="0" x2=".35" y2="1"><stop offset="0" stop-color="#131A25"/><stop offset=".45" stop-color="#0B1017"/><stop offset="1" stop-color="#05080D"/></linearGradient>
<linearGradient id="gSpec" x1="0" x2="1"><stop offset="0" stop-color="#DC143C"/><stop offset=".4" stop-color="#FFB000"/><stop offset=".7" stop-color="#00E5FF"/><stop offset="1" stop-color="#B026FF"/>
 <animateTransform attributeName="gradientTransform" type="translate" values="-1 0;1 0;-1 0" dur="8s" repeatCount="indefinite"/></linearGradient>
<radialGradient id="gVig" cx="50%" cy="45%" r="75%"><stop offset=".5" stop-color="#000" stop-opacity="0"/><stop offset="1" stop-color="#000" stop-opacity=".68"/></radialGradient>
<filter id="glow" x="-60%" y="-60%" width="220%" height="220%"><feGaussianBlur stdDeviation="3" result="b"/><feMerge><feMergeNode in="b"/><feMergeNode in="SourceGraphic"/></feMerge></filter>
</defs>
<rect width="1000" height="300" fill="url(#gPanel)"/>
<rect x="14" y="14" width="972" height="3" fill="url(#gSpec)" opacity=".9"/>
<rect x="14" y="283" width="972" height="3" fill="url(#gSpec)" opacity=".9"/>
<text x="500" y="52" text-anchor="middle" font-size="17" font-weight="800" letter-spacing="3" fill="url(#gSpec)">◈ SEAL OF RECORD</text>
<text x="500" y="72" text-anchor="middle" font-size="11" letter-spacing="2" fill="#8FA3BF">SHA-256 · README.md @ {commit} · re-stamped by CI on every push</text>
<g transform="translate(78,158)" filter="url(#glow)">
 <circle r="86" fill="none" stroke="#DC143C" stroke-width="1.4" opacity=".85"/>
 <circle r="76" fill="none" stroke="#00E5FF" stroke-width="1.2" stroke-dasharray="5 7" opacity=".9">
  <animateTransform attributeName="transform" type="rotate" from="0" to="360" dur="26s" repeatCount="indefinite"/></circle>
 <circle r="62" fill="none" stroke="#1B2533" stroke-width="10" opacity=".8"/>
 <circle r="8" fill="#DC143C" opacity=".9"><animate attributeName="opacity" values=".5;1;.5" dur="2.4s" repeatCount="indefinite"/></circle>
 <text y="-34" text-anchor="middle" font-size="12" font-weight="800" fill="#00E5FF" letter-spacing="1">MODULE'S</text>
 <text y="-20" text-anchor="middle" font-size="10" fill="#5A6B84">fingerprint</text>
 <text y="26" text-anchor="middle" font-size="13" font-weight="800" fill="#FFE9A0">SEALED</text>
 <text y="44" text-anchor="middle" font-size="9.5" fill="#8FA3BF">{stamp}</text>
</g>
<g transform="translate(232,0)">
 <text x="0" y="110" font-size="11" font-weight="800" fill="#FFB000" letter-spacing="2">$ sha256sum README.md</text>
 <text x="0" y="146" font-size="14.5" font-weight="700" fill="#00E5FF">{rows[0]}</text>
 <text x="0" y="170" font-size="14.5" font-weight="700" fill="#00E5FF">{rows[1]}</text>
 <text x="0" y="194" font-size="14.5" font-weight="700" fill="#00E5FF">{rows[2]}</text>
 <text x="0" y="218" font-size="14.5" font-weight="700" fill="#00E5FF">{rows[3]}</text>
 <text x="330" y="146" font-size="11" fill="#5A6B84">size     : {len(data):,} bytes</text>
 <text x="330" y="170" font-size="11" fill="#5A6B84">entropy  : {H:.4f} bits/byte</text>
 <text x="330" y="194" font-size="11" fill="#5A6B84">alphabet : {uniq}/256 byte values</text>
 <text x="330" y="218" font-size="11" fill="#8FA3BF">re-stamped : {stamp} (UTC)</text>
 <text x="0" y="252" font-size="10.5" fill="#DC143C">▸ verify, don't trust — run the left line yourself; a mismatch means the record was broken.</text>
</g>
<rect width="1000" height="300" fill="url(#gVig)" pointer-events="none"/>
</svg>'''

if os.path.exists(SEAL):
    old = open(SEAL, encoding="utf-8").read()
    if old == svg:
        print("seal unchanged — nothing to do")
        sys.exit(0)

open(SEAL, "w", encoding="utf-8").write(svg)
print(f"seal re-stamped: sha256={digest[:12]}… · {len(data):,} B · H={H:.4f} · {stamp}")
