# TCP Chat V1 — Project Context

## Goal

Tek istemcili bir C++ TCP client/server uygulamasi. Client newline ile sonlanan bir metin mesaji yollar; server mesaji okuyup newline ile sonlanan bir yanit doner.

## Source map

- `main.cpp`: TCP server
- `client.cpp`: TCP client
- `CMakeLists.txt`: `server` ve `client` executable target'lari
- `README.md`: kurulum, test ve sinirlar

## Build and run

```bash
cmake -S . -B build
cmake --build build
./build/server
./build/client
```

## Protocol and constraints

- TCP / IPv4 / port 5000
- Mesajlar `\n` ile sonlanir.
- Payload siniri 1024 byte'tir.
- `sendAll()` partial send'i, `recvLine()` partial recv'i ele alir.
- Tek istemci; thread, event loop, HTTP ve authentication kapsam disidir.

## Acceptance tests

1. Normal mesajlasma calisir ve birden fazla `recv()` chunk'i gorulur.
2. Server kapaliyken client `connect()` hatasiyla temiz kapanir.
3. Gecersiz IPv4 adresi `inet_pton()` tarafindan reddedilir.
4. Server yanit vermeden kapanirsa client `recv() == 0` durumunu isler.

## Change boundary

Mevcut V1'i multi-client veya HTTP projesine donusturme. Degisiklikler once protokol, hata akislari ve testlerle uyumlu olmali.
