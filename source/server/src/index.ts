import express from "express";
import type { Express, Request, Response } from "express";
import path from "path";
import { writeFile, readFile, rm } from "fs/promises";
import { spawn } from "child_process";

const PORT: number = 8000;
const app: Express = express();

app.use(express.json({ limit: "1mb" }));

function rootHandler(request: Request, response: Response): void {
    response.sendFile(path.join(__dirname, "..", "public", "index.html"));
}

function listenHandler(): void {
    console.log(`Server listening on Port: ${PORT}`);
}

function runCompose(projectRoot: string, args: string[]): Promise<number> {
    return new Promise((resolve, reject) => {
        const composeFile = path.join(projectRoot, "docker-compose.yml");
        const child = spawn("docker", ["compose", "-f", composeFile, ...args], {
            cwd: projectRoot,
            stdio: "inherit",
        });
        child.on("error", reject);
        child.on("close", (code) => resolve(code ?? 1));
    });
}

app.get("/", rootHandler);
app.use("/public", express.static(path.join(__dirname, "..", "public")));

app.post("/run", async (request: Request, response: Response): Promise<void> => {
    try {
        const code = typeof request.body?.code === "string" ? request.body.code : "";
        const projectRoot = path.resolve(__dirname, "..", "..", "..");
        const sourcePath = path.join(projectRoot, "source", "test.txt");
        const outputDir = path.join(projectRoot, "source", "shared_output_resources");

        await writeFile(sourcePath, code, "utf8");
        await writeFile(path.join(outputDir, "last_request.txt"), code, "utf8");

        await Promise.all([
            rm(path.join(outputDir, "result.txt"), { force: true }),
            rm(path.join(outputDir, "stderr.txt"), { force: true }),
            rm(path.join(outputDir, "exit_code.txt"), { force: true }),
            rm(path.join(outputDir, "out.asm"), { force: true }),
        ]);

        await runCompose(projectRoot, ["build"]);
        await runCompose(projectRoot, ["run", "--rm", "yox47"]);

        const resultPath = path.join(outputDir, "result.txt");
        const stderrPath = path.join(outputDir, "stderr.txt");
        const exitPath = path.join(outputDir, "exit_code.txt");

        const [result, stderr, exitCode] = await Promise.all([
            readFile(resultPath, "utf8").catch(() => ""),
            readFile(stderrPath, "utf8").catch(() => ""),
            readFile(exitPath, "utf8").catch(() => ""),
        ]);

        response.json({
            ok: true,
            result,
            stderr,
            exitCode: exitCode.trim(),
        });
    } catch (err) {
        response.status(500).json({ ok: false, error: String(err) });
    }
});

app.listen(PORT, listenHandler);
