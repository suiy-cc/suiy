basepath=$(cd `dirname $0`;pwd)
DATE=$(date +%m%d%H%M)
projectBaseName=${basepath##*/}
projectName=$projectBaseName-$DATE

if [ ! -d ./build ] ; then
    echo please make version first......
	exit 1
fi

cd ./build
if [ -d ./zip ] ; then
    rm -rf ./zip/$projectBaseName*
fi
zip_dir=zip/$projectName/$projectName
mkdir -p $zip_dir/add
mkdir -p $zip_dir/delete
mkdir -p $zip_dir/hardlink
mkdir -p $zip_dir/link
mkdir -p $zip_dir/patch
if [ -d ./delete ] ; then
    cp -rf ./delete/* ./$zip_dir/delete
fi
if [ -d ./hardlink ] ; then
    cp -rf ./hardlink/* ./$zip_dir/hardlink
fi
if [ -d ./link ] ; then
    cp -rf ./link/* ./$zip_dir/link
fi
if [ -d ./patch ] ; then
    cp -rf ./patch/* ./$zip_dir/patch
fi
make DESTDIR=$zip_dir/add install

cd ./zip
tar -czf $projectName.tar -C $projectName $projectName

#if [ ! -f ./update_make_package ] ; then
#	cp ../../update_make_package ./ && chmod +x ./update_make_package
#fi

#cp ../../make_update_package.sh ./ && chmod +x ./make_update_package.sh

wget \
    -qO update_make_package \
    ftp://sdkro:sdkro@10.1.55.11:/daily_build/update_image/update_make_package
	
wget \
    -qO make_update_package.sh \
    ftp://sdkro:sdkro@10.1.55.11:/daily_build/update_image/make_update_package.sh
	
chmod +x ./update_make_package
chmod +x ./make_update_package.sh

UPDIASIMAGE=iasImage_update_update_normal
rm -f $UPDIASIMAGE.md5sum.local $UPDIASIMAGE.md5sum.new $UPDIASIMAGE.md5sum
if [ -f "$UPDIASIMAGE" ] ; then
	md5sum $UPDIASIMAGE \
		> $UPDIASIMAGE.md5sum.local
else
	touch $UPDIASIMAGE.md5sum.local
fi
wget \
    -qO $UPDIASIMAGE.md5sum \
    ftp://sdkro:sdkro@10.1.55.11:/daily_build/update_image/iasImage_update_update_normal.md5sum
	
echo "The md5sum of local image is:"
cat $UPDIASIMAGE.md5sum.local
echo "The md5sum of gotten md5sum is:"
cat $UPDIASIMAGE.md5sum

## 这里需要做checksum检查，以决定是否需要下载
diff $UPDIASIMAGE.md5sum.local $UPDIASIMAGE.md5sum
if [ "x$?" == "x0" ] ; then
	echo "The local image is the last image, need not to download."
else
	echo "Downloading the new image..."
	
	wget \
		-qO ./$UPDIASIMAGE \
		ftp://sdkro:sdkro@10.1.55.11:/daily_build/update_image/iasImage_update_update_normal

	md5sum $UPDIASIMAGE \
		> $UPDIASIMAGE.md5sum.new
		
	## 这里需要进行checksum检查以确定文件完整性
	diff $UPDIASIMAGE.md5sum $UPDIASIMAGE.md5sum.new
	if [ "x$?" == "x0" ] ; then
		echo "image ok!"
	else
		echo "The md5sum of gotten image is:"
		cat $UPDIASIMAGE.md5sum.new
		echo "The md5sum of gotten md5sum is:"
		cat $UPDIASIMAGE.md5sum
		echo "They are not the same!!!"
		
		rm -f $UPDIASIMAGE
		echo "No $UPDIASIMAGE!!!"
		exit 1
	fi
fi

#$1 
ROOTFS_IVI=$projectName.tar
#$2 
PACKAGE_VERSION=$projectName
#$3 
BUILD_ID=$(date +%Y%m%d)"0000"-$(date +%m%d%H%M)
#$4 
PACKAGE_NAME=$projectName.zip
#$5
KERNEL_UPDATE=$UPDIASIMAGE
## export ROOTFS_IVI=/home/test/li/08_si/diff/961to971/9612971.tar
## export PACKAGE_VERSION=FHD1539MA-05-CN-0971a
## export BUILD_ID=201806120000-06121330
## export PACKAGE_NAME=FHD1539MA-05-CN-0971a-DIFF-0961a.zip
## export KERNEL_UPDATE=
./make_update_package.sh $ROOTFS_IVI $PACKAGE_VERSION $BUILD_ID $PACKAGE_NAME $KERNEL_UPDATE