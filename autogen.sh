#!/bin/sh
bs_dir=$(cd "$(dirname "$0")"; pwd)

../update_configure_ac_version.sh

autoreconf -fi "${bs_dir}"

if test -n "$1" && test -z "$NOCONFIGURE" ; then
	echo 'Configuring...'
	"$bs_dir"/configure "$@"
fi
