/*
 * Copyright (C) Internet Systems Consortium, Inc. ("ISC")
 * Copyright (C) Internet Software Consortium.
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
 * REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
 * INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
 * LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
 * OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
 * PERFORMANCE OF THIS SOFTWARE.
 */

/* $Id: version.c,v 1.5 2007/06/19 23:47:16 tbox Exp $ */

#include <versions.h>

#include <bind9/version.h>

LIBBIND9_EXTERNAL_DATA const char bind9_version[] = VERSION;

LIBBIND9_EXTERNAL_DATA const unsigned int bind9_libinterface = LIBINTERFACE;
LIBBIND9_EXTERNAL_DATA const unsigned int bind9_librevision = LIBREVISION;
LIBBIND9_EXTERNAL_DATA const unsigned int bind9_libage = LIBAGE;
