FROM ubuntu:20.04

RUN sed -i 's/archive.ubuntu.com/mirrors.tuna.tsinghua.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/security.ubuntu.com/mirrors.tuna.tsinghua.edu.cn/g' /etc/apt/sources.list && \
    sed -i 's/archive.canonical.com/mirrors.tuna.tsinghua.edu.cn/g' /etc/apt/sources.list

RUN apt-get update && apt-get install -y \
    gcc \
    g++ \
    make \
    clang-format \
    git \
    python3 \
    python3-pip \
    python3-venv

RUN pip3 install --upgrade pip && \
    pip3 install pipx && \
    pipx install pytest

COPY . /app

WORKDIR /app

CMD ["/bin/bash"]