rm -rf ./build
mkdir ./build
mkdir /run/shm/build
chmod  1777  /run/shm/build
sudo mount --bind /run/shm/build ./build
ls -ld ./build
