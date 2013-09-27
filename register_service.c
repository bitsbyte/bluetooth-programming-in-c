#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <stdlib.h>

sdp_session_t *register_service(int chnel, int l2_chnel)
{
    uint32_t service_uuid_int[] = { 0, 0, 0, 0xABCD };
    uint8_t rfcomm_channel = chnel;
    uint16_t l2cap_channel = l2_chnel;
    const char *service_name = "Roto-Rooter Data Router";
    const char *service_dsc = "An experimental plumbing router";
    const char *service_prov = "Roto-Rooter";

    uuid_t root_uuid, l2cap_uuid, rfcomm_uuid, svc_uuid;
    sdp_list_t *l2cap_list = 0, 
               *rfcomm_list = 0,
               *root_list = 0,
               *proto_list = 0, 
               *access_proto_list = 0,
               *class_id_list = 0;
    sdp_data_t *channel = 0, *psm = 0, *l2cap_port = 0;

    sdp_record_t *record = sdp_record_alloc();

    // set the general service ID
    sdp_uuid128_create( &svc_uuid, &service_uuid_int );
    class_id_list = sdp_list_append(0, &svc_uuid);
    //sdp_set_service_id( record, svc_uuid );
    sdp_set_service_classes( record, class_id_list);

    // make the service record publicly browsable
    sdp_uuid16_create(&root_uuid, PUBLIC_BROWSE_GROUP);
    root_list = sdp_list_append(0, &root_uuid);
    sdp_set_browse_groups( record, root_list );

    // set l2cap information
    sdp_uuid16_create(&l2cap_uuid, L2CAP_UUID);
    l2cap_list = sdp_list_append( 0, &l2cap_uuid );
    l2cap_port = sdp_data_alloc(SDP_UINT16, &l2cap_channel);
		sdp_list_append( l2cap_list, l2cap_port);
		proto_list = sdp_list_append( 0, l2cap_list );

    // set rfcomm information
    sdp_uuid16_create(&rfcomm_uuid, RFCOMM_UUID);
    channel = sdp_data_alloc(SDP_UINT8, &rfcomm_channel);
    rfcomm_list = sdp_list_append( 0, &rfcomm_uuid );
    sdp_list_append( rfcomm_list, channel );
    sdp_list_append( proto_list, rfcomm_list );

    // attach protocol information to service record
    access_proto_list = sdp_list_append( 0, proto_list );
    sdp_set_access_protos( record, access_proto_list );

    // set the name, provider, and description
    sdp_set_info_attr(record, service_name, service_prov, service_dsc);
    int err = 0;
    sdp_session_t *session = 0;

    // connect to the local SDP server, register the service record, and 
    // disconnect
    session = sdp_connect( BDADDR_ANY, BDADDR_LOCAL, SDP_RETRY_IF_BUSY );
    err = sdp_record_register(session, record, 0);

    // cleanup
    sdp_data_free( channel );
    sdp_list_free( l2cap_list, 0 );
    sdp_list_free( rfcomm_list, 0 );
    sdp_list_free( root_list, 0 );
    sdp_list_free( access_proto_list, 0 );

    return session;
}

int main(int argc, char *argv[])
{
    if( argc < 3)
    {
      printf("usage: %s <rfcomm-channel> <l2cap-channel>\n", argv[0]);
      exit(1);
    }
   	sdp_session_t *session = register_service(atoi(argv[1]), atoi( argv[2]));
    printf("service registered, sleeping now zzzzzz (verify using service-search-sdp.c)\n");
		sleep(1000); 
    return 0;
}
