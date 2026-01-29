FROM ubuntu:24.04

RUN apt-get update \
  && apt-get install -y --no-install-recommends build-essential nasm \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

WORKDIR /app
RUN chmod +x source/scripts/build.sh source/scripts/run.sh source/scripts/build_compiler.sh source/scripts/pipeline.sh
RUN mkdir -p /shared

CMD ["bash", "-lc", "source /app/source/.env && /app/source/scripts/pipeline.sh && cat /app/source/result.txt"]
