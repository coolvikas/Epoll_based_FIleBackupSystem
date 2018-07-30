#!/bin/sh 
while true
do    
    sudo sh -c "sync; echo 3 > /proc/sys/vm/drop_caches"
#    echo "done!!"
    sleep 5 
done

