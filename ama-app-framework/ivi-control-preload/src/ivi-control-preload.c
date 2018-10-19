/*
 * Copyright (C) 2016-2017 Neusoft, Inc.
 * All Rights Reserved.
 *
 * The following programs are the sole property of Neusoft Inc.,
 * and contain its proprietary and confidential information.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/inotify.h>
#include <unistd.h>


#include "ivi-layout-export.h"
#include "ivi-controller-interface.h"
#include "screen.h"
#include "ama_enum.h"

#define LAYER_ID( s, l )            ( ( ( (s) & 0x0F )<< 28 ) + ( ( (l) & 0xFF ) << 8 ) )

#define STOP_FILE                   "/dev/ivi-control-preload-stop"

typedef struct {
    struct weston_compositor *compositor;
    const struct ivi_controller_interface *interface;
    int notify_fd;
    int control_wd;
    struct wl_event_source *source;
} preload_info_t;

static void on_surface_remove_notification( struct ivi_layout_surface *ivisurf, void *user )
{
    const struct ivi_controller_interface *interface = user;
    struct ivi_layout_layer *ivilayer;
    uint32_t id_surface;
    uint32_t id_app;
    uint32_t id_screen;
    uint32_t id_layer;
    uint32_t id;

    id_surface = interface->get_id_of_surface( ivisurf );
    id_app = ( id_surface >> 16 ) & 0xFF;

    if( ( id_app == E_APPID_BOOT_ANIMATION )
        || ( id_app == E_APPID_RVC )
        || ( id_app == E_APPID_AVM )
        ){

        id_screen = ( id_surface >> 28 ) & 0x0F;
        id_layer = ( id_surface >> 8 ) & 0xFF;

        id = LAYER_ID( id_screen, id_layer );
        ivilayer = interface->get_layer_from_id( id );
        if( !ivilayer ){
            weston_log( "failed to get layer from surface id %08X\n", id_surface );
            return;
        }

        interface->layer_remove_surface( ivilayer, ivisurf );
        interface->commit_changes();

        //weston_log( "on_surface_remove %08X\n", id_surface );
    }
}

static void on_surface_configure_notification( struct ivi_layout_surface *ivisurf, void *user )
{

    const struct ivi_controller_interface *interface = user;
    struct ivi_layout_layer *ivilayer;
    uint32_t id_surface;
    uint32_t id_app;
    uint32_t id_screen;
    uint32_t id_layer;
    uint32_t id;

    id_surface = interface->get_id_of_surface( ivisurf );
    id_app = ( id_surface >> 16 ) & 0xFF;
//    weston_log( "on_surface_configure_notification(%08X)->IN", id_surface);

    if(get_screen_count()==0)
        init_screen_api(interface);

    id_layer = ( id_surface >> 8 ) & 0xFF;
    if( ( id_app == E_APPID_BOOT_ANIMATION )
        || ( id_app == E_APPID_RVC && id_layer == E_LAYER_CAMERAVIDEO )
        || ( id_app == E_APPID_AVM && id_layer == E_LAYER_CAMERAVIDEO )
        ){

        id_screen = ( id_surface >> 28 ) & 0x0F;

        id = LAYER_ID( id_screen, id_layer );
        ivilayer = interface->get_layer_from_id( id );
        if( !ivilayer ){
            weston_log( "failed to get layer from surface id %08X\n", id_surface );
            return;
        }

        struct screen* virtual_screen = get_virtual_screen(id_screen);
        if( !virtual_screen ){
            weston_log( "failed to get screen from surface id %08X\n", id_surface );
            return;
        }

        struct screen_output output;
        virtual_screen->get_output(virtual_screen, &output);

        if(id_layer==E_LAYER_BOOT_ANIMATION)
        {
            struct ivi_layout_layer_properties* properties = interface->get_properties_of_layer(ivilayer);
            if(properties)
            {
                output.x      = properties->dest_x;
                output.y      = properties->dest_y;
                output.width  = properties->dest_width;
                output.height = properties->dest_height;
                //free(properties);
            }
        }

        interface->surface_set_source_rectangle( ivisurf, 0, 0, output.width, output.height );
        interface->surface_set_destination_rectangle( ivisurf, 0, 0, output.width, output.height );
        interface->surface_set_visibility( ivisurf, 1 );
        interface->layer_add_surface( ivilayer, ivisurf );

        interface->commit_changes();

//        weston_log( "on_surface_configure %08X\n", id_surface );
    }

//    weston_log( "on_surface_configure_notification(%08X)->OUT", id_surface);
    //destroy_screen_api();
}

static unsigned int screen_share_layer_ids[] = {
    0x40001000,
    0x40002000,
    0x40003000,
};

static unsigned int screen_sharing_layer_ids[] = {
    0x50001000,
    0x50002000,
};

void create_layers(const struct ivi_controller_interface *interface)
{
    bool isSuccess = init_screen_api(interface);
    if(!isSuccess)
        weston_log( "screen api initialization failed!\n" );

    // count total layers
    unsigned int fixme_order_i = 0;
    unsigned int fixme_order_count = 0;
    for(int i = 0; i < get_screen_count(); ++i)
    {
        struct screen* virtual_screen = get_screen(i);
        if(!virtual_screen)
            continue;

        unsigned int *layer_ids_array = NULL;
        unsigned int layer_count = 0;
        virtual_screen->get_layer_ids(virtual_screen, &layer_ids_array, &layer_count);
        fixme_order_count += layer_count;
    }
    fixme_order_count += sizeof(screen_share_layer_ids)/sizeof(unsigned int);
    fixme_order_count += sizeof(screen_sharing_layer_ids)/sizeof(unsigned int);
    struct ivi_layout_layer** fixme_order_layer = malloc(fixme_order_count*sizeof(struct ivi_layout_layer*));

    for(int i = 0; i < sizeof(screen_sharing_layer_ids)/sizeof(unsigned int); ++i)
    {
        unsigned int layer_id = screen_sharing_layer_ids[i];
        struct ivi_layout_layer *layer = NULL;
//        layer = interface->layer_create_with_dimension( layer_id, 1920, 720);
//        interface->layer_set_destination_rectangle( layer, 3200, 0,  1920, 720 );
        layer = interface->layer_create_with_dimension( layer_id, 1920, 720);
        interface->layer_set_destination_rectangle( layer, 0, 0,  1920, 720 );
        interface->layer_set_visibility( layer, 1 );

        fixme_order_layer[fixme_order_i++] = layer;
    }

    // create layer screen by screen
    for(enum E_SEAT i = E_SEAT_TEST; i < E_SEAT_MAX; ++i)
    {
        struct screen* virtual_screen = get_virtual_screen(i);
        if(!virtual_screen)
            continue;

        unsigned int *layer_ids_array = NULL;
        unsigned int layer_count = 0;
        virtual_screen->get_layer_ids(virtual_screen, &layer_ids_array, &layer_count);
        struct screen_output output;
        virtual_screen->get_output(virtual_screen, &output);
        struct ivi_layout_layer** render_order = malloc(layer_count*sizeof(struct ivi_layout_layer*));
        for(int j = 0; j < layer_count; ++j)
        {
            unsigned int layer_id = layer_ids_array[j];

            struct ivi_layout_layer *layer = NULL;
            if(layer_id==LAYER_ID(E_SEAT_IVI, E_LAYER_BOOT_ANIMATION))
            {
                layer = interface->layer_create_with_dimension(layer_id, 3200, 720);
                interface->layer_set_destination_rectangle(layer, 0, 0, 3200, 720);
                interface->layer_set_visibility(layer, 1);
            }
            else
            {
                layer = interface->layer_create_with_dimension(layer_id, output.width, output.height);
                interface->layer_set_destination_rectangle(layer, output.x, output.y, output.width, output.height);
                interface->layer_set_visibility(layer, 1);
            }

            render_order[j] = layer;
            fixme_order_layer[fixme_order_i++] = layer;

//            weston_log( "create %d,%d - %dx%d  0x%08X to screen %d\n",
//                output.x, output.y,
//                output.width, output.height,
//                layer_id,
//                i );
        }
        virtual_screen->set_render_order(virtual_screen, render_order, layer_count);
        if(layer_ids_array)
            free(layer_ids_array);
        if(render_order)
            free(render_order);

        weston_log("layers on screen %d(seat number) is created\n", i);
    }

    for(int i = 0; i < sizeof(screen_share_layer_ids)/sizeof(unsigned int); ++i)
    {
        unsigned int layer_id = screen_share_layer_ids[i];
        struct ivi_layout_layer *layer = NULL;
        layer = interface->layer_create_with_dimension( layer_id, 3200, 1440);
        interface->layer_set_destination_rectangle( layer, 0, 0,  3200, 1440 );
        interface->layer_set_visibility( layer, 1 );

        fixme_order_layer[fixme_order_i++] = layer;
    }

    // find key screen
    struct ivi_layout_screen* fixme_screen = get_fixme_screen(interface);
    // add all layers onto key-screen
    if(fixme_screen)
        interface->screen_set_render_order(fixme_screen, fixme_order_layer, fixme_order_count);

    free(fixme_order_layer);

    destroy_screen_api();

    interface->commit_changes();
}

static int read_control( void )
{
    int fd;
    int value = 0;
    char buf[ 4 ];

    fd = open( STOP_FILE, O_RDONLY );
    if( fd < 0 )
        return -1;

    buf[0] = -1;
    read( fd, buf, 1 );
    close( fd );

    if( buf[0] == '0' )
        value = 0;
    else if( buf[0] == '1' )
        value = 1;

    return value;
}

static int
on_stop_file_written(int fd, uint32_t mask, void *data)
{
    int value;
    preload_info_t *info = data;
    const struct ivi_controller_interface *interface = info->interface;

    value = read_control();
    if( value == 1 ){

        interface->remove_notification_configure_surface( on_surface_configure_notification, ( void * )interface );
        interface->remove_notification_remove_surface( on_surface_remove_notification, ( void * )interface );

        wl_event_source_remove( info->source );
        inotify_rm_watch( info->notify_fd, info->control_wd );
        close( info->notify_fd );
        free( info );
    }

    return 1;
}

static int
check_stop( struct weston_compositor *compositor, const struct ivi_controller_interface *interface )
{
    struct wl_event_loop *loop;
    struct stat sb;
    int fd;
    int value;
    preload_info_t *info;

    if( stat( STOP_FILE, &sb ) ){
        fd = open( STOP_FILE, O_CREAT|O_TRUNC|O_WRONLY|O_NONBLOCK, 0666 );
        if( fd < 0 ){
            printf( "can create %s\n", STOP_FILE );
            goto e_open_control;
        }
        write( fd, "0", 1 );
        close( fd );
    }

    value = read_control();
    if( value == 0 ){

        info = calloc( sizeof( preload_info_t ), 1 );
        if( !info ){
            weston_log( "failed to alloc memory\n" );
            goto  e_alloc;
        }

        info->compositor = compositor;
        info->interface = interface;

        info->notify_fd = inotify_init1( IN_NONBLOCK );
        if( info->notify_fd < 0 ){
            printf( "failed to create notify fd\n" );
            goto e_inotify;
        }

        info->control_wd = inotify_add_watch( info->notify_fd, STOP_FILE, IN_CLOSE_WRITE );
        if( info->control_wd < 0 ){
            printf( "failed to monitor preload stop control\n" );
            goto e_watch_control;
        }

        loop = wl_display_get_event_loop(compositor->wl_display);
        info->source = wl_event_loop_add_fd(loop, info->notify_fd,
            WL_EVENT_READABLE, on_stop_file_written, info );
        goto end;

        e_watch_control:
        close( info->notify_fd );
        e_inotify:
        e_open_control:
        free( info );
        e_alloc:
        end:
        ;/* do nothing */
    }

    return value;
}

WL_EXPORT int ivi_control_preload_init( struct weston_compositor *compositor,
                                        const struct ivi_controller_interface *interface, size_t version )
{
    int32_t ret;
    int value;
    ret = -1;

    value = check_stop( compositor, interface );

//    if( value == 0 ){
//        ret = interface->add_notification_remove_surface( on_surface_remove_notification, ( void * )interface );
//        if( ret != IVI_SUCCEEDED ){
//            weston_log( "failed to add  notify for surface removing\n" );
//            goto  e_surface_remove;
//        }
//
//        ret = interface->add_notification_configure_surface( on_surface_configure_notification, ( void * )interface );
//        if( ret != IVI_SUCCEEDED ){
//            weston_log( "failed to add  configurea for surface removing\n" );
//            goto  e_surface_configure;
//        }
//    }

    create_layers(interface);
    return 0;

e_surface_configure:
    if( value == 0 )
    interface->remove_notification_remove_surface( on_surface_remove_notification, ( void * )interface );
e_surface_remove:
    //free( order );
e_get_screens:
    return -1;
}
