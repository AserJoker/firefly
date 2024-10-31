declare module "document" {
  interface Node {
    setProperty: (key: string, value: unknown) => void;
    getName: () => string;
  }
  interface Window extends Node {
    setTitle: (title: string) => void;
  }
  function select(id: string): Node | null;
  function isWindow(node: Node): node is Window;
}
