```markdown
<div align="center">

<style>
@keyframes glitch-slide {
  0% { transform: translateX(100%); opacity: 0; filter: hue-rotate(0deg); }
  3% { transform: translateX(0); opacity: 1; filter: hue-rotate(0deg); }
  20% { transform: translateX(0); opacity: 1; filter: hue-rotate(0deg); }
  23% { transform: translateX(-5px); opacity: 0.8; filter: hue-rotate(90deg); }
  25% { transform: translateX(0); opacity: 1; filter: hue-rotate(0deg); }
  33% { transform: translateX(0); opacity: 1; filter: hue-rotate(0deg); }
  36% { transform: translateX(-100%); opacity: 0; filter: hue-rotate(360deg); }
  100% { transform: translateX(-100%); opacity: 0; filter: hue-rotate(360deg); }
}
@keyframes scanline {
  0% { background-position: 0 0; }
  100% { background-position: 0 100%; }
}
@keyframes flicker {
  0%, 19%, 21%, 23%, 25%, 54%, 56%, 100% { opacity: 1; }
  20%, 24%, 55% { opacity: 0.2; }
}
.slider-container {
  width: 100%;
  max-width: 850px;
  height: 70px;
  overflow: hidden;
  position: relative;
  background: #000000;
  border: 3px solid #FF0000;
  box-shadow: 0 0 30px #FF0000, 0 0 60px #FF0000, 0 0 90px #FF0000, inset 0 0 30px #FF0000, 0 0 120px #8B0000;
  margin: 20px auto;
  animation: flicker 3s infinite;
}
.slider-container::before {
  content: "";
  position: absolute;
  top: 0;
  left: 0;
  width: 100%;
  height: 100%;
  background: repeating-linear-gradient(0deg, transparent, transparent 2px, rgba(255,0,0,0.03) 2px, rgba(255,0,0,0.03) 4px);
  animation: scanline 0.5s linear infinite;
  z-index: 2;
  pointer-events: none;
}
.slider-container::after {
  content: "◈ CLASSIFIED TRANSMISSION ◈";
  position: absolute;
  top: -12px;
  left: 50%;
  transform: translateX(-50%);
  background: #000000;
  color: #FF0000;
  font-family: 'Fira Code', monospace;
  font-size: 10px;
  font-weight: 900;
  padding: 0 10px;
  letter-spacing: 3px;
  z-index: 3;
  text-shadow: 0 0 10px #FF0000;
  border-left: 2px solid #FF0000;
  border-right: 2px solid #FF0000;
}
.slider-track {
  position: absolute;
  width: 100%;
  height: 100%;
  display: flex;
  align-items: center;
  justify-content: center;
}
.slide {
  position: absolute;
  width: 100%;
  text-align: center;
  font-family: 'Fira Code', monospace;
  font-weight: 900;
  font-size: 19px;
  color: #FF0000;
  text-shadow: 0 0 10px #FF0000, 0 0 20px #FF0000, 0 0 40px #FF0000, 0 0 80px #FF0000, 2px 2px 0 #000;
  letter-spacing: 2px;
  animation: glitch-slide 16s infinite;
}
.slide:nth-child(1) { animation-delay: 0s; }
.slide:nth-child(2) { animation-delay: 4s; }
.slide:nth-child(3) { animation-delay: 8s; }
.slide:nth-child(4) { animation-delay: 12s; }
.slide-decoration {
  color: #8B0000;
  font-size: 22px;
  margin: 0 15px;
  text-shadow: 0 0 15px #FF0000;
}
</style>

<div class="slider-container">
  <div class="slider-track">
    <div class="slide">☠️ <span class="slide-decoration">◈</span> KERNEL COMPROMISED — ROOT ACCESS GRANTED <span class="slide-decoration">◈</span> ☠️</div>
    <div class="slide">⚡ <span class="slide-decoration">◈</span> ASM → C → C++ → Rust → Go → C# → Java → Python → JS/TS <span class="slide-decoration">◈</span> ⚡</div>
    <div class="slide">🔴 <span class="slide-decoration">◈</span> FULL-METAL STACK — ALL LAYERS OWNED <span class="slide-decoration">◈</span> 🔴</div>
    <div class="slide">⚠️ <span class="slide-decoration">◈</span> THREAT LEVEL: CRITICAL — UNCONTAINED — NO MERCY <span class="slide-decoration">◈</span> ⚠️</div>
  </div>
</div>

<br>
<br>

![glitch](https://user-images.githubusercontent.com/74038190/212284158-e840e285-664b-44d7-b79f-e264b5e54825.gif)

# ☠️ H E S A M · J A M A L I ☠️

[![Typing SVG](https://readme-typing-svg.demolab.com?font=Fira+Code&weight=900&size=18&duration=1500&pause=300&color=FF0000&center=true&vCenter=true&width=800&lines=ROOT%40KERNEL%3A~%24+whoami;hesam-oxe%40FULL-METAL-STACK;ACCESS_LEVEL%3A+UNRESTRICTED;THREAT_LEVEL%3A+CRITICAL;KERNEL_COMPROMISED%3A+TRUE;ALL_SYSTEMS_OWNED)](https://git.io/typing-svg)

```
████████████████████████████████████████████████████████████████████████████████
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓ C L A S S I F I E D   O P E R A T O R ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
████████████████████████████████████████████████████████████████████████████████
```

| 🔴 LOW-LEVEL | 🟡 MID-LEVEL | 🟢 HIGH-LEVEL |
|:---:|:---:|:---:|
| `x86/x64 Assembly` | `C · C++` | `Python · JS · TS` |
| `Rust` | `C# · Java` | `React · Next.js` |
| `Go` | `Shell · Bash` | `Node.js · Django` |
| `Kernel Modules` | `Embedded C` | `GraphQL · REST` |

| ⚫ INFRA | ⚫ DEVOPS | ⚫ DATA |
|:---:|:---:|:---:|
| `Docker · K8s` | `CI/CD · Actions` | `SQL · NoSQL` |
| `Terraform` | `Ansible` | `Redis · Kafka` |
| `AWS · Azure` | `Helm · ArgoCD` | `ELK · Prometheus` |

```
████████████████████████████████████████████████████████████████████████████████
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  M I S S I O N   L O G  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
████████████████████████████████████████████████████████████████████████████████
```

[![GitHub Stats](https://github-readme-stats.vercel.app/api?username=hesam-oxe&show_icons=true&theme=codeSTACKr&hide_border=true&bg_color=000000&title_color=FF0000&icon_color=FF0000&text_color=FF0000&border_color=FF0000)](https://github.com/hesam-oxe)

[![Top Languages](https://github-readme-stats.vercel.app/api/top-langs/?username=hesam-oxe&layout=compact&theme=codeSTACKr&hide_border=true&bg_color=000000&title_color=FF0000&text_color=FF0000&border_color=FF0000)](https://github.com/hesam-oxe)

[![GitHub Streak](https://streak-stats.demolab.com?user=hesam-oxe&theme=black-ice&hide_border=true&background=000000&ring=FF0000&fire=FF0000&currStreakLabel=FF0000&sideLabels=FF0000&dates=FF0000&currStreakNum=FF0000&sideNums=FF0000)](https://git.io/streak-stats)

```
████████████████████████████████████████████████████████████████████████████████
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  S Y S T E M  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
████████████████████████████████████████████████████████████████████████████████
```

```
[████████████████████] KERNEL     : ROOT COMPROMISED
[████████████████████] RING 0     : ACCESS GRANTED
[████████████████████] STACK      : FULL METAL DEPLOYED
[████████████████████] THREAT LVL : CRITICAL - UNCONTAINED
[████████████░░░░░░░░] MERCY MODE : OFFLINE
```

```
                      ASM → C → C++ → Rust → Go → C# → Java → Python → JS/TS
                      ─────────────────────────────────────────────────────
                      BARE METAL ⚡ KERNEL ⚡ SYSTEMS ⚡ BACKEND ⚡ FRONTEND
                      ─────────────────────────────────────────────────────
                                ALL LAYERS COMPROMISED
```

```
████████████████████████████████████████████████████████████████████████████████
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  C O N T A C T  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
████████████████████████████████████████████████████████████████████████████████
```

[![GitHub](https://img.shields.io/badge/⛓️_GITHUB-hesam--oxe-000000?style=for-the-badge&logo=github&logoColor=FF0000&labelColor=000000&borderColor=FF0000)](https://github.com/hesam-oxe)

```
████████████████████████████████████████████████████████████████████████████████
▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓  W A R N I N G  ▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓▓
████████████████████████████████████████████████████████████████████████████████
```

> `$ root@kernel:~# SYSTEM COMPROMISED — NO LOGS — NO TRACE — NO MERCY`

![glitch](https://user-images.githubusercontent.com/74038190/212284158-e840e285-664b-44d7-b79f-e264b5e54825.gif)

```
                            [ SYSTEM OWNED — hesam-oxe ]
```

</div>
```
