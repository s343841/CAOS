/* AUTOMATICALLY GENERATED, DO NOT MODIFY */

/*
 * Schema-defined QAPI/QMP commands
 *
 * Copyright IBM, Corp. 2011
 * Copyright (C) 2014-2018 Red Hat, Inc.
 *
 * This work is licensed under the terms of the GNU LGPL, version 2.1 or later.
 * See the COPYING.LIB file in the top-level directory.
 */

#ifndef QAPI_COMMANDS_UI_H
#define QAPI_COMMANDS_UI_H

#include "qapi-commands-common.h"
#include "qapi-commands-sockets.h"
#include "qapi-types-ui.h"

void qmp_set_password(SetPasswordOptions *arg, Error **errp);
void qmp_marshal_set_password(QDict *args, QObject **ret, Error **errp);
void qmp_expire_password(ExpirePasswordOptions *arg, Error **errp);
void qmp_marshal_expire_password(QDict *args, QObject **ret, Error **errp);
void coroutine_fn qmp_screendump(const char *filename, const char *device, bool has_head, int64_t head, bool has_format, ImageFormat format, Error **errp);
void coroutine_fn qmp_marshal_screendump(QDict *args, QObject **ret, Error **errp);
#if defined(CONFIG_SPICE)
SpiceInfo *qmp_query_spice(Error **errp);
void qmp_marshal_query_spice(QDict *args, QObject **ret, Error **errp);
#endif /* defined(CONFIG_SPICE) */
#if defined(CONFIG_VNC)
VncInfo *qmp_query_vnc(Error **errp);
void qmp_marshal_query_vnc(QDict *args, QObject **ret, Error **errp);
#endif /* defined(CONFIG_VNC) */
#if defined(CONFIG_VNC)
VncInfo2List *qmp_query_vnc_servers(Error **errp);
void qmp_marshal_query_vnc_servers(QDict *args, QObject **ret, Error **errp);
#endif /* defined(CONFIG_VNC) */
#if defined(CONFIG_VNC)
void qmp_change_vnc_password(const char *password, Error **errp);
void qmp_marshal_change_vnc_password(QDict *args, QObject **ret, Error **errp);
#endif /* defined(CONFIG_VNC) */
MouseInfoList *qmp_query_mice(Error **errp);
void qmp_marshal_query_mice(QDict *args, QObject **ret, Error **errp);
void qmp_send_key(KeyValueList *keys, bool has_hold_time, int64_t hold_time, Error **errp);
void qmp_marshal_send_key(QDict *args, QObject **ret, Error **errp);
void qmp_input_send_event(const char *device, bool has_head, int64_t head, InputEventList *events, Error **errp);
void qmp_marshal_input_send_event(QDict *args, QObject **ret, Error **errp);
DisplayOptions *qmp_query_display_options(Error **errp);
void qmp_marshal_query_display_options(QDict *args, QObject **ret, Error **errp);
void qmp_display_reload(DisplayReloadOptions *arg, Error **errp);
void qmp_marshal_display_reload(QDict *args, QObject **ret, Error **errp);
void qmp_display_update(DisplayUpdateOptions *arg, Error **errp);
void qmp_marshal_display_update(QDict *args, QObject **ret, Error **errp);
void qmp_client_migrate_info(const char *protocol, const char *hostname, bool has_port, int64_t port, bool has_tls_port, int64_t tls_port, const char *cert_subject, Error **errp);
void qmp_marshal_client_migrate_info(QDict *args, QObject **ret, Error **errp);

#endif /* QAPI_COMMANDS_UI_H */