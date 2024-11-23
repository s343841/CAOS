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

#ifndef QAPI_COMMANDS_CXL_H
#define QAPI_COMMANDS_CXL_H

#include "qapi-types-cxl.h"

void qmp_cxl_inject_uncorrectable_errors(const char *path, CXLUncorErrorRecordList *errors, Error **errp);
void qmp_marshal_cxl_inject_uncorrectable_errors(QDict *args, QObject **ret, Error **errp);
void qmp_cxl_inject_correctable_error(const char *path, CxlCorErrorType type, Error **errp);
void qmp_marshal_cxl_inject_correctable_error(QDict *args, QObject **ret, Error **errp);

#endif /* QAPI_COMMANDS_CXL_H */
