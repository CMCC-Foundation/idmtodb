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

PROMPT_ON_INSERT=1
PROMPT_ON_UPDATE=1
INPUT_FILE="user_idm_$(date '+%Y-%m-%d').csv" #"user_idm_02052023.csv"
USERSIDM_SERVER=${1:-"127.0.0.1"}
USERSIDM_USER=${2:-"root"}
USERSIDM_PASSWORD="larx141293"
USERSIDM_DATABASE="users_idm"
MAIL_CMD="sendmail"
MAIL_FROM="scc-noreply@cmcc.it"
MAIL_TO="marco_chiarelli@yahoo.it"

./idmtodb $PROMPT_ON_INSERT $PROMPT_ON_UPDATE "$INPUT_FILE" "$USERSIDM_SERVER" "$USERSIDM_USER" "$USERSIDM_PASSWORD" "$USERSIDM_DATABASE" "$MAIL_CMD" "$MAIL_FROM" "$MAIL_TO"
