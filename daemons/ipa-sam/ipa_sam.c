#define HAVE_IMMEDIATE_STRUCTURES 1
#define LDAP_DEPRECATED 1

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <pwd.h>
#include <errno.h>
#include <ldap.h>

#include <talloc.h>

#include <param.h>
#include <ndr.h>
#include <util/data_blob.h>
#include <util/time.h>
#include <util/debug.h>

#include <core/ntstatus.h>
#include <gen_ndr/security.h>
#include <smbldap.h>

#include <gen_ndr/samr.h>

#include <passdb.h>

#include <sasl/sasl.h>
#include <krb5/krb5.h>
#include <time.h>

/* TODO: remove if smbrunsecret() is removed */
typedef struct connection_structi {} connection_struct;
struct current_user {
	connection_struct *conn;
	uint16_t vuid;
	struct security_unix_token ut;
	struct security_token *nt_user_token;
};
extern struct current_user current_user;

/* from drsblobs.h */
struct AuthInfoNone {
	uint32_t size;/* [value(0)] */
};

struct AuthInfoNT4Owf {
	uint32_t size;/* [value(16)] */
	struct samr_Password password;
};

struct AuthInfoClear {
	uint32_t size;
	uint8_t *password;
};

struct AuthInfoVersion {
	uint32_t size;/* [value(4)] */
	uint32_t version;
};

union AuthInfo {
	struct AuthInfoNone none;/* [case(TRUST_AUTH_TYPE_NONE)] */
	struct AuthInfoNT4Owf nt4owf;/* [case(TRUST_AUTH_TYPE_NT4OWF)] */
	struct AuthInfoClear clear;/* [case(TRUST_AUTH_TYPE_CLEAR)] */
	struct AuthInfoVersion version;/* [case(TRUST_AUTH_TYPE_VERSION)] */
}/* [nodiscriminant] */;

struct AuthenticationInformation {
	NTTIME LastUpdateTime;
	enum lsa_TrustAuthType AuthType;
	union AuthInfo AuthInfo;/* [switch_is(AuthType)] */
	DATA_BLOB _pad;/* [flag(LIBNDR_FLAG_ALIGN4)] */
}/* [public] */;

struct AuthenticationInformationArray {
	uint32_t count;
	struct AuthenticationInformation *array;
}/* [gensize,nopush,public,nopull] */;

struct trustAuthInOutBlob {
	uint32_t count;
	uint32_t current_offset;/* [value((count>0)?12:0)] */
	uint32_t previous_offset;/* [value((count>0)?12+ndr_size_AuthenticationInformationArray(&current,ndr->flags):0)] */
	struct AuthenticationInformationArray current;/* [subcontext_size((previous_offset)-(current_offset)),subcontext(0)] */
	struct AuthenticationInformationArray previous;/* [subcontext(0),flag(LIBNDR_FLAG_REMAINING)] */
}/* [gensize,public,nopush] */;


enum ndr_err_code ndr_pull_trustAuthInOutBlob(struct ndr_pull *ndr, int ndr_flags, struct trustAuthInOutBlob *r); /*available in libndr-samba.so */
bool fetch_ldap_pw(char **dn, char** pw); /* available in libpdb.so */
void nt_lm_owf_gen(const char *pwd, uint8_t nt_p16[16], uint8_t p16[16]); /* available in libcliauth.so */
bool sid_check_is_builtin(const struct dom_sid *sid); /* available in libpdb.so */
void strlower_m(char *s); /* available in libutil_str.so */
char *talloc_asprintf_strupper_m(TALLOC_CTX *t, const char *fmt, ...); /* available in libutil_str.so */
void sid_copy(struct dom_sid *dst, const struct dom_sid *src); /* available in libsecurity.so */
bool sid_linearize(char *outbuf, size_t len, const struct dom_sid *sid); /* available in libsmbconf.so */
bool string_to_sid(struct dom_sid *sidout, const char *sidstr); /* available in libsecurity.so */
bool sid_compose(struct dom_sid *dst, const struct dom_sid *domain_sid, uint32_t rid); /* available in libsecurity.so */
bool sid_peek_rid(const struct dom_sid *sid, uint32_t *rid); /* available in libsecurity.so */
int dom_sid_compare_domain(const struct dom_sid *sid1, const struct dom_sid *sid2); /* available in libsecurity.so */
char *sid_string_talloc(TALLOC_CTX *mem_ctx, const struct dom_sid *sid); /* available in libsmbconf.so */
char *sid_string_dbg(const struct dom_sid *sid); /* available in libsmbconf.so */
bool is_null_sid(const struct dom_sid *sid); /* available in libsecurity.so */
bool strnequal(const char *s1,const char *s2,size_t n); /* available in libutil_str.so */
int smbrunsecret(const char *cmd, const char *secret); /* available in libsmbconf.so */
bool trim_char(char *s,char cfront,char cback); /* available in libutil_str.so */
bool sid_peek_check_rid(const struct dom_sid *exp_dom_sid, const struct dom_sid *sid, uint32_t *rid); /* available in libsecurity.so */
char *escape_ldap_string(TALLOC_CTX *mem_ctx, const char *s); /* available in libsmbconf.so */
extern const struct dom_sid global_sid_Builtin; /* available in libsecurity.so */
bool secrets_store(const char *key, const void *data, size_t size); /* available in libpdb.so */
/* from smb_macros.h */
#define SMB_REALLOC_ARRAY(p,type,count) (type *)realloc_array((p),sizeof(type),(count),true) /* Always frees p on error or s == 0 */
#define ADD_TO_ARRAY(mem_ctx, type, elem, array, num) \
do { \
	*(array) = ((mem_ctx) != NULL) ? \
		talloc_realloc(mem_ctx, (*(array)), type, (*(num))+1) : \
		SMB_REALLOC_ARRAY((*(array)), type, (*(num))+1); \
	SMB_ASSERT((*(array)) != NULL); \
	(*(array))[*(num)] = (elem); \
	(*(num)) += 1; \
} while (0)


#define LDAP_SUFFIX "dc=ipa,dc=devel" /* FIXME !!! */
#define LDAP_PAGE_SIZE 1024
#define LDAP_OBJ_SAMBASAMACCOUNT "ipaNTUserAttrs"
#define LDAP_OBJ_TRUSTED_DOMAIN "ipaNTTrustedDomain"
#define LDAP_ATTRIBUTE_TRUST_SID "ipaNTTrustedDomainSID"
#define LDAP_ATTRIBUTE_SID "ipaNTSecurityIdentifier"
#define LDAP_OBJ_GROUPMAP "ipaNTGroupAttrs"

#define IPA_KEYTAB_SET_OID "2.16.840.1.113730.3.8.10.1"
#define IPA_KEYTAB_SET_OID_OLD "2.16.840.1.113730.3.8.3.1"
#define IPA_MAGIC_ID_STR "999"

#define LDAP_ATTRIBUTE_CN "cn"
#define LDAP_ATTRIBUTE_UID "uid"
#define LDAP_ATTRIBUTE_TRUST_TYPE "ipaNTTrustType"
#define LDAP_ATTRIBUTE_TRUST_ATTRIBUTES "ipaNTTrustAttributes"
#define LDAP_ATTRIBUTE_TRUST_DIRECTION "ipaNTTrustDirection"
#define LDAP_ATTRIBUTE_TRUST_POSIX_OFFSET "ipaNTTrustPosixOffset"
#define LDAP_ATTRIBUTE_SUPPORTED_ENC_TYPE "ipaNTSupportedEncryptionTypes"
#define LDAP_ATTRIBUTE_TRUST_PARTNER "ipaNTTrustPartner"
#define LDAP_ATTRIBUTE_FLAT_NAME "ipaNTFlatName"
#define LDAP_ATTRIBUTE_TRUST_AUTH_OUTGOING "ipaNTTrustAuthOutgoing"
#define LDAP_ATTRIBUTE_TRUST_AUTH_INCOMING "ipaNTTrustAuthIncoming"
#define LDAP_ATTRIBUTE_SECURITY_IDENTIFIER "ipaNTSecurityIdentifier"
#define LDAP_ATTRIBUTE_TRUST_FOREST_TRUST_INFO "ipaNTTrustForestTrustInfo"
#define LDAP_ATTRIBUTE_FALLBACK_PRIMARY_GROUP "ipaNTFallbackPrimaryGroup"
#define LDAP_ATTRIBUTE_OBJECTCLASS "objectClass"
#define LDAP_ATTRIBUTE_HOME_DRIVE "ipaNTHomeDirectoryDrive"
#define LDAP_ATTRIBUTE_HOME_PATH "ipaNTHomeDirectory"
#define LDAP_ATTRIBUTE_LOGON_SCRIPT "ipaNTLogonScript"
#define LDAP_ATTRIBUTE_PROFILE_PATH "ipaNTProfilePath"
#define LDAP_ATTRIBUTE_NTHASH "ipaNTHash"

#define LDAP_OBJ_KRB_PRINCIPAL "krbPrincipal"
#define LDAP_OBJ_KRB_PRINCIPAL_AUX "krbPrincipalAux"
#define LDAP_OBJ_KRB_TICKET_POLICY_AUX "krbTicketPolicyAux"
#define LDAP_ATTRIBUTE_KRB_PRINCIPAL "krbPrincipalName"

#define LDAP_OBJ_IPAOBJECT "ipaObject"
#define LDAP_OBJ_IPAHOST "ipaHost"
#define LDAP_OBJ_POSIXACCOUNT "posixAccount"

#define LDAP_OBJ_GROUPOFNAMES "groupOfNames"
#define LDAP_OBJ_NESTEDGROUP "nestedGroup"
#define LDAP_OBJ_IPAUSERGROUP "ipaUserGroup"
#define LDAP_OBJ_POSIXGROUP "posixGroup"

#define HAS_KRB_PRINCIPAL (1<<0)
#define HAS_KRB_PRINCIPAL_AUX (1<<1)
#define HAS_IPAOBJECT (1<<2)
#define HAS_IPAHOST (1<<3)
#define HAS_POSIXACCOUNT (1<<4)
#define HAS_GROUPOFNAMES (1<<5)
#define HAS_NESTEDGROUP (1<<6)
#define HAS_IPAUSERGROUP (1<<7)
#define HAS_POSIXGROUP (1<<8)
#define HAS_KRB_TICKET_POLICY_AUX (1<<9)

struct ipasam_privates {
	NTSTATUS (*ldapsam_add_sam_account)(struct pdb_methods *,
					    struct samu *sampass);
	NTSTATUS (*ldapsam_update_sam_account)(struct pdb_methods *,
					       struct samu *sampass);
	NTSTATUS (*ldapsam_create_user)(struct pdb_methods *my_methods,
					TALLOC_CTX *tmp_ctx, const char *name,
					uint32_t acb_info, uint32_t *rid);
	NTSTATUS (*ldapsam_create_dom_group)(struct pdb_methods *my_methods,
					     TALLOC_CTX *tmp_ctx,
					     const char *name,
					     uint32_t *rid);
	char *realm;
	char *base_dn;
	char *trust_dn;
	char *flat_name;
	char *fallback_primary_group;
};

static LDAP *priv2ld(struct ldapsam_privates *priv)
{
	return priv->smbldap_state->ldap_struct;
}

static char *get_single_attribute(TALLOC_CTX *mem_ctx, LDAP *ldap_struct,
				  LDAPMessage *entry, const char *attribute)
{
	struct berval **values;
	int c;
	char *result = NULL;
	size_t conv_size;

	if (attribute == NULL || entry == NULL) {
		return NULL;
	}

	values = ldap_get_values_len(ldap_struct, entry, attribute);
	if (values == NULL) {
		DEBUG(10, ("Attribute [%s] not found.\n", attribute));
		return NULL;
	}

	c = ldap_count_values_len(values);
	if (c != 1) {
		DEBUG(10, ("Found [%d] values for attribute [%s] but expected only 1.\n",
			   c, attribute));
		goto done;
	}

	if (!convert_string_talloc(mem_ctx, CH_UTF8, CH_UNIX,
				   values[0]->bv_val, values[0]->bv_len,
				   &result, &conv_size)) {
		DEBUG(10, ("Failed to convert value of [%s].\n", attribute));
		result = NULL;
		goto done;
	}

done:
	ldap_value_free_len(values);
	return result;
}

static char *get_dn(TALLOC_CTX *mem_ctx, LDAP *ld, LDAPMessage *entry)
{
	char *utf8_dn;
	char *unix_dn = NULL;
	size_t conv_size;

	utf8_dn = ldap_get_dn(ld, entry);
	if (utf8_dn == NULL) {
		DEBUG (10, ("ldap_get_dn failed\n"));
		return NULL;
	}
	if (!convert_string_talloc(mem_ctx, CH_UTF8, CH_UNIX,
				   utf8_dn, strlen(utf8_dn) + 1,
				   &unix_dn, &conv_size)) {
		DEBUG (10, ("Failed to convert [%s]\n", utf8_dn));
		unix_dn = NULL;
		goto done;
	}

done:
	ldap_memfree(utf8_dn);
	return unix_dn;
}





static bool ldapsam_extract_rid_from_entry(LDAP *ldap_struct,
					   LDAPMessage *entry,
					   const struct dom_sid *domain_sid,
					   uint32_t *rid)
{
	char *str;
	struct dom_sid sid;

	str = get_single_attribute(NULL, ldap_struct, entry,
				   LDAP_ATTRIBUTE_SID);
	if (str == NULL) {
		DEBUG(10, ("Could not find SID attribute\n"));
		return false;
	}

	if (!string_to_sid(&sid, str)) {
		talloc_free(str);
		DEBUG(10, ("Could not convert string %s to sid\n", str));
		return false;
	}
	talloc_free(str);

	if (dom_sid_compare_domain(&sid, domain_sid) != 0) {
		DEBUG(10, ("SID %s is not in expected domain %s\n",
			   str, sid_string_dbg(domain_sid)));
		return false;
	}

	if (!sid_peek_rid(&sid, rid)) {
		DEBUG(10, ("Could not peek into RID\n"));
		return false;
	}

	return true;
}

static NTSTATUS ldapsam_lookup_rids(struct pdb_methods *methods,
				    const struct dom_sid *domain_sid,
				    int num_rids,
				    uint32_t *rids,
				    const char **names,
				    enum lsa_SidType *attrs)
{
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	LDAPMessage *msg = NULL;
	LDAPMessage *entry;
	char *allsids = NULL;
	int i, rc, num_mapped;
	NTSTATUS result = NT_STATUS_NO_MEMORY;
	TALLOC_CTX *mem_ctx;
	LDAP *ld;
	bool is_builtin;

	mem_ctx = talloc_new(NULL);
	if (mem_ctx == NULL) {
		DEBUG(0, ("talloc_new failed\n"));
		goto done;
	}

	if (!sid_check_is_builtin(domain_sid) &&
	    !sid_check_is_domain(domain_sid)) {
		result = NT_STATUS_INVALID_PARAMETER;
		goto done;
	}

	if (num_rids == 0) {
		result = NT_STATUS_NONE_MAPPED;
		goto done;
	}

	for (i=0; i<num_rids; i++)
		attrs[i] = SID_NAME_UNKNOWN;

	allsids = talloc_strdup(mem_ctx, "");
	if (allsids == NULL) {
		goto done;
	}

	for (i=0; i<num_rids; i++) {
		struct dom_sid sid;
		sid_compose(&sid, domain_sid, rids[i]);
		allsids = talloc_asprintf_append_buffer(
			allsids, "(%s=%s)",
			LDAP_ATTRIBUTE_SID,
			sid_string_talloc(mem_ctx, &sid));
		if (allsids == NULL) {
			goto done;
		}
	}

	/* First look for users */

	{
		char *filter;
		const char *ldap_attrs[] = { "uid", LDAP_ATTRIBUTE_SID, NULL };

		filter = talloc_asprintf(
			mem_ctx, ("(&(objectClass=%s)(|%s))"),
			LDAP_OBJ_SAMBASAMACCOUNT, allsids);

		if (filter == NULL) {
			goto done;
		}

		rc = smbldap_search(ldap_state->smbldap_state,
				    ldap_state->ipasam_privates->base_dn,
				    LDAP_SCOPE_SUBTREE, filter, ldap_attrs, 0,
				    &msg);
		talloc_autofree_ldapmsg(mem_ctx, msg);
	}

	if (rc != LDAP_SUCCESS)
		goto done;

	ld = ldap_state->smbldap_state->ldap_struct;
	num_mapped = 0;

	for (entry = ldap_first_entry(ld, msg);
	     entry != NULL;
	     entry = ldap_next_entry(ld, entry)) {
		uint32_t rid;
		int rid_index;
		const char *name;

		if (!ldapsam_extract_rid_from_entry(ld, entry, domain_sid,
						    &rid)) {
			DEBUG(2, ("Could not find sid from ldap entry\n"));
			continue;
		}

		name = get_single_attribute(names, ld, entry, "uid");
		if (name == NULL) {
			DEBUG(2, ("Could not retrieve uid attribute\n"));
			continue;
		}

		for (rid_index = 0; rid_index < num_rids; rid_index++) {
			if (rid == rids[rid_index])
				break;
		}

		if (rid_index == num_rids) {
			DEBUG(2, ("Got a RID not asked for: %d\n", rid));
			continue;
		}

		attrs[rid_index] = SID_NAME_USER;
		names[rid_index] = name;
		num_mapped += 1;
	}

	if (num_mapped == num_rids) {
		/* No need to look for groups anymore -- we're done */
		result = NT_STATUS_OK;
		goto done;
	}

	/* Same game for groups */

	{
		char *filter;
		const char *ldap_attrs[] = { "cn", "displayName",
					     LDAP_ATTRIBUTE_SID,
					     NULL };

		filter = talloc_asprintf(
			mem_ctx, "(&(objectClass=%s)(|%s))",
			LDAP_OBJ_GROUPMAP, allsids);
		if (filter == NULL) {
			goto done;
		}

		rc = smbldap_search(ldap_state->smbldap_state,
				    ldap_state->ipasam_privates->base_dn,
				    LDAP_SCOPE_SUBTREE, filter, ldap_attrs, 0,
				    &msg);
		talloc_autofree_ldapmsg(mem_ctx, msg);
	}

	if (rc != LDAP_SUCCESS)
		goto done;

	/* ldap_struct might have changed due to a reconnect */

	ld = ldap_state->smbldap_state->ldap_struct;

	/* For consistency checks, we already checked we're only domain or builtin */

	is_builtin = sid_check_is_builtin(domain_sid);

	for (entry = ldap_first_entry(ld, msg);
	     entry != NULL;
	     entry = ldap_next_entry(ld, entry))
	{
		uint32_t rid;
		int rid_index;
		const char *attr;
		enum lsa_SidType type;
		const char *dn = get_dn(mem_ctx, ld, entry);

		type = SID_NAME_DOM_GRP;

		/* Consistency checks */
		if ((is_builtin && (type != SID_NAME_ALIAS)) ||
		    (!is_builtin && ((type != SID_NAME_ALIAS) &&
				     (type != SID_NAME_DOM_GRP)))) {
			DEBUG(2, ("Rejecting invalid group mapping entry %s\n", dn));
		}

		if (!ldapsam_extract_rid_from_entry(ld, entry, domain_sid,
						    &rid)) {
			DEBUG(2, ("Could not find sid from ldap entry %s\n", dn));
			continue;
		}

		attr = get_single_attribute(names, ld, entry, "displayName");

		if (attr == NULL) {
			DEBUG(10, ("Could not retrieve 'displayName' attribute from %s\n",
				   dn));
			attr = get_single_attribute(names, ld, entry, "cn");
		}

		if (attr == NULL) {
			DEBUG(2, ("Could not retrieve naming attribute from %s\n",
				  dn));
			continue;
		}

		for (rid_index = 0; rid_index < num_rids; rid_index++) {
			if (rid == rids[rid_index])
				break;
		}

		if (rid_index == num_rids) {
			DEBUG(2, ("Got a RID not asked for: %d\n", rid));
			continue;
		}

		attrs[rid_index] = type;
		names[rid_index] = attr;
		num_mapped += 1;
	}

	result = NT_STATUS_NONE_MAPPED;

	if (num_mapped > 0)
		result = (num_mapped == num_rids) ?
			NT_STATUS_OK : STATUS_SOME_UNMAPPED;
 done:
	TALLOC_FREE(mem_ctx);
	return result;
}

static bool ldapsam_sid_to_id(struct pdb_methods *methods,
			      const struct dom_sid *sid,
			      uid_t *uid, gid_t *gid,
			      enum lsa_SidType *type)
{
	struct ldapsam_privates *priv =
		(struct ldapsam_privates *)methods->private_data;
	char *filter;
	const char *attrs[] = { "objectClass", "gidNumber", "uidNumber",
				NULL };
	LDAPMessage *result = NULL;
	LDAPMessage *entry = NULL;
	bool ret = false;
	char *value;
	struct berval **values;
	size_t c;
	int rc;

	TALLOC_CTX *mem_ctx;

	mem_ctx = talloc_new(NULL);
	if (mem_ctx == NULL) {
		DEBUG(0, ("talloc_new failed\n"));
		return false;
	}

	filter = talloc_asprintf(mem_ctx,
				 "(&(%s=%s)"
				 "(|(objectClass=%s)(objectClass=%s)))",
				 LDAP_ATTRIBUTE_SID, sid_string_talloc(mem_ctx, sid),
				 LDAP_OBJ_GROUPMAP, LDAP_OBJ_SAMBASAMACCOUNT);
	if (filter == NULL) {
		DEBUG(5, ("talloc_asprintf failed\n"));
		goto done;
	}

	rc = smbldap_search_suffix(priv->smbldap_state, filter,
				   attrs, &result);
	if (rc != LDAP_SUCCESS) {
		goto done;
	}
	talloc_autofree_ldapmsg(mem_ctx, result);

	if (ldap_count_entries(priv2ld(priv), result) != 1) {
		DEBUG(10, ("Got %d entries, expected one\n",
			   ldap_count_entries(priv2ld(priv), result)));
		goto done;
	}

	entry = ldap_first_entry(priv2ld(priv), result);

	values = ldap_get_values_len(priv2ld(priv), entry, "objectClass");
	if (values == NULL) {
		DEBUG(10, ("Cannot find any objectclasses.\n"));
		goto done;
	}

	for (c = 0; values[c] != NULL; c++) {
		if (strncmp(LDAP_OBJ_GROUPMAP, values[c]->bv_val,
			                       values[c]->bv_len) == 0) {
			break;
		}
	}

	if (values[c] != NULL) {
		const char *gid_str;
		/* It's a group */

		gid_str = get_single_attribute(mem_ctx, priv2ld(priv), entry,
					       "gidNumber");
		if (gid_str == NULL) {
			DEBUG(1, ("%s has no gidNumber\n",
				  get_dn(mem_ctx, priv2ld(priv), entry)));
			goto done;
		}

		*gid = strtoul(gid_str, NULL, 10);
		*type = SID_NAME_DOM_GRP;
		ret = true;
		goto done;
	}

	/* It must be a user */

	value = get_single_attribute(mem_ctx, priv2ld(priv), entry,
				     "uidNumber");
	if (value == NULL) {
		DEBUG(1, ("Could not find uidNumber in %s\n",
			  get_dn(mem_ctx, priv2ld(priv), entry)));
		goto done;
	}

	*uid = strtoul(value, NULL, 10);
	*type = SID_NAME_USER;

	ret = true;
 done:
	TALLOC_FREE(mem_ctx);
	return ret;
}

static bool ldapsam_uid_to_sid(struct pdb_methods *methods, uid_t uid,
			       struct dom_sid *sid)
{
	struct ldapsam_privates *priv =
		(struct ldapsam_privates *)methods->private_data;
	char *filter;
	const char *attrs[] = { LDAP_ATTRIBUTE_SID, NULL };
	LDAPMessage *result = NULL;
	LDAPMessage *entry = NULL;
	bool ret = false;
	char *user_sid_string;
	struct dom_sid user_sid;
	int rc;
	TALLOC_CTX *tmp_ctx = talloc_stackframe();

	filter = talloc_asprintf(tmp_ctx,
				 "(&(uidNumber=%u)"
				 "(objectClass=%s)"
				 "(objectClass=%s))",
				 (unsigned int)uid,
				 LDAP_OBJ_POSIXACCOUNT,
				 LDAP_OBJ_SAMBASAMACCOUNT);
	if (filter == NULL) {
		DEBUG(3, ("talloc_asprintf failed\n"));
		goto done;
	}

	rc = smbldap_search_suffix(priv->smbldap_state, filter, attrs, &result);
	if (rc != LDAP_SUCCESS) {
		goto done;
	}
	talloc_autofree_ldapmsg(tmp_ctx, result);

	if (ldap_count_entries(priv2ld(priv), result) != 1) {
		DEBUG(3, ("ERROR: Got %d entries for uid %u, expected one\n",
			   ldap_count_entries(priv2ld(priv), result),
			   (unsigned int)uid));
		goto done;
	}

	entry = ldap_first_entry(priv2ld(priv), result);

	user_sid_string = get_single_attribute(tmp_ctx, priv2ld(priv), entry,
					       LDAP_ATTRIBUTE_SID);
	if (user_sid_string == NULL) {
		DEBUG(1, ("Could not find SID in object '%s'\n",
			  get_dn(tmp_ctx, priv2ld(priv), entry)));
		goto done;
	}

	if (!string_to_sid(&user_sid, user_sid_string)) {
		DEBUG(3, ("Error calling sid_string_talloc for sid '%s'\n",
			  user_sid_string));
		goto done;
	}

	sid_copy(sid, &user_sid);

	ret = true;

 done:
	TALLOC_FREE(tmp_ctx);
	return ret;
}

static bool ldapsam_gid_to_sid(struct pdb_methods *methods, gid_t gid,
			       struct dom_sid *sid)
{
	struct ldapsam_privates *priv =
		(struct ldapsam_privates *)methods->private_data;
	char *filter;
	const char *attrs[] = { LDAP_ATTRIBUTE_SID, NULL };
	LDAPMessage *result = NULL;
	LDAPMessage *entry = NULL;
	bool ret = false;
	char *group_sid_string;
	struct dom_sid group_sid;
	int rc;
	TALLOC_CTX *tmp_ctx = talloc_stackframe();

	filter = talloc_asprintf(tmp_ctx,
				 "(&(gidNumber=%u)"
				 "(objectClass=%s))",
				 (unsigned int)gid,
				 LDAP_OBJ_GROUPMAP);
	if (filter == NULL) {
		DEBUG(3, ("talloc_asprintf failed\n"));
		goto done;
	}

	rc = smbldap_search_suffix(priv->smbldap_state, filter, attrs, &result);
	if (rc != LDAP_SUCCESS) {
		goto done;
	}
	talloc_autofree_ldapmsg(tmp_ctx, result);

	if (ldap_count_entries(priv2ld(priv), result) != 1) {
		DEBUG(3, ("ERROR: Got %d entries for gid %u, expected one\n",
			   ldap_count_entries(priv2ld(priv), result),
			   (unsigned int)gid));
		goto done;
	}

	entry = ldap_first_entry(priv2ld(priv), result);

	group_sid_string = get_single_attribute(tmp_ctx, priv2ld(priv), entry,
						LDAP_ATTRIBUTE_SID);
	if (group_sid_string == NULL) {
		DEBUG(1, ("Could not find SID in object '%s'\n",
			  get_dn(tmp_ctx, priv2ld(priv), entry)));
		goto done;
	}

	if (!string_to_sid(&group_sid, group_sid_string)) {
		DEBUG(3, ("Error calling sid_string_talloc for sid '%s'\n",
			  group_sid_string));
		goto done;
	}

	sid_copy(sid, &group_sid);

	ret = true;

 done:
	TALLOC_FREE(tmp_ctx);
	return ret;
}


static char *get_ldap_filter(TALLOC_CTX *mem_ctx, const char *username)
{
	char *escaped = NULL;
	char *result = NULL;

	escaped = escape_ldap_string(mem_ctx, username);
	if (escaped == NULL) {
		return NULL;
	}

	result = talloc_asprintf(mem_ctx, "(&(uid=%s)(objectclass=%s))",
					  escaped, LDAP_OBJ_SAMBASAMACCOUNT);

	TALLOC_FREE(escaped);

	return result;
}

static const char **talloc_attrs(TALLOC_CTX *mem_ctx, ...)
{
	int i, num = 0;
	va_list ap;
	const char **result;

	va_start(ap, mem_ctx);
	while (va_arg(ap, const char *) != NULL)
		num += 1;
	va_end(ap);

	if ((result = talloc_array(mem_ctx, const char *, num+1)) == NULL) {
		return NULL;
	}

	va_start(ap, mem_ctx);
	for (i=0; i<num; i++) {
		result[i] = talloc_strdup(result, va_arg(ap, const char*));
		if (result[i] == NULL) {
			talloc_free(result);
			va_end(ap);
			return NULL;
		}
	}
	va_end(ap);

	result[num] = NULL;
	return result;
}


struct ldap_search_state {
	struct smbldap_state *connection;

	uint32_t acct_flags;
	uint16_t group_type;

	const char *base;
	int scope;
	const char *filter;
	const char **attrs;
	int attrsonly;
	void *pagedresults_cookie;

	LDAPMessage *entries, *current_entry;
	bool (*ldap2displayentry)(struct ldap_search_state *state,
				  TALLOC_CTX *mem_ctx,
				  LDAP *ld, LDAPMessage *entry,
				  struct samr_displayentry *result);
};

static bool ldapsam_search_firstpage(struct pdb_search *search)
{
	struct ldap_search_state *state =
		(struct ldap_search_state *)search->private_data;
	LDAP *ld;
	int rc = LDAP_OPERATIONS_ERROR;

	state->entries = NULL;

	if (state->connection->paged_results) {
		rc = smbldap_search_paged(state->connection, state->base,
					  state->scope, state->filter,
					  state->attrs, state->attrsonly,
					  LDAP_PAGE_SIZE, &state->entries,
					  &state->pagedresults_cookie);
	}

	if ((rc != LDAP_SUCCESS) || (state->entries == NULL)) {

		if (state->entries != NULL) {
			/* Left over from unsuccessful paged attempt */
			ldap_msgfree(state->entries);
			state->entries = NULL;
		}

		rc = smbldap_search(state->connection, state->base,
				    state->scope, state->filter, state->attrs,
				    state->attrsonly, &state->entries);

		if ((rc != LDAP_SUCCESS) || (state->entries == NULL))
			return false;

		/* Ok, the server was lying. It told us it could do paged
		 * searches when it could not. */
		state->connection->paged_results = false;
	}

        ld = state->connection->ldap_struct;
        if ( ld == NULL) {
                DEBUG(5, ("Don't have an LDAP connection right after a "
			  "search\n"));
                return false;
        }
        state->current_entry = ldap_first_entry(ld, state->entries);

	return true;
}

static bool ldapsam_search_nextpage(struct pdb_search *search)
{
	struct ldap_search_state *state =
		(struct ldap_search_state *)search->private_data;
	int rc;

	if (!state->connection->paged_results) {
		/* There is no next page when there are no paged results */
		return false;
	}

	rc = smbldap_search_paged(state->connection, state->base,
				  state->scope, state->filter, state->attrs,
				  state->attrsonly, LDAP_PAGE_SIZE,
				  &state->entries,
				  &state->pagedresults_cookie);

	if ((rc != LDAP_SUCCESS) || (state->entries == NULL))
		return false;

	state->current_entry = ldap_first_entry(state->connection->ldap_struct, state->entries);

	if (state->current_entry == NULL) {
		ldap_msgfree(state->entries);
		state->entries = NULL;
		return false;
	}

	return true;
}

static bool ldapsam_search_next_entry(struct pdb_search *search,
				      struct samr_displayentry *entry)
{
	struct ldap_search_state *state =
		(struct ldap_search_state *)search->private_data;
	bool result;

 retry:
	if ((state->entries == NULL) && (state->pagedresults_cookie == NULL))
		return false;

	if ((state->entries == NULL) &&
	    !ldapsam_search_nextpage(search))
		    return false;

	if (state->current_entry == NULL) {
		return false;
	}

	result = state->ldap2displayentry(state, search,
					  state->connection->ldap_struct,
					  state->current_entry, entry);

	if (!result) {
		char *dn;
		dn = ldap_get_dn(state->connection->ldap_struct,
				 state->current_entry);
		DEBUG(5, ("Skipping entry %s\n", dn != NULL ? dn : "<NULL>"));
		if (dn != NULL) ldap_memfree(dn);
	}

	state->current_entry = ldap_next_entry(state->connection->ldap_struct,
					       state->current_entry);

	if (state->current_entry == NULL) {
		ldap_msgfree(state->entries);
		state->entries = NULL;
	}

	if (!result) goto retry;

	return true;
}

static void ldapsam_search_end(struct pdb_search *search)
{
	struct ldap_search_state *state =
		(struct ldap_search_state *)search->private_data;
	int rc;

	if (state->pagedresults_cookie == NULL)
		return;

	if (state->entries != NULL)
		ldap_msgfree(state->entries);

	state->entries = NULL;
	state->current_entry = NULL;

	if (!state->connection->paged_results)
		return;

	/* Tell the LDAP server we're not interested in the rest anymore. */

	rc = smbldap_search_paged(state->connection, state->base, state->scope,
				  state->filter, state->attrs,
				  state->attrsonly, 0, &state->entries,
				  &state->pagedresults_cookie);

	if (rc != LDAP_SUCCESS)
		DEBUG(5, ("Could not end search properly\n"));

	return;
}

static bool ldapuser2displayentry(struct ldap_search_state *state,
				  TALLOC_CTX *mem_ctx,
				  LDAP *ld, LDAPMessage *entry,
				  struct samr_displayentry *result)
{
	char **vals;
	size_t converted_size;
	struct dom_sid sid;

/* FIXME: SB try to figure out which flags to set instead of hardcode them */
	result->acct_flags = 66048;
	result->account_name = "";
	result->fullname = "";
	result->description = "";

	vals = ldap_get_values(ld, entry, "uid");
	if ((vals == NULL) || (vals[0] == NULL)) {
		DEBUG(5, ("\"uid\" not found\n"));
		return false;
	}
	if (!pull_utf8_talloc(mem_ctx,
			      discard_const_p(char *, &result->account_name),
			      vals[0], &converted_size))
	{
		DEBUG(0,("ldapuser2displayentry: pull_utf8_talloc failed: %s",
			 strerror(errno)));
	}

	ldap_value_free(vals);

	vals = ldap_get_values(ld, entry, "displayName");
	if ((vals == NULL) || (vals[0] == NULL))
		DEBUG(8, ("\"displayName\" not found\n"));
	else if (!pull_utf8_talloc(mem_ctx,
				   discard_const_p(char *, &result->fullname),
				   vals[0], &converted_size))
	{
		DEBUG(0,("ldapuser2displayentry: pull_utf8_talloc failed: %s",
			 strerror(errno)));
	}

	ldap_value_free(vals);

	vals = ldap_get_values(ld, entry, "description");
	if ((vals == NULL) || (vals[0] == NULL))
		DEBUG(8, ("\"description\" not found\n"));
	else if (!pull_utf8_talloc(mem_ctx,
				   discard_const_p(char *, &result->description),
				   vals[0], &converted_size))
	{
		DEBUG(0,("ldapuser2displayentry: pull_utf8_talloc failed: %s",
			 strerror(errno)));
	}

	ldap_value_free(vals);

	if ((result->account_name == NULL) ||
	    (result->fullname == NULL) ||
	    (result->description == NULL)) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}

	vals = ldap_get_values(ld, entry, LDAP_ATTRIBUTE_SID);
	if ((vals == NULL) || (vals[0] == NULL)) {
		DEBUG(0, ("\"objectSid\" not found\n"));
		return false;
	}

	if (!string_to_sid(&sid, vals[0])) {
		DEBUG(0, ("Could not convert %s to SID\n", vals[0]));
		ldap_value_free(vals);
		return false;
	}
	ldap_value_free(vals);

	if (!sid_peek_check_rid(get_global_sam_sid(), &sid, &result->rid)) {
		DEBUG(0, ("sid does not belong to our domain\n"));
		return false;
	}

	return true;
}

static bool ldapsam_search_users(struct pdb_methods *methods,
				 struct pdb_search *search,
				 uint32_t acct_flags)
{
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	struct ldap_search_state *state;

	state = talloc(search, struct ldap_search_state);
	if (state == NULL) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}

	state->connection = ldap_state->smbldap_state;

	if ((acct_flags != 0) && ((acct_flags & ACB_NORMAL) != 0))
		state->base = LDAP_SUFFIX;
	else if ((acct_flags != 0) &&
		 ((acct_flags & (ACB_WSTRUST|ACB_SVRTRUST|ACB_DOMTRUST)) != 0))
		state->base = LDAP_SUFFIX;
	else
		state->base = LDAP_SUFFIX;

	state->acct_flags = acct_flags;
	state->base = talloc_strdup(search, state->base);
	state->scope = LDAP_SCOPE_SUBTREE;
	state->filter = get_ldap_filter(search, "*");
	state->attrs = talloc_attrs(search, "uid", LDAP_ATTRIBUTE_SID,
				    "displayName", "description",
				    NULL);
	state->attrsonly = 0;
	state->pagedresults_cookie = NULL;
	state->entries = NULL;
	state->ldap2displayentry = ldapuser2displayentry;

	if ((state->filter == NULL) || (state->attrs == NULL)) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}

	search->private_data = state;
	search->next_entry = ldapsam_search_next_entry;
	search->search_end = ldapsam_search_end;

	return ldapsam_search_firstpage(search);
}

static bool ldapgroup2displayentry(struct ldap_search_state *state,
				   TALLOC_CTX *mem_ctx,
				   LDAP *ld, LDAPMessage *entry,
				   struct samr_displayentry *result)
{
	char **vals = NULL;
	size_t converted_size;
	struct dom_sid sid;
	uint16_t group_type;

	result->account_name = "";
	result->fullname = "";
	result->description = "";

	group_type = SID_NAME_DOM_GRP;

	if ((state->group_type != 0) &&
	    ((state->group_type != group_type))) {
		ldap_value_free(vals);
		return false;
	}

	ldap_value_free(vals);

	/* display name is the NT group name */

	vals = ldap_get_values(ld, entry, "displayName");
	if ((vals == NULL) || (vals[0] == NULL)) {
		DEBUG(8, ("\"displayName\" not found\n"));

		/* fallback to the 'cn' attribute */
		vals = ldap_get_values(ld, entry, "cn");
		if ((vals == NULL) || (vals[0] == NULL)) {
			DEBUG(5, ("\"cn\" not found\n"));
			return false;
		}
		if (!pull_utf8_talloc(mem_ctx,
				      discard_const_p(char *,
						    &result->account_name),
				      vals[0], &converted_size))
		{
			DEBUG(0,("ldapgroup2displayentry: pull_utf8_talloc "
				  "failed: %s", strerror(errno)));
		}
	}
	else if (!pull_utf8_talloc(mem_ctx,
				   discard_const_p(char *,
						 &result->account_name),
				   vals[0], &converted_size))
	{
		DEBUG(0,("ldapgroup2displayentry: pull_utf8_talloc failed: %s",
			  strerror(errno)));
	}

	ldap_value_free(vals);

	vals = ldap_get_values(ld, entry, "description");
	if ((vals == NULL) || (vals[0] == NULL))
		DEBUG(8, ("\"description\" not found\n"));
	else if (!pull_utf8_talloc(mem_ctx,
				   discard_const_p(char *, &result->description),
				   vals[0], &converted_size))
	{
		DEBUG(0,("ldapgroup2displayentry: pull_utf8_talloc failed: %s",
			  strerror(errno)));
	}
	ldap_value_free(vals);

	if ((result->account_name == NULL) ||
	    (result->fullname == NULL) ||
	    (result->description == NULL)) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}

	vals = ldap_get_values(ld, entry, LDAP_ATTRIBUTE_SID);
	if ((vals == NULL) || (vals[0] == NULL)) {
		DEBUG(0, ("\"objectSid\" not found\n"));
		if (vals != NULL) {
			ldap_value_free(vals);
		}
		return false;
	}

	if (!string_to_sid(&sid, vals[0])) {
		DEBUG(0, ("Could not convert %s to SID\n", vals[0]));
		return false;
	}

	ldap_value_free(vals);

	switch (group_type) {
		case SID_NAME_DOM_GRP:
		case SID_NAME_ALIAS:

			if (!sid_peek_check_rid(get_global_sam_sid(), &sid, &result->rid)
				&& !sid_peek_check_rid(&global_sid_Builtin, &sid, &result->rid))
			{
				DEBUG(0, ("SID is not in our domain\n"));
				return false;
			}
			break;

		default:
			DEBUG(0,("unknown group type: %d\n", group_type));
			return false;
	}

	result->acct_flags = 0;

	return true;
}

static bool ldapsam_search_grouptype(struct pdb_methods *methods,
				     struct pdb_search *search,
                                     const struct dom_sid *sid,
				     enum lsa_SidType type)
{
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	struct ldap_search_state *state;

	state = talloc(search, struct ldap_search_state);
	if (state == NULL) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}

	state->connection = ldap_state->smbldap_state;

	state->base = talloc_strdup(search, LDAP_SUFFIX);
	state->connection = ldap_state->smbldap_state;
	state->scope = LDAP_SCOPE_SUBTREE;
	state->filter =	talloc_asprintf(search, "(&(objectclass=%s)"
					"(%s=%s*))",
					 LDAP_OBJ_GROUPMAP,
					 LDAP_ATTRIBUTE_SID,
					 sid_string_talloc(search, sid));
	state->attrs = talloc_attrs(search, "cn", LDAP_ATTRIBUTE_SID,
				    "displayName", "description",
				     NULL);
	state->attrsonly = 0;
	state->pagedresults_cookie = NULL;
	state->entries = NULL;
	state->group_type = type;
	state->ldap2displayentry = ldapgroup2displayentry;

	if ((state->filter == NULL) || (state->attrs == NULL)) {
		DEBUG(0, ("talloc failed\n"));
		return false;
	}

	search->private_data = state;
	search->next_entry = ldapsam_search_next_entry;
	search->search_end = ldapsam_search_end;

	return ldapsam_search_firstpage(search);
}

static bool ldapsam_search_groups(struct pdb_methods *methods,
				  struct pdb_search *search)
{
	return ldapsam_search_grouptype(methods, search, get_global_sam_sid(), SID_NAME_DOM_GRP);
}

static bool ldapsam_search_aliases(struct pdb_methods *methods,
				   struct pdb_search *search,
				   const struct dom_sid *sid)
{
	return ldapsam_search_groups(methods, search);
}










static char *trusted_domain_dn(TALLOC_CTX *mem_ctx,
			       struct ldapsam_privates *ldap_state,
			       const char *domain)
{
	return talloc_asprintf(mem_ctx, "%s=%s,%s",
			       LDAP_ATTRIBUTE_CN, domain,
			       ldap_state->ipasam_privates->trust_dn);
}

static NTSTATUS ipasam_get_objectclasses(struct ldapsam_privates *ldap_state,
					 const char *dn, LDAPMessage *entry,
					 uint32_t *has_objectclass)
{
	struct berval **bervals;
	size_t c;

	bervals = ldap_get_values_len(priv2ld(ldap_state), entry,
					LDAP_ATTRIBUTE_OBJECTCLASS);
	if (bervals == NULL) {
		DEBUG(0, ("Entry [%s] does not have any objectclasses.\n", dn));
		return NT_STATUS_INTERNAL_DB_CORRUPTION;
	}

	*has_objectclass = 0;
	for (c = 0; bervals[c] != NULL; c++) {
		if (strnequal(bervals[c]->bv_val, LDAP_OBJ_KRB_PRINCIPAL, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_KRB_PRINCIPAL;
		} else if (strnequal(bervals[c]->bv_val,
			   LDAP_OBJ_KRB_PRINCIPAL_AUX, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_KRB_PRINCIPAL_AUX;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_IPAOBJECT, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_IPAOBJECT;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_IPAHOST, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_IPAHOST;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_POSIXACCOUNT, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_POSIXACCOUNT;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_GROUPOFNAMES, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_GROUPOFNAMES;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_NESTEDGROUP, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_NESTEDGROUP;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_IPAUSERGROUP, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_IPAUSERGROUP;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_POSIXGROUP, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_POSIXGROUP;
		} else if (strnequal(bervals[c]->bv_val, LDAP_OBJ_KRB_TICKET_POLICY_AUX, bervals[c]->bv_len)) {
			*has_objectclass |= HAS_KRB_TICKET_POLICY_AUX;
		}
	}
	ldap_value_free_len(bervals);

	return NT_STATUS_OK;
}

static bool search_krb_princ(struct ldapsam_privates *ldap_state,
			     TALLOC_CTX *mem_ctx,
			     const char *princ, const char *base_dn,
			     LDAPMessage **entry)
{
	int rc;
	LDAPMessage *result = NULL;
	uint32_t num_result;
	char *filter;

	filter = talloc_asprintf(mem_ctx, "%s=%s",
				 LDAP_ATTRIBUTE_KRB_PRINCIPAL, princ);
	if (filter == NULL) {
		return false;
	}

	rc = smbldap_search(ldap_state->smbldap_state, base_dn,
			    LDAP_SCOPE_SUBTREE, filter, NULL, 0, &result);

	if (result != NULL) {
		talloc_autofree_ldapmsg(mem_ctx, result);
	}

	if (rc == LDAP_NO_SUCH_OBJECT) {
		*entry = NULL;
		return true;
	}

	if (rc != LDAP_SUCCESS) {
		return false;
	}

	num_result = ldap_count_entries(priv2ld(ldap_state), result);

	if (num_result > 1) {
		DEBUG(1, ("search_krb_princ: more than one object found "
			  "with filter '%s'?!\n", filter));
		return false;
	}

	if (num_result == 0) {
		DEBUG(1, ("get_trusted_domain_int: no object found "
			  "with filter '%s'.\n", filter));
		*entry = NULL;
	} else {
		*entry = ldap_first_entry(priv2ld(ldap_state), result);
	}

	return true;
}

static bool set_krb_princ(struct ldapsam_privates *ldap_state,
			  TALLOC_CTX *mem_ctx,
			  const char *princ, const char *pwd,
			  const char *base_dn)
{
	LDAPMessage *entry = NULL;
	LDAPMod **mods = NULL;
	char *dn = NULL;
	int ret;
	uint32_t has_objectclass = 0;
	NTSTATUS status;
	char *inp;

	if (!search_krb_princ(ldap_state, mem_ctx, princ, base_dn, &entry)) {
		return false;
	}

	if (entry) {
		dn = get_dn(mem_ctx, priv2ld(ldap_state), entry);
		if (!dn) {
			return false;
		}

		status = ipasam_get_objectclasses(ldap_state, dn, entry,
						  &has_objectclass);
		if (!NT_STATUS_IS_OK(status)) {
			return false;
		}
	} else {
		dn = talloc_asprintf(mem_ctx, "%s=%s,%s",
				     LDAP_ATTRIBUTE_KRB_PRINCIPAL, princ,
				     base_dn);
		if (!dn) {
			return false;
		}
	}

	if (!(has_objectclass & HAS_KRB_PRINCIPAL)) {
		smbldap_set_mod(&mods, LDAP_MOD_ADD,
				LDAP_ATTRIBUTE_OBJECTCLASS,
				LDAP_OBJ_KRB_PRINCIPAL);
	}

	if (!(has_objectclass & HAS_KRB_PRINCIPAL_AUX)) {
		smbldap_set_mod(&mods, LDAP_MOD_ADD,
				LDAP_ATTRIBUTE_OBJECTCLASS,
				LDAP_OBJ_KRB_PRINCIPAL_AUX);
	}

	if (!(has_objectclass & HAS_KRB_TICKET_POLICY_AUX)) {
		smbldap_set_mod(&mods, LDAP_MOD_ADD,
				LDAP_ATTRIBUTE_OBJECTCLASS,
				LDAP_OBJ_KRB_TICKET_POLICY_AUX);
	}

	smbldap_make_mod(priv2ld(ldap_state), entry, &mods,
			 LDAP_ATTRIBUTE_KRB_PRINCIPAL, princ);

	if (entry == NULL) {
		ret = smbldap_add(ldap_state->smbldap_state, dn, mods);
	} else {
		ret = smbldap_modify(ldap_state->smbldap_state, dn, mods);
	}
	if (ret != LDAP_SUCCESS) {
		DEBUG(1, ("error writing cross realm principal data!\n"));
		return false;
	}

	/* TODO: Call the appropriate expo if ipasam is part of the FreeIPA
	 * source tree */
	inp = talloc_asprintf(mem_ctx, "change_password -pw %s %s", pwd, princ);
	if (inp == NULL) {
		return false;
	}

	uid_t save_uid = current_user.ut.uid;
	gid_t save_gid = current_user.ut.gid;
	current_user.ut.uid = 0;
	current_user.ut.gid = 0;
	ret = smbrunsecret("kadmin.local", inp);
	current_user.ut.uid = save_uid;
	current_user.ut.gid = save_gid;
	if (ret != 0) {
		DEBUG(1, ("calling kadmin.local failed.\n"));
		return false;
	}

	return true;
}

static bool del_krb_princ(struct ldapsam_privates *ldap_state,
			  TALLOC_CTX *mem_ctx,
			  const char *princ, const char *base_dn)
{
	LDAPMessage *entry = NULL;
	char *dn = NULL;
	int ret;

	if (!search_krb_princ(ldap_state, mem_ctx, princ, base_dn, &entry)) {
		return false;
	}

	if (entry) {
		dn = get_dn(mem_ctx, priv2ld(ldap_state), entry);
		if (!dn) {
			return false;
		}

		ret = smbldap_delete(ldap_state->smbldap_state, dn);
		if (ret != LDAP_SUCCESS) {
			return false;
		}
	}

	return true;
}

enum princ_mod {
	SET_PRINC,
	DEL_PRINC
};

static bool handle_cross_realm_princs(struct ldapsam_privates *ldap_state,
				      const char *domain, const char *pwd,
				      enum princ_mod mod)
{
	char *trusted_dn;
	char *princ_l;
	char *princ_r;
	char *remote_realm;
	bool ok;
	TALLOC_CTX *tmp_ctx;

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return false;
	}

	remote_realm = talloc_strdup_upper(tmp_ctx, domain);
	if (remote_realm == NULL) {
		ok = false;
		goto done;
	}

	trusted_dn = trusted_domain_dn(tmp_ctx, ldap_state, domain);

	princ_l = talloc_asprintf(tmp_ctx, "krbtgt/%s@%s", remote_realm,
			ldap_state->ipasam_privates->realm);
	princ_r = talloc_asprintf(tmp_ctx, "krbtgt/%s@%s",
			ldap_state->ipasam_privates->realm, remote_realm);

	if (trusted_dn == NULL || princ_l == NULL || princ_r == NULL) {
		ok = false;
		goto done;
	}

	switch (mod) {
		case SET_PRINC:
			if (!set_krb_princ(ldap_state, tmp_ctx, princ_l, pwd,
					   trusted_dn) ||
			    !set_krb_princ(ldap_state, tmp_ctx, princ_r, pwd,
					   trusted_dn)) {
				ok = false;
				goto done;
			}
			break;
		case DEL_PRINC:
			if (!del_krb_princ(ldap_state, tmp_ctx, princ_l,
					   trusted_dn) ||
			    !del_krb_princ(ldap_state, tmp_ctx, princ_r,
					   trusted_dn)) {
				ok = false;
				goto done;
			}
			break;
		default:
			DEBUG(1, ("unknown operation.\n"));
			ok = false;
			goto done;
	}

	ok = true;
done:
	talloc_free(tmp_ctx);
	return ok;
}

static bool set_cross_realm_princs(struct ldapsam_privates *ldap_state,
				   const char *domain, const char *pwd)
{
	return handle_cross_realm_princs(ldap_state, domain, pwd, SET_PRINC);
}

static bool del_cross_realm_princs(struct ldapsam_privates *ldap_state,
				   const char *domain)
{
	return handle_cross_realm_princs(ldap_state, domain, NULL, DEL_PRINC);
}

static bool get_trusted_domain_int(struct ldapsam_privates *ldap_state,
				   TALLOC_CTX *mem_ctx,
				   const char *filter, LDAPMessage **entry)
{
	int rc;
	LDAPMessage *result = NULL;
	uint32_t num_result;

	rc = smbldap_search(ldap_state->smbldap_state,
			    ldap_state->ipasam_privates->trust_dn,
			    LDAP_SCOPE_SUBTREE, filter, NULL, 0, &result);

	if (result != NULL) {
		talloc_autofree_ldapmsg(mem_ctx, result);
	}

	if (rc == LDAP_NO_SUCH_OBJECT) {
		*entry = NULL;
		return true;
	}

	if (rc != LDAP_SUCCESS) {
		return false;
	}

	num_result = ldap_count_entries(priv2ld(ldap_state), result);

	if (num_result > 1) {
		DEBUG(1, ("get_trusted_domain_int: more than one "
			  "%s object with filter '%s'?!\n",
			  LDAP_OBJ_TRUSTED_DOMAIN, filter));
		return false;
	}

	if (num_result == 0) {
		DEBUG(1, ("get_trusted_domain_int: no "
			  "%s object with filter '%s'.\n",
			  LDAP_OBJ_TRUSTED_DOMAIN, filter));
		*entry = NULL;
	} else {
		*entry = ldap_first_entry(priv2ld(ldap_state), result);
	}

	return true;
}

static bool get_trusted_domain_by_name_int(struct ldapsam_privates *ldap_state,
					  TALLOC_CTX *mem_ctx,
					  const char *domain,
					  LDAPMessage **entry)
{
	char *filter = NULL;
	bool ok;

	filter = talloc_asprintf(mem_ctx,
				 "(&(objectClass=%s)(|(%s=%s)(%s=%s)(cn=%s)))",
				 LDAP_OBJ_TRUSTED_DOMAIN,
				 LDAP_ATTRIBUTE_FLAT_NAME, domain,
				 LDAP_ATTRIBUTE_TRUST_PARTNER, domain, domain);
	if (filter == NULL) {
		return false;
	}

	ok = get_trusted_domain_int(ldap_state, mem_ctx, filter, entry);
	talloc_free(filter);

	return ok;
}

static bool get_trusted_domain_by_sid_int(struct ldapsam_privates *ldap_state,
					   TALLOC_CTX *mem_ctx,
					   const char *sid, LDAPMessage **entry)
{
	char *filter = NULL;
	bool ok;

	filter = talloc_asprintf(mem_ctx, "(&(objectClass=%s)(%s=%s))",
				 LDAP_OBJ_TRUSTED_DOMAIN,
				 LDAP_ATTRIBUTE_TRUST_SID, sid);
	if (filter == NULL) {
		return false;
	}

	ok = get_trusted_domain_int(ldap_state, mem_ctx, filter, entry);
	talloc_free(filter);

	return ok;
}

static bool get_uint32_t_from_ldap_msg(struct ldapsam_privates *ldap_state,
				       LDAPMessage *entry,
				       const char *attr,
				       uint32_t *val)
{
	char *dummy;
	long int l;
	char *endptr;

	dummy = get_single_attribute(NULL, priv2ld(ldap_state), entry, attr);
	if (dummy == NULL) {
		DEBUG(9, ("Attribute %s not present.\n", attr));
		*val = 0;
		return true;
	}

	l = strtoul(dummy, &endptr, 10);
	TALLOC_FREE(dummy);

	if (l < 0 || l > UINT32_MAX || *endptr != '\0') {
		return false;
	}

	*val = l;

	return true;
}

static bool fill_pdb_trusted_domain(TALLOC_CTX *mem_ctx,
				    struct ldapsam_privates *ldap_state,
				    LDAPMessage *entry,
				    struct pdb_trusted_domain **_td)
{
	char *dummy;
	bool res;
	struct pdb_trusted_domain *td;

	if (entry == NULL) {
		return false;
	}

	td = talloc_zero(mem_ctx, struct pdb_trusted_domain);
	if (td == NULL) {
		return false;
	}

	/* All attributes are MAY */

	dummy = get_single_attribute(NULL, priv2ld(ldap_state), entry,
				     LDAP_ATTRIBUTE_TRUST_SID);
	if (dummy == NULL) {
		DEBUG(9, ("Attribute %s not present.\n",
			  LDAP_ATTRIBUTE_TRUST_SID));
		ZERO_STRUCT(td->security_identifier);
	} else {
		res = string_to_sid(&td->security_identifier, dummy);
		TALLOC_FREE(dummy);
		if (!res) {
			return false;
		}
	}

	if (!smbldap_talloc_single_blob(td, priv2ld(ldap_state), entry,
					LDAP_ATTRIBUTE_TRUST_AUTH_INCOMING,
					&td->trust_auth_incoming)) {
		DEBUG(9, ("Failed to set incoming auth info.\n"));
	}


	if (!smbldap_talloc_single_blob(td, priv2ld(ldap_state), entry,
					LDAP_ATTRIBUTE_TRUST_AUTH_OUTGOING,
					&td->trust_auth_outgoing)) {
		DEBUG(9, ("Failed to set outgoing auth info.\n"));
	}

	td->netbios_name = get_single_attribute(td, priv2ld(ldap_state), entry,
						LDAP_ATTRIBUTE_FLAT_NAME);
	if (td->netbios_name == NULL) {
		DEBUG(9, ("Attribute %s not present.\n",
			  LDAP_ATTRIBUTE_FLAT_NAME));
	}

	td->domain_name = get_single_attribute(td, priv2ld(ldap_state), entry,
					       LDAP_ATTRIBUTE_TRUST_PARTNER);
	if (td->domain_name == NULL) {
		DEBUG(9, ("Attribute %s not present.\n",
			  LDAP_ATTRIBUTE_TRUST_PARTNER));
	}

	res = get_uint32_t_from_ldap_msg(ldap_state, entry,
					 LDAP_ATTRIBUTE_TRUST_DIRECTION,
					 &td->trust_direction);
	if (!res) {
		return false;
	}

	res = get_uint32_t_from_ldap_msg(ldap_state, entry,
					 LDAP_ATTRIBUTE_TRUST_ATTRIBUTES,
					 &td->trust_attributes);
	if (!res) {
		return false;
	}

	res = get_uint32_t_from_ldap_msg(ldap_state, entry,
					 LDAP_ATTRIBUTE_TRUST_TYPE,
					 &td->trust_type);
	if (!res) {
		return false;
	}

	td->trust_posix_offset = talloc_zero(td, uint32_t);
	if (td->trust_posix_offset == NULL) {
		return false;
	}
	res = get_uint32_t_from_ldap_msg(ldap_state, entry,
					 LDAP_ATTRIBUTE_TRUST_POSIX_OFFSET,
					 td->trust_posix_offset);
	if (!res) {
		return false;
	}

	td->supported_enc_type = talloc_zero(td, uint32_t);
	if (td->supported_enc_type == NULL) {
		return false;
	}
	res = get_uint32_t_from_ldap_msg(ldap_state, entry,
					 LDAP_ATTRIBUTE_SUPPORTED_ENC_TYPE,
					 td->supported_enc_type);
	if (!res) {
		return false;
	}
	if (*td->supported_enc_type == 0) {
		*td->supported_enc_type = KERB_ENCTYPE_DES_CBC_CRC |
					  KERB_ENCTYPE_DES_CBC_MD5 |
					  KERB_ENCTYPE_RC4_HMAC_MD5 |
					  KERB_ENCTYPE_AES128_CTS_HMAC_SHA1_96 |
					  KERB_ENCTYPE_AES256_CTS_HMAC_SHA1_96;
	}

	if (!smbldap_talloc_single_blob(td, priv2ld(ldap_state), entry,
					LDAP_ATTRIBUTE_TRUST_FOREST_TRUST_INFO,
					&td->trust_forest_trust_info)) {
		DEBUG(9, ("Failed to set forest trust info.\n"));
	}

	*_td = td;

	return true;
}

static NTSTATUS ipasam_get_trusted_domain(struct pdb_methods *methods,
					  TALLOC_CTX *mem_ctx,
					  const char *domain,
					  struct pdb_trusted_domain **td)
{
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	LDAPMessage *entry = NULL;

	DEBUG(10, ("ipasam_get_trusted_domain called for domain %s\n", domain));

	if (!get_trusted_domain_by_name_int(ldap_state, mem_ctx, domain,
					    &entry)) {
		return NT_STATUS_UNSUCCESSFUL;
	}
	if (entry == NULL) {
		DEBUG(5, ("ipasam_get_trusted_domain: no such trusted domain: "
			  "%s\n", domain));
		return NT_STATUS_NO_SUCH_DOMAIN;
	}

	if (!fill_pdb_trusted_domain(mem_ctx, ldap_state, entry, td)) {
		return NT_STATUS_UNSUCCESSFUL;
	}

	return NT_STATUS_OK;
}

static NTSTATUS ipasam_get_trusted_domain_by_sid(struct pdb_methods *methods,
						 TALLOC_CTX *mem_ctx,
						 struct dom_sid *sid,
						 struct pdb_trusted_domain **td)
{
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	LDAPMessage *entry = NULL;
	char *sid_str;
	bool ok;

	sid_str = sid_string_talloc(mem_ctx, sid);
	if (sid_str == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	DEBUG(10, ("ipasam_get_trusted_domain_by_sid called for sid %s\n",
		   sid_str));

	ok = get_trusted_domain_by_sid_int(ldap_state, mem_ctx, sid_str,
					   &entry);
	talloc_free(sid_str);
	if (!ok) {
		return NT_STATUS_UNSUCCESSFUL;
	}
	if (entry == NULL) {
		DEBUG(5, ("ipasam_get_trusted_domain_by_sid: no trusted domain "
			  "with sid: %s\n", sid_str));
		return NT_STATUS_NO_SUCH_DOMAIN;
	}

	ok = fill_pdb_trusted_domain(mem_ctx, ldap_state, entry, td);
	if (!ok) {
		return NT_STATUS_UNSUCCESSFUL;
	}

	return NT_STATUS_OK;
}

static bool smbldap_make_mod_uint32_t(LDAP *ldap_struct, LDAPMessage *entry,
				      LDAPMod ***mods, const char *attribute,
				      const uint32_t val)
{
	char *dummy;

	dummy = talloc_asprintf(NULL, "%lu", (unsigned long) val);
	if (dummy == NULL) {
		return false;
	}
	smbldap_make_mod(ldap_struct, entry, mods, attribute, dummy);
	TALLOC_FREE(dummy);

	return true;
}

static NTSTATUS get_trust_pwd(TALLOC_CTX *mem_ctx, const DATA_BLOB *auth_blob,
			      char **pwd, NTTIME *last_update)
{
	NTSTATUS status;
	struct trustAuthInOutBlob iopw;
	enum ndr_err_code ndr_err;
	TALLOC_CTX *tmp_ctx;
	char *trustpw;
	size_t converted_size;

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	ndr_err = ndr_pull_struct_blob(auth_blob, tmp_ctx, &iopw,
			(ndr_pull_flags_fn_t)ndr_pull_trustAuthInOutBlob);
	if (!NDR_ERR_CODE_IS_SUCCESS(ndr_err)) {
		status = NT_STATUS_INVALID_PARAMETER;
		goto done;
	}

	if (iopw.count != 0 && iopw.current.count != 0 &&
	    iopw.current.array[0].AuthType == TRUST_AUTH_TYPE_CLEAR) {
		if (pwd != NULL) {
			if (!convert_string_talloc(tmp_ctx, CH_UTF16, CH_UNIX,
				iopw.current.array[0].AuthInfo.clear.password,
				iopw.current.array[0].AuthInfo.clear.size,
				&trustpw, &converted_size)) {

				status = NT_STATUS_NO_MEMORY;
				goto done;
			}

			*pwd = talloc_strndup(mem_ctx, trustpw, converted_size);
			if (*pwd == NULL) {
				status = NT_STATUS_NO_MEMORY;
				goto done;
			}
		}

		if (last_update != NULL) {
			*last_update = iopw.current.array[0].LastUpdateTime;
		}
	} else {
		status = NT_STATUS_INVALID_PARAMETER;
		goto done;
	}

	status = NT_STATUS_OK;

done:
	talloc_free(tmp_ctx);
	return status;
}

static NTSTATUS ipasam_set_trusted_domain(struct pdb_methods *methods,
					  const char* domain,
					  const struct pdb_trusted_domain *td)
{
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	LDAPMessage *entry = NULL;
	LDAPMod **mods;
	bool res;
	char *trusted_dn = NULL;
	int ret;
	NTSTATUS status;
	TALLOC_CTX *tmp_ctx;
	char *trustpw;

	DEBUG(10, ("ipasam_set_trusted_domain called for domain %s\n", domain));

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	res = get_trusted_domain_by_name_int(ldap_state, tmp_ctx, domain,
					     &entry);
	if (!res) {
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	mods = NULL;
	smbldap_make_mod(priv2ld(ldap_state), entry, &mods, "objectClass",
			 LDAP_OBJ_TRUSTED_DOMAIN);

	if (td->netbios_name != NULL) {
		smbldap_make_mod(priv2ld(ldap_state), entry, &mods,
				 LDAP_ATTRIBUTE_FLAT_NAME,
				 td->netbios_name);
	}

	if (td->domain_name != NULL) {
		smbldap_make_mod(priv2ld(ldap_state), entry, &mods,
				 LDAP_ATTRIBUTE_TRUST_PARTNER,
				 td->domain_name);
	}

	if (!is_null_sid(&td->security_identifier)) {
		smbldap_make_mod(priv2ld(ldap_state), entry, &mods,
				 LDAP_ATTRIBUTE_TRUST_SID,
				 sid_string_talloc(tmp_ctx, &td->security_identifier));
	}

	if (td->trust_type != 0) {
		res = smbldap_make_mod_uint32_t(priv2ld(ldap_state), entry,
						&mods, LDAP_ATTRIBUTE_TRUST_TYPE,
						td->trust_type);
		if (!res) {
			status = NT_STATUS_UNSUCCESSFUL;
			goto done;
		}
	}

	if (td->trust_attributes != 0) {
		res = smbldap_make_mod_uint32_t(priv2ld(ldap_state), entry,
						&mods,
						LDAP_ATTRIBUTE_TRUST_ATTRIBUTES,
						td->trust_attributes);
		if (!res) {
			status = NT_STATUS_UNSUCCESSFUL;
			goto done;
		}
	}

	if (td->trust_direction != 0) {
		res = smbldap_make_mod_uint32_t(priv2ld(ldap_state), entry,
						&mods,
						LDAP_ATTRIBUTE_TRUST_DIRECTION,
						td->trust_direction);
		if (!res) {
			status = NT_STATUS_UNSUCCESSFUL;
			goto done;
		}
	}

	if (td->trust_posix_offset != NULL) {
		res = smbldap_make_mod_uint32_t(priv2ld(ldap_state), entry,
						&mods,
						LDAP_ATTRIBUTE_TRUST_POSIX_OFFSET,
						*td->trust_posix_offset);
		if (!res) {
			status = NT_STATUS_UNSUCCESSFUL;
			goto done;
		}
	}

	if (td->supported_enc_type != NULL) {
		res = smbldap_make_mod_uint32_t(priv2ld(ldap_state), entry,
						&mods,
						LDAP_ATTRIBUTE_SUPPORTED_ENC_TYPE,
						*td->supported_enc_type);
		if (!res) {
			status = NT_STATUS_UNSUCCESSFUL;
			goto done;
		}
	}

	if (td->trust_auth_outgoing.data != NULL) {
		smbldap_make_mod_blob(priv2ld(ldap_state), entry, &mods,
				      LDAP_ATTRIBUTE_TRUST_AUTH_OUTGOING,
				      &td->trust_auth_outgoing);
	}

	if (td->trust_auth_incoming.data != NULL) {
		smbldap_make_mod_blob(priv2ld(ldap_state), entry, &mods,
				      LDAP_ATTRIBUTE_TRUST_AUTH_INCOMING,
				      &td->trust_auth_incoming);
	}

	if (td->trust_forest_trust_info.data != NULL) {
		smbldap_make_mod_blob(priv2ld(ldap_state), entry, &mods,
				      LDAP_ATTRIBUTE_TRUST_FOREST_TRUST_INFO,
				      &td->trust_forest_trust_info);
	}

	talloc_autofree_ldapmod(tmp_ctx, mods);

	trusted_dn = trusted_domain_dn(tmp_ctx, ldap_state, domain);
	if (trusted_dn == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto done;
	}
	if (entry == NULL) {
		ret = smbldap_add(ldap_state->smbldap_state, trusted_dn, mods);
	} else {
		ret = smbldap_modify(ldap_state->smbldap_state, trusted_dn, mods);
	}
	if (ret != LDAP_SUCCESS) {
		DEBUG(1, ("error writing trusted domain data!\n"));
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	if (entry == NULL) { /* FIXME: allow password updates here */
		status = get_trust_pwd(tmp_ctx, &td->trust_auth_incoming,
				       &trustpw, NULL);
		if (!NT_STATUS_IS_OK(status)) {
			goto done;
		}
		res = set_cross_realm_princs(ldap_state, td->domain_name,
					     trustpw);
		memset(trustpw, 0, strlen(trustpw));
		if (!res) {
			DEBUG(1, ("error writing cross realm principals!\n"));
			status = NT_STATUS_UNSUCCESSFUL;
			goto done;
		}
	}

	status = NT_STATUS_OK;
done:
	talloc_free(tmp_ctx);
	return status;
}

static NTSTATUS ipasam_del_trusted_domain(struct pdb_methods *methods,
					   const char *domain)
{
	int ret;
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	LDAPMessage *entry = NULL;
	const char *dn;
	const char *domain_name;
	TALLOC_CTX *tmp_ctx;
	NTSTATUS status;

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	if (!get_trusted_domain_by_name_int(ldap_state, tmp_ctx, domain,
					    &entry)) {
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	if (entry == NULL) {
		DEBUG(5, ("ipasam_del_trusted_domain: no such trusted domain: "
			  "%s\n", domain));
		status = NT_STATUS_NO_SUCH_DOMAIN;
		goto done;
	}

	dn = get_dn(tmp_ctx, priv2ld(ldap_state), entry);
	if (dn == NULL) {
		DEBUG(0,("ipasam_del_trusted_domain: Out of memory!\n"));
		status = NT_STATUS_NO_MEMORY;
		goto done;
	}

	domain_name = get_single_attribute(tmp_ctx, priv2ld(ldap_state), entry,
					   LDAP_ATTRIBUTE_TRUST_PARTNER);
	if (domain_name == NULL) {
		DEBUG(1, ("Attribute %s not present.\n",
			  LDAP_ATTRIBUTE_TRUST_PARTNER));
		status = NT_STATUS_INVALID_PARAMETER;
		goto done;
	}

	if (!del_cross_realm_princs(ldap_state, domain_name)) {
		DEBUG(1, ("error deleting cross realm principals!\n"));
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	ret = smbldap_delete(ldap_state->smbldap_state, dn);
	if (ret != LDAP_SUCCESS) {
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	status = NT_STATUS_OK;
done:
	talloc_free(tmp_ctx);
	return status;
}

static NTSTATUS ipasam_enum_trusted_domains(struct pdb_methods *methods,
					    TALLOC_CTX *mem_ctx,
					    uint32_t *num_domains,
					    struct pdb_trusted_domain ***domains)
{
	int rc;
	struct ldapsam_privates *ldap_state =
		(struct ldapsam_privates *)methods->private_data;
	char *filter = NULL;
	int scope = LDAP_SCOPE_SUBTREE;
	LDAPMessage *result = NULL;
	LDAPMessage *entry = NULL;

	filter = talloc_asprintf(mem_ctx, "(objectClass=%s)",
				 LDAP_OBJ_TRUSTED_DOMAIN);
	if (filter == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	rc = smbldap_search(ldap_state->smbldap_state,
			    ldap_state->ipasam_privates->trust_dn,
			    scope, filter, NULL, 0, &result);
	TALLOC_FREE(filter);

	if (result != NULL) {
		talloc_autofree_ldapmsg(mem_ctx, result);
	}

	if (rc == LDAP_NO_SUCH_OBJECT) {
		*num_domains = 0;
		*domains = NULL;
		return NT_STATUS_OK;
	}

	if (rc != LDAP_SUCCESS) {
		return NT_STATUS_UNSUCCESSFUL;
	}

	*num_domains = 0;
	if (!(*domains = talloc_array(mem_ctx, struct pdb_trusted_domain *, 1))) {
		DEBUG(1, ("talloc failed\n"));
		return NT_STATUS_NO_MEMORY;
	}

	for (entry = ldap_first_entry(priv2ld(ldap_state), result);
	     entry != NULL;
	     entry = ldap_next_entry(priv2ld(ldap_state), entry))
	{
		struct pdb_trusted_domain *dom_info;

		if (!fill_pdb_trusted_domain(*domains, ldap_state, entry,
					     &dom_info)) {
			return NT_STATUS_UNSUCCESSFUL;
		}

		ADD_TO_ARRAY(*domains, struct pdb_trusted_domain *, dom_info,
			     domains, num_domains);

		if (*domains == NULL) {
			DEBUG(1, ("talloc failed\n"));
			return NT_STATUS_NO_MEMORY;
		}
	}

	DEBUG(5, ("ipasam_enum_trusted_domains: got %d domains\n", *num_domains));
	return NT_STATUS_OK;
}

static NTSTATUS ipasam_enum_trusteddoms(struct pdb_methods *methods,
					 TALLOC_CTX *mem_ctx,
					 uint32_t *num_domains,
					 struct trustdom_info ***domains)
{
	NTSTATUS status;
	struct pdb_trusted_domain **td;
	int i;

	status = ipasam_enum_trusted_domains(methods, mem_ctx,
					     num_domains, &td);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	if (*num_domains == 0) {
		return NT_STATUS_OK;
	}

	if (!(*domains = talloc_array(mem_ctx, struct trustdom_info *,
				      *num_domains))) {
		DEBUG(1, ("talloc failed\n"));
		goto fail;
	}

	for (i = 0; i < *num_domains; i++) {
		struct trustdom_info *dom_info;

		dom_info = talloc(*domains, struct trustdom_info);
		if (dom_info == NULL) {
			DEBUG(1, ("talloc failed\n"));
			goto fail;
		}

		dom_info->name = talloc_steal(mem_ctx, td[i]->netbios_name);
		sid_copy(&dom_info->sid, &td[i]->security_identifier);

		(*domains)[i] = dom_info;
	}

	return NT_STATUS_OK;

fail:
	talloc_free(td);
	talloc_free(*domains);

	return NT_STATUS_NO_MEMORY;
}

static uint32_t pdb_ipasam_capabilities(struct pdb_methods *methods)
{
	return PDB_CAP_STORE_RIDS | PDB_CAP_ADS | PDB_CAP_TRUSTED_DOMAINS_EX;
}

static bool init_sam_from_td(struct samu *user, struct pdb_trusted_domain *td,
			     struct ldapsam_privates *ldap_state)
{
	NTSTATUS status;
	struct dom_sid u_sid;
	char *name;
	uint8_t smblmpwd[LM_HASH_LEN];
	uint8_t smbntpwd[NT_HASH_LEN];
	char *trustpw;

	if (!pdb_set_acct_ctrl(user, ACB_DOMTRUST | ACB_TRUSTED_FOR_DELEGATION,
			      PDB_SET)) {
		return false;
	}

	if (!pdb_set_domain(user, ldap_state->domain_name, PDB_DEFAULT)) {
		return false;
	}

	name = talloc_asprintf(user, "%s$", td->netbios_name);
	if (name == NULL) {
		return false;
	}

	if (!pdb_set_username(user, name, PDB_SET)) {
		return false;
	}

	if (!pdb_set_nt_username(user, name, PDB_SET)) {
		return false;
	}

	/* FIXME: create a proper SID here */
	if (!sid_compose(&u_sid, &ldap_state->domain_sid, 6789)) {
		return false;
	}

	if (!pdb_set_user_sid(user, &u_sid, PDB_SET)) {
		return false;
	}

	status = get_trust_pwd(user, &td->trust_auth_incoming, &trustpw, NULL);
	if (!NT_STATUS_IS_OK(status)) {
		return false;
	}
	nt_lm_owf_gen(trustpw, smbntpwd, smblmpwd);
	memset(trustpw, 0, strlen(trustpw));
	talloc_free(trustpw);
	if (!pdb_set_lanman_passwd(user, smblmpwd, PDB_SET)) {
		return false;
	}
	if (!pdb_set_nt_passwd(user, smbntpwd, PDB_SET)) {
		return false;
	}

	return true;
}

static bool init_sam_from_ldap(struct ldapsam_privates *ldap_state,
				struct samu * sampass,
				LDAPMessage * entry)
{
	char *username = NULL;
	char *domain = NULL;
	char *nt_username = NULL;
	char *fullname = NULL;
	char *homedir = NULL;
	char *dir_drive = NULL;
	char *logon_script = NULL;
	char *profile_path = NULL;
	char *temp = NULL;
	bool ret = false;
	DATA_BLOB nthash;

	TALLOC_CTX *tmp_ctx = talloc_init("init_sam_from_ldap");
	if (!tmp_ctx) {
		return false;
	}
	if (sampass == NULL || ldap_state == NULL || entry == NULL) {
		DEBUG(0, ("init_sam_from_ldap: NULL parameters found!\n"));
		goto fn_exit;
	}

	if (priv2ld(ldap_state) == NULL) {
		DEBUG(0, ("init_sam_from_ldap: ldap_state->smbldap_state->"
			  "ldap_struct is NULL!\n"));
		goto fn_exit;
	}

	if (!(username = smbldap_talloc_first_attribute(priv2ld(ldap_state),
					entry, LDAP_ATTRIBUTE_UID, tmp_ctx))) {
		DEBUG(1, ("init_sam_from_ldap: No uid attribute found for "
			  "this user!\n"));
		goto fn_exit;
	}

	DEBUG(2, ("init_sam_from_ldap: Entry found for user: %s\n", username));

	nt_username = talloc_strdup(tmp_ctx, username);
	if (!nt_username) {
		goto fn_exit;
	}

	domain = talloc_strdup(tmp_ctx, ldap_state->domain_name);
	if (!domain) {
		goto fn_exit;
	}

	pdb_set_username(sampass, username, PDB_SET);

	pdb_set_domain(sampass, domain, PDB_DEFAULT);
	pdb_set_nt_username(sampass, nt_username, PDB_SET);

	if ((temp = smbldap_talloc_single_attribute(
			ldap_state->smbldap_state->ldap_struct,
			entry, LDAP_ATTRIBUTE_SECURITY_IDENTIFIER,
			tmp_ctx)) != NULL) {
		pdb_set_user_sid_from_string(sampass, temp, PDB_SET);
	} else {
		goto fn_exit;
	}

	fullname = smbldap_talloc_single_attribute(
			ldap_state->smbldap_state->ldap_struct,
			entry,
			LDAP_ATTRIBUTE_CN,
			tmp_ctx);
	if (fullname) {
		pdb_set_fullname(sampass, fullname, PDB_SET);
	}

	dir_drive = smbldap_talloc_single_attribute(
			ldap_state->smbldap_state->ldap_struct,
			entry, LDAP_ATTRIBUTE_HOME_DRIVE, tmp_ctx);
	if (dir_drive) {
		pdb_set_dir_drive(sampass, dir_drive, PDB_SET);
	}

	homedir = smbldap_talloc_single_attribute(
			ldap_state->smbldap_state->ldap_struct,
			entry, LDAP_ATTRIBUTE_HOME_PATH, tmp_ctx);
	if (homedir) {
		pdb_set_homedir(sampass, homedir, PDB_SET);
	}

	logon_script = smbldap_talloc_single_attribute(
			ldap_state->smbldap_state->ldap_struct,
			entry, LDAP_ATTRIBUTE_LOGON_SCRIPT, tmp_ctx);
	if (logon_script) {
		pdb_set_logon_script(sampass, logon_script, PDB_SET);
	}

	profile_path = smbldap_talloc_single_attribute(
			ldap_state->smbldap_state->ldap_struct,
			entry, LDAP_ATTRIBUTE_PROFILE_PATH, tmp_ctx);
	if (profile_path) {
		pdb_set_profile_path(sampass, profile_path, PDB_SET);
	}


	pdb_set_acct_ctrl(sampass, ACB_NORMAL, PDB_SET);

	if (!smbldap_talloc_single_blob(tmp_ctx,
					ldap_state->smbldap_state->ldap_struct,
					entry, LDAP_ATTRIBUTE_NTHASH,
					&nthash)) {
		DEBUG(5, ("Failed to read NT hash form LDAP response.\n"));
	}
	if (nthash.length != NT_HASH_LEN && nthash.length != 0) {
		DEBUG(5, ("NT hash from LDAP has the wrong size.\n"));
	} else {
		if (!pdb_set_nt_passwd(sampass, nthash.data, PDB_SET)) {
			DEBUG(5, ("Failed to set NT hash.\n"));
		}
	}
/* FIXME: */
	if (!pdb_set_pass_last_set_time(sampass, (time_t) 1, PDB_SET)) {
		DEBUG(5, ("Failed to set last time set.\n"));
	}

	ret = true;

fn_exit:

	talloc_free(tmp_ctx);
	return ret;
}

static NTSTATUS getsam_interdom_trust_account(struct pdb_methods *methods,
					      struct samu *user,
					      const char *sname)
{
	char *dom_name;
	struct ldapsam_privates *ldap_state =
			(struct ldapsam_privates *) methods->private_data;
	int slen;
	TALLOC_CTX *tmp_ctx;
	struct pdb_trusted_domain *td;
	NTSTATUS status;

	slen = strlen(sname);
	if (sname[slen - 1] != '.') {
		DEBUG(5, ("Requested account [%s] is not a inter domain "
			  "trust account.\n", sname));
		return NT_STATUS_NO_SUCH_USER;
	}

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	dom_name = talloc_strdup(tmp_ctx, sname);
	if (dom_name == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto done;
	}
	dom_name[slen - 1] = '\0';

	status = ipasam_get_trusted_domain(methods, tmp_ctx, dom_name, &td);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(5, ("ipasam_get_trusted_domain failed.\n"));
		goto done;
	}

	if (!init_sam_from_td(user, td, ldap_state)) {
		DEBUG(5, ("init_sam_from_td failed.\n"));
		status = NT_STATUS_NO_SUCH_USER;
		goto done;
	}

	status = NT_STATUS_OK;

done:
	talloc_free(tmp_ctx);
	return status;
}
static NTSTATUS ldapsam_getsampwnam(struct pdb_methods *methods,
				    struct samu *user,
				    const char *sname)
{
	struct ldapsam_privates *ldap_state =
			(struct ldapsam_privates *) methods->private_data;
	int slen;
	TALLOC_CTX *tmp_ctx;
	NTSTATUS status;
	char *filter;
	char *escaped_user;
	LDAPMessage *result = NULL;
	LDAPMessage *entry = NULL;
	int ret;
	int count;

	slen = strlen(sname);
	if (sname[slen - 1] == '.') {
		return getsam_interdom_trust_account(methods, user, sname);
	}

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	escaped_user = escape_ldap_string(tmp_ctx, sname);
	if (escaped_user == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto done;
	}

	filter = talloc_asprintf(tmp_ctx, "(&(%s=%s)(%s=%s))",
					  LDAP_ATTRIBUTE_OBJECTCLASS,
					  LDAP_OBJ_SAMBASAMACCOUNT,
					  LDAP_ATTRIBUTE_UID, escaped_user);
	if (filter == NULL) {
		status = NT_STATUS_NO_MEMORY;
		goto done;
	}

	ret = smbldap_search(ldap_state->smbldap_state,
			     ldap_state->ipasam_privates->base_dn,
			     LDAP_SCOPE_SUBTREE,filter, NULL, 0,
			     &result);
	if (ret != LDAP_SUCCESS) {
		status = NT_STATUS_NO_SUCH_USER;
		goto done;
	}

	count = ldap_count_entries(ldap_state->smbldap_state->ldap_struct,
				   result);
	if (count != 1) {
		status = NT_STATUS_NO_SUCH_USER;
		goto done;
	}

	entry = ldap_first_entry(ldap_state->smbldap_state->ldap_struct, result);
	if (entry == NULL) {
		status = NT_STATUS_NO_SUCH_USER;
		goto done;
	}

	if (!init_sam_from_ldap(ldap_state, user, entry)) {
		status = NT_STATUS_NO_SUCH_USER;
		goto done;
	}

	status = NT_STATUS_OK;

done:
	ldap_msgfree(result);
	talloc_free(tmp_ctx);
	return status;
}

static bool ipasam_get_trusteddom_pw(struct pdb_methods *methods,
				      const char *domain,
				      char** pwd,
				      struct dom_sid *sid,
				      time_t *pass_last_set_time)
{
	NTSTATUS status;
	TALLOC_CTX *tmp_ctx;
	struct pdb_trusted_domain *td;
	bool ret = false;
	char *trustpw;
	NTTIME last_update;

	tmp_ctx = talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return false;
	}

	status = ipasam_get_trusted_domain(methods, tmp_ctx, domain, &td);
	if (!NT_STATUS_IS_OK(status)) {
		ret = false;
		goto done;
	}

	/* trusteddom_pw routines do not use talloc yet... */
	if (pwd != NULL) {
		status = get_trust_pwd(tmp_ctx, &td->trust_auth_incoming,
				       &trustpw, &last_update);
		if (!NT_STATUS_IS_OK(status)) {
			ret = false;
			goto done;
		}
		*pwd = strdup(trustpw);
		memset(trustpw, 0, strlen(trustpw));
		talloc_free(trustpw);
		if (*pwd == NULL) {
			ret =false;
			goto done;
		}
	}

	if (pass_last_set_time != NULL) {
		*pass_last_set_time = nt_time_to_unix(last_update);
	}

	if (sid != NULL) {
		sid_copy(sid, &td->security_identifier);
	}

	ret = true;
done:
	talloc_free(tmp_ctx);
	return ret;
}

static bool ipasam_set_trusteddom_pw(struct pdb_methods *methods,
				      const char* domain,
				      const char* pwd,
				      const struct dom_sid *sid)
{
	return false;
}

static bool ipasam_del_trusteddom_pw(struct pdb_methods *methods,
				      const char *domain)
{
	return false;
}

static struct pdb_domain_info *pdb_ipasam_get_domain_info(struct pdb_methods *pdb_methods,
							  TALLOC_CTX *mem_ctx)
{
	struct pdb_domain_info *info;
	struct ldapsam_privates *ldap_state =
			(struct ldapsam_privates *)pdb_methods->private_data;
	char sid_buf[24];
	DATA_BLOB sid_blob;
	NTSTATUS status;

	info = talloc(mem_ctx, struct pdb_domain_info);
	if (info == NULL) {
		DEBUG(1, ("talloc failed\n"));
		return NULL;
	}

	info->name = talloc_strdup(info, ldap_state->ipasam_privates->flat_name);
	if (info->name == NULL) {
		DEBUG(1, ("talloc_strdup domain_name failed\n"));
		goto fail;
	}

	/* TODO: read dns_domain, dns_forest and guid from LDAP */
	info->dns_domain = talloc_strdup(info, ldap_state->ipasam_privates->realm);
	if (info->dns_domain == NULL) {
		goto fail;
	}
	strlower_m(info->dns_domain);
	info->dns_forest = talloc_strdup(info, info->dns_domain);

	/* we expect a domain SID to have 4 sub IDs */
	if (ldap_state->domain_sid.num_auths != 4) {
		goto fail;
	}

	sid_copy(&info->sid, &ldap_state->domain_sid);

	if (!sid_linearize(sid_buf, sizeof(sid_buf), &info->sid)) {
		goto fail;
	}

	/* the first 8 bytes of the linearized SID are not random,
	 * so we skip them */
	sid_blob.data = (uint8_t *) sid_buf + 8 ;
	sid_blob.length = 16;

	status = GUID_from_ndr_blob(&sid_blob, &info->guid);
	if (!NT_STATUS_IS_OK(status)) {
		goto fail;
	}

	return info;

fail:
	TALLOC_FREE(info);
	return NULL;
}

static void ipasam_free_private_data(void **vp)
{
	struct ldapsam_privates **ldap_state = (struct ldapsam_privates **)vp;

	smbldap_free_struct(&(*ldap_state)->smbldap_state);

	if ((*ldap_state)->result != NULL) {
		ldap_msgfree((*ldap_state)->result);
		(*ldap_state)->result = NULL;
	}
	if ((*ldap_state)->domain_dn != NULL) {
		SAFE_FREE((*ldap_state)->domain_dn);
	}

	*ldap_state = NULL;

	/* No need to free any further, as it is talloc()ed */
}

static NTSTATUS ipasam_search_domain_info(struct smbldap_state *ldap_state,
					    LDAPMessage ** result)
{
	const char *filter = "objectClass=ipaNTDomainAttrs";
	const char *attr_list[] = {
					LDAP_ATTRIBUTE_FLAT_NAME,
					LDAP_ATTRIBUTE_SID,
					LDAP_ATTRIBUTE_FALLBACK_PRIMARY_GROUP,
					LDAP_ATTRIBUTE_OBJECTCLASS,
					NULL};
	int count;
	int ret;

	ret = smbldap_search_suffix(ldap_state, filter, attr_list , result);

	if (ret != LDAP_SUCCESS) {
		DEBUG(2,("ipasam_search_domain_info: "
			 "smbldap_search_suffix failed: %s\n",
			 ldap_err2string (ret)));
		DEBUG(2,("ipasam_search_domain_info: Query was: %s\n", filter));
		return NT_STATUS_UNSUCCESSFUL;
	}

	count = ldap_count_entries(ldap_state->ldap_struct, *result);

	if (count == 1) {
		return NT_STATUS_OK;
	}

	DEBUG(0, ("iapsam_search_domain_info: Got [%d] domain info entries, "
		  "but expected only 1.\n", count));

	return NT_STATUS_UNSUCCESSFUL;
}

static NTSTATUS ipasam_get_base_dn(struct smbldap_state *smbldap_state,
				   TALLOC_CTX *mem_ctx, char **base_dn)
{
	int ret;
	LDAPMessage *result;
	LDAPMessage *entry = NULL;
	int count;
	char *nc;
	const char *attr_list[] = {
					"namingContexts",
					"defaultNamingContext",
					NULL
				  };

	ret = smbldap_search(smbldap_state, "", LDAP_SCOPE_BASE,
			     "(objectclass=*)", attr_list, 0, &result);
	if (ret != LDAP_SUCCESS) {
		DEBUG(1, ("Failed to get base DN from RootDSE: %s\n",
			  ldap_err2string (ret)));
		return NT_STATUS_UNSUCCESSFUL;
	}

	count = ldap_count_entries(smbldap_state->ldap_struct, result);

	if (count != 1) {
		DEBUG(1, ("Unexpected number of results [%d] for base DN "
			  "search.\n", count));
		ldap_msgfree(result);
		return NT_STATUS_OK;
	}

	entry = ldap_first_entry(smbldap_state->ldap_struct,
				 result);
	if (entry == NULL) {
		DEBUG(0, ("Could not get RootDSE entry\n"));
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	nc = get_single_attribute(mem_ctx, smbldap_state->ldap_struct, entry,
				  "defaultNamingContext");
	if (nc != NULL) {
		*base_dn = nc;
		ldap_msgfree(result);
		return NT_STATUS_OK;
	}

	nc = get_single_attribute(mem_ctx, smbldap_state->ldap_struct, entry,
				  "namingContexts");
	if (nc != NULL) {
		*base_dn = nc;
		ldap_msgfree(result);
		return NT_STATUS_OK;
	}

	ldap_msgfree(result);
	return NT_STATUS_UNSUCCESSFUL;
}

static NTSTATUS ipasam_get_domain_name(struct ldapsam_privates *ldap_state,
				       TALLOC_CTX *mem_ctx,
				       const char **domain_name)
{
	int ret;
	LDAPMessage *result;
	LDAPMessage *entry = NULL;
	int count;
	char *cn;
	struct smbldap_state *smbldap_state = ldap_state->smbldap_state;
	const char *attr_list[] = {
					"associatedDomain",
					NULL
				  };

	ret = smbldap_search(smbldap_state,
			     ldap_state->ipasam_privates->base_dn,
			     LDAP_SCOPE_SUBTREE,
			     "objectclass=domainRelatedObject", attr_list, 0,
			     &result);
	if (ret != LDAP_SUCCESS) {
		DEBUG(1, ("Failed to get domain name: %s\n",
			  ldap_err2string (ret)));
		return NT_STATUS_UNSUCCESSFUL;
	}

	count = ldap_count_entries(smbldap_state->ldap_struct, result);

	if (count != 1) {
		DEBUG(1, ("Unexpected number of results [%d] for domain name "
			  "search.\n", count));
		ldap_msgfree(result);
		return NT_STATUS_OK;
	}

	entry = ldap_first_entry(smbldap_state->ldap_struct,
				 result);
	if (entry == NULL) {
		DEBUG(0, ("Could not get domainRelatedObject entry\n"));
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	cn = get_single_attribute(mem_ctx, smbldap_state->ldap_struct, entry,
				  "associatedDomain");
	if (cn == NULL) {
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	*domain_name = cn;
	ldap_msgfree(result);
	return NT_STATUS_OK;
}

static NTSTATUS ipasam_get_realm(struct ldapsam_privates *ldap_state,
				 TALLOC_CTX *mem_ctx,
				 char **realm)
{
	int ret;
	LDAPMessage *result;
	LDAPMessage *entry = NULL;
	int count;
	char *cn;
	struct smbldap_state *smbldap_state = ldap_state->smbldap_state;
	const char *attr_list[] = {
					"cn",
					NULL
				  };

	ret = smbldap_search(smbldap_state,
			     ldap_state->ipasam_privates->base_dn,
			     LDAP_SCOPE_SUBTREE,
			     "objectclass=krbrealmcontainer", attr_list, 0,
			     &result);
	if (ret != LDAP_SUCCESS) {
		DEBUG(1, ("Failed to get realm: %s\n",
			  ldap_err2string (ret)));
		return NT_STATUS_UNSUCCESSFUL;
	}

	count = ldap_count_entries(smbldap_state->ldap_struct, result);

	if (count != 1) {
		DEBUG(1, ("Unexpected number of results [%d] for realm "
			  "search.\n", count));
		ldap_msgfree(result);
		return NT_STATUS_OK;
	}

	entry = ldap_first_entry(smbldap_state->ldap_struct,
				 result);
	if (entry == NULL) {
		DEBUG(0, ("Could not get krbrealmcontainer entry\n"));
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	cn = get_single_attribute(mem_ctx, smbldap_state->ldap_struct, entry,
				  "cn");
	if (cn == NULL) {
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	*realm = cn;
	ldap_msgfree(result);
	return NT_STATUS_OK;
}

#define SECRETS_DOMAIN_SID    "SECRETS/SID"
static char *sec_key(TALLOC_CTX *mem_ctx, const char *d)
{
	return talloc_asprintf_strupper_m(mem_ctx, "%s/%s",
					  SECRETS_DOMAIN_SID, d);
}

static NTSTATUS save_sid_to_secret(struct ldapsam_privates *ldap_state)
{
	char hostname[255];
	int ret;
	char *p;
	TALLOC_CTX *tmp_ctx;
	NTSTATUS status = NT_STATUS_UNSUCCESSFUL;

	tmp_ctx =talloc_new(NULL);
	if (tmp_ctx == NULL) {
		return NT_STATUS_NO_MEMORY;
	}

	if (!secrets_store(sec_key(tmp_ctx, ldap_state->domain_name),
			   &ldap_state->domain_sid, sizeof(struct dom_sid))) {
		DEBUG(1, ("Failed to store domain SID"));
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	if (!secrets_store(sec_key(tmp_ctx,
				   ldap_state->ipasam_privates->flat_name),
			   &ldap_state->domain_sid, sizeof(struct dom_sid))) {
		DEBUG(1, ("Failed to store domain SID"));
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	ret = gethostname(hostname, sizeof(hostname));
	if (ret == -1) {
		DEBUG(1, ("gethostname failed.\n"));
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}
	hostname[sizeof(hostname)-1] = '\0';
	p = strchr(hostname, '.');
	if (p != NULL) {
		*p = '\0';
	}

	if (!secrets_store(sec_key(tmp_ctx, hostname),
			   &ldap_state->domain_sid, sizeof(struct dom_sid))) {
		DEBUG(1, ("Failed to store domain SID"));
		status = NT_STATUS_UNSUCCESSFUL;
		goto done;
	}

	status = NT_STATUS_OK;

done:
	talloc_free(tmp_ctx);
	return status;
}

struct ipasam_sasl_interact_priv {
	krb5_context context;
	krb5_principal principal;
	krb5_keytab keytab;
	krb5_get_init_creds_opt *options;
	krb5_creds creds;
	krb5_ccache ccache;
	const char *name;
	int name_len;
};

static int ldap_sasl_interact(LDAP *ld, unsigned flags, void *priv_data, void *sit)
{
	sasl_interact_t *in = NULL;
	int ret = LDAP_OTHER;
	struct ipasam_sasl_interact_priv *data = (struct ipasam_sasl_interact_priv*) priv_data;
	krb5_context krbctx;
	char *outname = NULL;
	krb5_error_code krberr;

	if (!ld) return LDAP_PARAM_ERROR;

	for (in = sit; in && in->id != SASL_CB_LIST_END; in++) {
		switch(in->id) {
		case SASL_CB_USER:
			in->result = data->name;
			in->len = data->name_len;
			ret = LDAP_SUCCESS;
			break;
		case SASL_CB_GETREALM:
			in->result = data->principal->realm.data;
			in->len = data->principal->realm.length;
			ret = LDAP_SUCCESS;
			break;
		default:
			in->result = NULL;
			in->len = 0;
			ret = LDAP_OTHER;
		}
	}
	return ret;
}

extern const char *lp_parm_const_string(int snum, const char *type, const char *option, const char *def);
extern void become_root();
extern void unbecome_root();
static int bind_callback(LDAP *ldap_struct, struct smbldap_state *ldap_state)
{
	char *ccache_name = NULL;
	krb5_error_code rc;

	struct ipasam_sasl_interact_priv data;
	int ret;

	data.name = lp_parm_const_string(-1, "ipasam", "principal", NULL);
	if (data.name == NULL) {
		DEBUG(0, ("bind_callback: ipasam:principal is not set, cannot use GSSAPI bind\n"));
		return LDAP_LOCAL_ERROR;
	}

	/*
	 * In order to modify the ccache we need to wrap in become/unbecome root here
	 */
	become_root();
	data.name_len = strlen(data.name);

	rc = krb5_init_context(&data.context);

	rc = krb5_parse_name(data.context, data.name, &data.principal);
	DEBUG(0,("principal is %p (%d)\n", (void*) data.principal, rc));

	rc = krb5_cc_default(data.context, &data.ccache);

	rc = krb5_cc_initialize(data.context, data.ccache, data.principal);

	rc = krb5_cc_get_full_name(data.context, data.ccache, &ccache_name);
	rc = krb5_cc_set_default_name(data.context,  ccache_name);
	DEBUG(0, ("default ccache is %s\n", krb5_cc_default_name(data.context)));

	rc = krb5_kt_resolve(data.context, "FILE:/etc/samba/samba.keytab", &data.keytab);
	DEBUG(0,("keytab is %p (%d)\n", (void*) data.keytab, rc));

	rc = krb5_get_init_creds_opt_alloc(data.context, &data.options);
	DEBUG(0,("options are %p (%d)\n", (void*) data.options, rc));

	rc = krb5_get_init_creds_opt_set_out_ccache(data.context, data.options, data.ccache);
	DEBUG(0,("options are using the ccache (%d)\n", rc));

	rc = krb5_get_init_creds_keytab(data.context, &data.creds, data.principal, data.keytab, 
					0, NULL, data.options);
	DEBUG(0,("creds uses keytab (%d)\n", rc));

	ret = ldap_sasl_interactive_bind_s(ldap_struct,
					   NULL, "GSSAPI",
					   NULL, NULL,
					   LDAP_SASL_QUIET,
					   ldap_sasl_interact, &data);
	if (ret != LDAP_SUCCESS) {
		DEBUG(0, ("bind_callback: cannot perform interactive SASL bind with GSSAPI\n"));
	}

	krb5_get_init_creds_opt_free(data.context, data.options);
	krb5_kt_close(data.context, data.keytab);
	krb5_cc_close(data.context, data.ccache);
	krb5_free_context(data.context);
	unbecome_root();
	return ret;
}

static NTSTATUS pdb_init_ipasam(struct pdb_methods **pdb_method,
				const char *location)
{
	struct ldapsam_privates *ldap_state;

	char *uri;
	NTSTATUS status;
	char *dn = NULL;
	char *domain_sid_string = NULL;
	struct dom_sid ldap_domain_sid;
	char *bind_dn = NULL;
	char *bind_secret = NULL;
	const char *service_principal = NULL;

	LDAPMessage *result = NULL;
	LDAPMessage *entry = NULL;

	status = make_pdb_method(pdb_method);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	(*pdb_method)->name = "ipasam";

	if ( !(ldap_state = talloc_zero(*pdb_method, struct ldapsam_privates)) ) {
		DEBUG(0, ("pdb_init_ipasam: talloc() failed for ldapsam private_data!\n"));
		return NT_STATUS_NO_MEMORY;
	}

	ldap_state->ipasam_privates = talloc_zero(ldap_state,
						  struct ipasam_privates);
	if (ldap_state->ipasam_privates == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	ldap_state->is_ipa_ldap = true;

	uri = talloc_strdup( NULL, location );
	if (uri == NULL) {
		return NT_STATUS_NO_MEMORY;
	}
	trim_char( uri, '\"', '\"' );

	service_principal = lp_parm_const_string(-1, "ipasam", "principal", NULL);

	if (service_principal == NULL) {
		if (!fetch_ldap_pw(&bind_dn, &bind_secret)) {
			DEBUG(0, ("pdb_init_ipasam: Failed to retrieve LDAP password from secrets.tdb\n"));
			return NT_STATUS_NO_MEMORY;
		}
		status = smbldap_init(*pdb_method, pdb_get_tevent_context(),
			      uri, false, bind_dn, bind_secret,
			      &ldap_state->smbldap_state);
	} else {
		/* We authenticate via GSSAPI and thus will use kerberos principal to bind our access */
		status = smbldap_init(*pdb_method, pdb_get_tevent_context(),
			      uri, false, NULL, NULL,
			      &ldap_state->smbldap_state);
		if (NT_STATUS_IS_OK(status)) {
			ldap_state->smbldap_state->bind_callback = bind_callback;
		}
	}

	talloc_free(uri);
	if (!NT_STATUS_IS_OK(status)) {
		return status;
	}

	(*pdb_method)->private_data = ldap_state;
	(*pdb_method)->free_private_data = ipasam_free_private_data;

	status = ipasam_get_base_dn(ldap_state->smbldap_state,
				    ldap_state->ipasam_privates,
				    &ldap_state->ipasam_privates->base_dn);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Failed to get base DN.\n"));
		return status;
	}

	if (!(smbldap_has_extension(priv2ld(ldap_state), IPA_KEYTAB_SET_OID) ||
	      smbldap_has_extension(priv2ld(ldap_state), IPA_KEYTAB_SET_OID_OLD))) {
		DEBUG(0, ("Server is not an IPA server.\n"));
		return NT_STATUS_INVALID_PARAMETER;
	}

	ldap_state->ipasam_privates->trust_dn = talloc_asprintf(
					  ldap_state->ipasam_privates,
					  "cn=ad,cn=trusts,%s",
					  ldap_state->ipasam_privates->base_dn);
	if (ldap_state->ipasam_privates->trust_dn == NULL) {
		DEBUG(0, ("Failed to create trsut DN.\n"));
		return NT_STATUS_NO_MEMORY;
	}

	status = ipasam_get_domain_name(ldap_state, ldap_state,
					&ldap_state->domain_name);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Failed to get domain name.\n"));
		return status;
	}

	status = ipasam_get_realm(ldap_state, ldap_state->ipasam_privates,
				  &ldap_state->ipasam_privates->realm);
	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("Failed to get realm.\n"));
		return status;
	}

	status = ipasam_search_domain_info(ldap_state->smbldap_state, &result);

	if (!NT_STATUS_IS_OK(status)) {
		DEBUG(0, ("pdb_init_ldapsam: WARNING: Could not get domain "
			  "info, nor add one to the domain. "
			  "We cannot work reliably without it.\n"));
		return NT_STATUS_CANT_ACCESS_DOMAIN_INFO;
	}

	entry = ldap_first_entry(ldap_state->smbldap_state->ldap_struct,
				 result);
	if (entry == NULL) {
		DEBUG(0, ("pdb_init_ipasam: Could not get domain info "
			  "entry\n"));
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	dn = get_dn(ldap_state, ldap_state->smbldap_state->ldap_struct, entry);
	if (dn == NULL) {
		ldap_msgfree(result);
		return NT_STATUS_UNSUCCESSFUL;
	}

	ldap_state->domain_dn = smb_xstrdup(dn);
	talloc_free(dn);

	ldap_state->ipasam_privates->flat_name = get_single_attribute(
					ldap_state,
					ldap_state->smbldap_state->ldap_struct,
					entry,
					LDAP_ATTRIBUTE_FLAT_NAME);
	if (ldap_state->ipasam_privates->flat_name == NULL) {
		DEBUG(0, ("Missing mandatory attribute %s.\n",
			  LDAP_ATTRIBUTE_FLAT_NAME));
		return NT_STATUS_INVALID_PARAMETER;
	}

	domain_sid_string = get_single_attribute(
				ldap_state,
				ldap_state->smbldap_state->ldap_struct,
				entry,
				LDAP_ATTRIBUTE_SID);

	if (domain_sid_string) {
		if (!string_to_sid(&ldap_domain_sid, domain_sid_string)) {
			DEBUG(1, ("pdb_init_ldapsam: SID [%s] could not be "
				  "read as a valid SID\n", domain_sid_string));
			ldap_msgfree(result);
			TALLOC_FREE(domain_sid_string);
			return NT_STATUS_INVALID_PARAMETER;
		}
		sid_copy(&ldap_state->domain_sid, &ldap_domain_sid);
		talloc_free(domain_sid_string);

		status = save_sid_to_secret(ldap_state);
		if (!NT_STATUS_IS_OK(status)) {
			return status;
		}
	}



	(*pdb_method)->getsampwnam = ldapsam_getsampwnam;
	(*pdb_method)->search_users = ldapsam_search_users;
	(*pdb_method)->search_groups = ldapsam_search_groups;
	(*pdb_method)->search_aliases = ldapsam_search_aliases;
	(*pdb_method)->lookup_rids = ldapsam_lookup_rids;
	(*pdb_method)->sid_to_id = ldapsam_sid_to_id;
	(*pdb_method)->uid_to_sid = ldapsam_uid_to_sid;
	(*pdb_method)->gid_to_sid = ldapsam_gid_to_sid;

	(*pdb_method)->capabilities = pdb_ipasam_capabilities;
	(*pdb_method)->get_domain_info = pdb_ipasam_get_domain_info;

	(*pdb_method)->get_trusteddom_pw = ipasam_get_trusteddom_pw;
	(*pdb_method)->set_trusteddom_pw = ipasam_set_trusteddom_pw;
	(*pdb_method)->del_trusteddom_pw = ipasam_del_trusteddom_pw;
	(*pdb_method)->enum_trusteddoms = ipasam_enum_trusteddoms;

	(*pdb_method)->get_trusted_domain = ipasam_get_trusted_domain;
	(*pdb_method)->get_trusted_domain_by_sid = ipasam_get_trusted_domain_by_sid;
	(*pdb_method)->set_trusted_domain = ipasam_set_trusted_domain;
	(*pdb_method)->del_trusted_domain = ipasam_del_trusted_domain;
	(*pdb_method)->enum_trusted_domains = ipasam_enum_trusted_domains;

	return NT_STATUS_OK;
}

NTSTATUS samba_module_init(void)
{
	return smb_register_passdb(PASSDB_INTERFACE_VERSION, "ipasam",
				   pdb_init_ipasam);
}

NTSTATUS samba_init_module(void)
{
	return smb_register_passdb(PASSDB_INTERFACE_VERSION, "ipasam",
				   pdb_init_ipasam);
}
