/**
 *		Tempesta TLS
 *
 * TLS ciphersuites.
 *
 * Copyright (C) 2006-2015, ARM Limited, All Rights Reserved
 * Copyright (C) 2015-2018 Tempesta Technologies, Inc.
 * SPDX-License-Identifier: GPL-2.0
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */
#include "config.h"
#include "ssl_ciphersuites.h"
#include "ttls.h"

/*
 * Ordered from most preferred to least preferred in terms of security.
 *
 * Current rule:
 * 1. By key exchange: Forward-secure non-PSK > other non-PSK
 * 2. By key length and cipher: AES-256 > AES-128
 * 3. By cipher mode when relevant GCM > CCM > CCM_8
 * 4. By hash function used when relevant
 * 5. By key exchange/auth again: EC > non-EC
 *
 * Note that there is no TLS_RSA_WITH_AES_128_CBC_SHA required by RFC 5246.
 * Current TLS version is 1.3, so we support TLS 1.2 for legacy only clients.
 * Next, CBC mode has security issues (so it was removed from TLS 1.3) and
 * incurs significant pipeline stalls that hamper its efficiency and
 * performance. Last, it requires additional code work flow.
 */
static const int ciphersuite_preference[] = {
	/* All AES-256 ephemeral suites */
	TTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384,
	TTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384,
	TTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384,
	TTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CCM,
	TTLS_TLS_DHE_RSA_WITH_AES_256_CCM,
	TTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8,
	TTLS_TLS_DHE_RSA_WITH_AES_256_CCM_8,

	/* All AES-128 ephemeral suites */
	TTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256,
	TTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256,
	TTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256,
	TTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM,
	TTLS_TLS_DHE_RSA_WITH_AES_128_CCM,
	TTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8,
	TTLS_TLS_DHE_RSA_WITH_AES_128_CCM_8,

	/* All AES-256 suites */
	TTLS_TLS_RSA_WITH_AES_256_GCM_SHA384,
	TTLS_TLS_RSA_WITH_AES_256_CCM,
	TTLS_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384,
	TTLS_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384,
	TTLS_TLS_RSA_WITH_AES_256_CCM_8,

	/* All AES-128 suites */
	TTLS_TLS_RSA_WITH_AES_128_GCM_SHA256,
	TTLS_TLS_RSA_WITH_AES_128_CCM,
	TTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256,
	TTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256,
	TTLS_TLS_RSA_WITH_AES_128_CCM_8,

	0
};

static const ttls_ciphersuite_t ciphersuite_definitions[] =
{
	{ TTLS_TLS_ECDHE_ECDSA_WITH_AES_128_GCM_SHA256, "TLS-ECDHE-ECDSA-WITH-AES-128-GCM-SHA256",
	  TTLS_CIPHER_AES_128_GCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDHE_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDHE_ECDSA_WITH_AES_256_GCM_SHA384, "TLS-ECDHE-ECDSA-WITH-AES-256-GCM-SHA384",
	  TTLS_CIPHER_AES_256_GCM, TTLS_MD_SHA384, TTLS_KEY_EXCHANGE_ECDHE_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CCM, "TLS-ECDHE-ECDSA-WITH-AES-256-CCM",
	  TTLS_CIPHER_AES_256_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDHE_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDHE_ECDSA_WITH_AES_256_CCM_8, "TLS-ECDHE-ECDSA-WITH-AES-256-CCM-8",
	  TTLS_CIPHER_AES_256_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDHE_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_CIPHERSUITE_SHORT_TAG },
	{ TTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM, "TLS-ECDHE-ECDSA-WITH-AES-128-CCM",
	  TTLS_CIPHER_AES_128_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDHE_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDHE_ECDSA_WITH_AES_128_CCM_8, "TLS-ECDHE-ECDSA-WITH-AES-128-CCM-8",
	  TTLS_CIPHER_AES_128_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDHE_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_CIPHERSUITE_SHORT_TAG },

	{ TTLS_TLS_ECDHE_RSA_WITH_AES_128_GCM_SHA256, "TLS-ECDHE-RSA-WITH-AES-128-GCM-SHA256",
	  TTLS_CIPHER_AES_128_GCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDHE_RSA_WITH_AES_256_GCM_SHA384, "TLS-ECDHE-RSA-WITH-AES-256-GCM-SHA384",
	  TTLS_CIPHER_AES_256_GCM, TTLS_MD_SHA384, TTLS_KEY_EXCHANGE_ECDHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },

	{ TTLS_TLS_DHE_RSA_WITH_AES_256_GCM_SHA384, "TLS-DHE-RSA-WITH-AES-256-GCM-SHA384",
	  TTLS_CIPHER_AES_256_GCM, TTLS_MD_SHA384, TTLS_KEY_EXCHANGE_DHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },

	{ TTLS_TLS_DHE_RSA_WITH_AES_128_GCM_SHA256, "TLS-DHE-RSA-WITH-AES-128-GCM-SHA256",
	  TTLS_CIPHER_AES_128_GCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_DHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },

	{ TTLS_TLS_DHE_RSA_WITH_AES_256_CCM, "TLS-DHE-RSA-WITH-AES-256-CCM",
	  TTLS_CIPHER_AES_256_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_DHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_DHE_RSA_WITH_AES_256_CCM_8, "TLS-DHE-RSA-WITH-AES-256-CCM-8",
	  TTLS_CIPHER_AES_256_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_DHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_CIPHERSUITE_SHORT_TAG },
	{ TTLS_TLS_DHE_RSA_WITH_AES_128_CCM, "TLS-DHE-RSA-WITH-AES-128-CCM",
	  TTLS_CIPHER_AES_128_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_DHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_DHE_RSA_WITH_AES_128_CCM_8, "TLS-DHE-RSA-WITH-AES-128-CCM-8",
	  TTLS_CIPHER_AES_128_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_DHE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_CIPHERSUITE_SHORT_TAG },

	{ TTLS_TLS_RSA_WITH_AES_256_GCM_SHA384, "TLS-RSA-WITH-AES-256-GCM-SHA384",
	  TTLS_CIPHER_AES_256_GCM, TTLS_MD_SHA384, TTLS_KEY_EXCHANGE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },

	{ TTLS_TLS_RSA_WITH_AES_128_GCM_SHA256, "TLS-RSA-WITH-AES-128-GCM-SHA256",
	  TTLS_CIPHER_AES_128_GCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_RSA_WITH_AES_256_CCM, "TLS-RSA-WITH-AES-256-CCM",
	  TTLS_CIPHER_AES_256_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_RSA_WITH_AES_256_CCM_8, "TLS-RSA-WITH-AES-256-CCM-8",
	  TTLS_CIPHER_AES_256_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_CIPHERSUITE_SHORT_TAG },
	{ TTLS_TLS_RSA_WITH_AES_128_CCM, "TLS-RSA-WITH-AES-128-CCM",
	  TTLS_CIPHER_AES_128_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_RSA_WITH_AES_128_CCM_8, "TLS-RSA-WITH-AES-128-CCM-8",
	  TTLS_CIPHER_AES_128_CCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_CIPHERSUITE_SHORT_TAG },

	{ TTLS_TLS_ECDH_RSA_WITH_AES_128_GCM_SHA256, "TLS-ECDH-RSA-WITH-AES-128-GCM-SHA256",
	  TTLS_CIPHER_AES_128_GCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDH_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDH_RSA_WITH_AES_256_GCM_SHA384, "TLS-ECDH-RSA-WITH-AES-256-GCM-SHA384",
	  TTLS_CIPHER_AES_256_GCM, TTLS_MD_SHA384, TTLS_KEY_EXCHANGE_ECDH_RSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },

	{ TTLS_TLS_ECDH_ECDSA_WITH_AES_128_GCM_SHA256, "TLS-ECDH-ECDSA-WITH-AES-128-GCM-SHA256",
	  TTLS_CIPHER_AES_128_GCM, TTLS_MD_SHA256, TTLS_KEY_EXCHANGE_ECDH_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },
	{ TTLS_TLS_ECDH_ECDSA_WITH_AES_256_GCM_SHA384, "TLS-ECDH-ECDSA-WITH-AES-256-GCM-SHA384",
	  TTLS_CIPHER_AES_256_GCM, TTLS_MD_SHA384, TTLS_KEY_EXCHANGE_ECDH_ECDSA,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  TTLS_MAJOR_VERSION_3, TTLS_MINOR_VERSION_3,
	  0 },

	{ 0, "",
	  TTLS_CIPHER_NONE, TTLS_MD_NONE, TTLS_KEY_EXCHANGE_NONE,
	  0, 0, 0, 0, 0 }
};

#define MAX_CIPHERSUITES	ARRAY_SIZE(ciphersuite_definitions)
static int supported_ciphersuites[MAX_CIPHERSUITES];
static int supported_init = 0;

const int *ttls_list_ciphersuites(void)
{
	/*
	 * On initial call filter out all ciphersuites not supported by current
	 * build based on presence in the ciphersuite_definitions.
	 */
	if (supported_init == 0)
	{
		const int *p;
		int *q;

		for (p = ciphersuite_preference, q = supported_ciphersuites;
			 *p != 0 && q < supported_ciphersuites + MAX_CIPHERSUITES - 1;
			 p++)
		{
			const ttls_ciphersuite_t *cs_info;
			if ((cs_info = ttls_ciphersuite_from_id(*p)))
				*(q++) = *p;
		}
		*q = 0;

		supported_init = 1;
	}

	return(supported_ciphersuites);
}

const ttls_ciphersuite_t *ttls_ciphersuite_from_string(
			const char *ciphersuite_name)
{
	const ttls_ciphersuite_t *cur = ciphersuite_definitions;

	if (NULL == ciphersuite_name)
		return(NULL);

	while (cur->id != 0)
	{
		if (0 == strcmp(cur->name, ciphersuite_name))
			return(cur);

		cur++;
	}

	return(NULL);
}

const ttls_ciphersuite_t *
ttls_ciphersuite_from_id(int ciphersuite)
{
	const ttls_ciphersuite_t *cur = ciphersuite_definitions;

	while (cur->id) {
		if (cur->id == ciphersuite)
			return cur;
		cur++;
	}

	return NULL;
}

const char *ttls_get_ciphersuite_name(const int ciphersuite_id)
{
	const ttls_ciphersuite_t *cur;

	cur = ttls_ciphersuite_from_id(ciphersuite_id);

	if (cur == NULL)
		return("unknown");

	return(cur->name);
}

int ttls_get_ciphersuite_id(const char *ciphersuite_name)
{
	const ttls_ciphersuite_t *cur;

	cur = ttls_ciphersuite_from_string(ciphersuite_name);

	if (cur == NULL)
		return 0;

	return(cur->id);
}

ttls_pk_type_t
ttls_get_ciphersuite_sig_pk_alg(const ttls_ciphersuite_t *info)
{
	switch (info->key_exchange) {
	case TTLS_KEY_EXCHANGE_RSA:
	case TTLS_KEY_EXCHANGE_DHE_RSA:
	case TTLS_KEY_EXCHANGE_ECDHE_RSA:
	case TTLS_KEY_EXCHANGE_RSA_PSK:
		return TTLS_PK_RSA;

	case TTLS_KEY_EXCHANGE_ECDHE_ECDSA:
		return TTLS_PK_ECDSA;

	case TTLS_KEY_EXCHANGE_ECDH_RSA:
	case TTLS_KEY_EXCHANGE_ECDH_ECDSA:
		return TTLS_PK_ECKEY;

	default:
		return TTLS_PK_NONE;
	}
}

ttls_pk_type_t ttls_get_ciphersuite_sig_alg(const ttls_ciphersuite_t *info)
{
	switch(info->key_exchange)
	{
		case TTLS_KEY_EXCHANGE_RSA:
		case TTLS_KEY_EXCHANGE_DHE_RSA:
		case TTLS_KEY_EXCHANGE_ECDHE_RSA:
			return(TTLS_PK_RSA);

		case TTLS_KEY_EXCHANGE_ECDHE_ECDSA:
			return(TTLS_PK_ECDSA);

		default:
			return(TTLS_PK_NONE);
	}
}

int
ttls_ciphersuite_uses_ec(const ttls_ciphersuite_t *info)
{
	switch (info->key_exchange) {
		case TTLS_KEY_EXCHANGE_ECDHE_RSA:
		case TTLS_KEY_EXCHANGE_ECDHE_ECDSA:
		case TTLS_KEY_EXCHANGE_ECDHE_PSK:
		case TTLS_KEY_EXCHANGE_ECDH_RSA:
		case TTLS_KEY_EXCHANGE_ECDH_ECDSA:
			return 1;
		default:
			return 0;
	}
}
