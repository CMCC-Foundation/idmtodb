#!/bin/sh
IFS=$'\n'
SEP=','

date_keyword="date"

sed -i "s/,$date_keyword,/,$(date '+%Y-%m-%d'),/g" "$1"
cnt_file=0

for line in $(tail "$1" -n+2); do
    IFS=' '
    cnt_file=$(($cnt_file+1))
    username=$(echo $line|cut -f1 -d"$SEP")
    first=$(echo $line|cut -f2 -d"$SEP")
    last=$(echo $line|cut -f3 -d"$SEP")
    # Not collecting encrypted password because we need cleartext password to create kerberos key
    uid=$(echo $line|cut -f4 -d"$SEP")
    gid=$(echo $line|cut -f5 -d"$SEP")
    group_name=$(echo $line |cut -f6 -d"$SEP") 

    ipa group-show "$group_name" 1>/dev/null 2>/dev/null
	
    if [[ "$?" == "2" ]];
    then
	echo "ERROR [""$username"", line ""$(($cnt_file+1))""]: The specified primary group: ""$group_name"" does not exist into IDM groups." >&2
	continue
	#exit "$?"
    fi

    div=$(echo $line|cut -f7 -d"$SEP")
	
    ipa group-show "$div" 1>/dev/null 2>/dev/null

    if [[ "$?" == "2" ]];
    then
    	echo "ERROR: [""$username"", line ""$(($cnt_file+1))""]: The specified division: ""$div"" does not exist into IDM groups." >&2
        continue
	#exit "$?"
    fi
	
    creation_date=$(echo $line |cut -f8 -d"$SEP")

    tmp_expdate=$(echo $line|cut -f9 -d"$SEP")
    if [[ "$tmp_expdate" = "" ]];then
        expdate=$tmp_expdate
    else
        expdate=$(echo $line|cut -f9 -d"$SEP")'T00:00Z'
    fi

    email=$(echo $line|cut -f11 -d"$SEP")
    shell="/bin/bash"
    gecos=$(echo $first" "$last )

    pwd=$(echo $line|cut -f13 -d"$SEP")

    #first=$(echo $gecos| cut -d' ' -f1 )
    #last=$(echo $gecos| cut -d' ' -f 2- )

    #pass=$( tr -cd '[:alnum:]' < /dev/urandom | fold -w8| head -1 )

    # Now create this entry

    #echo "ipa user-add $username --first=$first --last=$last --gidnumber=$gid --uid=$uid --gecos=\"$gecos\" --homedir=\"/users_home/$div/$username\" --shell=$shell --email=$email --user-auth-type=otp --random --principal-expiration=$expdate"
    #echo "ipa otptoken-add --type=totp --owner=$username"

    #echo $uid, $gid, $first, $last, $div, $email, $expadate

    echo "USERNAME: ""$username"
    echo "FIRST: ""$first"
    echo "LAST: ""$last"
    echo "UID: ""$uid"
    echo "GID: ""$gid"
    echo "GROUP_NAME: ""$group_name"
    echo "DIVISION: ""$div"
    echo "CREATION_DATE: ""$creation_date"
    echo "EXPIRATION_DATE: ""$expdate"
    echo "EMAIL: ""$email"
    echo "GECOS: ""$gecos"
    echo "PSW: ""$pwd"

    #exit

    #### BEGIN
    echo $pwd | ipa user-add $username --first="$first" --last="$last" --password --gidnumber=$gid --uid=$uid --gecos="$gecos" --homedir="/users_home/$div/$username" --shell="$shell" --email=$email --user-auth-type=otp --principal-expiration=$expdate >> "$1""_logs"
    if (( $? == 0 ));then
	echo "  Password for $username is: $pwd" >> "$1""_logs"
	ipa otptoken-add --type=totp --owner=$username >> "$1""_logs"
    else
	echo "Error while creating the user!"
    fi
    #### END


    #echo $pwd
    #echo $expdate

    #### BEGIN
    echo "*************************************************" >> "$1""_logs"
    #### END

     #continue
    #### PARTE INSERIMENTO GRUPPI
    ipa group-add-member "juno-users" --users="$username" 1>/dev/null

    if [[ "$?" != "0" ]];
    then
        echo "ERROR: [""$username"", line ""$(($cnt_file+1))""]: Failed to add ""$username"" user to the \"juno-users\" group." >&2
        continue
    fi


    if [[ "$group_name" != "juno-ext" ]] && [[ "$division" != "juno-ext" ]]; # decidere se "juno-ext" sarà indicato su division o group_name
    then
        ipa group-add-member "juno-cmcc" --users="$username" 1>/dev/null
    	if [[ "$?" != "0" ]];
    	then
            echo "ERROR: [""$username"", line ""$(($cnt_file+1))""]: Failed to add ""$username"" user to the \"juno-cmcc\" group." >&2
            continue
    	fi
    else
        ipa group-add-member "juno-ext" --users="$username" 1>/dev/null
        if [[ "$?" != "0" ]];
        then
            echo "ERROR: [""$username"", line ""$(($cnt_file+1))""]: Failed to add ""$username"" user to the \"juno-ext\" group." >&2
            continue
        fi
    fi

    ipa group-add-member "$div" --users="$username" 1>/dev/null

    if [[ "$?" != "0" ]];
    then
        echo "ERROR: [""$username"", line ""$(($cnt_file+1))""]: Failed to add ""$username"" user to the \"""$div""\" group." >&2
        continue
    fi


if [[ "$group_name" != "$div" ]]; then
        ipa group-add-member "$group_name" --users="$username" 1>/dev/null

        if [[ "$?" != "0" ]];
        then
            echo "ERROR: [""$username"", line ""$(($cnt_file+1))""]: Failed to add ""$username"" user to the \"""$group_name""\" group." >&2
            continue
        fi
    fi

    ####


	#ipa user-show $username
	#cnt_file=$(($cnt_file+1))
done
