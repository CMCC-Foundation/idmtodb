/*********************************************************************
 *    Identity Management to Relational MySQL DB (IDM-TO-DB) v1.0    *
 *********************************************************************
 * ██╗██████╗ ███╗   ███╗   ████████╗ ██████╗       ██████╗ ██████╗  *
 * ██║██╔══██╗████╗ ████║   ╚══██╔══╝██╔═══██╗      ██╔══██╗██╔══██╗ *
 * ██║██║  ██║██╔████╔██║█████╗██║   ██║   ██║█████╗██║  ██║██████╔╝ *
 * ██║██║  ██║██║╚██╔╝██║╚════╝██║   ██║   ██║╚════╝██║  ██║██╔══██╗ *
 * ██║██████╔╝██║ ╚═╝ ██║      ██║   ╚██████╔╝      ██████╔╝██████╔╝ *
 * ╚═╝╚═════╝ ╚═╝     ╚═╝      ╚═╝    ╚═════╝       ╚═════╝ ╚═════╝  *
 *********************************************************************
 * License: AGPL v3.
 * Giuseppe Calò                giuseppe.calo@cmcc.it
 * Danilo Mazzarella            danilo.mazzarella@cmcc.it 
 * Marco Chiarelli              marco.chiarelli@cmcc.it
 *                              marco_chiarelli@yahoo.it
*/

#include <mysql.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#define STRINGIFY(s) _STRINGIFY(s)
#define _STRINGIFY(s) #s

#define PADDING "                                                     " // "#####################################################"
#define BORDER  "-----------------------------------------------------" 

#define DEBUG_MODE
#define INSERT_USER "CALL insert_user(?,?,?,?,?,?,?,?,?,?,?)"
#define UPDATE_USER "CALL update_user(?,?,?,?,?,?,?,?,?,?,?,?)"
#define SELECT_ALL_USERS "CALL select_all_users( )"

typedef bool my_bool;

enum
{
    P_IN_USERNAME,
    P_IN_NAME,
    P_IN_SURNAME,
    P_IN_UID,
    P_IN_GID,
    P_IN_GROUPNAME,
    P_IN_DIVISION,
    P_IN_CREATION_DATE,
    P_IN_EXPIRATION_DATE,
    P_IN_VPN_EXPIRATION_DATE,
    P_IN_EMAIL,
    MAX_IN_USER_IDM_PARAMS
} insert_in_user_idm_enum;

#define MAX_INSERT_USER_PARAMS MAX_IN_USER_IDM_PARAMS

enum
{
    P_INOUT_IDUSER_IDM,
    P_INOUT_USERNAME,
    P_INOUT_NAME,
    P_INOUT_SURNAME,
    P_INOUT_UID,
    P_INOUT_GID,
    P_INOUT_GROUPNAME,
    P_INOUT_DIVISION,
    P_INOUT_CREATION_DATE,
    P_INOUT_EXPIRATION_DATE,
    P_INOUT_VPN_EXPIRATION_DATE,
    P_INOUT_EMAIL,
    MAX_INOUT_USER_IDM_PARAMS
} insert_up_user_idm_enum;


#define MAX_UPDATE_USER_PARAMS MAX_INOUT_USER_IDM_PARAMS
#define MAX_SELECT_ALL_USERS_PARAMS MAX_INOUT_USER_IDM_PARAMS

#define DEFAULT_PROMPT_ON_INSERT 1
#define DEFAULT_PROMPT_ON_UPDATE 1

#define DEFAULT_MAIL_COMMAND "sendmail"
#define DEFAULT_FROM_MAIL "idmtodb"
#define MAX_MAILCMDTO_LEN 128
#define MAX_LOGBUF_LEN 130000
#define MAX_MAIL_BUF 160000

#define USERSIDM_SERVER "127.0.0.1"
#define USERSIDM_USER "root"
#define USERSIDM_PASSWORD "root"
#define USERSIDM_DATABASE "database"

#define DEFAULT_INPUT_FILE "user_idm.csv"

#define MAX_PASSWORD_LEN 512

#define MAX_USERNAME_LEN 16
#define MAX_NAME_LEN 32
#define MAX_SURNAME_LEN 32
#define MAX_GROUP_NAME_LEN 32
#define MAX_DIVISION_LEN 32
#define MAX_DATE_LEN 100
#define MAX_EMAIL_LEN 64

#define _MAX_NAME_LEN 16
#define _MAX_SURNAME_LEN 16
#define _MAX_GROUP_NAME_LEN 16
#define _MAX_DIVISION_LEN 16
#define _MAX_DATE_LEN 16
#define _MAX_EMAIL_LEN 16

#define MAX_LINE_LEN 2*(MAX_USERNAME_LEN+MAX_NAME_LEN+MAX_SURNAME_LEN+MAX_GROUP_NAME_LEN+MAX_DIVISION_LEN+MAX_DATE_LEN+MAX_EMAIL_LEN+6) // added 2* for safety
#define MAX_BUFLINE_LEN MAX_LINE_LEN
#define NUMBERS_FIXED_LEN 16 // 21 // 16

#define MAX_USERS 1000

#define NULL_IDENTIFIER "None" // python-like, "NULL" is C-like, but it doesn't preserve consistency between Python-generated .csv input_file and user_idm DB's table.
#define INSERT_CODE -99
#define STAY_CODE -98

typedef struct _idm_user_t
{
        int iduser_idm; // 
        char username[MAX_USERNAME_LEN];
        char name[MAX_NAME_LEN];
        char surname[MAX_SURNAME_LEN];
        int uid;
        int gid;
        char group_name[MAX_GROUP_NAME_LEN];
        char division[MAX_DIVISION_LEN];
        char creation_date[MAX_DATE_LEN];
        char expiration_date[MAX_DATE_LEN];
        char vpn_expiration_date[MAX_DATE_LEN];
        char email[MAX_EMAIL_LEN];
        
} idm_user_t;

/*
typedef struct _id_idm_user_t
{
        int iduser_idm;
        idm_user_t idm_user;
} id_idm_user_t;
*/

static void sendmail(const char * from_mail, const char * to_mail, const char * mail_cmd, const char * message)
{
	FILE * pp;
	static char buf[MAX_MAIL_BUF]; 

	sprintf(buf, "Subject: IDM-TO-DB Report - Identity Management to Relational MySQL DB\n"
		     "From: %s\n"
		     "To: %s\n"
		     "Mime-Version: 1.0\n"
		     "Content-Type: multipart/related; boundary=\"boundary-example\"; type=\"text/html\"\n"
		     "\n"
		     "--boundary-example\n"
		     "Content-Type: text/html; charset=ISO-8859-15\n"
		     "Content-Transfer-Encoding: 7bit\n"
		     "\n"
		     "<html>"
		     "<head>"
		     "<meta http-equiv=\"content-type\" content=\"text/html; charset=ISO-8859-15\">"
		     "</head>"
		     "<body bgcolor=\"#ffffff\" text=\"#000000\">"
		     "<font face=\"Courier New\">%s</font><br><br><br><br><br>" 
                     "Best Regards,<br>Marco Chiarelli, Danilo Mazzarella.<br><br>"
		     "</body>\n"
		     "</html>\n"	
		    "--boundary-example\n\n", from_mail, to_mail, message);

	if((pp = popen(mail_cmd, "w")) == NULL)
        {
                // fencing_fault = LFR_IO;
                fprintf(stderr, "Cannot send mail through the %s command.\n", mail_cmd);
		return;
	}

	// fwrite(message, 1, strlen(message)+1, pp);
	fprintf(pp, buf); // message);
	pclose(pp);
	return;
}

int main(int argc, char *argv[])
{
        const unsigned char prompt_on_insert = argc > 1 ? atoi(argv[1]): DEFAULT_PROMPT_ON_INSERT;
        const unsigned char prompt_on_update = argc > 2 ? atoi(argv[2]): DEFAULT_PROMPT_ON_UPDATE;
        const int max_users = argc > 3 ? atoi(argv[3]) : MAX_USERS;
        const char * input_file = argc > 4 ? argv[4] : DEFAULT_INPUT_FILE;
        const char * server = argc > 5 ? argv[5] : USERSIDM_SERVER;
        const char * user = argc > 6 ? argv[6] : USERSIDM_USER;
        const char * password = argc > 7 ? argv[7] : USERSIDM_PASSWORD;
        const char * database = argc > 8 ? argv[8] : USERSIDM_DATABASE;
        const char * mail_cmd = argc > 9 ? argv[9] : DEFAULT_MAIL_COMMAND;
        const char * from_mail = argc > 10 ? argv[10] : DEFAULT_FROM_MAIL;
        const char * to_mail = argc > 11 ? argv[11] : NULL;

        // char * token;

        #ifdef DEBUG_MODE
        char psw_asterisks[MAX_PASSWORD_LEN];
        char *token_aux = psw_asterisks;
        for(const char * token = password; *token != '\0'; ++token, ++token_aux)
            *token_aux = '*';
        *token_aux = '\0';

        printf("prompt_on_insert: %u\nprompt_on_update: %u\nmax_users: %d\ninput_file: %s\nserver: %s\nuser: %s\npassword: %s\ndatabase: %s\nmail_cmd: %s\nfrom_mail: %s\nto_mail: %s\n",
                prompt_on_insert, prompt_on_update, max_users, input_file, server, user, psw_asterisks, database, mail_cmd, from_mail, to_mail);
        // exit(0);
        #endif

        char mail_cmd_to[MAX_MAILCMDTO_LEN];

        if(to_mail)
            sprintf(mail_cmd_to, "%s \"%s\"", mail_cmd, to_mail);
            
        #define is_mail_active (from_mail && to_mail)

        FILE * fp;
        int line_num;

        MYSQL *conn;
        MYSQL_RES *res;
        MYSQL_ROW row;

        // char mail_cmd_to[MAX_MAILCMDTO_LEN];

        conn = mysql_init(NULL);

        //apro la connessione al DB
        if (!mysql_real_connect(conn, server, user, password, database, 0, NULL, 0))
        {
                fprintf(stderr, "Error on mysql_real_connect!\n");
                exit(1);
        }

        MYSQL_STMT * insert_stmt;
        MYSQL_STMT * update_stmt;
        MYSQL_STMT * select_stmt;
        MYSQL_RES  *prepare_meta_result;
        MYSQL_BIND ps_insert_user_params[MAX_INSERT_USER_PARAMS];
        MYSQL_BIND ps_update_user_params[MAX_UPDATE_USER_PARAMS];
        // MYSQL_BIND out_params[MAX_INSERT_USER_PARAMS];

        printf("connected to database!\n");   


        /* initialize and prepare CALL statement with parameter placeholders */
        #ifdef DEBUG_MODE
        printf("\nBefore mysql_stmt_init()\n");
        #endif
        
        insert_stmt = mysql_stmt_init(conn);

        if (!insert_stmt)
        {
                printf("\nerror when init insert_stmt statement\n");
                mysql_close(conn);
                exit(1);
        }
        
        update_stmt = mysql_stmt_init(conn);

        if (!update_stmt)
        {
                printf("\nerror when init update_stmt statement\n");
                mysql_close(conn);
                exit(1);
        }
        
        select_stmt = mysql_stmt_init(conn);

        if (!select_stmt)
        {
                printf("\nerror when init select_stmt statement\n");
                mysql_close(conn);
                exit(1);
        }

        #ifdef DEBUG_MODE
        printf("After mysql_stmt_init() and before mysql_stmt_prepare\n");
        #endif

        int status = mysql_stmt_prepare(insert_stmt, INSERT_USER, strlen(INSERT_USER));

        #ifdef DEBUG_MODE
        printf("\nstatus of insert_stmt prepare: %d\n", status);
        #endif

        status = mysql_stmt_prepare(update_stmt, UPDATE_USER, strlen(UPDATE_USER));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_stmt prepare: %d\n", status);
        #endif
        
        status = mysql_stmt_prepare(select_stmt, SELECT_ALL_USERS, strlen(SELECT_ALL_USERS));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of select_stmt prepare: %d\n", status);
        printf("After mysql_stmt_prepare and before memset\n");
        #endif

        if ((status = mysql_stmt_execute(select_stmt)))
        {
                fprintf(stderr, "execution of statement failed\n");
                exit(1);
        }

        #ifdef DEBUG_MODE
        int num_fields = mysql_stmt_field_count(select_stmt);
        printf("select_stmt num_fields: %d\n", num_fields);
        #endif

        /* initialize parameters: p_in, p_out, p_inout (all INT) */
        memset(ps_insert_user_params, 0, sizeof(ps_insert_user_params));
        memset(ps_update_user_params, 0, sizeof(ps_update_user_params));
        // memset(out_params, 0, sizeof(out_params));

        #ifdef DEBUG_MODE
        printf("\n\n *** AFTER MEMSET AND BEFORE ALL PS_PARAMS STUFFS *** \n\n");
        #endif
        
        unsigned long ul_zero_value = 0;
        
        // iduser_idm
        int p_up_iduser_idm = 0;
        ps_update_user_params[P_INOUT_IDUSER_IDM].buffer_type = MYSQL_TYPE_LONG;
        ps_update_user_params[P_INOUT_IDUSER_IDM].buffer = (int *) &p_up_iduser_idm;
        ps_update_user_params[P_INOUT_IDUSER_IDM].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_IDUSER_IDM].is_null = 0;
        
        
        // username
        unsigned long p_in_username_length = 0;
        unsigned long p_up_username_length = 0;
        // unsigned long p_out_username_length = 0;
        char p_in_username[MAX_USERNAME_LEN];
        char p_up_username[MAX_USERNAME_LEN]; 
        // char p_out_username[MAX_USERNAME_LEN]; 
        ps_insert_user_params[P_IN_USERNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_USERNAME].buffer = (char *) p_in_username; 
        ps_insert_user_params[P_IN_USERNAME].buffer_length = MAX_USERNAME_LEN;
        ps_insert_user_params[P_IN_USERNAME].is_null = 0;
        ps_insert_user_params[P_IN_USERNAME].length = &p_in_username_length;
        
        ps_update_user_params[P_INOUT_USERNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_USERNAME].buffer = (char *) p_up_username; 
        ps_update_user_params[P_INOUT_USERNAME].buffer_length = MAX_USERNAME_LEN;
        ps_update_user_params[P_INOUT_USERNAME].is_null = 0;
        ps_update_user_params[P_INOUT_USERNAME].length = &p_up_username_length;
        
        
        // name
        unsigned long p_in_name_length = 0;
        unsigned long p_up_name_length = 0;
        char p_in_name[MAX_NAME_LEN];
        char p_up_name[MAX_NAME_LEN];
        ps_insert_user_params[P_IN_NAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_NAME].buffer = (char *) p_in_name; 
        ps_insert_user_params[P_IN_NAME].buffer_length = MAX_NAME_LEN;
        ps_insert_user_params[P_IN_NAME].is_null = 0;
        ps_insert_user_params[P_IN_NAME].length = &p_in_name_length;
        
        ps_update_user_params[P_INOUT_NAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_NAME].buffer = (char *) p_up_name; 
        ps_update_user_params[P_INOUT_NAME].buffer_length = MAX_NAME_LEN;
        ps_update_user_params[P_INOUT_NAME].is_null = 0;
        ps_update_user_params[P_INOUT_NAME].length = &p_up_name_length;
        
        // surname
        unsigned long p_in_surname_length = 0;
        unsigned long p_up_surname_length = 0;
        char p_in_surname[MAX_SURNAME_LEN];
        char p_up_surname[MAX_SURNAME_LEN];
        ps_insert_user_params[P_IN_SURNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_SURNAME].buffer = (char *) p_in_surname; 
        ps_insert_user_params[P_IN_SURNAME].buffer_length = MAX_SURNAME_LEN;
        ps_insert_user_params[P_IN_SURNAME].is_null = 0;
        ps_insert_user_params[P_IN_SURNAME].length = &p_in_surname_length;
        
        ps_update_user_params[P_INOUT_SURNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_SURNAME].buffer = (char *) p_up_surname; 
        ps_update_user_params[P_INOUT_SURNAME].buffer_length = MAX_SURNAME_LEN;
        ps_update_user_params[P_INOUT_SURNAME].is_null = 0;
        ps_update_user_params[P_INOUT_SURNAME].length = &p_up_surname_length;
        
        // uid
        int p_in_uid = 0;
        int p_up_uid = 0;
        ps_insert_user_params[P_IN_UID].buffer_type = MYSQL_TYPE_LONG;
        ps_insert_user_params[P_IN_UID].buffer = (int *) &p_in_uid;
        ps_insert_user_params[P_IN_UID].length = &ul_zero_value;
        ps_insert_user_params[P_IN_UID].is_null = 0;
        
        ps_update_user_params[P_INOUT_UID].buffer_type = MYSQL_TYPE_LONG;
        ps_update_user_params[P_INOUT_UID].buffer = (int *) &p_up_uid;
        ps_update_user_params[P_INOUT_UID].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_UID].is_null = 0;
        
        // gid
        int p_in_gid = 0;
        int p_up_gid = 0;
        ps_insert_user_params[P_IN_GID].buffer_type = MYSQL_TYPE_LONG;
        ps_insert_user_params[P_IN_GID].buffer = (int *) &p_in_gid;
        ps_insert_user_params[P_IN_GID].length = &ul_zero_value;
        ps_insert_user_params[P_IN_GID].is_null = 0;
        
        ps_update_user_params[P_INOUT_GID].buffer_type = MYSQL_TYPE_LONG;
        ps_update_user_params[P_INOUT_GID].buffer = (int *) &p_up_gid;
        ps_update_user_params[P_INOUT_GID].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_GID].is_null = 0;
        
        // group_name
        unsigned long p_in_group_name_length = 0;
        unsigned long p_up_group_name_length = 0;
        char p_in_group_name[MAX_GROUP_NAME_LEN];
        char p_up_group_name[MAX_GROUP_NAME_LEN];
        ps_insert_user_params[P_IN_GROUPNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_GROUPNAME].buffer = (char *) p_in_group_name; 
        ps_insert_user_params[P_IN_GROUPNAME].buffer_length = MAX_GROUP_NAME_LEN;
        ps_insert_user_params[P_IN_GROUPNAME].is_null = 0;
        ps_insert_user_params[P_IN_GROUPNAME].length = &p_in_group_name_length;
        
        ps_update_user_params[P_INOUT_GROUPNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_GROUPNAME].buffer = (char *) p_up_group_name; 
        ps_update_user_params[P_INOUT_GROUPNAME].buffer_length = MAX_GROUP_NAME_LEN;
        ps_update_user_params[P_INOUT_GROUPNAME].is_null = 0;
        ps_update_user_params[P_INOUT_GROUPNAME].length = &p_up_group_name_length;
        
        // division
        unsigned long p_in_division_length = 0;
        unsigned long p_up_division_length = 0;
        char p_in_division[MAX_DIVISION_LEN];
        char p_up_division[MAX_DIVISION_LEN];
        ps_insert_user_params[P_IN_DIVISION].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_DIVISION].buffer = (char *) p_in_division; 
        ps_insert_user_params[P_IN_DIVISION].buffer_length = MAX_DIVISION_LEN;
        ps_insert_user_params[P_IN_DIVISION].is_null = 0;
        ps_insert_user_params[P_IN_DIVISION].length = &p_in_division_length;
        
        ps_update_user_params[P_INOUT_DIVISION].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_DIVISION].buffer = (char *) p_up_division; 
        ps_update_user_params[P_INOUT_DIVISION].buffer_length = MAX_DIVISION_LEN;
        ps_update_user_params[P_INOUT_DIVISION].is_null = 0;
        ps_update_user_params[P_INOUT_DIVISION].length = &p_up_division_length;
        
        // creation_date
        unsigned long p_in_creation_date_length = 0;
        unsigned long p_up_creation_date_length = 0;
        char p_in_creation_date[MAX_DATE_LEN];
        char p_up_creation_date[MAX_DATE_LEN];
        ps_insert_user_params[P_IN_CREATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_CREATION_DATE].buffer = (char *) p_in_creation_date;
        ps_insert_user_params[P_IN_CREATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_user_params[P_IN_CREATION_DATE].length = &p_in_creation_date_length;
        ps_insert_user_params[P_IN_CREATION_DATE].is_null = 0;
        
        ps_update_user_params[P_INOUT_CREATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_CREATION_DATE].buffer = (char *) p_up_creation_date;
        ps_update_user_params[P_INOUT_CREATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_CREATION_DATE].length = &p_up_creation_date_length;
        ps_update_user_params[P_INOUT_CREATION_DATE].is_null = 0;

        // expiration_date
        my_bool is_null_in_expiration_date;
        my_bool is_null_up_expiration_date;
        unsigned long p_in_expiration_date_length = 0;
        unsigned long p_up_expiration_date_length = 0;
        char p_in_expiration_date[MAX_DATE_LEN];
        char p_up_expiration_date[MAX_DATE_LEN];
        ps_insert_user_params[P_IN_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].buffer = (char *) p_in_expiration_date;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].length = &p_in_expiration_date_length;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].is_null = &is_null_in_expiration_date;
        
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].buffer = (char *) p_up_expiration_date;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].length = &p_up_expiration_date_length;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].is_null = &is_null_up_expiration_date;
        
        // vpn_expiration_date
        my_bool is_null_in_vpn_expiration_date;
        my_bool is_null_up_vpn_expiration_date;
        unsigned long p_in_vpn_expiration_date_length = 0;
        unsigned long p_up_vpn_expiration_date_length = 0;
        char p_in_vpn_expiration_date[MAX_DATE_LEN];
        char p_up_vpn_expiration_date[MAX_DATE_LEN];
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].buffer = (char *) p_in_vpn_expiration_date;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].length = &p_in_vpn_expiration_date_length;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].is_null = &is_null_in_vpn_expiration_date;
        
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].buffer = (char *) p_up_vpn_expiration_date;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].length = &p_up_vpn_expiration_date_length;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].is_null = &is_null_up_vpn_expiration_date;
        
        // email
        my_bool is_null_in_email;
        my_bool is_null_up_email;
        unsigned long p_in_email_length = 0;
        unsigned long p_up_email_length = 0;
        char p_in_email[MAX_DIVISION_LEN];
        char p_up_email[MAX_DIVISION_LEN];
        ps_insert_user_params[P_IN_EMAIL].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_EMAIL].buffer = (char *) p_in_email; 
        ps_insert_user_params[P_IN_EMAIL].buffer_length = MAX_EMAIL_LEN;
        ps_insert_user_params[P_IN_EMAIL].is_null = &is_null_in_email;
        ps_insert_user_params[P_IN_EMAIL].length = &p_in_email_length;
        
        ps_update_user_params[P_INOUT_EMAIL].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_EMAIL].buffer = (char *) p_up_email; 
        ps_update_user_params[P_INOUT_EMAIL].buffer_length = MAX_EMAIL_LEN;
        ps_update_user_params[P_INOUT_EMAIL].is_null = &is_null_up_email;
        ps_update_user_params[P_INOUT_EMAIL].length = &p_up_email_length;
        
        #ifdef DEBUG_MODE
        printf("\nbefore mysql_stmt_bind_param.\n");
        #endif
        
        if((status = mysql_stmt_bind_param(insert_stmt, ps_insert_user_params)))
        {
                fprintf(stderr, "insert_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of insert_stmt mysql_stmt_bind_param: %d\n", status);
        #endif
        
        if((status = mysql_stmt_bind_param(update_stmt, ps_update_user_params)))
        {
                fprintf(stderr, "update_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_stmt mysql_stmt_bind_param: %d\n", status);
        #endif
        
        if((status = mysql_stmt_bind_result(select_stmt, ps_update_user_params)))
        {
                fprintf(stderr, "select_stmt mysql_stmt_bind_result failed\n");
                exit(1);
        }
        
        /* Now buffer all results to client (optional step) */
        if (mysql_stmt_store_result(select_stmt))
        {
                fprintf(stderr, " mysql_stmt_store_result() failed\n");
                fprintf(stderr, " %s\n", mysql_stmt_error(select_stmt));
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of select_stmt mysql_stmt_bind_result: %d\n", status);
        #endif
        
        if((fp = fopen(input_file, "r+")) == NULL)
        {
                fprintf(stderr, "Error on opening %s input file.\n", input_file);
                exit(2);
        }
        
        int aux = 0;
        char buf[MAX_LINE_LEN];
        char * token = NULL;
        char * token2 = NULL;
        idm_user_t users[max_users];
        idm_user_t * pnt_user = NULL;
        
        printf("\n");
        
        (void) fgets(buf, MAX_LINE_LEN, fp); // exclude header line
        
        for(line_num=0; fgets(buf, MAX_LINE_LEN, fp); ++line_num)
        {
                pnt_user = &users[line_num];
                
                pnt_user->iduser_idm = STAY_CODE;
                // printf("[%d]: %s", line_num, buf);
                token = buf;
                
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("username is: %s\n", token);
                #endif
                strcpy(pnt_user->username, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("name is: %s\n", token);
                #endif
                strcpy(pnt_user->name, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("surname is: %s\n", token);
                #endif
                strcpy(pnt_user->surname, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                aux = atoi(token);
                #ifdef DEBUG_MODE
                printf("uid is: %d\n", aux);
                #endif
                pnt_user->uid = aux;
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                aux = atoi(token);
                #ifdef DEBUG_MODE
                printf("gid is: %d\n", aux);
                #endif
                pnt_user->gid = aux;
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("group_name is: %s\n", token);
                #endif
                strcpy(pnt_user->group_name, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("division is: %s\n", token);
                #endif
                strcpy(pnt_user->division, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("creation_date is: %s\n", token);
                #endif
                strcpy(pnt_user->creation_date, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("expiration_date is: %s\n", token);
                #endif
                strcpy(pnt_user->expiration_date, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("vpn_expiration_date is: %s\n", token);
                #endif
                strcpy(pnt_user->vpn_expiration_date, token);
                token = token2+1;
                for(token2 = token; *token2 != ',' && *token2 != '\0'; ++token2);
                *(token2-1) = '\0';
                #ifdef DEBUG_MODE
                printf("email is: %s\n\n", token);
                #endif
                strcpy(pnt_user->email, token);
        }
        
        fclose(fp);
        printf("Total line num: %d\n", line_num);
        
        int i, j;
        int rows;
        int result;
        
        idm_user_t users_db[max_users];
        
        for(rows=0; !(result = mysql_stmt_fetch(select_stmt)); ++rows)
        {
                pnt_user = &users_db[rows]; // &(users_db[rows].idm_user);
                
                #ifdef DEBUG_MODE
                printf("PRE %d, %s, %s, %s, %d, %d, %s, %s, %s, %s, %s, %s\n", p_up_iduser_idm, p_up_username, p_up_name, p_up_surname, p_up_uid, p_up_gid, p_up_group_name, p_up_division, p_up_creation_date, is_null_up_expiration_date ? NULL_IDENTIFIER : p_up_expiration_date, is_null_up_vpn_expiration_date ? NULL_IDENTIFIER : p_up_vpn_expiration_date, is_null_up_email ? NULL_IDENTIFIER : p_up_email);
                #endif
               
                pnt_user->iduser_idm = p_up_iduser_idm; // users_db[rows].iduser_idm = p_up_iduser_idm; // pnt_user->iduser_idm = p_up_iduser_idm;
                strcpy(pnt_user->username, p_up_username);
                strcpy(pnt_user->name, p_up_name);
                strcpy(pnt_user->surname, p_up_surname);
                pnt_user->uid = p_up_uid;
                pnt_user->gid = p_up_gid;
                strcpy(pnt_user->group_name, p_up_group_name);
                strcpy(pnt_user->division, p_up_division);
                strcpy(pnt_user->creation_date, p_up_creation_date);
                strcpy(pnt_user->expiration_date, is_null_up_expiration_date ? NULL_IDENTIFIER : p_up_expiration_date);
                strcpy(pnt_user->vpn_expiration_date, is_null_up_vpn_expiration_date ? NULL_IDENTIFIER : p_up_vpn_expiration_date);
                strcpy(pnt_user->email, is_null_up_email ? NULL_IDENTIFIER : p_up_email);
                
                #ifdef DEBUG_MODE
                printf("POST %d, %s, %s, %s, %d, %d, %s, %s, %s, %s, %s, %s\n\n", pnt_user->iduser_idm, pnt_user->username, pnt_user->name, pnt_user->surname, pnt_user->uid, pnt_user->gid, pnt_user->group_name, pnt_user->division, pnt_user->creation_date, pnt_user->expiration_date, pnt_user->vpn_expiration_date, pnt_user->email);
                #endif
                
        }
        
        for(; mysql_next_result(conn) == 0;);
        mysql_stmt_close(select_stmt); // close select_stmt
        
        #ifdef DEBUG_MODE
        printf("mysql_stmt_fetch: %d\n", result);
        #endif
        
        printf("Total rows num: %d\n", rows);
        
        idm_user_t * pnt_user_db = NULL;
        
        /*
        id_idm_user_t users_idm_to_update[max_users];
        idm_user_t users_idm_to_insert[max_users];
        */
        
        // COMPARISON CODE
        printf("line_num: %d\n\n", line_num);
        
        for(i=0; i<line_num; ++i)
        {
                pnt_user = &users[i];
                for(j=0; j<rows; ++j)
                {
                        pnt_user_db = &users_db[j];
                        // printf("idm_uid: %d, db_uid: %d\n", pnt_user->uid, pnt_user_db->uid);
                        if(pnt_user->uid == pnt_user_db->uid)
                        {
                                /*
                                #ifdef DEBUG_MODE
                                printf("Hallelujah\n");
                                #endif
                                */
                                if(strcmp(pnt_user->username, pnt_user_db->username) || strcmp(pnt_user->name, pnt_user_db->name) || strcmp(pnt_user->surname, pnt_user_db->surname)||
                                   pnt_user->gid != pnt_user_db->gid || strcmp(pnt_user->group_name, pnt_user_db->group_name) || strcmp(pnt_user->division, pnt_user_db->division)  ||
                                   strcmp(pnt_user->creation_date, pnt_user_db->creation_date) || strcmp(pnt_user->expiration_date, pnt_user_db->expiration_date) ||
                                   strcmp(pnt_user->vpn_expiration_date, pnt_user_db->vpn_expiration_date) || strcmp(pnt_user->email, pnt_user_db->email))
                                        pnt_user->iduser_idm = pnt_user_db->iduser_idm; // update record on DB with IDM's record values, UPDATE_CODE.
                                break; // do it in any case, if iduser_idm field of pnt_user structure still hasn't been valued from there, however it should have the very initial value, STAY_CODE.
                        }    
                }
                if(j == rows)
                        -- pnt_user->iduser_idm; // from STAY_CODE to INSERT_CODE.
        }
        
        int to_insert = 0;
        const char * padding = PADDING; //  "#####################################################";
        const char * border_padding = BORDER; 
        char p_numbers[2][33]; // for uid and gid, respectively
        char buffer[MAX_BUFLINE_LEN];
        char border_buffer[MAX_BUFLINE_LEN];
        char mail_buffer[MAX_LOGBUF_LEN];
        int padLens[9];
        int numbersPadLens[2];
        
        // char mail_format_buf[_MAX_MAIL_LEN];
        
        sprintf(buffer, "| username%*.*s| name%*.*s| surname%*.*s| uid%*.*s| gid%*.*s| group_name%*.*s| division%*.*s| creation_date%*.*s| exp_date%*.*s| vpn_exp_date%*.*s| email%*.*s|\n", 7, 7, padding, 11, 11, padding, 8, 8, padding, 12, 12, padding, 12, 12, padding, 5, 5, padding, 7, 7, padding, 2, 2, padding, 7, 7, padding, 3, 3, padding, 10, 10, padding);
        sprintf(border_buffer, "+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+\n", MAX_USERNAME_LEN, MAX_USERNAME_LEN, border_padding, _MAX_NAME_LEN, _MAX_NAME_LEN, border_padding, _MAX_SURNAME_LEN, _MAX_SURNAME_LEN, border_padding, NUMBERS_FIXED_LEN, NUMBERS_FIXED_LEN, border_padding, NUMBERS_FIXED_LEN, NUMBERS_FIXED_LEN, border_padding, _MAX_GROUP_NAME_LEN, _MAX_GROUP_NAME_LEN, border_padding, _MAX_DIVISION_LEN, _MAX_DIVISION_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding, _MAX_EMAIL_LEN, _MAX_EMAIL_LEN, border_padding);
        
        printf(border_buffer);
        printf(buffer);
        printf(border_buffer);
        
        sprintf(mail_buffer, "<p style=\"background-color: black;\"><table style=\"background-color: black; color: #adff29;\"><br><tr style=\"color: red; font-weight: bold;\"><td>username</td><td>name</td><td>surname</td><td>uid</td><td>gid</td><td>group_name</td><td>division</td><td>creation_date</td><td>expiration_date</td><td>vpn_expiration_date</td><td>email</td></tr><br>");

        for(i=0; i<line_num; ++i)
        {
                pnt_user = &users[i];
                // printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                if(pnt_user->iduser_idm == INSERT_CODE) // INSERT_CODE
                {
                        ++ to_insert;
                        padLens[0] = MAX_USERNAME_LEN - strlen(pnt_user->username) -1;
                        padLens[1] = _MAX_NAME_LEN - strlen(pnt_user->name) -1;
                        
                        if(padLens[1] <= 0)
                                padLens[1] = 0;
                        
                        padLens[2] = _MAX_SURNAME_LEN - strlen(pnt_user->surname) -1;
                        
                        if(padLens[2] <= 0)
                                padLens[2] = 0;
                        
                        padLens[3] = _MAX_GROUP_NAME_LEN - strlen(pnt_user->group_name) -1;
                        
                        if(padLens[3] <= 0)
                                padLens[3] = 0;
                        
                        padLens[4] = _MAX_DIVISION_LEN - strlen(pnt_user->division) -1;
                        
                        if(padLens[4] <= 0)
                                padLens[4] = 0;
                        
                        padLens[5] = _MAX_DATE_LEN - strlen(pnt_user->creation_date) -1;
                        padLens[6] = _MAX_DATE_LEN - strlen(pnt_user->expiration_date) -1;
                        padLens[7] = _MAX_DATE_LEN - strlen(pnt_user->vpn_expiration_date) -1;

                        padLens[8] = _MAX_EMAIL_LEN - strlen(pnt_user->email) -1;
                        
                        sprintf(p_numbers[0], "%d", pnt_user->uid);
                        numbersPadLens[0] = NUMBERS_FIXED_LEN - strlen(p_numbers[0]);
                        sprintf(p_numbers[1], "%d", pnt_user->gid);
                        numbersPadLens[1] = NUMBERS_FIXED_LEN - strlen(p_numbers[1]);
                        
                        /*
                        #ifdef DEBUG_MODE
                        printf("strlen username: %d, padlens 0: %d\n", strlen(pnt_user->username), padLens[0]);
                        printf("strlen mail: %d, padlens 8: %d\n", mail_len, padLens[8]);
                        #endif
                        */
                                                
                        if(padLens[8] <= 0)
                                padLens[8] = 0;   
                    
                                printf("| %s%*.*s| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s|\n", pnt_user->username, padLens[0], padLens[0], padding, pnt_user->name, padLens[1], padLens[1], padding, pnt_user->surname, padLens[2], padLens[2], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_user->group_name, padLens[3], padLens[3], padding, pnt_user->division, padLens[4], padLens[4], padding, pnt_user->creation_date, padLens[5], padLens[5], padding, pnt_user->expiration_date, padLens[6], padLens[6], padding, pnt_user->vpn_expiration_date, padLens[7], padLens[7], padding, pnt_user->email, padLens[8], padLens[8], padding);
                                
                        sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td></tr>", mail_buffer, pnt_user->username, pnt_user->name, pnt_user->surname, pnt_user->uid, pnt_user->gid, pnt_user->group_name, pnt_user->creation_date, pnt_user->expiration_date, pnt_user->vpn_expiration_date, pnt_user->email);
                        
                } 
        }
        
        printf(border_buffer);
        
        if(to_insert)
        {
                printf("\n\nThe following %d records have to be INSERTED into the %s DB. ", to_insert, database);
                
                if(prompt_on_insert)
                {
                        printf("Would you like to proceed? [Y/n]\n");
                        
                        char ch;
                        
                        // printf("PRESS KEY: %c\n", ch);
                        
                        if((ch = getchar()) == 'n')
                                to_insert = 0;
                                
                        #ifdef DEBUG_MODE 
                        printf("PRESSED KEY: %c\n", ch);
                        #endif
                }
                printf("\n\n");
        }
              
              
        sprintf(mail_buffer, "%s<br></table><br>%d results INSERTED into the %s DB.</p><br><br><br>", mail_buffer, to_insert, database);      
                
        int to_update = 0;
        
        printf(border_buffer);
        printf(buffer);
        printf(border_buffer);
        
        sprintf(mail_buffer, "<p style=\"background-color: black;\"><table style=\"background-color: black; color: #adff29;\"><br><tr style=\"color: red; font-weight: bold;\"><td>username</td><td>name</td><td>surname</td><td>uid</td><td>gid</td><td>group_name</td><td>division</td><td>creation_date</td><td>expiration_date</td><td>vpn_expiration_date</td><td>email</td></tr><br>");
        
        for(i=0; i<line_num; ++i)
        {
                pnt_user = &users[i];
                // printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                if(pnt_user->iduser_idm >= 0) // UPDATE_CODE
                {
                        ++ to_update;
                        padLens[0] = MAX_USERNAME_LEN - strlen(pnt_user->username) -1;
                        padLens[1] = _MAX_NAME_LEN - strlen(pnt_user->name) -1;
                        
                        if(padLens[1] <= 0)
                                padLens[1] = 0;
                        
                        padLens[2] = _MAX_SURNAME_LEN - strlen(pnt_user->surname) -1;
                        
                        if(padLens[2] <= 0)
                                padLens[2] = 0;
                        
                        padLens[3] = _MAX_GROUP_NAME_LEN - strlen(pnt_user->group_name) -1;
                        
                        if(padLens[3] <= 0)
                                padLens[3] = 0;
                        
                        padLens[4] = _MAX_DIVISION_LEN - strlen(pnt_user->division) -1;
                        
                        if(padLens[4] <= 0)
                                padLens[4] = 0;
                        
                        padLens[5] = _MAX_DATE_LEN - strlen(pnt_user->creation_date) -1;
                        padLens[6] = _MAX_DATE_LEN - strlen(pnt_user->expiration_date) -1;
                        padLens[7] = _MAX_DATE_LEN - strlen(pnt_user->vpn_expiration_date) -1;

                        padLens[8] = _MAX_EMAIL_LEN - strlen(pnt_user->email) -1;
                        
                        sprintf(p_numbers[0], "%d", pnt_user->uid);
                        numbersPadLens[0] = NUMBERS_FIXED_LEN - strlen(p_numbers[0]);
                        sprintf(p_numbers[1], "%d", pnt_user->gid);
                        numbersPadLens[1] = NUMBERS_FIXED_LEN - strlen(p_numbers[1]);
                        
                        /*
                        #ifdef DEBUG_MODE
                        printf("strlen username: %d, padlens 0: %d\n", strlen(pnt_user->username), padLens[0]);
                        printf("strlen mail: %d, padlens 8: %d\n", mail_len, padLens[8]);
                        #endif
                        */
                                                
                        if(padLens[8] <= 0)
                                padLens[8] = 0;   
                    
                                printf("| %s%*.*s| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s|\n", pnt_user->username, padLens[0], padLens[0], padding, pnt_user->name, padLens[1], padLens[1], padding, pnt_user->surname, padLens[2], padLens[2], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_user->group_name, padLens[3], padLens[3], padding, pnt_user->division, padLens[4], padLens[4], padding, pnt_user->creation_date, padLens[5], padLens[5], padding, pnt_user->expiration_date, padLens[6], padLens[6], padding, pnt_user->vpn_expiration_date, padLens[7], padLens[7], padding, pnt_user->email, padLens[8], padLens[8], padding);
                                
                        sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td></tr>", mail_buffer, pnt_user->username, pnt_user->name, pnt_user->surname, pnt_user->uid, pnt_user->gid, pnt_user->group_name, pnt_user->creation_date, pnt_user->expiration_date, pnt_user->vpn_expiration_date, pnt_user->email);
                        
                } 
        }
        
        printf(border_buffer);
        
        if(to_update)
        {
                printf("\n\nThe following %d records have to be UPDATED into the %s DB. ", to_update, database);
                
                if(prompt_on_update)
                {
                        printf("Would you like to proceed? [Y/n]\n");
                        
                        char ch;
                        
                        // printf("PRESS KEY: %c\n", ch);
                        
                        (void) getchar();
                        
                        if((ch = getchar()) == 'n')
                                to_update = 0;
                                
                        #ifdef DEBUG_MODE 
                        printf("PRESSED KEY: %c\n", ch);
                        #endif
                }
                printf("\n\n");
        }
        
        sprintf(mail_buffer, "%s<br></table><br>%d results UPDATED into the %s DB.</p><br><br><br>", mail_buffer, to_update, database);    
        
        for(i=0; i<line_num; ++i)
        {
                pnt_user = &users[i];
                if(to_insert && pnt_user->iduser_idm == INSERT_CODE) // INSERT_CODE
                {
                        #ifdef DEBUG_MODE
                        printf("i: %d\n", i);
                        printf("pnt_user->username: %s\n", pnt_user->username);
                        #endif
                        strcpy(p_in_username, pnt_user->username);
                        p_in_username_length = strlen(p_in_username);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->name: %s\n", pnt_user->name);
                        #endif
                        strcpy(p_in_name, pnt_user->name);
                        p_in_name_length = strlen(p_in_name);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->surname: %s\n", pnt_user->surname);
                        #endif
                        strcpy(p_in_surname, pnt_user->surname);
                        p_in_surname_length = strlen(p_in_surname);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->uid: %d\n", pnt_user->uid);
                        #endif
                        p_in_uid = pnt_user->uid;
                        #ifdef DEBUG_MODE
                        printf("pnt_user->gid: %d\n", pnt_user->gid);
                        #endif
                        p_in_gid = pnt_user->gid;
                        #ifdef DEBUG_MODE
                        printf("pnt_user->group_name: %s\n", pnt_user->group_name);
                        #endif
                        strcpy(p_in_group_name, pnt_user->group_name);
                        p_in_group_name_length = strlen(p_in_group_name);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->division: %s\n", pnt_user->division);
                        #endif
                        strcpy(p_in_division, pnt_user->division);
                        p_in_division_length = strlen(p_in_division);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->creation_date: %s\n", pnt_user->creation_date);
                        #endif
                        strcpy(p_in_creation_date, pnt_user->creation_date);
                        p_in_creation_date_length = strlen(p_in_creation_date);
                        
                        if(strcmp(pnt_user->expiration_date, NULL_IDENTIFIER))
                        {
                                
                                is_null_in_expiration_date = 0;
                                #ifdef DEBUG_MODE
                                printf("exp not null\n");
                                printf("pnt_user->expiration_date: %s\n", pnt_user->expiration_date);
                                #endif
                                strcpy(p_in_expiration_date, pnt_user->expiration_date);
                                p_in_expiration_date_length = strlen(p_in_expiration_date);
                        }
                        else
                        {
                                #ifdef DEBUG_MODE
                                printf("exp null\n");
                                #endif
                                is_null_in_expiration_date = 1;
                                p_in_expiration_date_length = 0;
                        }
                        
                        if(strcmp(pnt_user->vpn_expiration_date, NULL_IDENTIFIER))
                        {
                                is_null_in_vpn_expiration_date = 0;
                                #ifdef DEBUG_MODE
                                printf("vpn_exp not null\n");
                                printf("pnt_user->vpn_expiration_date: %s\n", pnt_user->vpn_expiration_date);
                                #endif
                                strcpy(p_in_vpn_expiration_date, pnt_user->vpn_expiration_date);
                                p_in_vpn_expiration_date_length = strlen(p_in_vpn_expiration_date);
                        }
                        else
                        {
                                #ifdef DEBUG_MODE
                                printf("vpn_exp null\n");
                                #endif
                                is_null_in_vpn_expiration_date = 1;
                                p_in_vpn_expiration_date_length = 0;
                        }
                        
                        if(strcmp(pnt_user->email, NULL_IDENTIFIER))
                        {
                                is_null_in_email = 0;
                                #ifdef DEBUG_MODE
                                printf("mail not null\n");
                                printf("pnt_user->email: %s\n", pnt_user->email);
                                #endif
                                strcpy(p_in_email, pnt_user->email);
                                p_in_email_length = strlen(p_in_email);
                        }
                        else
                        {
                                #ifdef DEBUG_MODE
                                printf("mail null\n");
                                #endif
                                is_null_in_email = 1;
                                p_in_email_length = 0;
                        }
                        
                        if((status = mysql_stmt_execute(insert_stmt)))
                        {
                                fprintf(stderr, "Error on insert_stmt mysql_stmt_execute: %d\n", status);
                                fprintf(stderr, "Error: %s\n", mysql_stmt_error(insert_stmt));
                                exit(1);
                        }
                }
                else if(to_update && pnt_user->iduser_idm >= 0) // UPDATE_CODE
                {
                        #ifdef DEBUG_MODE
                        printf("i: %d\n", i);
                        printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                        #endif
                        p_up_iduser_idm = pnt_user->iduser_idm; // 
                        #ifdef DEBUG_MODE
                        printf("pnt_user->username: %s\n", pnt_user->username);
                        #endif
                        strcpy(p_up_username, pnt_user->username);
                        p_up_username_length = strlen(p_up_username);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->name: %s\n", pnt_user->name);
                        #endif
                        strcpy(p_up_name, pnt_user->name);
                        p_up_name_length = strlen(p_up_name);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->name: %s\n", pnt_user->name);
                        #endif
                        strcpy(p_up_surname, pnt_user->surname);
                        p_up_surname_length = strlen(p_up_surname);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->uid: %d\n", pnt_user->uid);
                        #endif
                        p_up_uid = pnt_user->uid;
                        #ifdef DEBUG_MODE
                        printf("pnt_user->gid: %d\n", pnt_user->gid);
                        #endif
                        p_up_gid = pnt_user->gid;
                        #ifdef DEBUG_MODE
                        printf("pnt_user->group_name: %s\n", pnt_user->group_name);
                        #endif
                        strcpy(p_up_group_name, pnt_user->group_name);
                        p_up_group_name_length = strlen(p_up_group_name);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->division: %s\n", pnt_user->division);
                        #endif
                        strcpy(p_up_division, pnt_user->division);
                        p_up_division_length = strlen(p_up_division);
                        #ifdef DEBUG_MODE
                        printf("pnt_user->creation_date: %s\n", pnt_user->creation_date);
                        #endif
                        strcpy(p_up_creation_date, pnt_user->creation_date);
                        p_up_creation_date_length = strlen(p_up_creation_date);
                        
                        if(strcmp(pnt_user->expiration_date, NULL_IDENTIFIER))
                        {
                                is_null_up_expiration_date = 0;
                                #ifdef DEBUG_MODE
                                printf("exp not null\n");
                                printf("pnt_user->expiration_date: %s\n", pnt_user->expiration_date);
                                #endif
                                strcpy(p_up_expiration_date, pnt_user->expiration_date);
                                p_up_expiration_date_length = strlen(p_up_expiration_date);
                        }
                        else
                        {
                                #ifdef DEBUG_MODE
                                printf("exp null\n");
                                #endif
                                is_null_up_expiration_date = 1;
                                p_up_expiration_date_length = 0;
                        }
                        
                        if(strcmp(pnt_user->vpn_expiration_date, NULL_IDENTIFIER))
                        {
                                is_null_up_vpn_expiration_date = 0;
                                #ifdef DEBUG_MODE
                                printf("vpn_exp not null\n");
                                printf("pnt_user->vpn_expiration_date: %s\n", pnt_user->vpn_expiration_date);
                                #endif
                                strcpy(p_up_vpn_expiration_date, pnt_user->vpn_expiration_date);
                                p_up_vpn_expiration_date_length = strlen(p_up_vpn_expiration_date);
                        }
                        else
                        {
                                #ifdef DEBUG_MODE
                                printf("vpn_exp null\n");
                                #endif
                                is_null_up_vpn_expiration_date = 1;
                                p_up_vpn_expiration_date_length = 0;
                        }
                        
                        if(strcmp(pnt_user->email, NULL_IDENTIFIER))
                        {
                                is_null_up_email = 0;
                                #ifdef DEBUG_MODE
                                printf("mail not null\n");
                                printf("pnt_user->email: %s\n", pnt_user->email);
                                #endif
                                strcpy(p_up_email, pnt_user->email);
                                p_up_email_length = strlen(p_up_email);
                        }
                        else
                        {
                                #ifdef DEBUG_MODE
                                printf("mail null\n");
                                #endif
                                is_null_up_email = 1;
                                p_up_email_length = 0;
                        }
                
                        if((status = mysql_stmt_execute(update_stmt)))
                        {
                                fprintf(stderr, "Error on update_stmt mysql_stmt_execute\n");
                                exit(1);
                        }
                        
                }
        }
           
        mysql_stmt_close(insert_stmt);
        mysql_stmt_close(update_stmt);
        mysql_close(conn); 
        
        #ifdef DEBUG_MODE
        printf("mail_buffer length: %ld\n", strlen(mail_buffer));
        #endif
        
        if(is_mail_active)
                sendmail(from_mail, to_mail, mail_cmd_to, mail_buffer);
    

        #undef is_mail_active 
                        
        return 0;
}


