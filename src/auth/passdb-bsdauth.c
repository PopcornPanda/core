/* Copyright (c) 2002-2018 Dovecot authors, see the included COPYING file */

#include "auth-common.h"
#include "passdb.h"

#ifdef PASSDB_BSDAUTH

#include "safe-memset.h"
#include "auth-cache.h"
#include "ipwd.h"
#include "mycrypt.h"

#include <login_cap.h>
#include <bsd_auth.h>

static void
bsdauth_verify_plain(struct auth_request *request, const char *password,
		    verify_plain_callback_t *callback)
{
	struct passwd pw;
	const char *type;
	int result;

	e_debug(authdb_event(request), "lookup");

	switch (i_getpwnam(request->fields.user, &pw)) {
	case -1:
		e_error(authdb_event(request),
			"getpwnam() failed: %m");
		callback(PASSDB_RESULT_INTERNAL_FAILURE, request);
		return;
	case 0:
		auth_request_log_unknown_user(request, AUTH_SUBSYS_DB);
		callback(PASSDB_RESULT_USER_UNKNOWN, request);
		return;
	}

	/* check if the password is valid */
	type = t_strdup_printf("auth-%s", request->fields.service);
	result = auth_userokay(request->fields.user, NULL,
			       t_strdup_noconst(type),
			       t_strdup_noconst(password));

	/* clear the passwords from memory */
	safe_memset(pw.pw_passwd, 0, strlen(pw.pw_passwd));

	if (result == 0) {
		auth_request_log_password_mismatch(request, AUTH_SUBSYS_DB);
		callback(PASSDB_RESULT_PASSWORD_MISMATCH, request);
		return;
	}

	/* make sure we're using the username exactly as it's in the database */
        auth_request_set_field(request, "user", pw.pw_name, NULL);

	callback(PASSDB_RESULT_OK, request);
}

static struct passdb_module *
bsdauth_preinit(pool_t pool, const char *args)
{
	struct passdb_module *module;
	const char *value;

	module = p_new(pool, struct passdb_module, 1);
	module->default_pass_scheme = "PLAIN"; /* same reason as PAM */
	module->blocking = TRUE;

	if (strcmp(args, "blocking=no") == 0)
		module->blocking = FALSE;
	else if (str_begins(args, "cache_key=", &value))
		module->default_cache_key = auth_cache_parse_key(pool, value);
	else if (*args != '\0')
		i_fatal("passdb bsdauth: Unknown setting: %s", args);
	return module;
}

static void bsdauth_deinit(struct passdb_module *module ATTR_UNUSED)
{
	endpwent();
}

struct passdb_module_interface passdb_bsdauth = {
	"bsdauth",

	bsdauth_preinit,
	NULL,
	bsdauth_deinit,

	bsdauth_verify_plain,
	NULL,
	NULL
};
#else
struct passdb_module_interface passdb_bsdauth = {
	.name = "bsdauth"
};
#endif
