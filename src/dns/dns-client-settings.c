/* Copyright (c) 2010-2018 Dovecot authors, see the included COPYING file */

#include "lib.h"
#include "buffer.h"
#include "settings-parser.h"
#include "service-settings.h"

#include <stddef.h>

/* <settings checks> */
static struct file_listener_settings dns_client_unix_listeners_array[] = {
	{
		.path = "dns-client",
		.mode = 0666,
		.user = "",
		.group = "",
	},
	{
		.path = "login/dns-client",
		.mode = 0666,
		.user = "",
		.group = "",
	},
};
static struct file_listener_settings *dns_client_unix_listeners[] = {
	&dns_client_unix_listeners_array[0],
        &dns_client_unix_listeners_array[1],
};
static buffer_t dns_client_unix_listeners_buf = {
	{ { dns_client_unix_listeners, sizeof(dns_client_unix_listeners) } }
};
/* </settings checks> */

struct service_settings dns_client_service_settings = {
	.name = "dns-client",
	.protocol = "",
	.type = "",
	.executable = "dns-client",
	.user = "$default_internal_user",
	.group = "",
	.privileged_group = "",
	.extra_groups = "",
	.chroot = "",

	.drop_priv_before_exec = FALSE,

	.process_min_avail = 0,
	.process_limit = 0,
	.client_limit = 1,
	.service_count = 0,
	.idle_kill = 0,
	.vsz_limit = UOFF_T_MAX,

	.unix_listeners = { { &dns_client_unix_listeners_buf,
			      sizeof(dns_client_unix_listeners[0]) } },
	.fifo_listeners = ARRAY_INIT,
	.inet_listeners = ARRAY_INIT
};
