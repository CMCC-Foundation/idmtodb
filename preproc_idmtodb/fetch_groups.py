import sys
from group_found_json import groups_dict
DEBUG_MODE=False #True


jolly_groups=[] #"ipausers", "juno-users", "juno-cmcc", "juno-ext"]

print("name,description,gid") #,no_cmcc,closing_date,status")I

#users_dict.extend(users_dict_preserved)
#users_ldap_dict.update(users_ldap_preserved_dict)

status = 1
groups_mapping = dict( )

for i in groups_dict:
    if("gidnumber" in i.keys()):
        groups_mapping[i["gidnumber"][0]] = i["cn"][0]

for i in groups_dict:
   
    this_group_key = i.keys()

    group_name = i["cn"][0]
    description = i["description"][0].replace(',','') if "description" in this_group_key else None 

    gidnumber = i["gidnumber"][0] if "gidnumber" in this_group_key else None
  
    if(group_name not in jolly_groups):
        print("{};{};{};{}".format(group_name, description, status, gidnumber))




