# TCP Chat V1

Tek istemcili, TCP tabanli bir C++ client/server denemesi. Client newline ile sonlanan bir metin mesaji gonderir; server mesaji okuyup newline ile sonlanan bir yanit doner.

## Neler ogreniliyor?

- TCP socket yasam dongusu: `socket -> bind -> listen -> accept -> recv -> send -> close`
- Client akisi: `socket -> connect -> send -> recv -> close`
- `inet_pton()` ile IPv4 adresini socket yapisina cevirme
- `htons()` ile portu network byte order'a cevirme
- `SO_REUSEADDR` ile portun yeniden kullanilabilmesi
- Hata durumunda file descriptor'lari temiz kapatma
- TCP'nin mesaj degil, sirali bir byte stream oldugu

## Protokol

Bu V1 protokolu metin tabanlidir:

```text
client request:  Hello from client\n
server response: Mesajin alindi!\n
```

`\n`, bir mesajin bittigini belirtir. `recvLine()` TCP'den gelen byte'lari biriktirir ve newline gorunce mesaji tamamlanmis kabul eder.

TCP, `send()` cagrilarinin karsida ayni sayida veya ayni boyutta `recv()` cagrisiyla alinacagini garanti etmez. Deneyin gorunur olmasi icin `recvLine()` 8 byte'lik bir buffer kullanir; normal mesaj bile birden fazla chunk'ta gelebilir.

## Partial I/O

`sendAll()` bir `send()` cagrisi tum veriyi gondermezse kalan byte'lari gondermeye devam eder.

`recvLine()` her `recv()` sonucunu `std::string` icinde biriktirir. Boylece asagidaki gibi bolunmeler dogru calisir:

```text
recv() #1: "Hello fr"
recv() #2: "om clien"
recv() #3: "t\n"
```

Mesaj payload'i en fazla 1024 byte olabilir. Sinir asilirsa server mesaji reddeder.

## Gereksinimler

- Linux veya WSL
- CMake
- C++20 derleyicisi

## Derleme

Proje klasorunde:

```bash
cmake -S . -B build
cmake --build build
```

Bu komutlar iki executable uretir:

- `build/server`
- `build/client`

## Calistirma

Ilk WSL terminalinde server'i baslat:

```bash
./build/server
```

Ikinci WSL terminalinde client'i calistir:

```bash
./build/client
```

Beklenen normal akis:

```text
Server:
recv() chunk size: 8
recv() chunk size: 8
recv() chunk size: 2
Client: Hello from client

Client:
recv() chunk size: 8
recv() chunk size: 8
Server: Mesajin alindi!
```

Chunk boyutlari ag ve isletim sistemi zamanlamasina gore degisebilir. Onemli olan, mesajin birden fazla `recv()` sonucunda eksiksiz birlesmesidir.

## Test edilen senaryolar

- Normal client/server mesajlasmasi ve partial `recv()` chunk'larinin gozlemlenmesi.
- Server kapaliyken client calistirildiginda `connect()` hatasi ve temiz kapanis.
- Gecersiz IPv4 adresinde `inet_pton()` basarisiz olur; client baglanmayi denemez.
- Server yanit gondermeden baglantiyi kapatirsa client `recv() == 0` durumunu isler.
- `accept()`, `recv()` ve `send()` hatalarinda acik file descriptor'lar kapatilir.

## Bilinen sinirlar

- Server ayni anda yalnizca bir istemciyi isler.
- Protokol yalnizca tek satirlik metin mesaji icindir; binary veri desteklenmez.
- Bir `recv()` icinde birden fazla mesaj gelirse sonraki mesaja ait byte'lar icin kalici buffer yonetimi yoktur.
- Timeout, authentication, TLS ve reconnect destegi yoktur.

## Sonraki iyilestirmeler

- Length-prefix protocol ve `htonl()` / `ntohl()` ile binary-safe mesaj sinirlari.
- Birden fazla istemci icin thread veya event loop.
- Timeout, logging ve otomatik testler.
