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
MAX_USERS=${5:-"1000"}
INPUT_FILE=${6:-"user_idm_20231012.csv"}
USERSIDM_SERVER=${7:-"127.0.0.1"}
USERSIDM_USER=${8:-"root"}
USERSIDM_PASSWORD="root" #"root"
USERSIDM_DATABASE="users_idm" #"idmdb_wip"
MAIL_CMD="sendmail"
MAIL_FROM="scc-noreply@cmcc.it"
MAIL_TO="marco_chiarelli@yahoo.it"

./idmtodb $PROMPT_ON_INSERT $PROMPT_ON_UPDATE $PROMPT_ON_DELETE $IGNORE_GROUPS $MAX_USERS "$INPUT_FILE" "$USERSIDM_SERVER" "$USERSIDM_USER" "$USERSIDM_PASSWORD" "$USERSIDM_DATABASE" "$MAIL_CMD" "$MAIL_FROM" "$MAIL_TO"
