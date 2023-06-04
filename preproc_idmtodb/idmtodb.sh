#!/bin/bash

sel_date="$(date '+%Y%m%d')"

#echo "VPN_Zeus_""$sel_date"".csv"
#echo "VPN_Zeus_""$sel_date""_mod.py"
#echo "user_idm_""$sel_date"".csv"


ipa -vv user-find --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/users_dict=[/g' | sed '$s/.$//' | sed 's/        /    /g' > user_found_json.py
ipa -vv user-find --preserved=true --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/users_dict_preserved=[/g' | sed '$s/.$//' | sed 's/        /    /g' > user_found_preserved_json.py
echo 'users_ldap_dict={' > user_ldap_found.py && ldapsearch  "(memberOf=cn=juno-users,cn=groups,cn=accounts,dc=idm,dc=cmcc,dc=scc)" [options] uidnumber createTimestamp 2>/dev/null | grep -e uidnumber -e "createTimestamp:" | sed ':a;N;$!ba;s/\ncreateTimestamp: /": "/g' | sed ':a;N;$!ba;s/Z\nuidnumber: /",\n"/g' | sed 's/uidnumber: /"/g' | sed ':a;N;$!ba;s/Z/"\n}/g' >> user_ldap_found.py
echo 'users_vpn_dict={' > "VPN_Zeus_""$sel_date""_mod.py" && cat "VPN_Zeus_""$sel_date"".csv" | tr ',' ':' | sed ':a;N;$!ba;s/\n/",\n"/g' | sed 's/""//g' | tail -n +2 | head -n -1 | sed 's/:/":"/g' | sed ':a;N;$!ba;s/.$/\n}/g' >> VPN_Zeus_"$sel_date"_mod.py
python3 sanitize_krbprincipalexpiration_unified.py > "user_idm_""$sel_date"".csv" # eventually you may want to redirect 2>/dev/null
