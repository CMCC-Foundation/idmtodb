#!/bin/bash

sel_date="$(date '+%Y%m%d')"

ipa -vv group-find --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/groups_dict=[/g' | sed '$s/.$//' | sed 's/        /    /g' > group_found_json.py
ipa -vv user-find --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/users_dict=[/g' | sed '$s/.$//' | sed 's/        /    /g' > user_found_json.py
#ipa -vv user-find --preserved=true --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/users_dict_preserved=[/g' | sed '$s/.$//' | sed 's/        /    /g' > user_found_preserved_json.py
echo 'users_ldap_dict={' > user_ldap_found.py && ldapsearch  "(memberOf=cn=juno-users,cn=groups,cn=accounts,dc=idm,dc=cmcc,dc=scc)" [options] uidnumber createTimestamp 2>/dev/null | grep -e uidnumber -e "createTimestamp:" | sed ':a;N;$!ba;s/\ncreateTimestamp: /": "/g' | sed ':a;N;$!ba;s/Z\nuidnumber: /",\n"/g' | sed 's/uidnumber: /"/g' | sed ':a;N;$!ba;s/Z/"\n}/g' >> user_ldap_found.py
mysql idmdb --raw -h 172.16.3.23 -u sysm01 -p960coreP6 < vpn_query > VPN_expiration_date.csv
#echo 'users_vpn_dict={' > "VPN_Zeus_""$sel_date""_mod.py" && cat "VPN_Zeus_""$sel_date"".csv" | tr ',' ':' | sed ':a;N;$!ba;s/\n/",\n"/g' | sed 's/""//g' | tail -n +2 | head -n -1 | sed 's/:/":"/g' | sed ':a;N;$!ba;s/.$/\n}/g' >> VPN_Zeus_"$sel_date"_mod.py
echo 'users_vpn_dict={' > "VPN_expiration_date_mod.py" && cat "VPN_expiration_date.csv" | sed "s/	/:/g" | sed ':a;N;$!ba;s/\n/",\n"/g' | sed 's/""//g' | tail -n +2 | head -n -1 | sed 's/:/":"/g' | sed ':a;N;$!ba;s/.$/\n}/g' >> VPN_expiration_date_mod.py
python3 fetch_users.py > "user_idm_""$sel_date"".csv" # eventually you may want to redirect 2>/dev/null
python3 fetch_groups.py > "group_idm_""$sel_date"".csv"
