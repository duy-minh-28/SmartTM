"use strict";
Object.defineProperty(exports, "__esModule", { value: true });
const isBrowser = (typeof window !== 'undefined' && typeof window.document !== 'undefined') ||
    (typeof self !== 'undefined' && typeof self.postMessage === 'function');
exports.default = isBrowser;
//# sourceMappingURL=is-browser.js.map