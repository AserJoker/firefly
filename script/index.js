let index = 0;
label: while (index < 10) {
  try {
    print(index);
    index++;
    break label;
  } finally {
    print(234);
  }
}
