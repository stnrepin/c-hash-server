#!/bin/bash

URL='127.0.0.1:3000'

function make_post_request() {
    echo "POST Data: $1"
    curl -X POST --http1.0 --data "$1"          \
         --output -                             \
         -H "Content-Type: application/json"    \
         $URL
    echo
}

declare -a datas=(
   $'{"data":"test"}'
   $'\n\n{\n"data":   "\n"}'
   $'{\n\t"data":"hhadhskfhjdkslhfklashdfklhadshfklashdfklhasklfhashdfklhasfkjhasdkdhfkshdf"\n}'

   $'{"data": }'
   $'{"data1":""}'
   $'{"data":"", "data1":""}'
)

i=1
for d in "${datas[@]}";
do
    echo "$i ==="
    make_post_request "$d"
    ((i++))
done;

echo
echo "Do bad requests ==="
data='{"data": "test"}'

curl -X GET --http1.0 --data "$data"        \
     --output -                             \
     -H "Content-Type: application/json"    \
     $URL

curl -X POST --http1.0 --data "$data"   \
     --output -                         \
     $URL

curl -X POST --http1.0 --data "$data"   \
     --output -                         \
     -H "Content-Type: test/plain"      \
     $URL

curl -X POST --http1.0 --data "$data"   \
     --output -                         \
     -H "Content-Type: test/plain"      \
     $URL/some-path
