/*
 *  NFSv4 ACL Code
 *  Print the contents of an nfs4 ACL
 *
 *  Permission mapping:
 *  r - NFS4_ACE_READ_DATA
 *  l - NFS4_ACE_LIST_DIRECTORY
 *  w - NFS4_ACE_WRITE_DATA
 *  f - NFS4_ACE_ADD_FILE
 *  a - NFS4_ACE_APPEND_DATA
 *  s - NFS4_ACE_ADD_SUBDIRECTORY
 *  n - NFS4_ACE_READ_NAMED_ATTRS
 *  N - NFS4_ACE_WRITE_NAMED_ATTRS
 *  x - NFS4_ACE_EXECUTE
 *  D - NFS4_ACE_DELETE_CHILD
 *  t - NFS4_ACE_READ_ATTRIBUTES
 *  T - NFS4_ACE_WRITE_ATTRIBUTES
 *  d - NFS4_ACE_DELETE
 *  c - NFS4_ACE_READ_ACL
 *  C - NFS4_ACE_WRITE_ACL
 *  o - NFS4_ACE_WRITE_OWNER
 *  y - NFS4_ACE_SYNCHRONIZE
 *
 *
 *  Copyright (c) 2002, 2003, 2006 The Regents of the University of Michigan.
 *  All rights reserved.
 *
 *  Nathaniel Gallaher <ngallahe@umich.edu>
 *  David M. Richter <richterd@citi.umich.edu>
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

#include <stdio.h>
#include "libacl_nfs4.h"


void nfs4_print_acl(FILE *fp, struct nfs4_acl *acl)
{
	struct nfs4_ace* ace;
	int ace_count = 1;

	ace = nfs4_get_first_ace(acl);
	while (1) {
		if (ace == NULL) {
			if (acl->naces > ace_count)
				goto unexp_failed;
			else
				break;
		}
		nfs4_print_ace(fp, ace, acl->is_directory);
		nfs4_get_next_ace(&ace);
		ace_count++;
	}
	fflush(fp);

	return;

unexp_failed:
	fprintf(stderr, "An unexpected failure has occurred. \n");
	return;
}
