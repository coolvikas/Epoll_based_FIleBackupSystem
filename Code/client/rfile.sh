#! /bin/bash
base64 /dev/urandom | head -c 5000192 > file.txt
