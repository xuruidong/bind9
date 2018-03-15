#!/bin/sh
#
# Copyright (C) Internet Systems Consortium, Inc. ("ISC")
#
# Permission to use, copy, modify, and/or distribute this software for any
# purpose with or without fee is hereby granted, provided that the above
# copyright notice and this permission notice appear in all copies.
#
# THE SOFTWARE IS PROVIDED "AS IS" AND ISC DISCLAIMS ALL WARRANTIES WITH
# REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
# AND FITNESS.  IN NO EVENT SHALL ISC BE LIABLE FOR ANY SPECIAL, DIRECT,
# INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM
# LOSS OF USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE
# OR OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
# PERFORMANCE OF THIS SOFTWARE.

SYSTEMTESTTOP=..
. $SYSTEMTESTTOP/conf.sh

DIGOPTS="-p ${PORT}"

status=0
n=0

n=`expr $n + 1`
echo_i "checking drop edns server setup ($n)"
ret=0
$DIG $DIGOPTS +edns @10.53.0.2 dropedns soa > dig.out.1.test$n
grep "connection timed out; no servers could be reached" dig.out.1.test$n > /dev/null || ret=1
$DIG $DIGOPTS +noedns @10.53.0.2 dropedns soa > dig.out.2.test$n || ret=1
grep "status: NOERROR" dig.out.2.test$n > /dev/null || ret=1
grep "EDNS: version:" dig.out.2.test$n > /dev/null && ret=1
$DIG $DIGOPTS +noedns +tcp @10.53.0.2 dropedns soa > dig.out.3.test$n || ret=1
grep "status: NOERROR" dig.out.3.test$n > /dev/null || ret=1
grep "EDNS: version:" dig.out.3.test$n > /dev/null && ret=1
$DIG $DIGOPTS +edns +tcp @10.53.0.2 dropedns soa > dig.out.4.test$n
grep "connection timed out; no servers could be reached" dig.out.4.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking recursive lookup to drop edns server succeeds ($n)"
ret=0
$DIG $DIGOPTS +tcp @10.53.0.1 dropedns soa > dig.out.test$n || ret=1
grep "status: NOERROR" dig.out.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking drop edns + no tcp server setup ($n)"
ret=0
$DIG $DIGOPTS +edns @10.53.0.3 dropedns-notcp soa > dig.out.1.test$n
grep "connection timed out; no servers could be reached" dig.out.1.test$n > /dev/null || ret=1
$DIG $DIGOPTS +noedns +tcp @10.53.0.3 dropedns-notcp soa > dig.out.2.test$n
grep "connection timed out; no servers could be reached" dig.out.2.test$n > /dev/null
$DIG $DIGOPTS +noedns @10.53.0.3 dropedns-notcp soa > dig.out.3.test$n || ret=1
grep "status: NOERROR" dig.out.3.test$n > /dev/null || ret=1
grep "EDNS: version:" dig.out.3.test$n > /dev/null && ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking recursive lookup to drop edns + no tcp server succeeds ($n)"
ret=0
$DIG $DIGOPTS +tcp @10.53.0.1 dropedns-notcp soa > dig.out.test$n || ret=1
grep "status: NOERROR" dig.out.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking plain dns server setup ($n)"
ret=0
$DIG $DIGOPTS +edns @10.53.0.4 plain soa > dig.out.1.test$n || ret=1
grep "status: NOERROR" dig.out.1.test$n > /dev/null || ret=1
grep "EDNS: version:" dig.out.1.test$n > /dev/null && ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking recursive lookup to plain dns server succeeds ($n)"
ret=0
$DIG $DIGOPTS +tcp @10.53.0.1 plain soa > dig.out.test$n || ret=1
grep "status: NOERROR" dig.out.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking plain dns + no tcp server setup ($n)"
ret=0
$DIG $DIGOPTS +edns @10.53.0.5 plain-notcp soa > dig.out.1.test$n || ret=1
grep "status: NOERROR" dig.out.1.test$n > /dev/null || ret=1
grep "EDNS: version:" dig.out.1.test$n > /dev/null && ret=1
$DIG $DIGOPTS +edns +tcp @10.53.0.5 plain-notcp soa > dig.out.2.test$n
grep "connection timed out; no servers could be reached" dig.out.2.test$n > /dev/null
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking recursive lookup to plain dns + no tcp server succeeds ($n)"
ret=0
$DIG $DIGOPTS +tcp @10.53.0.1 plain-notcp soa > dig.out.test$n || ret=1
grep "status: NOERROR" dig.out.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`
n=`expr $n + 1`

echo_i "checking edns 512 server setup ($n)"
ret=0
$DIG $DIGOPTS +edns @10.53.0.6 edns512 soa > dig.out.1.test$n || ret=1
grep "status: NOERROR" dig.out.1.test$n > /dev/null || ret=1
$DIG $DIGOPTS +edns +tcp @10.53.0.6 edns512 soa > dig.out.2.test$n || ret=1
grep "status: NOERROR" dig.out.1.test$n > /dev/null || ret=1
$DIG $DIGOPTS +edns @10.53.0.6 txt500.edns512 txt > dig.out.3.test$n
grep "connection timed out; no servers could be reached" dig.out.3.test$n > /dev/null
$DIG $DIGOPTS +edns +bufsize=512 +ignor @10.53.0.6 txt500.edns512 txt > dig.out.4.test$n
grep "status: NOERROR" dig.out.4.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking recursive lookup to edns 512 server succeeds ($n)"
ret=0
$DIG $DIGOPTS +tcp @10.53.0.1 txt500.edns512 txt > dig.out.test$n || ret=1
grep "status: NOERROR" dig.out.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking edns 512 + no tcp server setup ($n)"
ret=0
$DIG $DIGOPTS +noedns @10.53.0.7 edns512-notcp soa > dig.out.1.test$n || ret=1
grep "status: NOERROR" dig.out.1.test$n > /dev/null || ret=1
$DIG $DIGOPTS +noedns +tcp @10.53.0.7 edns512-notcp soa > dig.out.2.test$n
grep "connection timed out; no servers could be reached" dig.out.2.test$n > /dev/null
$DIG $DIGOPTS +edns @10.53.0.7 edns512-notcp soa > dig.out.3.test$n
grep "connection timed out; no servers could be reached" dig.out.3.test$n > /dev/null
$DIG $DIGOPTS +edns +bufsize=512 +ignor @10.53.0.7 edns512-notcp soa > dig.out.4.test$n
grep "status: NOERROR" dig.out.4.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

n=`expr $n + 1`
echo_i "checking recursive lookup to edns 512 + no tcp server succeeds ($n)"
ret=0
$DIG $DIGOPTS +tcp @10.53.0.1 edns512-notcp soa > dig.out.test$n || ret=1
grep "status: NOERROR" dig.out.test$n > /dev/null || ret=1
if [ $ret != 0 ]; then echo_i "failed"; fi
status=`expr $status + $ret`

if $SHELL ../testcrypto.sh > /dev/null 2>&1
then
    $PERL $SYSTEMTESTTOP/stop.pl . ns1

    copy_setports ns1/named2.conf.in ns1/named.conf

    $PERL $SYSTEMTESTTOP/start.pl --noclean --restart --port ${PORT} . ns1

    n=`expr $n + 1`
    echo_i "checking recursive lookup to edns 512 + no tcp + trust anchor fails ($n)"
    ret=0
    $DIG $DIGOPTS +tcp @10.53.0.1 edns512-notcp soa > dig.out.test$n
    grep "status: SERVFAIL" dig.out.test$n > /dev/null ||
        grep "connection timed out;" dig.out.test$n > /dev/null || ret=1
    if [ $ret != 0 ]; then echo_i "failed"; fi
    status=`expr $status + $ret`
else
    echo_i "skipping checking recursive lookup to edns 512 + no tcp + trust anchor fails as crypto not enabled"
fi 

echo_i "exit status: $status"
[ $status -eq 0 ] || exit 1
