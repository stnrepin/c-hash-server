# Hash Server

This repository contains source files for a simple HTTP server written in C99.

The HTTP server gets JSON-string via POST-request, e.g.

```json
{
    "data": "hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhashdfklhasfkjhasdkdhfkshdf"
}
```

And the server sends back JSON-string with sha512 and gost hashes for `data`:

```json
{
    "sha512": "4adfbbf00cd146301af5f0661b55d13439a14f67555b6dd03ad55ccf5efc85c8
        12347f1050f2e481620d7a7ba588a5df5f30bc04f896903ae09217a6ae6d4abd",
    "gost": "b75a0ac84397a0b2329316e333a58338a32a0ec2cc6e0e9843f9cfa80eb696f0"
}
```

Program also uses external libraries: Unity (unit-tests), OpenSSL (hashes),
pthead (concurrency)

## Requirements

* Linux
* gcc
* cmake (>=3.10.0)

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
$ ./bin/hash-server
```

## Testing

Run unit-tests with

```bash
$ cmake test
```

Run integration-tests with

```bash
$ python3 test/run_integrations.py
```

