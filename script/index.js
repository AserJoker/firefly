import { select } from "document";
import { i18n } from "locale";
import { on } from "runtime";

/**
 *
 * @param {import("document").Node|null} node
 * @returns {node is import("document").Window}
 */
function isWindow(node) {
  return node && node.getName() == "window";
}

on("gameLoaded", () => {
  const window = select("main-window");
  if (isWindow(window)) {
    window.setTitle(i18n("duskland.title.text"));
  }
});
