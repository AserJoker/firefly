const arr = [1, 2, 3];
function test(...arr) {
  print(arr);
}
test("a", "b", "c", ...arr);
