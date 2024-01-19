#!/bin/bash

PREFIX_DEXTER="/root/gdrive_API"
cd "$PREFIX_DEXTER"

FIELDS=",,,,,,,,,,,,,"
FIELDS_OUT=";;;;;;;;;;;;;"
USERSIDM_SERVER=${13:-"127.0.0.1"}
USERSIDM_USER=${14:-"sysm01"}
USERSIDM_PASSWORD="960coreP6" #"root"
USERSIDM_DATABASE="idmdb"

IDMTODB_PROMPT_ON_INSERT=${7:-"1"}
IDMTODB_PROMPT_ON_UPDATE=${8:-"1"}
IDMTODB_PROMPT_ON_DELETE=${9:-"2"} #${3:-"1"} # insert a number > 1 (i.e. 2) in order to permanently disable delete synchronization
IDMTODB_IGNORE_GROUPS=${10:-"0"} #1"} 
IDMTODB_IGNORE_DIVISION_GROUP_NAME=${11:-"0"}
IDMTODB_MAX_USERS=${12:-"1000"}

DB_LOC="\"$USERSIDM_USER\"@\"$USERSIDM_SERVER\""
DB_LOC_EXT="\"$USERSIDM_USER\"@\"$USERSIDM_SERVER\""".$USERSIDM_DATABASE"

DEXTER_LINE_SEP="##############################################"
AUP_HEADER="username;name;surname;uid;gid;group_names;expiration_date;vpn_expiration_date;email;mach;notify"
DRIVE_CMCC_SCC_FOLDER_NAME="SCC-HSM/CMCC_IDM_users_credentials/00_IDM_users_list"
DEXTER_CSV_FILENAME="dexter_users.csv"
DEXTER_CSV_FILENAME_OUT=$(echo "$DEXTER_CSV_FILENAME" | cut -d'.' -f1)"_OUT.csv"
DEXTER_CSV_FILENAME_OUT_TMP=$(echo "$DEXTER_CSV_FILENAME" | cut -d'.' -f1)"_OUT.csv" #_TMP.csv"
DEXTER_CSV_FILENAME_OUT_IN_ROTATED="$(echo $DEXTER_CSV_FILENAME | cut -d '.' -f1)""_IN_""$(date '+%Y%m%d')"".csv"
DEXTER_CSV_FILENAME_OUT_UP_ROTATED="$(echo $DEXTER_CSV_FILENAME | cut -d '.' -f1)""_UP_""$(date '+%Y%m%d')"".csv"
MAX_USERS="$IDMTODB_MAX_USERS" #1000
MAX_USERS_IN="$MAX_USERS"
MAX_USERS_UP="$MAX_USERS"
MASTER_AUP_PATH="/root/add-zeus-user-group/new_script_sccdb_v2"
GDRIVE_API_VENV="/root/gdrive_API/gdrive_venv"
MASTER_HOST="172.16.3.11"
TOKEN_FILE="$1"

REMOTE_AUP_COMMAND=${6:-"add-users-parametric-cmcc.sh"} #"add-users-parametric.sh"
PRESERVE_WIP_CHANGES=${5:-"1"} #true

MODE_IN=${2:-"1"} #true
MODE_UP=${3:-"1"} #true
MODE_OUT=${4:-"1"} #true

echo "Activating VirtualEnv (venv) virtual environment: gdrive_venv"
source "$GDRIVE_API_VENV""/bin/activate"
echo "Done."

# used for debugging
#MODE=0
#MODE_CRE=1

#if [[ "$MODE" -eq 0 ]];
#then

echo "Fetching-Downloading ""$DEXTER_CSV_FILENAME_OUT"".."
python3 dexter.py "$DEXTER_CSV_FILENAME_OUT" "$TOKEN_FILE"
echo "Done. Checking for correctness.."

#exit

if [[ $(cat "$DEXTER_CSV_FILENAME_OUT" | wc -l) -gt 0 ]];
then
	if [[ "$MODE_IN" -eq 1 ]];
	then
		echo "$DEXTER_LINE_SEP"
		echo "CREATION PART: Drive->IDM->(""$DB_LOC_EXT"")"
		echo "$DEXTER_LINE_SEP"
		echo "Done. Pre-processing it as ""$DEXTER_CSV_FILENAME_OUT_IN_ROTATED"".."
		echo "$AUP_HEADER" > "$DEXTER_CSV_FILENAME_OUT_IN_ROTATED"
	
		for line in $(cat "$DEXTER_CSV_FILENAME_OUT" | grep "$FIELDS" -A"$MAX_USERS_IN" | tail -n+2 | tr -s ' ' '#');
		do	
			if [[ ! -z $(echo $line | grep '"') ]];
			then
				this_groups=$(echo $line | cut -d'"' -f2 | cut -d'"' -f1);
				mach=$(echo $this_groups | tr ',' '\n' | grep -- "-users" | cut -d'-' -f1);
				echo "$(echo $line | cut -d',' -f1-5 --output-delimiter=';')"";""$this_groups"";""$(echo $line | cut -d'"' -f3 | cut -d',' -f3,4,7 --output-delimiter=';')"";""$mach"";""$( if [[ "$(echo $line | cut -d'"' -f3 | cut -d',' -f6)" == "YES" ]]; then echo "1"; else echo "0"; fi)"
			else
				this_group=$(echo $line | cut -d',' -f6);
				mach=$(echo $this_groups | tr ',' '\n' | grep -- "-users" | cut -d'-' -f1);
				echo "$(echo $line | cut -d',' -f1-6,8,9,12 --output-delimiter=';')"";""$mach"";""$(if [[ "$(echo $line | cut -d',' -f11)" == "YES" ]]; then echo "1"; else echo "0"; fi)"
			fi
		done | tr '#' ' ' >> "$DEXTER_CSV_FILENAME_OUT_IN_ROTATED"

		#for line in $(cat "$DEXTER_CSV_FILENAME_OUT" | grep ',,,,,,,,,,,,,' -A"$MAX_USERS_IN" | tail -n+2); do this_groups=$(echo $line | cut -d'"' -f2 | cut -d'"' -f1); mach=$(echo $this_groups | tr ',' '\n' | grep -- "-users" | cut -d'-' -f1); echo "$(echo $line | cut -d',' -f1,2,3,4,5 --output-delimiter=';')"";""$(echo $this_groups)"";""$(echo $line | cut -d'"' -f3 | cut -d',' -f3,4,7 --output-delimiter=';')"";""$mach"";""$( if [[ ! -z $(echo $line | grep ',YES,') ]]; then echo 1; else 0; fi)"; done >> "$DEXTER_CSV_FILENAME_OUT_IN_ROTATED"
		echo -e "" >> $DEXTER_CSV_FILENAME_OUT_IN_ROTATED
	
		echo "Done. Checking for correctness.."
		if [[ $(cat "$DEXTER_CSV_FILENAME_OUT_IN_ROTATED" | wc -l) -gt 1 ]];
		then
			echo "Done. Transferring ""$DEXTER_CSV_FILENAME_OUT_IN_ROTATED"".."
			scp "$DEXTER_CSV_FILENAME_OUT_IN_ROTATED" "root@""$MASTER_HOST":"$MASTER_AUP_PATH"
			echo "Done. Users creation.."
			ssh -l root "$MASTER_HOST" "cd $MASTER_AUP_PATH ; ./$REMOTE_AUP_COMMAND $DEXTER_CSV_FILENAME_OUT_IN_ROTATED $TOKEN_FILE"
			echo "Done. Removing temporary file.."
			rm -f "$DEXTER_CSV_FILENAME_OUT_IN_ROTATED"
		else
			echo "Invalid file." >&2
		fi
	fi
	if [[ "$MODE_UP" -eq 1 ]];
	then
		echo "$DEXTER_LINE_SEP"
		echo "UPDATE PART: Drive->(""$DB_LOC_EXT"")"  
		echo "$DEXTER_LINE_SEP"

		echo "Pre-processing it as ""$DEXTER_CSV_FILENAME_OUT_UP_ROTATED"".."
		echo "$AUP_HEADER" > "$DEXTER_CSV_FILENAME_OUT_UP_ROTATED"
	

		for line in $(cat "$DEXTER_CSV_FILENAME_OUT" | grep "$FIELDS" -B"$MAX_USERS_UP" | tail -n+2 | head -n-1 | tr -s ' ' '#');
		do
			if [[ ! -z $(echo $line | grep -- '"') ]];
			then
				this_groups=$(echo $line | cut -d'"' -f2);
				mach=$(echo $this_groups | tr ',' '\n' | grep -- "-users" | cut -d'-' -f1);
				echo "$(echo $line | cut -d',' -f1-5 --output-delimiter=';')"";""$this_groups"";""$(echo $line | cut -d'"' -f3 | cut -d',' -f2-8 --output-delimiter=';')"";""$(if [[ $(echo $line | cut -d'"' -f3 | cut -d',' -f9 | tr -d '\r') == "open" ]]; then echo "0"; else echo "1"; fi)"";password;""$mach"
			else
				this_group=$(echo $line | cut -d',' -f6);
				mach=$(echo $this_groups | tr ',' '\n' | grep -- "-users" | cut -d'-' -f1);
				echo "$(echo $line | cut -d',' -f1-13 --output-delimiter=';')"";""$(if [[ $(echo $line | cut -d',' -f14 | tr -d '\r') == "open" ]]; then echo "0"; else echo "1"; fi)"";password;""$mach"";";
			fi	
		done | tr '#' ' ' >> "$DEXTER_CSV_FILENAME_OUT_UP_ROTATED" #sed ':a;N;$!ba;s/;\n/\n/g' >> "$DEXTER_CSV_FILENAME_OUT_UP_ROTATED"
	
		echo "Done. Checking for correctness.."
		if [[ $(cat "$DEXTER_CSV_FILENAME_OUT_UP_ROTATED" | wc -l) -gt 1 ]];
		then
			echo "Done. Launching idmtodb.."
			../idmtodb/idmtodb_launcher_users.sh "$IDMTODB_PROMPT_ON_INSERT" "$IDMTODB_PROMPT_ON_UPDATE" "$IDMTODB_PROMPT_ON_DELETE" "$IDMTODB_IGNORE_GROUPS" "$IDMTODB_IGNORE_DIVISION_GROUP_NAME" "$IDMTODB_MAX_USERS" "$DEXTER_CSV_FILENAME_OUT_UP_ROTATED"
			echo "Done. Removing temporary file.."
			rm -f "$DEXTER_CSV_FILENAME_OUT_UP_ROTATED"
		else
			echo "Invalid file." >&2
		fi
	fi
	
	if [[ "$PRESERVE_WIP_CHANGES" -ne 1 ]];
	then
		echo "Rotating ""$DEXTER_CSV_FILENAME_OUT"
		mkdir -p "snapshots/drive_snapshots"
		mv "$DEXTER_CSV_FILENAME_OUT" "snapshots/drive_snapshots/"$(echo "$DEXTER_CSV_FILENAME_OUT" | cut -d'.' -f1)"_""$(date '+%Y%m%d')"".csv"
	fi
else
	echo "Invalid file." >&2
fi



#echo "Deactivating environment.."
#deactivate
#echo "Done."


#exit
# to create DEXTER snapshots
#mkdir -p "snapshots/db_snapshots"

if [[ "$MODE_OUT" -eq 1 ]];
then
	echo "Fetching $USERSIDM_DATABASE table from ""$DB_LOC"".."
	mysql "$USERSIDM_DATABASE" --raw -h "$USERSIDM_SERVER" -u "$USERSIDM_USER" -p"$USERSIDM_PASSWORD" <<<$(echo "CALL select_all_users_3()") | tr '\t' ';' > "$DEXTER_CSV_FILENAME"
	echo "Done. Checking for correctness."

	if [[ $(cat "$DEXTER_CSV_FILENAME" | wc -l) -gt 0 ]];
	then
		echo "Done. Uploading ""$DEXTER_CSV_FILENAME"" CSV file to Google Drive ""$DRIVE_CMCC_SCC_FOLDER_NAME"" CMCC-SCC shared folder.."
		
		if [[ "$PRESERVE_WIP_CHANGES" -eq 1 ]];
		then
			echo "Gathering WIP (work-in-progress) changes"
			#python3 dexter.py "$DEXTER_CSV_FILENAME_OUT_TMP" "$TOKEN_FILE" #already downloaded, so commented
			if [[ $(cat "$DEXTER_CSV_FILENAME_OUT_TMP" | wc -l) -gt 0 ]];
			then
				echo "$FIELDS_OUT" >> "$DEXTER_CSV_FILENAME"
				for line in $(cat "$DEXTER_CSV_FILENAME_OUT_TMP" | grep "$FIELDS" -A"$MAX_USERS_IN" | tail -n+2 | tr -s ' ' '#');
				do
					this_groups=$(echo $line | cut -d'"' -f2);
					echo "$(echo $line | cut -d',' -f1-5 --output-delimiter=';')"";""$this_groups"";""$(echo $line | cut -d'"' -f3 | cut -d',' -f2- --output-delimiter=';')"
				done | tr -s '#' ' ' >> "$DEXTER_CSV_FILENAME"
				
				#rm -f "$DEXTER_CSV_FILENAME_OUT_TMP"
			else
				echo "Invalid file." >&2
			fi

			echo "Rotating ""$DEXTER_CSV_FILENAME_OUT"
                	mkdir -p "snapshots/drive_snapshots"
                	mv "$DEXTER_CSV_FILENAME_OUT" "snapshots/drive_snapshots/"$(echo "$DEXTER_CSV_FILENAME_OUT" | cut -d'.' -f1)"_""$(date '+%Y%m%d')"".csv"
		fi

		echo "Done. Checking for correctness.."
		python3 dexter.py "$DEXTER_CSV_FILENAME" "$TOKEN_FILE" | grep "dexter_idm_fileid" > dexter_idm_fileid.py.0
		mv dexter_idm_fileid.py.0 dexter_idm_fileid.py
		echo "Done. Rotating ""$DEXTER_CSV_FILENAME"".."
		mkdir -p "snapshots/db_snapshots"
		mv "$DEXTER_CSV_FILENAME" "snapshots/db_snapshots/"$(echo "$DEXTER_CSV_FILENAME" | cut -d'.' -f1)"_""$(date '+%Y%m%d')"".csv"
	else
		echo "Invalid file."
	fi
fi

echo "Deactivating environment.."
deactivate
echo "Done."
