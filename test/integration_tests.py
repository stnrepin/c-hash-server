#!/bin/python3

import sys
import json
import urllib.request, urllib.error
import http.client
import hashlib
import subprocess
import time

URL = 'http://127.0.0.1:3000/'

server_process = None

def print_mes(mes):
    print('[Python test]:', mes)

def run_server(path):
    global server_process
    server_process = subprocess.Popen([path])

def post_request(url, data, headers):
    http.client.HTTPConnection._http_vsn = 10
    http.client.HTTPConnection._http_vsn_str = 'HTTP/1.0'
    method = "POST"
    handler = urllib.request.HTTPHandler()
    opener = urllib.request.build_opener(handler)

    request = urllib.request.Request(url, data=bytes(data, 'utf-8'))
    for k, v in headers.items():
        request.add_header(k, v)
    request.get_method = lambda: method
    try:
        connection = opener.open(request)
    except urllib.error.HTTPError as e:
        connection = e
    return (connection.code, connection.read())


def test_basic_posts():
    request_jsons = [
            { 'data': 'test' },
            { 'data': '' },
            { 'data': 'hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhash' +
                            'dfklhasfkjhasdkdhfkshdf' },
    ]
    headers = { 'Content-Type': 'application/json' }
    for d in request_jsons:
        print_mes(f'POST to / with {json.dumps(d)}')
        rc, res_json = post_request(URL, json.dumps(d), headers)
        
        if rc != 200:
            return False
        print_mes(f'Response: {res_json}')
        res_obj = json.loads(res_json)

        if 'sha512' not in res_obj:
            return False

        expected_sha512 = hashlib.sha512(bytes(d['data'], 'utf-8')).hexdigest()
        if res_obj['sha512'] != expected_sha512:
            print_mes(f'expect: {expected_sha512}')
            print_mes(f'actual: {res_obj["sha512"]}')
            return False
    return True

def main():
    global server_process
    if len(sys.argv) != 2:
        print_mes('Invalid command line arguments')
        return
    try:
        run_server(sys.argv[1])
        res = test_basic_posts()
        if not res:
            print_mes('Error')
            raise Exception()
        print_mes('Passed')
    finally:
        if server_process:
            server_process.kill()
        raise

if __name__ == '__main__':
    main()
