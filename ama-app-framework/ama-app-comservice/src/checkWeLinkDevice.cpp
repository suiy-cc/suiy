/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include "checkWeLinkDevice.h"
#include "ProCtrl.h"
#include "CommonServiceLog.h"

#include <pthread.h>
#include <glib.h>
#include <libudev.h>
#include <poll.h>

static struct udev_monitor * udevMonitor;
static struct udev * udevCtx;
static GPollFD udevFd;
GMainContext * context;
GMainLoop * mainloop;

static gboolean _prepare( GSource * source, gint * timeout )
{
	* timeout	=	-1;

	return FALSE;
}
static gboolean _check( GSource * source )
{
	gboolean _ret	=	FALSE;

	if( udevFd.revents != 0 )
	{
		_ret	=	TRUE;
	}

	return _ret;
}
static gboolean _dispatch( GSource * source, GSourceFunc callback, gpointer user_data )
{
	return callback( user_data );
}
static GSourceFuncs _funcs	=
{
	.prepare	=	_prepare,
	.check		=	_check,
	.dispatch	=	_dispatch
};

static gboolean _is_idev( struct udev_list_entry * properties )
{
	struct udev_list_entry * _entry, * _model;

	_model	=	udev_list_entry_get_by_name( properties, "ID_VENDOR_ID" );

	if( _model != NULL && g_ascii_strcasecmp( udev_list_entry_get_value( _model ), "05ac" ) == 0 )
	{
		_model	=	udev_list_entry_get_by_name( properties, "ID_MODEL_ID" );

		if( g_ascii_strncasecmp( udev_list_entry_get_value( _model ), "12", 2 ) == 0 )
		{
			return TRUE;
		}
	}

	return FALSE;
}

static gboolean _is_usb_otg(struct udev_list_entry * properties)
{
	struct udev_list_entry * _model;
	const gchar* _devPath;
	_model	=	udev_list_entry_get_by_name(properties, "DEVPATH");
	_devPath = udev_list_entry_get_value(_model);

	if((NULL!=_model)&&(g_strrstr(_devPath,"pci")!=NULL)){
		logInfo(COS,"intel chididea otg controller is found");
		if(g_strrstr(_devPath,"1-1")!=NULL){
			logInfo(COS,"intel chipidea otg controller is found 1-1");
			return TRUE;
		}
	}

	return FALSE;
}

static gboolean _parse_events( gpointer user_data )
{
	struct udev_device * _udevDev;
	struct udev_list_entry * _properties;

	_udevDev	=	udev_monitor_receive_device( udevMonitor );

	if( _udevDev )
	{
		const gchar * _udevAction;

		_udevAction	=	udev_device_get_action( _udevDev );

		_properties	=	udev_device_get_properties_list_entry( _udevDev );

		if((_is_idev(_properties))&&(_is_usb_otg(_properties))){
			logInfo(COS,"udev is a iOS device");
			if(( g_strcmp0( _udevAction, "add" ) == 0 ))
			{
				logInfo(COS,"iOS device attached");
				updateWelinkIOSDeviceAttacheStaToPro(true);
			}
			else
			if( g_strcmp0( _udevAction, "remove" ) == 0 )
			{
				logInfo(COS,"iOS device detached");
				updateWelinkIOSDeviceAttacheStaToPro(false);
			}
		}else{
			logInfo(COS,"udev is not a iOS device");
		}

		udev_device_unref( _udevDev );
	}

	return G_SOURCE_CONTINUE;
}

static void* _register_monitor(void *)
{
    logDebug(COS,"_register_monitor()-->>");

	GSource * _source	=	NULL;
    udevCtx = udev_new();
    if(NULL==udevCtx){
        logWarn(COS,"udev_new failed");
        goto NEED_FREE_CTX;
    }

	udevMonitor	=	udev_monitor_new_from_netlink( udevCtx, "udev" );

	context = g_main_context_new();
	mainloop = g_main_loop_new(context, FALSE);

	if( udevMonitor == NULL )
	{
		g_warning( "udev_monitor_new_from_netlink failed" );
        logWarn(COS,"udev_monitor_new_from_netlink failed");

		goto NEED_FREE_CTX;
	}

	if( udev_monitor_filter_add_match_subsystem_devtype( udevMonitor, "usb", 0 ) < 0 )
	{
		g_warning( "udev_monitor_filter_add_match_subsystem_devtype failed" );
        logWarn(COS,"udev_monitor_filter_add_match_subsystem_devtype failed");
		goto NEED_FREE_MONITOR;
	}

	if ( udev_monitor_enable_receiving( udevMonitor ) < 0 )
	{
		g_warning( "udev_monitor_enable_receiving failed" );
        logWarn(COS,"udev_monitor_enable_receiving failed");
		goto NEED_FREE_MONITOR;
	}

	udevFd.fd	=	udev_monitor_get_fd( udevMonitor );

	if( udevFd.fd > 0 )
	{
		udevFd.events	=	POLLIN;

		_source	=	g_source_new( &_funcs, sizeof( GSource ));

		g_source_add_poll( _source, &udevFd );

		g_source_set_callback( _source, _parse_events, NULL, NULL );

		g_source_attach( _source, context );

		g_source_unref( _source );
	}
	else
	{
		g_warning( "udev_monitor_get_fd faild" );
        logWarn(COS,"udev_monitor_get_fd faild");
		goto NEED_FREE_MONITOR;
	}

	g_main_loop_run( mainloop );

	g_main_loop_unref( mainloop );

	mainloop = NULL;

	return NULL;

NEED_FREE_MONITOR:
	udev_monitor_unref( udevMonitor );
NEED_FREE_CTX:
	udev_unref( udevCtx );

	return NULL;
}

static gboolean _scan_idev()
{
	struct udev_enumerate * _enum;
	struct udev_list_entry * _idev,* _properties;
	struct udev_list_entry * _entry = NULL;
	struct udev_device * _udevDev;

	_enum	=	udev_enumerate_new( udevCtx );

	if( _enum == NULL )
	{
		g_warning( "udev_enumerate_new failed" );

		return FALSE;
	}

	udev_enumerate_add_match_subsystem( _enum, "usb" );

	udev_enumerate_scan_devices( _enum );

	_idev	=	udev_enumerate_get_list_entry( _enum );

	udev_list_entry_foreach( _entry, _idev )
	{
		const gchar * _path	=	udev_list_entry_get_name( _entry );
		struct udev_list_entry * _entry1;

		_udevDev	=	udev_device_new_from_syspath( udevCtx, _path );

		_properties	=	udev_device_get_properties_list_entry( _udevDev );

		if((_is_idev(_properties))&&(_is_usb_otg(_properties))){
			updateWelinkIOSDeviceAttacheStaToPro(true);
		}else{
			updateWelinkIOSDeviceAttacheStaToPro(false);
		}

		udev_device_unref( _udevDev );
	}

	udev_enumerate_unref( _enum );

	return TRUE;
}

void startScaningWelinkIOSDevice(void)
{
    logDebug(COS,"startScaningWelinkIOSDevice()-->>");

    pthread_t checkThread;
    pthread_create( &checkThread, NULL, _register_monitor, NULL );

	_scan_idev();
}
