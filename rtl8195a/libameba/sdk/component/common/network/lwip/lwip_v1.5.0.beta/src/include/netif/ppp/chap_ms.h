/*
 * chap_ms.h - Challenge Handshake Authentication Protocol definitions.
 *
 * Copyright (c) 1995 Eric Rosenquist.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. The name(s) of the authors of this software must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission.
 *
 * THE AUTHORS OF THIS SOFTWARE DISCLAIM ALL WARRANTIES WITH REGARD TO
 * THIS SOFTWARE, INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY
 * AND FITNESS, IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY
 * SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
 * AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING
 * OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 *
 * $Id: chap_ms.h,v 1.13 2004/11/15 22:13:26 paulus Exp $
 */

#include "lwip/opt.h"
#if PPP_SUPPORT && MSCHAP_SUPPORT  /* don't build if not configured for use in lwipopts.h */

#ifndef __CHAPMS_INCLUDE__

#define MD4_SIGNATURE_SIZE	16	/* 16 bytes in a MD4 message digest */
#define MAX_NT_PASSWORD		256	/* Max (Unicode) chars in an NT pass */

#define MS_CHAP_RESPONSE_LEN	49	/* Response length for MS-CHAP */
#define MS_CHAP2_RESPONSE_LEN	49	/* Response length for MS-CHAPv2 */
#define MS_AUTH_RESPONSE_LENGTH	40	/* MS-CHAPv2 authenticator response, */
					/* as ASCII */

/* Error codes for MS-CHAP failure messages. */
#define MS_CHAP_ERROR_RESTRICTED_LOGON_HOURS	646
#define MS_CHAP_ERROR_ACCT_DISABLED		647
#define MS_CHAP_ERROR_PASSWD_EXPIRED		648
#define MS_CHAP_ERROR_NO_DIALIN_PERMISSION	649
#define MS_CHAP_ERROR_AUTHENTICATION_FAILURE	691
#define MS_CHAP_ERROR_CHANGING_PASSWORD		709

/*
 * Offsets within the response field for MS-CHAP
 */
#define MS_CHAP_LANMANRESP	0
#define MS_CHAP_LANMANRESP_LEN	24
#define MS_CHAP_NTRESP		24
#define MS_CHAP_NTRESP_LEN	24
#define MS_CHAP_USENT		48

/*
 * Offsets within the response field for MS-CHAP2
 */
#define MS_CHAP2_PEER_CHALLENGE	0
#define MS_CHAP2_PEER_CHAL_LEN	16
#define MS_CHAP2_RESERVED_LEN	8
#define MS_CHAP2_NTRESP		24
#define MS_CHAP2_NTRESP_LEN	24
#define MS_CHAP2_FLAGS		48

#ifdef MPPE
#include "mppe.h"	/* MPPE_MAX_KEY_LEN */
extern u_char mppe_send_key[MPPE_MAX_KEY_LEN];
extern u_char mppe_recv_key[MPPE_MAX_KEY_LEN];
extern int mppe_keys_set;

/* These values are the RADIUS attribute values--see RFC 2548. */
#define MPPE_ENC_POL_ENC_ALLOWED 1
#define MPPE_ENC_POL_ENC_REQUIRED 2
#define MPPE_ENC_TYPES_RC4_40 2
#define MPPE_ENC_TYPES_RC4_128 4

/* used by plugins (using above values) */
extern void set_mppe_enc_types(int, int);
#endif

/* Are we the authenticator or authenticatee?  For MS-CHAPv2 key derivation. */
#define MS_CHAP2_AUTHENTICATEE 0
#define MS_CHAP2_AUTHENTICATOR 1

void ChapMS (u_char *, char *, int, u_char *);
void ChapMS2 (u_char *, u_char *, char *, char *, int,
		  u_char *, u_char[MS_AUTH_RESPONSE_LENGTH+1], int);
#ifdef MPPE
void mppe_set_keys (u_char *, u_char[MD4_SIGNATURE_SIZE]);
void mppe_set_keys2(u_char PasswordHashHash[MD4_SIGNATURE_SIZE],
		    u_char NTResponse[24], int IsServer);
#endif

void	ChallengeHash (u_char[16], u_char *, char *, u_char[8]);

void GenerateAuthenticatorResponse(u_char PasswordHashHash[MD4_SIGNATURE_SIZE],
			u_char NTResponse[24], u_char PeerChallenge[16],
			u_char *rchallenge, char *username,
			u_char authResponse[MS_AUTH_RESPONSE_LENGTH+1]);

extern const struct chap_digest_type chapms_digest;
extern const struct chap_digest_type chapms2_digest;

#define __CHAPMS_INCLUDE__
#endif /* __CHAPMS_INCLUDE__ */

#endif /* PPP_SUPPORT && MSCHAP_SUPPORT */
