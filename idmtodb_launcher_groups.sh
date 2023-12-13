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
MAX_GROUPS=${4:-"128"}
INPUT_FILE=${5:-"user_idm_20231012.csv"} #"opa_user_20230707_stage"} #${5:-"user_idm_20230603.csv"} #${5:-"user_idm_20230515.csv"} #${4:-"user_idm_$(date '+%Y-%m-%d').csv"} #"user_idm_02052023.csv"
GROUPSIDM_SERVER=${6:-"127.0.0.1"}
GROUPSIDM_USER=${7:-"root"}
GROUPSIDM_PASSWORD="root" #"root"
GROUPSIDM_DATABASE="idmdb" #"idmdb_wip"
MAIL_CMD="sendmail"
MAIL_FROM="scc-noreply@cmcc.it"
MAIL_TO="marco_chiarelli@yahoo.it"

./idmtodb_groups $PROMPT_ON_INSERT $PROMPT_ON_UPDATE $PROMPT_ON_DELETE $MAX_GROUPS "$INPUT_FILE" "$GROUPSIDM_SERVER" "$GROUPSIDM_USER" "$GROUPSIDM_PASSWORD" "$GROUPSIDM_DATABASE" "$MAIL_CMD" "$MAIL_FROM" "$MAIL_TO"
