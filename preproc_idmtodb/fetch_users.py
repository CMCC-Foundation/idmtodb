import sys
from group_found_json import groups_dict
from user_found_json import users_dict
from user_ldap_found import users_ldap_dict
#from user_found_preserved_json import users_dict_preserved
#from user_ldap_preserved_found import users_ldap_preserved_dict
from VPN_expiration_date_mod import users_vpn_dict

DEBUG_MODE=False #True
DIVISION_GROUP_STR="This is a division group"

#print("users_dict len: {}".format(len(users_dict)))
cnt=0
cnt_managed=0
divisions=["asc", "opa", "csp", "ecip", "remhi", "sysm", "oda", "seme", "iafes", "raas", "iscd"] #, "cmcc", "scc", "ext"]
jolly_groups=[] #"ipausers", "juno-users", "juno-cmcc", "juno-ext"]

print("username,name,surname,uid,gid,group_names,creation_date,expiration_date,vpn_expiration_date,email,closing_date,nsaccount_lock,password,mach") #,no_cmcc,closing_date,status")I

#users_dict.extend(users_dict_preserved)
#users_ldap_dict.update(users_ldap_preserved_dict)

groups_mapping = dict( )
groups_desc = dict( )

for i in groups_dict:
    i_keys=i.keys()
    groups_desc[i["cn"][0]] = i["description"][0] if "description" in i_keys else '' #None
    if("gidnumber" in i_keys):
        groups_mapping[i["gidnumber"][0]] = i["cn"][0]
        #groups_desc[i["gidnumber"][0]] = i["description"][0] if "description" in i_keys else '' #None


#print("groups desc")
#print(groups_desc)
#print("---")

groups_mapping_keys = groups_mapping.keys()
groups_desc_keys = groups_desc.keys()

for i in users_dict:
    username=i["uid"][0]
    
    if(username == "admin"):
        continue

    #print("---")
    #print(i["memberOf"])
    #print("---")

    #if("memberOf" in i.keys()):
    #groups_mapping_keys = groups_mapping.keys()
    memberOf = i["memberOf"] if("memberOf" in i.keys()) else None
    orderedMemberOf = []
    is_preserved = (memberOf is None)

    mach = ''
    group = None
    division = None
    creation_date = None
    closing_date = None
    krbPrincipalExpiration = None
    vpn_expiration_date = None
    uidnumber_str = i["uidnumber"][0]
    uidnumber = int(uidnumber_str)
    gidnumber_str = i["gidnumber"][0]
    gidnumber = int(gidnumber_str)
    nsaccount_lock = int(i["nsaccountlock"][0] == "TRUE")
    password = "password"

    if(not is_preserved):
        for j in memberOf:
            first_group_token=j.split(",")[0].split("=")
            #this_group = None

            if(first_group_token[0] != "ipaUniqueID"):
                this_group = first_group_token[1]
                splitted_this_group = this_group.split("-")
                #print(this_group)
            
                if(len(splitted_this_group) > 1 and splitted_this_group[1] == "users"):
                    mach = splitted_this_group[0]
                elif(this_group in divisions or (this_group in groups_desc_keys and DIVISION_GROUP_STR in groups_desc[this_group])):
                    division = this_group 
                elif(this_group not in jolly_groups and gidnumber_str in groups_mapping_keys and this_group == groups_mapping[gidnumber_str] ):
                    group = this_group
                else:
                    orderedMemberOf.append(this_group)


        if(group == None and division != None):
            group = division

        if(group):
            orderedMemberOf = [group] + orderedMemberOf

        if(division):
            orderedMemberOf = [division] + orderedMemberOf


    '''
    for j in users_ldap_dict:
        if(int(j["uidnumber"]) == uidnumber):  #if(j["dn"].split(",")[0].split("=")[1] == username):
            creation_date_raw = j["createTimestamp"]
            creation_date = creation_date_raw[:4]+'-'+creation_date_raw[4:6]+'-'+creation_date_raw[6:8]
    '''

    if(uidnumber_str in users_ldap_dict.keys()):
        if(not is_preserved):
            creation_date_raw = users_ldap_dict[uidnumber_str] #j["createTimestamp"]
            creation_date = creation_date_raw[:4]+'-'+creation_date_raw[4:6]+'-'+creation_date_raw[6:8]
        else:
            closing_date_raw = users_ldap_dict[uidnumber_str]
            closing_date = closing_date_raw[:4]+'-'+closing_date_raw[4:6]+'-'+closing_date_raw[6:8]

    if(username in users_vpn_dict.keys()):
        vpn_expiration_date = users_vpn_dict[username]

    if("krbPrincipalExpiration" not in i.keys()):
        krbPrincipalExpiration = None #''
        

        #i["krbPrincipalExpiration"] = [ '' ]
        cnt_managed += 1
        
        #print(i["krbPrincipalExpiration"])
        

        if(DEBUG_MODE):
            print("added blank krbPrincipalExpiration to: {}".format(username))

    else:
        krbPrincipalExpirationRaw = i["krbPrincipalExpiration"][0]
        krbPrincipalExpiration = krbPrincipalExpirationRaw[:4]+'-'+krbPrincipalExpirationRaw[4:6]+'-'+krbPrincipalExpirationRaw[6:8]
        if(DEBUG_MODE):
            print("{}'s krbPrincipalExpiration: {}".format(username, i["krbPrincipalExpiration"][0]))


    if(DEBUG_MODE): #True): #DEBUG_MODE):
        print("---")
        print("USER: {}".format(username))
        print("memberOf: {}".format(i["memberOf"]))
        print("krbPrincipalExpiration: {}".format(krbPrincipalExpiration))
        print("creation_date: {}".format(creation_date))
        print("group: {}, division: {}".format(group, division))
        print("memberOf: {}".format(memberOf))
        #print("---")

    print("{};{};{};{};{};{};{};{};{};{};{};{};{};{}".format(username, i["givenname"][0], i["sn"][0], uidnumber, gidnumber, ','.join(map(str,orderedMemberOf)), creation_date, krbPrincipalExpiration, vpn_expiration_date, i["mail"][0], closing_date, nsaccount_lock, password, mach)) 
    cnt += 1

if(DEBUG_MODE):
    print("---")

sys.stderr.write("\n\nprocessed total {} elements.\n".format(cnt))
sys.stderr.write("unmanaged {} elements.\n".format(cnt-cnt_managed))
sys.stderr.write("managed {} elements.\n".format(cnt_managed))
sys.stderr.write("m1b54 & d4n110\n\n")
