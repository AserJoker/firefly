function* gen() {
  yield 1;
  yield 2;
  return 2;
}
function* test() {
  let data = yield* gen();
  print(data);
}
const obj = test();
obj.next();
obj.next(1);
obj.next('a');
