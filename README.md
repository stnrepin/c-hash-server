# Hash Server

This repository contains source files for a simple HTTP 1.0 server written in C99.

The HTTP server gets JSON-string via POST-request, e.g.

```json
{
    "data": "hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhashdfklhasfkjhasdkdhfkshdf"
}
```

And the server sends back JSON-string with SHA512 and Gost (GOST R 34.11-94 CryptoPro)  hashes for `data`:

```json
{
    "sha512": "62f810d09eee0865f4bbd3fd98bce21a0724b280b02deaec2af5c57452d5ddd29c95024081ba4a6ad4dac06b61cb35810ccc69dc05d2183f06cc6e0bc7f2dbeb",
    "gost": "7250b9eb1e8b82ef421babc4241aa7c752bec1f95c93f4702e2687e98693574a"
}
```

Program also uses external libraries: Unity (unit-tests), OpenSSL+gost-engine (hashes),
pthead (concurrency), c-thread-pool (thread pool).

Project is available on [Github](https://github.com/stnrepin/c-hash-server).

## Requirements

* Linux
* gcc
* cmake (>=3.10.0)
* libssl-dev (>= 1.1)

The latest versions are recommended.

## Building

Run the following command:

```bash
$ mkdir build
$ cd build
$ cmake -DBUILD_TYPE=Release ..
$ make
```

To debug build with enabled tests run:

```bash
$ mkdir build
$ cd build
$ cmake -DBUILD_TYPE=Debug -DBUILD_TESTS=1 ..
$ make
```

## Running

```bash
$ ../bin/hash-server
```

## Testing

Run unit-tests with

```bash
$ ctest -V
```

