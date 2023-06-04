import sys
from user_found_json import users_dict
from user_ldap_found import users_ldap_dict
from user_found_preserved_json import users_dict_preserved
from user_ldap_preserved_found import users_ldap_preserved_dict
from VPN_Zeus_20230515_mod import users_vpn_dict

DEBUG_MODE=False #True

#print("users_dict len: {}".format(len(users_dict)))
cnt=0
cnt_managed=0
divisions=["asc", "opa", "csp", "ecip", "remhi", "sysm", "oda", "seme", "iafes", "raas"] #, "cmcc", "scc", "ext"]
jolly_groups=["ipausers", "juno-users", "juno-cmcc", "juno-ext"]

print("username,name,surname,uid,gid,group_name,division,creation_date,expiration_date,vpn_expiration_date,email,closing_date") #,no_cmcc,closing_date,status")I

users_dict.extend(users_dict_preserved)
users_ldap_dict.update(users_ldap_preserved_dict)

for i in users_dict:
    username=i["uid"][0]
    
    if(username == "admin"):
        continue

    #print("---")
    #print(i["memberOf"])
    #print("---")

    #if("memberOf" in i.keys()):
    memberOf = i["memberOf"] if("memberOf" in i.keys()) else None
    is_preserved = (memberOf is None)

    group = None
    division = None
    creation_date = None
    closing_date = None
    krbPrincipalExpiration = None
    vpn_expiration_date = None
    uidnumber_str = i["uidnumber"][0]
    uidnumber = int(uidnumber_str)


    if(not is_preserved):
        for j in memberOf:
            first_group_token=j.split(",")[0].split("=")
            #this_group = None

            if(first_group_token[0] != "ipaUniqueID"):
                this_group = first_group_token[1]
                #print(this_group)
        
                if(this_group in divisions):
                    division = this_group
                elif(this_group not in jolly_groups):
                    group = this_group


        if(group == None and division != None):
            group = division


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

    if(uidnumber_str in users_vpn_dict.keys()):
        vpn_expiration_date = users_vpn_dict[uidnumber_str]

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

    print("{},{},{},{},{},{},{},{},{},{},{},{}".format(username, i["givenname"][0], i["sn"][0], uidnumber, int(i["gidnumber"][0]), group, division, creation_date, krbPrincipalExpiration, vpn_expiration_date, i["mail"][0], closing_date)) 
    cnt += 1

if(DEBUG_MODE):
    print("---")

sys.stderr.write("\n\nprocessed total {} elements.\n".format(cnt))
sys.stderr.write("unmanaged {} elements.\n".format(cnt-cnt_managed))
sys.stderr.write("managed {} elements.\n".format(cnt_managed))
sys.stderr.write("m1b54 & d4n110\n\n")
