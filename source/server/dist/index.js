"use strict";
var __importDefault = (this && this.__importDefault) || function (mod) {
    return (mod && mod.__esModule) ? mod : { "default": mod };
};
Object.defineProperty(exports, "__esModule", { value: true });
const express_1 = __importDefault(require("express"));
const PORT = 8000;
const app = (0, express_1.default)();
function rootHandler(request, response) {
    response.send("Hello world!");
}
function listenHandler() {
    console.log(`Server listening on Port: ${PORT}`);
}
app.get("/", rootHandler);
app.listen(PORT, listenHandler);
//# sourceMappingURL=index.js.map