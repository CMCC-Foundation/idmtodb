#!/bin/bash

preserved_users=$(ipa -vv user-find --preserved=true --raw --all 2>/dev/null | grep "uid: " | cut -d' ' -f4)

echo 'users_ldap_preserved_dict={' > user_ldap_preserved_found.py

for i in ${preserved_users[@]};
do
	ldapsearch "(uid=$i)" [options] uid modifyTimestamp 2>/dev/null | grep -e "uid:" -e "modifyTimestamp:" | sed ':a;N;$!ba;s/\nmodifyTimestamp: /": "/g' | sed ':a;N;$!ba;s/Z\nuid: /",\n"/g' | sed 's/uid: /"/g' | sed ':a;N;$!ba;s/Z/",/g' >> user_ldap_preserved_found.py
done

sed -i ':a;N;$!ba;$s/.$/\n}/' user_ldap_preserved_found.py
