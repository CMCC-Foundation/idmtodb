#!/bin/bash

parent_folder="cmcc"

for i in $(tail -n+2 "$1" | cut -d';' -f1,6,13); #7);
do
 	if [[ $(echo "$i" | cut -d';' -f3) == "" ]];
	then
		continue
	fi

	echo "################################################"
        this_user=$(echo "$i" | cut -d';' -f1)
	this_grps=$(echo "$i" | cut -d';' -f2)


	#this_div=$(echo "$this_grps" | cut -d',' -f1)
	this_grp=$(echo "$this_grps" | cut -d',' -f2)

        #echo "This user: ""$this_user"", div: ""$this_div", primary group: ""$this_grp""
        echo "This user: ""$this_user"", parent folder: ""$parent_folder", primary group: ""$this_grp""

	# decomment this for debugging purposes
	# continue

	su - "$this_user" -c "exit" # decomment this in order to create /users_home/division/user
        ls -lah "/users_home/""$parent_folder" | grep "$this_user"
        echo "Back to parent shell."
        work_dir="/work/""$parent_folder""/""$this_user"
        echo "Creating /work dir: ""$work_dir"
        mkdir -p "$work_dir"
        echo "Setting UNIX ownerships.."
        chown "$this_user":"$this_grp" "$work_dir"
        chmod g+s "/users_home/""$parent_folder""/""$this_user"
	echo "Done."
        echo "Setting UNIX permissions.."
        chmod 750 "$work_dir"
	chmod g+s "$work_dir"
        ls -lah "/work/""$parent_folder" | grep "$this_user"
        echo "Done."
        data_dir="/data/""$parent_folder""/""$this_user"
        echo "Creating /data dir: ""$data_dir"
        mkdir -p "$data_dir"
        echo "Setting UNIX ownerships.."
        chown "$this_user":"$this_grp" "$data_dir"
        echo "Done."
        echo "Setting UNIX permissions.."
        chmod 700 "$data_dir"
	chmod g+s "$data_dir"
        ls -lah "/data/""$parent_folder" | grep "$this_user"
        echo "Done."
        echo "################################################"
done
