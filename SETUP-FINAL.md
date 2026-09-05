# ☠️ FINAL — activation checklist (manual steps code cannot do)

## 1. Private counts (5 min)
1. GitHub → Settings → Developer settings → Personal access tokens → **Tokens (classic)** → Generate
2. Scopes: `repo`, `read:user`, `read:org`
3. Repo `hesam-oxe/hesam-oxe` → Settings → Secrets → Actions → New secret: `METRICS_TOKEN` = token
4. Re-run **Metrics** workflow. Without it, only public data is counted (graceful, nothing breaks).

## 2. Coding hours (3 min)
1. https://wakatime.com → account → API Key
2. Same secrets page → `WAKATIME_API_KEY` = key
3. Re-run **WakaTime** workflow. Editor plugins (VS Code etc.) make the chart real.

## 3. Blog feeds (2 min)
Edit `FEEDS.json` → put real RSS URLs in `feeds`, e.g.:
```json
{ "feeds": ["https://dev.to/feed/YOURNAME"], "max_items": 5 }
```
Push → **Blog RSS** workflow fills the block within 6h (or Run workflow now).

## 4. Off-GitHub proof (2 min)
- **LeetCode:** replace `hesam-oxe` placeholder in README with your username, or add card:
  `https://img.shields.io/badge/LeetCode-hesam--oxe-FFA116?style=for-the-badge&logo=leetcode`
- **StackOverflow:** replace `0000000` with your numeric user ID from your SO profile URL.

## 5. Pins + social (3 min, profile page)
1. Profile → Customize pins → pin 6 strongest repos (seatunnel, line, site, vyos + 2)
2. Repo → Settings → Social preview → upload 1280×640 banner (use `assets/glitch-name.svg` exported to PNG)
3. Profile → Set status: `🔥 forging weapons` + Profile README section is automatic (this repo).

## 6. Fortress site
Repo `hesam-oxe.github.io` → Settings → Pages → Deploy from **GitHub Actions** → visit `https://hesam-oxe.github.io`.
No custom domain needed. To add one later: `echo "yourdomain.com" > CNAME` + DNS CNAME → `hesam-oxe.github.io`.
