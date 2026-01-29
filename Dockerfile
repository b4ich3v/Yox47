FROM ubuntu:24.04

RUN apt-get update \
  && apt-get install -y --no-install-recommends build-essential nasm \
  && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

WORKDIR /app
RUN chmod +x source/scripts/build.sh source/scripts/run.sh source/scripts/build_compiler.sh source/scripts/pipeline.sh
RUN mkdir -p /app/source/shared_output_resources

CMD ["bash", "-lc", "tail -f /dev/null"]
