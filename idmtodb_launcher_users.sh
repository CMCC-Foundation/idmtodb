#####################################################################
#    Identity Management to Relational MySQL DB (IDM-TO-DB) v1.0    #
#####################################################################
# ██╗██████╗ ███╗   ███╗   ████████╗ ██████╗       ██████╗ ██████╗  #
# ██║██╔══██╗████╗ ████║   ╚══██╔══╝██╔═══██╗      ██╔══██╗██╔══██╗ #
# ██║██║  ██║██╔████╔██║█████╗██║   ██║   ██║█████╗██║  ██║██████╔╝ #
# ██║██║  ██║██║╚██╔╝██║╚════╝██║   ██║   ██║╚════╝██║  ██║██╔══██╗ #
# ██║██████╔╝██║ ╚═╝ ██║      ██║   ╚██████╔╝      ██████╔╝██████╔╝ #
# ╚═╝╚═════╝ ╚═╝     ╚═╝      ╚═╝    ╚═════╝       ╚═════╝ ╚═════╝  #
#####################################################################
 # License: AGPL v3.
 # Giuseppe Calò                giuseppe.calo@cmcc.it
 # Danilo Mazzarella            danilo.mazzarella@cmcc.it 
 # Marco Chiarelli              marco.chiarelli@cmcc.it
 #                              marco_chiarelli@yahoo.it

#!/bin/bash

PROMPT_ON_INSERT=${1:-"1"}
PROMPT_ON_UPDATE=${2:-"1"}
PROMPT_ON_DELETE=${3:-"2"} #${3:-"1"} # insert a number > 1 (i.e. 2) in order to permanently disable delete synchronization
IGNORE_GROUPS=${4:-"1"}
IGNORE_DIVISION_GROUP_NAME=${5:-"1"}
MAX_USERS=${6:-"1000"}
INPUT_FILE=${7:-"user_idm_20231012.csv"} #"opa_user_20230707_stage"} #${5:-"user_idm_20230603.csv"} #${5:-"user_idm_20230515.csv"} #${4:-"user_idm_$(date '+%Y-%m-%d').csv"} #"user_idm_02052023.csv"
USERSIDM_SERVER=${8:-"127.0.0.1"}
USERSIDM_USER=${9:-"root"}
USERSIDM_PASSWORD="root" #"root"
USERSIDM_DATABASE="idmdb" #"idmdb_wip"
MAIL_CMD="sendmail"
MAIL_FROM="scc-noreply@cmcc.it"
MAIL_TO="marco_chiarelli@yahoo.it"

./idmtodb_users $PROMPT_ON_INSERT $PROMPT_ON_UPDATE $PROMPT_ON_DELETE $IGNORE_GROUPS $IGNORE_DIVISION_GROUP_NAME $MAX_USERS "$INPUT_FILE" "$USERSIDM_SERVER" "$USERSIDM_USER" "$USERSIDM_PASSWORD" "$USERSIDM_DATABASE" "$MAIL_CMD" "$MAIL_FROM" "$MAIL_TO"
