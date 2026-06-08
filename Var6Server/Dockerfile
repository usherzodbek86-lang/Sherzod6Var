FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && apt-get install -y \
    qt6-base-dev \
    qt6-base-dev-tools \
    libqt6sql6-sqlite \
    cmake \
    g++ \
    make \
    && rm -rf /var/lib/apt/lists/*

WORKDIR /app
COPY . .

RUN qmake6 Var6Server.pro && make -j$(nproc)

EXPOSE 9000

CMD ["./Var6Server"]
