#!/bin/sh

FILE_CHECK=true
PARA_CHECK=true
DBUS_PATH=dbus_so
APP_SERVICE_PATH=app_service
APP_CLIENT_PATH=app_client
FIDL_PATH_CORE=core
FIDL_PATH_DBUS=dbus

ORIGDIR=`pwd`
#echo $ORIGDIR

#####step1 check file
cd $ORIGDIR
FIDL_FILE=`find -maxdepth 1 -name *.fidl`
CMAKE_FILE=CMakeLists.txt
CMAKE_FILE_BAK=`find -maxdepth 1 -name CMakeLists.txt.bak`

test -f "${CMAKE_FILE_BAK}" && `mv -f ${CMAKE_FILE_BAK} ${CMAKE_FILE}`
CMAKE_FILE=`find -maxdepth 1 -name CMakeLists.txt`

test ! -f "${FIDL_FILE}" && echo FIDL file not exist. Please check... && FILE_CHECK=fause
test ! -f "${CMAKE_FILE}" && echo CMAKE file not exist. Please check... && FILE_CHECK=fause
if [ "${FILE_CHECK}" = "fause" ] ; then
    exit -1
fi

#####step2 check paras
#####*.fidl
#APP_PKG=`cat ${FIDL_FILE} | grep package -w | sed s/package// | sed 's/^[ \t]*//g'`
#APP_MAJ=`cat ${FIDL_FILE} | grep major -w | sed s/major// | sed 's/^[ \t]*//g'`
#APP_MIN=`cat ${FIDL_FILE} | grep minor -w | sed s/minor// | sed 's/^[ \t]*//g'`
#APP_INF=`cat ${FIDL_FILE} | grep interface -w | sed s/interface// | sed 's/^[ \t]*//g'`
APP_PKG=`cat ${FIDL_FILE} | grep package -w | sed s/package// | sed 's/^[ \t]*//g' | sed 's/ //g'`
APP_MAJ=`cat ${FIDL_FILE} | grep major -w | sed s/major// | sed 's/^[ \t]*//g' | sed 's/ //g'`
APP_MIN=`cat ${FIDL_FILE} | grep minor -w | sed s/minor// | sed 's/^[ \t]*//g' | sed 's/ //g'`
APP_INF=`cat ${FIDL_FILE} | grep interface -w | sed s/interface// | sed 's/^[ \t]*//g' | sed 's/ //g'`

test -z "${APP_PKG}" && echo Para \"package\" is error. Please check \"${FIDL_FILE}\". && PARA_CHECK=fause
test -z "${APP_MAJ}" && echo Para \"ver_major\" is error. Please check \"${FIDL_FILE}\". && PARA_CHECK=fause
test -z "${APP_MIN}" && echo Para \"ver_minor\" is error. Please check \"${FIDL_FILE}\". && PARA_CHECK=fause
test -z "${APP_INF}" && echo Para \"interface\" is error. Please check \"${FIDL_FILE}\". && PARA_CHECK=fause

#####CMakeLists.txt
CMAKE_PACKAGE_APP_NAME=`cat ${CMAKE_FILE} | grep PACKAGE_APP_NAME -w`
CMAKE_PACKAGE_INTERFACE_VERSION=`cat ${CMAKE_FILE} | grep PACKAGE_INTERFACE_VERSION -w`

test -z "${CMAKE_PACKAGE_APP_NAME}" && echo Para \"PRJ_NAME\" is error. Please check \"${CMAKE_FILE}\". && PARA_CHECK=fause
test -z "${CMAKE_PACKAGE_INTERFACE_VERSION}" && echo Para \"VERSION_PATH\" is error. Please check \"${CMAKE_FILE}\". && PARA_CHECK=fause

if [ "${PARA_CHECK}" = "fause" ] ; then
    exit -1
fi

sed -i.bak -e "s/PACKAGE_APP_NAME/${APP_PKG}/g" -e "s/PACKAGE_INTERFACE_VERSION/v${APP_MAJ}_${APP_MIN}/g" ${CMAKE_FILE}

echo The APP INFO:
echo ============================
echo package      :$APP_PKG
echo version_major:$APP_MAJ
echo version_minor:$APP_MIN
echo interface    :$APP_INF
echo ============================
echo wait 3s. please check...
sleep 3

#####step3 deleate history & check clean or not
cd $ORIGDIR
rm -rf ${DBUS_PATH}
rm -rf lib${APP_PKG}-dbus.so
rm -rf $ORIGDIR/${APP_SERVICE_PATH}/include/v*
rm -rf $ORIGDIR/${APP_CLIENT_PATH}/include/v*

if [ "$#" = "1" ] ; then
  CLEAN_FLAG=`echo "$1" | tr "[A-Z]" "[a-z]"`
	if [ "${CLEAN_FLAG}" = "clean" ] ; then
        echo success to clean lib${APP_PKG}-dbus.so ${APP_SERVICE_PATH}/include/ ${APP_CLIENT_PATH}/include/
        exit 1
  else
        echo Para only support clean.
        exit -1
  fi
fi

#####step4 make so
echo Create lib${APP_PKG}-dbus.so...
cd $ORIGDIR
mkdir -p ${DBUS_PATH} && cd ${DBUS_PATH}
cp -f $ORIGDIR/${FIDL_FILE} ./
cp -f $ORIGDIR/${CMAKE_FILE} ./

/home/common_api/01_tools/core/commonapi-generator-linux-x86 ${FIDL_FILE} -d ${FIDL_PATH_CORE} -sk >/dev/null
/home/common_api/01_tools/dbus/commonapi-dbus-generator-linux-x86 ${FIDL_FILE} -d ${FIDL_PATH_DBUS} >/dev/null
mkdir -p build && cd build
#pwd
cmake .. >/dev/null
make >/dev/null

#####step 5 cp so   step by step with step4
SO_FILE=`find ./ -name lib*.so`
test ! -e "${SO_FILE}" && echo "so" file not exist && exit -1
cp -f lib${APP_PKG}-dbus.so /usr/local/lib/
cp -f lib${APP_PKG}-dbus.so $ORIGDIR/
echo success to create ${SO_FILE} and copy to $ORIGDIR/

#####step 6 create service
echo Create ${APP_SERVICE_PATH}/include/...
cd $ORIGDIR
#pwd
mkdir -p ${APP_SERVICE_PATH}/include/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/ && cd ${APP_SERVICE_PATH}/include/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/
#pwd
cp -f $ORIGDIR/${DBUS_PATH}/${FIDL_PATH_CORE}/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/${APP_INF}.hpp ./
cp -f $ORIGDIR/${DBUS_PATH}/${FIDL_PATH_CORE}/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/${APP_INF}Stub*.hpp ./
echo success to create include for app_service and the path is:$ORIGDIR/${APP_SERVICE_PATH}

#####step 7 create client
echo Create ${APP_CLIENT_PATH}/include/...
cd $ORIGDIR
#pwd
mkdir -p ${APP_CLIENT_PATH}/include/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/ && cd ${APP_CLIENT_PATH}/include/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/
#pwd
cp -f $ORIGDIR/${DBUS_PATH}/${FIDL_PATH_CORE}/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/${APP_INF}.hpp ./
cp -f $ORIGDIR/${DBUS_PATH}/${FIDL_PATH_CORE}/v${APP_MAJ}_${APP_MIN}/${APP_PKG}/${APP_INF}Proxy*.hpp ./
echo success to create include for app_client and the path is:$ORIGDIR/${APP_CLIENT_PATH}
echo ls to show