#!/bin/python3

import json
import requests
import hashlib
import subprocess

URL = '127.0.0.1:3000'

server_process = None

def run_server():
    global server_process
    server_process = subprocess.Popen(['../bin/hash-server'])
    if not server_process.poll():
        server_process.wait()
        print(server_process.stdout.readlines())
        raise EnvironmentError("Server can't launched")

def test_basic_posts():
    request_jsons = [
            { 'data': 'test' },
            { 'data': '' },
            { 'data': 'hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhash' +
                            'dfklhasfkjhasdkdhfkshdf' },
    ]
    headers = { 'content-type': 'application/json' }
    sha512 = hashlib.sha512()
    for d in request_jsons:
        print(f'POST / with {json.dumps(d)}')
        r = requests.post(URL, data=d, headers=headers)
        if r.status_code != 200:
            return False
        j = r.json()
        print(f'Response: {j}')

        if 'sha512' not in j:
            return False
        expected_sha512 = sha512.update(bytes(d['data'], 'utf-8')).hexdigest()
        if j['sha512'] != expected_sha512:
            return False
        print()

def main():
    run_server()
    res = test_basic_posts()
    if not res:
        print('Error')
        return
    if server_process:
        server_process.kill()

if __name__ == '__main__':
    main()

