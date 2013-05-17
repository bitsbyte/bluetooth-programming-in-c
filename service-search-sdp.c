#include <bluetooth/bluetooth.h>
#include <bluetooth/sdp.h>
#include <bluetooth/sdp_lib.h>
#include <stdlib.h>

int main(int argc, char **argv)
{
    /* on Samsung Galaxy S3
    Service Name: LIVIO_CONNECT
    Service RecHandle: 0x1000a
    Service Class ID List:
      UUID 128: fbe4be12-374a-486b-a473-24e39408a24d
    Protocol Descriptor List:
      "L2CAP" (0x0100)
      "RFCOMM" (0x0003)
    */ 
   uint8_t svc_uuid_int[] = { 0xfb, 0xe4, 0xbe, 0x12, 0x37, 0x4a, 0x48, 0x6b,
                               0xa4, 0x73, 0x24, 0xe3, 0x94, 0x08, 0xa2, 0x4d}; 
    uuid_t svc_uuid;
    int err;
    bdaddr_t target;
    sdp_list_t *response_list = NULL, *search_list, *attrid_list;
    sdp_session_t *session = 0;

    if(argc < 2)
    {
      printf("usage: %s <bt-addr> \n",argv[0]);
			exit(1);
    }

    str2ba( argv[1], &target );

    // connect to the SDP server running on the remote machine
    session = sdp_connect( BDADDR_ANY, &target, SDP_RETRY_IF_BUSY );

    // specify the UUID of the application we're searching for
    sdp_uuid128_create( &svc_uuid, &svc_uuid_int );
    search_list = sdp_list_append( NULL, &svc_uuid );
    // specify that we want a list of all the matching applications' attributes
    uint32_t range = 0x0000ffff;
    attrid_list = sdp_list_append( NULL, &range );
		printf("starting search...\n");
    // get a list of service records that have UUID 0xabcd
    err = sdp_service_search_attr_req( session, search_list, \
            SDP_ATTR_REQ_RANGE, attrid_list, &response_list);


    sdp_list_t *r = response_list;
		printf("search completed\n");
    // go through each of the service records
    for (; r; r = r->next ) {
        sdp_record_t *rec = (sdp_record_t*) r->data;
        sdp_list_t *proto_list;
        
        // get a list of the protocol sequences
        if( sdp_get_access_protos( rec, &proto_list ) == 0 ) {
        sdp_list_t *p = proto_list;

        // go through each protocol sequence
        for( ; p ; p = p->next ) {
            sdp_list_t *pds = (sdp_list_t*)p->data;

            // go through each protocol list of the protocol sequence
            for( ; pds ; pds = pds->next ) {

                // check the protocol attributes
                sdp_data_t *d = (sdp_data_t*)pds->data;
                int proto = 0;
                for( ; d; d = d->next ) {
                    switch( d->dtd ) { 
                        case SDP_UUID16:
                        case SDP_UUID32:
                        case SDP_UUID128:
                            proto = sdp_uuid_to_proto( &d->val.uuid );
                            printf("\tproto: 0x%x\n", proto);
                            break;
                        case SDP_UINT8:
                            if( proto == RFCOMM_UUID ) {
                                printf("rfcomm channel: %d\n",d->val.int8);
                            }
                            break;
                    }
                }
            }
            sdp_list_free( (sdp_list_t*)p->data, 0 );
        }
        sdp_list_free( proto_list, 0 );

        }

        printf("found service record 0x%x\n", rec->handle);
        sdp_record_free( rec );
    }

    sdp_close(session);
}
