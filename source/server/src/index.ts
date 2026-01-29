import express from "express";
import type { Express, Request, Response } from "express";

const PORT: number = 8000;
const app: Express = express();

function rootHandler(request: Request, response: Response): void {
    response.send("Hello world!");
}

function listenHandler(): void {
    console.log(`Server listening on Port: ${PORT}`);    
}

app.get("/", rootHandler);
app.listen(PORT, listenHandler);
