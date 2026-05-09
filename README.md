<div align="center">

<style>
.terminal-frame {
  width: 100%;
  max-width: 920px;
  margin: 0 auto;
  padding: 0;
  background: #000;
  border: 2px solid #ff0000;
  box-shadow: 0 0 30px #ff0000, 0 0 60px #8b0000, inset 0 0 20px #ff0000;
  position: relative;
  overflow: hidden;
  animation: flicker 2s infinite;
}
.terminal-frame::before {
  content: "";
  position: absolute;
  top: 0; left: 0;
  width: 100%; height: 100%;
  background: repeating-linear-gradient(0deg, rgba(255,0,0,0.03) 0px, rgba(255,0,0,0.03) 2px, transparent 2px, transparent 4px);
  pointer-events: none;
  z-index: 2;
  animation: scanlines 0.5s linear infinite;
}
.terminal-frame::after {
  content: "◈ ROOT TERMINAL ◈";
  position: absolute;
  top: -10px; left: 50%;
  transform: translateX(-50%);
  background: #000;
  color: #ff0000;
  font-family: 'Fira Code', monospace;
  font-size: 10px;
  font-weight: 900;
  padding: 2px 10px;
  letter-spacing: 3px;
  z-index: 3;
  text-shadow: 0 0 10px #ff0000;
  border-left: 1px solid #ff0000;
  border-right: 1px solid #ff0000;
}
@keyframes flicker {
  0%, 100% { opacity: 1; }
  8% { opacity: 0.9; }
  10% { opacity: 1; }
  20% { opacity: 0.8; }
  22% { opacity: 1; }
  40% { opacity: 0.95; }
  42% { opacity: 1; }
  50% { opacity: 0.9; }
  52% { opacity: 1; }
  70% { opacity: 1; }
  72% { opacity: 0.7; }
  74% { opacity: 1; }
  90% { opacity: 0.85; }
  92% { opacity: 1; }
}
@keyframes scanlines {
  0% { background-position: 0 0; }
  100% { background-position: 0 100%; }
}
</style>

<div class="terminal-frame">
<img src="https://readme-typing-svg.demolab.com?font=Fira+Code&weight=900&size=10&duration=1000&pause=0&color=FF0000&center=true&vCenter=true&width=900&height=600&lines=%5BBOOT%5D+BIOS+handoff+to+bootloader;%5BBOOT%5D+Loading+initramfs...;%5BKERNEL%5D+Decompressing+vmlinuz...;%5BKERNEL%5D+Initializing+memory+management;%5BKERNEL%5D+Starting+process+scheduler;%5BINIT%5D+Systemd+starting+services;%5BINIT%5D+Network+reachable;%5BSSH%5D+SSHD+started+on+port+1337;%5BHIDE%5D+Process+hidden+from+ps%2Ftop%2Flsof;%5BHIDE%5D+Kernel+module+concealed+from+lsmod;%5BHOOK%5D+Syscall+table+patched%3A+sys_read%2C+sys_write%2C+sys_open;%5BHOOK%5D+Interrupt+descriptor+table+redirected;%5BROOT%5D+Privilege+escalation+to+ring+0;%5BROOT%5D+UID%2FGID+set+to+0+%28root%29;%5BMEM%5D+Direct+memory+access+enabled;%5BMEM%5D+Kernel+heap+spray+prepared;%5BDEPLOY%5D+Loading+x86%2F64+Assembly+payload;%5BDEPLOY%5D+Assembly+shellcode+injected+into+kernel+space;%5BDEPLOY%5D+Loading+C+exploit+chain;%5BDEPLOY%5D+C+rootkit+attached+to+sys_call_table;%5BDEPLOY%5D+Loading+C%2B%2B+kernel+module;%5BDEPLOY%5D+C%2B%2B+virtual+functions+hooked;%5BDEPLOY%5D+Loading+Rust+memory-safe+backdoor;%5BDEPLOY%5D+Rust+ownership+bypassed;%5BDEPLOY%5D+Loading+Go+reverseshell;%5BDEPLOY%5D+Go+goroutine+spawned+on+all+CPUs;%5BDEPLOY%5D+Loading+C%23+managed+payload;%5BDEPLOY%5D+C%23+CLR+host+compromised;%5BDEPLOY%5D+Loading+Java+bytecode+injection;%5BDEPLOY%5D+Java+JVM+ti+agent+installed;%5BDEPLOY%5D+Loading+Python+interpreter+hijack;%5BDEPLOY%5D+Python+C+extension+rootkit+loaded;%5BDEPLOY%5D+Loading+JavaScript%2FTypeScript+Node.js+addon;%5BDEPLOY%5D+Node.js+V8+engine+owned;%5BDEPLOY%5D+React%2FNext.js+frontend+control+established;%5BINFRA%5D+Docker+daemon+compromised;%5BINFRA%5D+Kubernetes+API+server+hooked;%5BINFRA%5D+Terraform+state+manipulated;%5BINFRA%5D+AWS%2FAzure+IAM+roles+assumed;%5BDATA%5D+SQL+databases+dumped;%5BDATA%5D+NoSQL+collections+exfiltrated;%5BDATA%5D+Redis+keyspace+overwritten;%5BDATA%5D+Kafka+brokers+intercepted;%5BSPREAD%5D+Propagating+across+subnets;%5BSPREAD%5D+Lateral+movement+via+SSH+trust;%5BSPREAD%5D+All+nodes+in+cluster+owned;%5BFIN%5D+SYSTEM+COMPROMISED;%5BFIN%5D+NO+LOGS+NO+TRACE+NO+MERCY;%5BFIN%5D+FULL-METAL+STACK+DEPLOYED;%5BFIN%5D+hesam-oxe+OWNER+OF+ALL+LAYERS" alt="heavy-terminal-slider" />
</div>

</div>
