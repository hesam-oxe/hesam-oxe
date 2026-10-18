fn max(a: int, b: int) -> int {
    if (a > b) { return a; }
    return b;
}
fn main() -> int {
    print(max(3, 7));
    print(max(9, 2));
    return 0;
}
