function* test() {
  yield 1;
  yield 2;
  return 3;
}
const [a, b, c, d] = test();
print(a, b, c, d);
