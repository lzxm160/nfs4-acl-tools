/*
 *  NFSv4 ACL Code
 *  Convert NFSv4 xattr values to a posix ACL
 *
 *  Copyright (c) 2002, 2003, 2006 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Nathaniel Gallaher <ngallahe@umich.edu>
 *
 *  Redistribution and use in source and binary forms, with or without
 *  modification, are permitted provided that the following conditions
 *  are met:
 *
 *  1. Redistributions of source code must retain the above copyright
 *     notice, this list of conditions and the following disclaimer.
 *  2. Redistributions in binary form must reproduce the above copyright
 *     notice, this list of conditions and the following disclaimer in the
 *     documentation and/or other materials provided with the distribution.
 *  3. Neither the name of the University nor the names of its
 *     contributors may be used to endorse or promote products derived
 *     from this software without specific prior written permission.
 *
 *  THIS SOFTWARE IS PROVIDED ``AS IS'' AND ANY EXPRESS OR IMPLIED
 *  WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
 *  MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 *  DISCLAIMED. IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
 *  FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 *  CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 *  SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
 *  BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 *  LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 *  NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 *  SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#include <netinet/in.h>
#include "libacl_nfs4.h"


struct nfs4_acl * acl_nfs4_xattr_load(char *xattr_v, int xattr_size, u32 is_dir)
{
	struct nfs4_acl *acl;
	struct nfs4_ace *ace;
	char *bufp = xattr_v;
	int bufs = xattr_size;
	u32 ace_n;
	u32	wholen;
	char *who;
	int d_ptr;
	u32 num_aces;
	u32 type, flag, access_mask;

	if (xattr_size < sizeof(u32)) {
		errno = EINVAL;
		return NULL;
	}

	if ((acl = nfs4_new_acl(is_dir)) == NULL) {
		errno = ENOMEM;
		return NULL;
	}

	/* Grab the number of aces in the acl */
	num_aces = (u32)ntohl(*((u32*)(bufp)));

#ifdef LIBACL_NFS4_DEBUG
	printf(" Got number of aces: %d\n", acl->naces);
#endif

	d_ptr = sizeof(u32);
	bufp += d_ptr;
	bufs -= d_ptr;

	for(ace_n = 0; num_aces > ace_n ; ace_n++) {
#ifdef LIBACL_NFS4_DEBUG
		printf(" Getting Ace #%d of %d\n", ace_n, num_aces);
#endif
		/* Get the acl type */
		if(bufs <= 0) {
			errno = EINVAL;
			goto err1;
		}

		type = (u32)ntohl(*((u32*)bufp));
#ifdef LIBACL_NFS4_DEBUG
		printf("  Type: %x\n", type);
#endif

		d_ptr = sizeof(u32);
		bufp += d_ptr;
		bufs -= d_ptr;

		/* Get the acl flag */
		if (bufs <= 0) {
			errno = EINVAL;
			goto err1;
		}

		flag = (u32)ntohl(*((u32*)bufp));
#ifdef LIBACL_NFS4_DEBUG
		printf("  Flag: %x\n", flag);
#endif

		bufp += d_ptr;
		bufs -= d_ptr;

		/* Get the access mask */
		if (bufs <= 0) {
			errno = EINVAL;
			goto err1;
		}

		access_mask = (u32)ntohl(*((u32*)bufp));
#ifdef LIBACL_NFS4_DEBUG
		printf("  Access Mask: %x\n", access_mask);
#endif

		bufp += d_ptr;
		bufs -= d_ptr;

		/* Get the who string length*/
		if (bufs <= 0) {
			errno = EINVAL;
			goto err1;
		}

		wholen = (u32)ntohl(*((u32*)bufp));
#ifdef LIBACL_NFS4_DEBUG
		printf("  Wholen: %d\n", wholen);
#endif

		bufp += d_ptr;
		bufs -= d_ptr;

		/* Get the who string */
		if (bufs <= 0) {
			errno = EINVAL;
			goto err1;
		}

		who = (char *) malloc((wholen+1) * sizeof(char));
		if (who == NULL) {
			errno = ENOMEM;
			goto err1;
		}

		memcpy(who, bufp, wholen);
		who[wholen] = '\0';

#ifdef LIBACL_NFS4_DEBUG
		printf("  Who: %s\n", who);
#endif

		d_ptr = ((wholen / sizeof(u32))*sizeof(u32));
		if (wholen % sizeof(u32) != 0)
			d_ptr += sizeof(u32);

		bufp += d_ptr;
		bufs -= d_ptr;

		/* Make sure we aren't outside our domain */
		if (bufs < 0)
			goto err2;

		ace = nfs4_new_ace(is_dir, type, flag, access_mask, acl_nfs4_get_whotype(who), who);
		if (ace == NULL)
			goto err2;

		if (nfs4_append_ace(acl, ace))
			goto err2;

		free(who);
	}
	return acl;

err2:
	free(who);
err1:
	nfs4_free_acl(acl);
	return NULL;
}
