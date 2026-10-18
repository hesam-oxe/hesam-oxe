#!/usr/bin/env bash
echo "=== BEHEMOTH benchmark ==="
cat > /tmp/fib30.bst << 'EOF'
fn fib(n: int) -> int {
    if (n < 2) { return n; }
    return fib(n-1) + fib(n-2);
}
fn main() -> int {
    print(fib(30));
    return 0;
}
EOF
echo "--- fib(30) ---"
./behemoth bench /tmp/fib30.bst
echo
cat > /tmp/loop.bst << 'EOF'
fn main() -> int {
    let i: int = 0;
    let s: int = 0;
    while (i < 1000000) {
        s = s + i;
        i = i + 1;
    }
    print(s);
    return 0;
}
EOF
echo "--- loop 1e6 ---"
./behemoth bench /tmp/loop.bst
