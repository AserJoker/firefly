'use strict'
const obj = {};
Object.defineProperty(obj, "a", {
  configurable: true,
  writable: false,
  value: 123,
});
obj.a = 234;
