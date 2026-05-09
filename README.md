<div align="center">

<svg width="900" height="600" viewBox="0 0 900 600" xmlns="http://www.w3.org/2000/svg">
  <defs>
    <linearGradient id="bg" x1="0" y1="0" x2="0" y2="1">
      <stop offset="0%" stop-color="#0a0a0a"/>
      <stop offset="100%" stop-color="#000000"/>
    </linearGradient>
    <linearGradient id="border-glow" x1="0" y1="0" x2="1" y2="1">
      <stop offset="0%" stop-color="#ff0000"/>
      <stop offset="50%" stop-color="#8b0000"/>
      <stop offset="100%" stop-color="#ff0000"/>
    </linearGradient>
    <filter id="glow">
      <feGaussianBlur stdDeviation="3" result="blur"/>
      <feMerge><feMergeNode in="blur"/><feMergeNode in="SourceGraphic"/></feMerge>
    </filter>
    <filter id="heavy-glow">
      <feGaussianBlur stdDeviation="6" result="blur"/>
      <feMerge><feMergeNode in="blur"/><feMergeNode in="blur"/><feMergeNode in="SourceGraphic"/></feMerge>
    </filter>
    <filter id="scanline">
      <feColorMatrix type="saturate" values="0.5"/>
    </filter>
  </defs>

  <!-- Background -->
  <rect width="900" height="600" fill="url(#bg)"/>
  
  <!-- Scanlines -->
  <g opacity="0.05">
    <line x1="0" y1="0" x2="900" y2="0" stroke="#ff0000" stroke-width="1"/>
    <line x1="0" y1="3" x2="900" y2="3" stroke="#ff0000" stroke-width="1"/>
    <line x1="0" y1="6" x2="900" y2="6" stroke="#ff0000" stroke-width="1"/>
    <line x1="0" y1="9" x2="900" y2="9" stroke="#ff0000" stroke-width="1"/>
    <line x1="0" y1="12" x2="900" y2="12" stroke="#ff0000" stroke-width="1"/>
  </g>

  <!-- Border -->
  <rect x="10" y="10" width="880" height="580" fill="none" stroke="url(#border-glow)" stroke-width="2" opacity="0.8">
    <animate attributeName="opacity" values="0.8;0.4;0.8" dur="0.5s" repeatCount="indefinite"/>
  </rect>

  <!-- Header -->
  <text x="450" y="45" text-anchor="middle" font-family="monospace" font-size="14" font-weight="bold" fill="#ff0000" filter="url(#glow)">
    ╔══════════════════════════════ ROOT TERMINAL ══════════════════════════════╗
  </text>

  <!-- Terminal content -->
  <g font-family="monospace" font-size="11" fill="#ff0000">
    <text x="30" y="75" filter="url(#glow)">┌──(root㉿kernel)-[~]</text>
    <text x="30" y="93" filter="url(#glow)">└─$ ./deploy_payload.sh --full-stack --no-mercy</text>
    
    <text x="30" y="120" fill="#ff4444">[BOOT] Booting kernel... OK</text>
    <text x="30" y="138" fill="#ff4444">[KERNEL] vmlinuz decompressed... OK</text>
    <text x="30" y="156" fill="#ff4444">[INIT] Systemd services online... OK</text>
    <text x="30" y="174" fill="#ff4444">[SSH] Port 1337 open... OK</text>
    
    <text x="30" y="201" fill="#ff0000">[HOOK] Syscall table patched — 318/318 syscalls owned</text>
    <text x="30" y="219" fill="#ff0000">[HOOK] IDT redirected — all interrupts intercepted</text>
    <text x="30" y="237" fill="#ff0000">[HIDE] Process 1337 hidden from ps, top, lsof, /proc</text>
    <text x="30" y="255" fill="#ff0000">[ROOT] Privilege escalation: RING 3 → RING 0 — SUCCESS</text>
    
    <text x="30" y="282" fill="#ff4444">[DEPLOY] Loading Assembly payload (x86/x64)... INJECTED</text>
    <text x="30" y="300" fill="#ff4444">[DEPLOY] Loading C rootkit... ATTACHED TO SYSCALLS</text>
    <text x="30" y="318" fill="#ff4444">[DEPLOY] Loading C++ vtable hooks... INSTALLED</text>
    <text x="30" y="336" fill="#ff4444">[DEPLOY] Loading Rust memory backdoor... MAPPED</text>
    <text x="30" y="354" fill="#ff4444">[DEPLOY] Loading Go goroutine shell... SPAWNED x16 CPU</text>
    
    <text x="30" y="381" fill="#ff0000">[DEPLOY] Loading C# CLR host exploit... COMPROMISED</text>
    <text x="30" y="399" fill="#ff0000">[DEPLOY] Loading Java JVM TI agent... INJECTED</text>
    <text x="30" y="417" fill="#ff0000">[DEPLOY] Loading Python C extension rootkit... LOADED</text>
    
    <text x="30" y="444" fill="#ff4444">[DEPLOY] Loading Node.js/V8 addon... ENGINE OWNED</text>
    <text x="30" y="462" fill="#ff4444">[DEPLOY] React fiber tree... CONTROLLED</text>
    <text x="30" y="480" fill="#ff4444">[DEPLOY] Next.js middleware... INTERCEPTED</text>
    
    <text x="30" y="507" fill="#ff0000">[INFRA] Docker daemon socket owned — /var/run/docker.sock</text>
    <text x="30" y="525" fill="#ff0000">[INFRA] Kubernetes API hooked — etcd keys dumped</text>
    <text x="30" y="543" fill="#ff0000">[INFRA] Terraform state modified — AWS/Azure owned</text>
    
    <text x="30" y="570" fill="#ff4444">[DATA] PostgreSQL/MySQL dumped — MongoDB exfiltrated</text>
    <text x="30" y="588" fill="#ff4444">[DATA] Redis FLUSHALL — Kafka brokers intercepted</text>
  </g>

  <!-- Status Bar -->
  <rect x="10" y="580" width="880" height="20" fill="#ff0000" opacity="0.1"/>
  <text x="20" y="594" font-family="monospace" font-size="10" fill="#ff0000" filter="url(#heavy-glow)">
    hesam-oxe@kali:~# 
    <animate attributeName="opacity" values="1;0;1" dur="1s" repeatCount="indefinite"/>
    _
  </text>
  <text x="870" y="594" text-anchor="end" font-family="monospace" font-size="10" fill="#ff0000">THREAT LVL: CRITICAL</text>
</svg>

</div>
