
#include "mongoose.h"

#include "esp_log.h"

#include "user_mongoose.h"

static const char *TAG = "user_smartconfig";

static struct mg_serve_http_opts opts;

static void mg_ev_http_handler(struct mg_connection *nc, int ev, void *p)
{
    switch (ev)
    {
    case MG_EV_ACCEPT:
    {
        char addr[32];
        mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
        ESP_LOGD(TAG, "Connection %p from %s\n", nc, addr);
        break;
    }
    case MG_EV_HTTP_REQUEST:
    {
        char addr[32];
        struct http_message *hm = (struct http_message *)p;
        mg_sock_addr_to_str(&nc->sa, addr, sizeof(addr), MG_SOCK_STRINGIFY_IP | MG_SOCK_STRINGIFY_PORT);
        ESP_LOGD(TAG, "HTTP request from %s: %.*s %.*s\n", addr, (int)hm->method.len,
               hm->method.p, (int)hm->uri.len, hm->uri.p);
        
        
        mg_serve_http(nc, hm, opts);
        break;
    }
    case MG_EV_CLOSE:
    {
        ESP_LOGD(TAG, "Connection %p closed\n", nc);
        ESP_LOGD(TAG, "\nwebdav working...");
        break;
    }
    case MG_EV_POLL:
        break;
    case MG_EV_RECV:
        break;
    case MG_EV_SEND:
    {
        break;
    }
    default:
        ESP_LOGD(TAG, "UnKnown %p result : %d\n", nc, ev);
        break;
    }
}

static void mg_ev_dns_handler(struct mg_connection *nc, int ev, void *ev_data)
{
    in_addr_t s_our_ip_addr = inet_addr((const char*)"192.168.4.1");
    switch (ev)
    {
        case MG_DNS_MESSAGE:
        {
            struct mg_dns_message *msg = (struct mg_dns_message *)ev_data;
            struct mg_dns_reply reply = mg_dns_create_reply(&nc->send_mbuf, msg);

            for (int i = 0; i < msg->num_questions; i++)
            {
                char name[64];
                struct mg_dns_resource_record *rr = &msg->questions[i];
                if (rr->rtype == MG_DNS_A_RECORD)
                {
                    mg_dns_uncompress_name(msg, &rr->name, name, sizeof(name) - 1);

                    if (strcmp(name, BitHostName) == 0)
                    {
                        mg_dns_reply_record(&reply, rr, NULL, rr->rtype, 3600, &s_our_ip_addr, 4);
                    }
                    else if (strcmp(name, "www."BitHostName) == 0)
                    {
                        mg_dns_reply_record(&reply, rr, NULL, MG_DNS_CNAME_RECORD, 3600, BitHostName, strlen(BitHostName));

                        mg_dns_reply_record(&reply, rr, BitHostName, rr->rtype, 3600, &s_our_ip_addr, 4);
                    }
                }
            }
            mg_dns_send_reply(nc, &reply);
            nc->flags |= MG_F_SEND_AND_CLOSE;
            break;
        }
    default:
        break;
    }
}

static struct mg_mgr mgr_http;
static struct mg_mgr mgr_dns;
static bool mongoose_state = false;

void mg_init()
{
    if(mongoose_state)
    {
        mg_mgr_free(&mgr_http);
        mg_mgr_free(&mgr_dns);
    }
    // webdav
    opts.document_root = ".";
    opts.dav_document_root = ".";
    opts.dav_auth_file = "-";

    ESP_LOGD(TAG, "Starting web-server on port %s\n", MG_PORT_HTTP);
    
    mg_mgr_init(&mgr_http, NULL);

    struct mg_connection *nc_http = mg_bind(&mgr_http, MG_PORT_HTTP, mg_ev_http_handler);

    mg_set_protocol_http_websocket(nc_http);

    if (nc_http == NULL)
    {
        mg_mgr_free(&mgr_http);
        ESP_LOGD(TAG, "Error nc_http setting up listener!\n");
        return;
    }

    ESP_LOGD(TAG, "Starting dns-server on port %s\n", MG_PORT_DNS);

    mg_mgr_init(&mgr_dns, NULL);

    mg_set_nameserver(&mgr_dns, BitHostName);

    struct mg_connection *nc_dns = mg_bind(&mgr_dns, MG_PORT_DNS, mg_ev_dns_handler);

    mg_set_protocol_dns(nc_dns);

    if (nc_dns == NULL)
    {
        mg_mgr_free(&mgr_dns);
        ESP_LOGD(TAG, "Error nc_dns setting up listener!\n");
        return;
    }
    
    mongoose_state = true;
}

void mg_poll()
{
    if(mongoose_state)
    {
        mg_mgr_poll(&mgr_dns, 20);
        mg_mgr_poll(&mgr_http, 200);
    }
}

