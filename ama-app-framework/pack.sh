basepath=$(cd `dirname $0`;pwd)
projectName=${basepath##*/}
echo $projectName
cd ./build
mkdir tmp
make DESTDIR=tmp install
cd ./tmp
tar zcvf $projectName.tgz ./*
cp $projectName.tgz ../
cd ../
rm tmp -rf
