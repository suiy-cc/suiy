echo ${SDKTARGETSYSROOT}
echo "rm framework*.h"
rm /${SDKTARGETSYSROOT}/usr/include/ama-app-common/* -R
rm /${SDKTARGETSYSROOT}/usr/include/appSDK/* -R
echo "rm framework*.so"
rm /${SDKTARGETSYSROOT}/usr/lib/libama-app-common.so
rm /${SDKTARGETSYSROOT}/usr/lib/libama-app-picture-display.so
rm /${SDKTARGETSYSROOT}/usr/lib/libama-app-surface-creater.so
rm /${SDKTARGETSYSROOT}/usr/lib/libama-log.so
rm /${SDKTARGETSYSROOT}/usr/lib/libama-utility.so
rm /${SDKTARGETSYSROOT}/usr/lib/libAppMgr-dbus.so
rm /${SDKTARGETSYSROOT}/usr/lib/libappSDK.so
rm /${SDKTARGETSYSROOT}/usr/lib/librpc-wrapper.so
rm /${SDKTARGETSYSROOT}/usr/lib/weston/ivi-control-preload.so
echo "rm framework*.pc"
rm /${SDKTARGETSYSROOT}/usr/lib/pkgconfig/libama-app-common.pc
rm /${SDKTARGETSYSROOT}/usr/lib/pkgconfig/libama-app-picture-display.pc
rm /${SDKTARGETSYSROOT}/usr/lib/pkgconfig/libama-log.pc
rm /${SDKTARGETSYSROOT}/usr/lib/pkgconfig/libappSDK.pc
rm /${SDKTARGETSYSROOT}/usr/lib/pkgconfig/libsurfaceCreater.pc
