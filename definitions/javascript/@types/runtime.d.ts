declare module "runtime" {
  function on(event: "gameLoaded", callback: () => void): void;
  function on(event: "keydown", callback: (scancode: number) => void): void;
}
