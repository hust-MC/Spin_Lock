#build.sh

make -C $KERNEL_PATH M=/root/android/adt-bundle-linux-x86-20140321/eclipse/workspace/spin_lock/src
rmmod spin_lock
insmod src/spin_lock.ko
