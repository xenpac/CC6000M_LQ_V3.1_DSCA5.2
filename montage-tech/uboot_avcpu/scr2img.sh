dos2unix $1
mkimage -A mips -O linux -T script -C none -a 0 -e 0 -n "Boot Script"  -d $1 $2