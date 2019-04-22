/*
 * Copyright (C) Yichun Zhang (agentzh)
 */


#ifndef DDEBUG
#define DDEBUG 0
#endif
#include "ddebug.h"


#include <ngx_core.h>
#include <ngx_stream.h>
#include <lauxlib.h>
#include "ngx_stream_lua_api.h"


ngx_module_t ngx_stream_lua_upstream_module;


static ngx_int_t ngx_stream_lua_upstream_init(ngx_conf_t *cf);
static int ngx_stream_lua_upstream_create_module(lua_State * L);
static int ngx_stream_lua_upstream_get_upstreams(lua_State * L);
static int ngx_stream_lua_upstream_get_servers(lua_State * L);
static ngx_stream_upstream_main_conf_t *
    ngx_stream_lua_upstream_get_upstream_main_conf(lua_State *L);
static int ngx_stream_lua_upstream_get_primary_peers(lua_State * L);
static int ngx_stream_lua_upstream_get_backup_peers(lua_State * L);
static int ngx_stream_lua_get_peer(lua_State *L,
    ngx_stream_upstream_rr_peer_t *peer, ngx_uint_t id);
static ngx_stream_upstream_srv_conf_t *
    ngx_stream_lua_upstream_find_upstream(lua_State *L, ngx_str_t *host);
static ngx_stream_upstream_rr_peer_t *
    ngx_stream_lua_upstream_lookup_peer(lua_State *L);
static int ngx_stream_lua_upstream_set_peer_down(lua_State * L);
//static int ngx_stream_lua_upstream_current_upstream_name(lua_State *L);


static ngx_stream_module_t ngx_stream_lua_upstream_ctx = {
    NULL,                           /* preconfiguration */
    ngx_stream_lua_upstream_init,     /* postconfiguration */
    NULL,                           /* create main configuration */
    NULL,                           /* init main configuration */
    NULL,                           /* create server configuration */
    NULL                            /* merge server configuration */
};


ngx_module_t ngx_stream_lua_upstream_module = {
    NGX_MODULE_V1,
    &ngx_stream_lua_upstream_ctx,  /* module context */
    NULL,                        /* module directives */
    NGX_STREAM_MODULE,             /* module type */
    NULL,                        /* init master */
    NULL,                        /* init module */
    NULL,                        /* init process */
    NULL,                        /* init thread */
    NULL,                        /* exit thread */
    NULL,                        /* exit process */
    NULL,                        /* exit master */
    NGX_MODULE_V1_PADDING
};


static ngx_int_t
ngx_stream_lua_upstream_init(ngx_conf_t *cf)
{
    if (ngx_stream_lua_add_package_preload(cf, "ngx.stream.upstream",
                                         ngx_stream_lua_upstream_create_module)
        != NGX_OK)
    {
        return NGX_ERROR;
    }

    return NGX_OK;
}


static int
ngx_stream_lua_upstream_create_module(lua_State * L)
{
    lua_createtable(L, 0, 6);

    lua_pushcfunction(L, ngx_stream_lua_upstream_get_upstreams);
    lua_setfield(L, -2, "get_upstreams");

    lua_pushcfunction(L, ngx_stream_lua_upstream_get_servers);
    lua_setfield(L, -2, "get_servers");

    lua_pushcfunction(L, ngx_stream_lua_upstream_get_primary_peers);
    lua_setfield(L, -2, "get_primary_peers");

    lua_pushcfunction(L, ngx_stream_lua_upstream_get_backup_peers);
    lua_setfield(L, -2, "get_backup_peers");

    lua_pushcfunction(L, ngx_stream_lua_upstream_set_peer_down);
    lua_setfield(L, -2, "set_peer_down");

    return 1;
}
