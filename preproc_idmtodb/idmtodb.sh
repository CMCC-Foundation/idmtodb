#!/bin/bash

sel_date="$(date '+%Y%m%d')"
USERSIDM_SERVER=${8:-"127.0.0.1"}
USERSIDM_USER=${9:-"root"}
USERSIDM_PASSWORD="root" #"root"
USERSIDM_DATABASE="idmdb"

ipa -vv group-find --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/groups_dict=[/g' | sed '$s/.$//' | sed 's/        /    /g' > group_found_json.py
ipa -vv user-find --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/users_dict=[/g' | sed '$s/.$//' | sed 's/        /    /g' > user_found_json.py
#ipa -vv user-find --preserved=true --raw --all 2>&1 >/dev/null | sed -n '/"result"\: \[/,/"summary": "/p' | head -n -1 | sed 's/        "result": \[/users_dict_preserved=[/g' | sed '$s/.$//' | sed 's/        /    /g' > user_found_preserved_json.py
#./ldapsearch_preserved.sh
#echo 'users_ldap_dict={' > user_ldap_found.py && ldapsearch  "(memberOf=cn=juno-users,cn=groups,cn=accounts,dc=idm,dc=cmcc,dc=scc)" [options] uidnumber createTimestamp 2>/dev/null | grep -e uidnumber -e "createTimestamp:" | sed ':a;N;$!ba;s/\ncreateTimestamp: /": "/g' | sed ':a;N;$!ba;s/Z\nuidnumber: /",\n"/g' | sed 's/uidnumber: /"/g' | sed ':a;N;$!ba;s/Z/"\n}/g' >> user_ldap_found.py
echo 'users_ldap_dict={' > user_ldap_found.py && ldapsearch  "(memberOf=cn=ipausers,cn=groups,cn=accounts,dc=idm,dc=cmcc,dc=scc)" [options] uid createTimestamp 2>/dev/null | grep -e "uid:" -e "createTimestamp:" | sed ':a;N;$!ba;s/\ncreateTimestamp: /": "/g' | sed ':a;N;$!ba;s/Z\nuid: /",\n"/g' | sed 's/uid: /"/g' | sed ':a;N;$!ba;s/Z/"\n}/g' >> user_ldap_found.py
mysql "$USERSIDM_DATABASE" --raw -h "$USERSIDM_SERVER" -u "$USERSIDM_USER" -p"$USERSIDM_PASSWORD" <<<$(echo "SELECT username, vpn_expiration_date FROM user_idm WHERE vpn_expiration_date IS NOT NULL") > VPN_expiration_date.csv
#echo 'users_vpn_dict={' > "VPN_Zeus_""$sel_date""_mod.py" && cat "VPN_Zeus_""$sel_date"".csv" | tr ',' ':' | sed ':a;N;$!ba;s/\n/",\n"/g' | sed 's/""//g' | tail -n +2 | head -n -1 | sed 's/:/":"/g' | sed ':a;N;$!ba;s/.$/\n}/g' >> VPN_Zeus_"$sel_date"_mod.py
echo 'users_vpn_dict={' > "VPN_expiration_date_mod_tmp.py" && cat "VPN_expiration_date.csv" | tr '\t' ':' | sed ':a;N;$!ba;s/\n/",\n"/g' | sed 's/""//g' | tail -n +2 | sed 's/:/":"/g' >> VPN_expiration_date_mod_tmp.py
echo -ne "\"SPEC_CHAR" >> "VPN_expiration_date_mod_tmp.py" && cat "VPN_expiration_date_mod_tmp.py" | sed ':a;N;$!ba;s/\n\"SPEC_CHAR/"\n}/g' > "VPN_expiration_date_mod.py"
rm -f "VPN_expiration_date_mod_tmp.py"

mysql "$USERSIDM_DATABASE" --raw -h "$USERSIDM_SERVER" -u "$USERSIDM_USER" -p"$USERSIDM_PASSWORD" <<<$(echo "SELECT username, notification_date FROM user_idm WHERE notification_date IS NOT NULL")
echo 'users_notif_dict={' > "notification_date_mod_tmp.py" && cat "notification_date.csv" | tr '\t' ':' | sed ':a;N;$!ba;s/\n/",\n"/g' | sed 's/""//g' | tail -n +2 | sed 's/:/":"/g' >> notification_date_mod_tmp.py
echo -ne "\"SPEC_CHAR" >> "notification_date_mod_tmp.py" && cat "notification_date_mod_tmp.py" | sed ':a;N;$!ba;s/\n\"SPEC_CHAR/"\n}/g' > "notification_date_mod.py"
rm -f "notification_date_mod_tmp.py"

python3 fetch_users.py > "user_idm_""$sel_date"".csv" # eventually you may want to redirect 2>/dev/null
python3 fetch_groups.py > "group_idm_""$sel_date"".csv"
