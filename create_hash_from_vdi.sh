#!/bin/bash

# Make sure we have an arg:
if [ "$#" -ne 1 ]; then
	echo "specify vdi!"
	exit
else
	echo using $1
fi

# Check for root:
if [[ $EUID -ne 0 ]]; then
	echo "Need root!"
	exit
fi

# Extract filename from path
name=$(echo "$1" | rev | cut -d/ -f-1 | rev)
echo name is $name

# Hackily make sure clean:
umount /mnt/tmp
qemu-nbd -d /dev/nbd0

# Mount the vdi:
qemu-nbd -c /dev/nbd0 $1
mount /dev/nbd0p1 /mnt/tmp
# cd /mnt/tmp/

# test:
ls /mnt/tmp/

# Begin time:
start=$(date +%s)

# Walk mount and md5 all files:
find /mnt/tmp/ -type f -name "*" -printf "%p\n" | xargs -d '\n' md5sum > /home/bob/temp/hookread/af_force/data/$(echo $name\_hashes).data


# ./home/bob/temp/hookread/af_force/count_from_root.py >> /home/bob/temp/hookread/af_force/data/path_list.tmp
# cat /home/bob/temp/hookread/af_force/data/path_list.tmp



# Take a list of paths, generate hash list:
# while read p; do 
	# echo $(echo $p | md5sum) $p >> /home/bob/temp/hookread/af_force/data/baseline_hash_list.list;
# done < /home/bob/temp/hookread/af_force/data/path_list.tmp


# Stop time, report:
end=$(date +%s)
echo "Execution time:" $(echo "($end-$start)/60" | bc -l) "minutes"

# Clean up:
umount /mnt/tmp
qemu-nbd -d /dev/nbd0
# qemu-nbd -d /dev/nbd0p1