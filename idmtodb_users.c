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

// #define DEBUG_MODE
#define INSERT_USER "CALL insert_user_2(?,?,?,?,?,?,?,?,?,?,?)"
#define UPDATE_USER "CALL update_user_2(?,?,?,?,?,?,?,?,?,?,?,?,?)"
#define UPDATE_USER_IGNORE_DIVISION_GROUP_NAME "CALL update_user_ignore_division_group_name(?,?,?,?,?,?,?,?,?,?,?,?,?)"
#define UPDATE_PRES_USER "CALL update_pres_user(?,?)"
#define DELETE_USER "CALL delete_user(?)"
#define SELECT_ALL_USERS "CALL select_all_users_2( )"
#define SELECT_ALL_USERS_IGNORE_DIVISION_GROUP_NAME "CALL select_all_users_ignore_division_group_name( )"

// typedef bool my_bool;

enum
{
    P_IN_USERNAME,
    P_IN_NAME,
    P_IN_SURNAME,
    P_IN_UID,
    P_IN_GID,
    P_IN_GROUPNAMES, 
    P_IN_CREATION_DATE,
    P_IN_EXPIRATION_DATE,
    P_IN_VPN_EXPIRATION_DATE,
    P_IN_EMAIL,
    P_IN_IS_MACH_USER,
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
    P_INOUT_GROUPNAMES,
    P_INOUT_CREATION_DATE,
    P_INOUT_EXPIRATION_DATE,
    P_INOUT_VPN_EXPIRATION_DATE,
    P_INOUT_EMAIL,
    P_INOUT_CLOSING_DATE,
    P_INOUT_IS_MACH_USER,
    MAX_INOUT_USER_IDM_PARAMS
} insert_up_user_idm_enum;

enum
{
    P_INOUT_PRES_IDUSER_IDM,
    P_INOUT_PRES_CLOSING_DATE,
    MAX_INOUT_PRES_USER_IDM_PARAMS
} insert_up_pres_user_idm_enum;

#define MAX_UPDATE_USER_PARAMS MAX_INOUT_USER_IDM_PARAMS
#define MAX_UPDATE_PRES_USER_PARAMS MAX_INOUT_PRES_USER_IDM_PARAMS
#define MAX_DELETE_USER_PARAMS (P_INOUT_IDUSER_IDM+1)
#define MAX_SELECT_ALL_USERS_PARAMS MAX_INOUT_USER_IDM_PARAMS

#define DEFAULT_PROMPT_ON_INSERT 1
#define DEFAULT_PROMPT_ON_UPDATE 1
#define DEFAULT_PROMPT_ON_DELETE 1

#define	CSV_SEPARATOR ';'

#define DEFAULT_IGNORE_GROUPS 1
#define DEFAULT_IGNORE_DIVISION_GROUP_NAME 1

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

#define MAX_BUFTIME_LENGTH 160

#define MAX_PASSWORD_LEN 512

#define MAX_USERNAME_LEN 16
#define MAX_NAME_LEN 32
#define MAX_SURNAME_LEN 32
#define MAX_GROUP_NAME_LEN 32
#define MAX_DIVISION_LEN 32
#define MAX_GROUP_NAMES_LEN 320 // 32*10 (on average user belonging to 10+ IDM groups)
#define MAX_DATE_LEN 100
#define MAX_EMAIL_LEN 64

#define MAX_GROUPS 10 // according to the previous comment on MAX_GROUP_NAMES_LEN

#define _MAX_NAME_LEN 16
#define _MAX_SURNAME_LEN 16
#define _MAX_GROUP_NAME_LEN 16
#define _MAX_DIVISION_LEN 16
#define _MAX_GROUP_NAMES_LEN 16
#define _MAX_DATE_LEN 16
#define _MAX_EMAIL_LEN 16

#define MAX_LINE_LEN 2*(MAX_USERNAME_LEN+MAX_NAME_LEN+MAX_SURNAME_LEN+MAX_GROUP_NAME_LEN+MAX_DIVISION_LEN+MAX_DATE_LEN+MAX_EMAIL_LEN+6) // added 2* for safety
#define MAX_BUFLINE_LEN MAX_LINE_LEN
#define NUMBERS_FIXED_LEN 16 // 21 // 16

#define MAX_USERS 1000

#define NULL_IDENTIFIER "None" // python-like, "NULL" is C-like, but it doesn't preserve consistency between Python-generated .csv input_file and user_idm DB's table.
#define INSERT_CODE -99
#define STAY_CODE -98
#define DELETE_CODE -97
#define UPDATE_PRES_CODE -96

#define NO_DELETE_CODE 2

typedef struct _idm_user_t
{
        int iduser_idm; // 
        char username[MAX_USERNAME_LEN];
        char name[MAX_NAME_LEN];
        char surname[MAX_SURNAME_LEN];
        int uid;
        int gid;
        char group_names[MAX_GROUP_NAMES_LEN];
        char creation_date[MAX_DATE_LEN];
        char expiration_date[MAX_DATE_LEN];
        char vpn_expiration_date[MAX_DATE_LEN];
        char email[MAX_EMAIL_LEN];
        char closing_date[MAX_DATE_LEN];
	char is_mach_user; // only valid for .csv->DB sync operations
} idm_user_t;

/*
typedef struct _id_idm_user_t
{
        int iduser_idm;
        idm_user_t idm_user;
} id_idm_user_t;
*/

static inline const char * gettime(time_t * aux_time)
{
	static char buftime[MAX_BUFTIME_LENGTH];
        static time_t logging_timestamp;
        static struct tm ts;

	logging_timestamp = time(NULL);

	if(aux_time)
		*aux_time = logging_timestamp;

        ts = *localtime(&logging_timestamp);
        strftime(buftime, sizeof(buftime), "%Y-%m-%d-%H:%M:%S", &ts);
	return buftime;
}


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
 
// Defining comparator function as per the requirement 
static int myCompare(const void* a, const void* b) 
{ 
    return strcmp(a, b); 
} 

static int are_groups_same(const char * group_names_a, const char * group_names_b)
{
	char group_names_a_array[MAX_GROUPS][MAX_GROUP_NAMES_LEN];
	char group_names_b_array[MAX_GROUPS][MAX_GROUP_NAMES_LEN];

	const char * pnt;
	char * pnt_2;
	int idx;
	int len_a;
	int len_b;

	idx = 0;

	#ifdef DEBUG_MODE
	printf("before first for\n");
	#endif

	for(pnt = group_names_a, pnt_2=group_names_a_array[0]; *pnt != '\0'; ++ pnt)
	{
		#ifdef DEBUG_MODE
		printf("pnt: %c\n", *pnt); // printf("pnt_2: %c\n", *pnt, *pnt_2);
		#endif
		if(*pnt == ',')
		{
			*pnt_2 = '\0';
			#ifdef DEBUG_MODE
			printf("pnt_2 after comma: %s\n", group_names_a_array[idx]);
			#endif
			pnt_2=group_names_a_array[++idx];
			continue;
		}

		*pnt_2 = *pnt;
		#ifdef DEBUG_MODE
		printf("pnt_2: %c\n", *pnt_2);
		#endif
		++ pnt_2;
	}	

	*pnt_2 = '\0';
	#ifdef DEBUG_MODE
	printf("FINAL pnt_2: %s\n", group_names_a_array[idx]);
	#endif
	len_a = idx+1;
	idx = 0;

	for(pnt = group_names_b, pnt_2=group_names_b_array[0]; *pnt != '\0'; ++ pnt)
        {

                if(*pnt == ',')
                {
                        *pnt_2 = '\0';
			#ifdef DEBUG_MODE
			printf("pnt_2 after comma: %s\n", group_names_b_array[idx]);
                        #endif
			pnt_2=group_names_b_array[++idx];
                        continue;
                }

                *pnt_2 = *pnt;
		#ifdef DEBUG_MODE
		printf("pnt_2: %c\n", *pnt_2);
		#endif
		++ pnt_2;
        }

        *pnt_2 = '\0';
	#ifdef DEBUG_MODE
	printf("FINAL pnt_2: %s\n", group_names_b_array[idx]);
	#endif
	len_b = idx+1;

	int i, j;

	if(len_a != len_b)
	{
		#ifdef DEBUG_MODE
		printf("two group names different!\n");
		#endif
		return 0;
	}

	#ifdef DEBUG_MODE
	printf("len_a: %d, len_b: %d\n\n", len_a, len_b);

	for(i=0; i<len_a; ++i)
        {
                printf("group_names_a_array: %s\n", group_names_a_array[i]);
                printf("group_names_b_array: %s\n", group_names_b_array[i]);
	}

	printf("before qsort: %d\n", sizeof(group_names_a_array[0]));
	#endif

	qsort(group_names_a_array, len_a, sizeof(group_names_a_array[0]), myCompare);
	
	#ifdef DEBUG_MODE
	printf("after qsort: %d\n", sizeof(group_names_a_array[0]));
	printf("before qsort: %d\n", sizeof(group_names_b_array[0]));
	#endif
	
	qsort(group_names_b_array, len_b, sizeof(group_names_b_array[0]), myCompare);
	
	#ifdef DEBUG_MODE
	printf("after qsort: %d\n", sizeof(group_names_b_array[0]));
	#endif

	for(i=0; i<len_a; ++i)
	{
		#ifdef DEBUG_MODE
		printf("group_names_a_array: %s\n", group_names_a_array[i]);
 	       	printf("group_names_b_array: %s\n", group_names_b_array[i]);
		#endif
		
		if(strcmp(group_names_a_array[i], group_names_b_array[i]))
			return 0;
	}


	return 1;
}

int main(int argc, char *argv[])
{
        const unsigned char prompt_on_insert = argc > 1 ? atoi(argv[1]): DEFAULT_PROMPT_ON_INSERT;
        const unsigned char prompt_on_update = argc > 2 ? atoi(argv[2]): DEFAULT_PROMPT_ON_UPDATE;
        const unsigned char prompt_on_delete = argc > 3 ? atoi(argv[3]): DEFAULT_PROMPT_ON_DELETE;
	const unsigned char ignore_groups = argc > 4 ? atoi(argv[4]) : DEFAULT_IGNORE_GROUPS;
        const unsigned char ignore_division_group_name = argc > 5 ? atoi(argv[5]) : DEFAULT_IGNORE_DIVISION_GROUP_NAME;
	const int max_users = argc > 6 ? atoi(argv[6]) : MAX_USERS;
        const char * input_file = argc > 7 ? argv[7] : DEFAULT_INPUT_FILE;
        const char * server = argc > 8 ? argv[8] : USERSIDM_SERVER;
        const char * user = argc > 9 ? argv[9] : USERSIDM_USER;
        const char * password = argc > 10 ? argv[10] : USERSIDM_PASSWORD;
        const char * database = argc > 11 ? argv[11] : USERSIDM_DATABASE;
        const char * mail_cmd = argc > 12 ? argv[12] : DEFAULT_MAIL_COMMAND;
        const char * from_mail = argc > 13 ? argv[13] : DEFAULT_FROM_MAIL;
        const char * to_mail = argc > 14 ? argv[14] : NULL;

        // char * token;

        #ifdef DEBUG_MODE
        char psw_asterisks[MAX_PASSWORD_LEN];
        char *token_aux = psw_asterisks;
        for(const char * token = password; *token != '\0'; ++token, ++token_aux)
            *token_aux = '*';
        *token_aux = '\0';

        printf("prompt_on_insert: %u\nprompt_on_update: %u\nprompt_on_delete: %u\nignore_groups: %u\nmax_users: %d\ninput_file: %s\nserver: %s\nuser: %s\npassword: %s\ndatabase: %s\nmail_cmd: %s\nfrom_mail: %s\nto_mail: %s\n",
                prompt_on_insert, prompt_on_update, prompt_on_delete, ignore_groups, max_users, input_file, server, user, psw_asterisks, database, mail_cmd, from_mail, to_mail);
        
	
	printf("are_groups_same: %d\n", are_groups_same("sysm,amm,xss,zap,sap", "asc,sysm,sysm,opa,abc"));
	
	exit(0);
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
        MYSQL_STMT * update_pres_stmt;
        MYSQL_STMT * delete_stmt;
        MYSQL_STMT * select_stmt;
        MYSQL_RES  *prepare_meta_result;
        MYSQL_BIND ps_insert_user_params[MAX_INSERT_USER_PARAMS];
        MYSQL_BIND ps_update_user_params[MAX_UPDATE_USER_PARAMS];
        MYSQL_BIND ps_update_pres_user_params[MAX_UPDATE_PRES_USER_PARAMS];
        MYSQL_BIND ps_delete_user_params[MAX_DELETE_USER_PARAMS];
        // MYSQL_BIND out_params[MAX_INSERT_USER_PARAMS];

	#ifdef DEBUG_MODE
        printf("connected to database!\n");   
	#endif

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
        
        update_pres_stmt = mysql_stmt_init(conn);
        
        if (!update_pres_stmt)
        {
                printf("\nerror when init update_pres_stmt statement\n");
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
        
        delete_stmt = mysql_stmt_init(conn);

        if (!delete_stmt)
        {
                printf("\nerror when init delete_stmt statement\n");
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

        status = ignore_division_group_name ? mysql_stmt_prepare(update_stmt, UPDATE_USER_IGNORE_DIVISION_GROUP_NAME, strlen(UPDATE_USER_IGNORE_DIVISION_GROUP_NAME)) :
						mysql_stmt_prepare(update_stmt, UPDATE_USER, strlen(UPDATE_USER));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_stmt prepare: %d\n", status);
        #endif
        
        status = mysql_stmt_prepare(update_pres_stmt, UPDATE_PRES_USER, strlen(UPDATE_PRES_USER));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_pres_stmt prepare: %d\n", status);
        #endif
        
        status = mysql_stmt_prepare(delete_stmt, DELETE_USER, strlen(DELETE_USER));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of delete_stmt prepare: %d\n", status);
        #endif
        
        status = ignore_division_group_name ? mysql_stmt_prepare(select_stmt, SELECT_ALL_USERS_IGNORE_DIVISION_GROUP_NAME, strlen(SELECT_ALL_USERS_IGNORE_DIVISION_GROUP_NAME)) :
						mysql_stmt_prepare(select_stmt, SELECT_ALL_USERS, strlen(SELECT_ALL_USERS));
        

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
        memset(ps_update_pres_user_params, 0, sizeof(ps_update_pres_user_params));
        memset(ps_delete_user_params, 0, sizeof(ps_delete_user_params));
        // memset(out_params, 0, sizeof(out_params));

        #ifdef DEBUG_MODE
        printf("\n\n *** AFTER MEMSET AND BEFORE ALL PS_PARAMS STUFFS *** \n\n");
        #endif
        
        unsigned long ul_zero_value = 0;
        
        // iduser_idm
        int p_iduser_idm = 0;
        ps_update_user_params[P_INOUT_IDUSER_IDM].buffer_type = MYSQL_TYPE_LONG;
        ps_update_user_params[P_INOUT_IDUSER_IDM].buffer = (int *) &p_iduser_idm;
        ps_update_user_params[P_INOUT_IDUSER_IDM].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_IDUSER_IDM].is_null = 0;
        
        ps_update_pres_user_params[P_INOUT_PRES_IDUSER_IDM].buffer_type = MYSQL_TYPE_LONG;
        ps_update_pres_user_params[P_INOUT_PRES_IDUSER_IDM].buffer = (int *) &p_iduser_idm;
        ps_update_pres_user_params[P_INOUT_PRES_IDUSER_IDM].length = &ul_zero_value;
        ps_update_pres_user_params[P_INOUT_PRES_IDUSER_IDM].is_null = 0;
        
        ps_delete_user_params[P_INOUT_IDUSER_IDM].buffer_type = MYSQL_TYPE_LONG;
        ps_delete_user_params[P_INOUT_IDUSER_IDM].buffer = (int *) &p_iduser_idm;
        ps_delete_user_params[P_INOUT_IDUSER_IDM].length = &ul_zero_value;
        ps_delete_user_params[P_INOUT_IDUSER_IDM].is_null = 0;
        
        // closing_date 
        my_bool is_null_closing_date;
        unsigned long p_closing_date_length = 0;
        char p_closing_date[MAX_DATE_LEN];
        
        ps_update_pres_user_params[P_INOUT_PRES_CLOSING_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_pres_user_params[P_INOUT_PRES_CLOSING_DATE].buffer = (char *) p_closing_date;
        ps_update_pres_user_params[P_INOUT_PRES_CLOSING_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_pres_user_params[P_INOUT_PRES_CLOSING_DATE].length = &p_closing_date_length;
        ps_update_pres_user_params[P_INOUT_PRES_CLOSING_DATE].is_null = &is_null_closing_date;
        
        // username
        unsigned long p_username_length = 0;
        char p_username[MAX_USERNAME_LEN];
        ps_insert_user_params[P_IN_USERNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_USERNAME].buffer = (char *) p_username; 
        ps_insert_user_params[P_IN_USERNAME].buffer_length = MAX_USERNAME_LEN;
        ps_insert_user_params[P_IN_USERNAME].is_null = 0;
        ps_insert_user_params[P_IN_USERNAME].length = &p_username_length;
        
        ps_update_user_params[P_INOUT_USERNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_USERNAME].buffer = (char *) p_username; 
        ps_update_user_params[P_INOUT_USERNAME].buffer_length = MAX_USERNAME_LEN;
        ps_update_user_params[P_INOUT_USERNAME].is_null = 0;
        ps_update_user_params[P_INOUT_USERNAME].length = &p_username_length;
        
        
        // name
        unsigned long p_name_length = 0;
        char p_name[MAX_NAME_LEN];
        ps_insert_user_params[P_IN_NAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_NAME].buffer = (char *) p_name; 
        ps_insert_user_params[P_IN_NAME].buffer_length = MAX_NAME_LEN;
        ps_insert_user_params[P_IN_NAME].is_null = 0;
        ps_insert_user_params[P_IN_NAME].length = &p_name_length;
        
        ps_update_user_params[P_INOUT_NAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_NAME].buffer = (char *) p_name; 
        ps_update_user_params[P_INOUT_NAME].buffer_length = MAX_NAME_LEN;
        ps_update_user_params[P_INOUT_NAME].is_null = 0;
        ps_update_user_params[P_INOUT_NAME].length = &p_name_length;
        
        // surname
        unsigned long p_surname_length = 0;
        char p_surname[MAX_SURNAME_LEN];
        ps_insert_user_params[P_IN_SURNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_SURNAME].buffer = (char *) p_surname; 
        ps_insert_user_params[P_IN_SURNAME].buffer_length = MAX_SURNAME_LEN;
        ps_insert_user_params[P_IN_SURNAME].is_null = 0;
        ps_insert_user_params[P_IN_SURNAME].length = &p_surname_length;
        
        ps_update_user_params[P_INOUT_SURNAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_SURNAME].buffer = (char *) p_surname; 
        ps_update_user_params[P_INOUT_SURNAME].buffer_length = MAX_SURNAME_LEN;
        ps_update_user_params[P_INOUT_SURNAME].is_null = 0;
        ps_update_user_params[P_INOUT_SURNAME].length = &p_surname_length;
        
        // uid
        int p_uid = 0;
        ps_insert_user_params[P_IN_UID].buffer_type = MYSQL_TYPE_LONG;
        ps_insert_user_params[P_IN_UID].buffer = (int *) &p_uid;
        ps_insert_user_params[P_IN_UID].length = &ul_zero_value;
        ps_insert_user_params[P_IN_UID].is_null = 0;
        
        ps_update_user_params[P_INOUT_UID].buffer_type = MYSQL_TYPE_LONG;
        ps_update_user_params[P_INOUT_UID].buffer = (int *) &p_uid;
        ps_update_user_params[P_INOUT_UID].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_UID].is_null = 0;
        
        // gid
        int p_gid = 0;
        ps_insert_user_params[P_IN_GID].buffer_type = MYSQL_TYPE_LONG;
        ps_insert_user_params[P_IN_GID].buffer = (int *) &p_gid;
        ps_insert_user_params[P_IN_GID].length = &ul_zero_value;
        ps_insert_user_params[P_IN_GID].is_null = 0;
        
        ps_update_user_params[P_INOUT_GID].buffer_type = MYSQL_TYPE_LONG;
        ps_update_user_params[P_INOUT_GID].buffer = (int *) &p_gid;
        ps_update_user_params[P_INOUT_GID].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_GID].is_null = 0;
        
	 // group_names
        unsigned long p_group_names_length = 0;
        char p_group_names[MAX_GROUP_NAMES_LEN];
        ps_insert_user_params[P_IN_GROUPNAMES].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_GROUPNAMES].buffer = (char *) p_group_names;
        ps_insert_user_params[P_IN_GROUPNAMES].buffer_length = MAX_GROUP_NAMES_LEN;
        ps_insert_user_params[P_IN_GROUPNAMES].is_null = 0;
        ps_insert_user_params[P_IN_GROUPNAMES].length = &p_group_names_length;

        ps_update_user_params[P_INOUT_GROUPNAMES].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_GROUPNAMES].buffer = (char *) p_group_names;
        ps_update_user_params[P_INOUT_GROUPNAMES].buffer_length = MAX_GROUP_NAMES_LEN;
        ps_update_user_params[P_INOUT_GROUPNAMES].is_null = 0;
        ps_update_user_params[P_INOUT_GROUPNAMES].length = &p_group_names_length;
        

	// creation_date
        my_bool is_null_creation_date;
	unsigned long p_creation_date_length = 0;
        char p_creation_date[MAX_DATE_LEN];
        ps_insert_user_params[P_IN_CREATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_CREATION_DATE].buffer = (char *) p_creation_date;
        ps_insert_user_params[P_IN_CREATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_user_params[P_IN_CREATION_DATE].length = &p_creation_date_length;
        ps_insert_user_params[P_IN_CREATION_DATE].is_null = &is_null_creation_date;
        
        ps_update_user_params[P_INOUT_CREATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_CREATION_DATE].buffer = (char *) p_creation_date;
        ps_update_user_params[P_INOUT_CREATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_CREATION_DATE].length = &p_creation_date_length;
        ps_update_user_params[P_INOUT_CREATION_DATE].is_null = &is_null_creation_date;

        // expiration_date
        my_bool is_null_expiration_date;
        unsigned long p_expiration_date_length = 0;
        char p_expiration_date[MAX_DATE_LEN];
        ps_insert_user_params[P_IN_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].buffer = (char *) p_expiration_date;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].length = &p_expiration_date_length;
        ps_insert_user_params[P_IN_EXPIRATION_DATE].is_null = &is_null_expiration_date;
        
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].buffer = (char *) p_expiration_date;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].length = &p_expiration_date_length;
        ps_update_user_params[P_INOUT_EXPIRATION_DATE].is_null = &is_null_expiration_date;
        
        // vpn_expiration_date
        my_bool is_null_vpn_expiration_date;
        unsigned long p_vpn_expiration_date_length = 0;
        char p_vpn_expiration_date[MAX_DATE_LEN];
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].buffer = (char *) p_vpn_expiration_date;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].length = &p_vpn_expiration_date_length;
        ps_insert_user_params[P_IN_VPN_EXPIRATION_DATE].is_null = &is_null_vpn_expiration_date;
        
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].buffer = (char *) p_vpn_expiration_date;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].length = &p_vpn_expiration_date_length;
        ps_update_user_params[P_INOUT_VPN_EXPIRATION_DATE].is_null = &is_null_vpn_expiration_date;
        
        // email
        my_bool is_null_email;
        unsigned long p_email_length = 0;
        char p_email[MAX_EMAIL_LEN];
        ps_insert_user_params[P_IN_EMAIL].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_user_params[P_IN_EMAIL].buffer = (char *) p_email; 
        ps_insert_user_params[P_IN_EMAIL].buffer_length = MAX_EMAIL_LEN;
        ps_insert_user_params[P_IN_EMAIL].is_null = &is_null_email;
        ps_insert_user_params[P_IN_EMAIL].length = &p_email_length;
        
        ps_update_user_params[P_INOUT_EMAIL].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_user_params[P_INOUT_EMAIL].buffer = (char *) p_email; 
        ps_update_user_params[P_INOUT_EMAIL].buffer_length = MAX_EMAIL_LEN;
        ps_update_user_params[P_INOUT_EMAIL].is_null = &is_null_email;
        ps_update_user_params[P_INOUT_EMAIL].length = &p_email_length;
        
        // closing_date
        ps_update_user_params[P_INOUT_CLOSING_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_user_params[P_INOUT_CLOSING_DATE].buffer = (char *) p_closing_date;
        ps_update_user_params[P_INOUT_CLOSING_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_user_params[P_INOUT_CLOSING_DATE].length = &p_closing_date_length;
        ps_update_user_params[P_INOUT_CLOSING_DATE].is_null = &is_null_closing_date;
        
	// is_mach_user
        char p_is_mach_user = 0;
        ps_insert_user_params[P_IN_IS_MACH_USER].buffer_type = MYSQL_TYPE_TINY;
        ps_insert_user_params[P_IN_IS_MACH_USER].buffer = (char *) &p_is_mach_user;
        ps_insert_user_params[P_IN_IS_MACH_USER].length = &ul_zero_value;
        ps_insert_user_params[P_IN_IS_MACH_USER].is_null = 0;

        ps_update_user_params[P_INOUT_IS_MACH_USER].buffer_type = MYSQL_TYPE_TINY;
        ps_update_user_params[P_INOUT_IS_MACH_USER].buffer = (int *) &p_is_mach_user;
        ps_update_user_params[P_INOUT_IS_MACH_USER].length = &ul_zero_value;
        ps_update_user_params[P_INOUT_IS_MACH_USER].is_null = 0;


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
        
        if((status = mysql_stmt_bind_param(update_pres_stmt, ps_update_pres_user_params)))
        {
                fprintf(stderr, "update_pres_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_pres_stmt mysql_stmt_bind_param: %d\n", status);
        #endif
        
        if((status = mysql_stmt_bind_param(delete_stmt, ps_delete_user_params)))
        {
                fprintf(stderr, "delete_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of delete_stmt mysql_stmt_bind_param: %d\n", status);
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
        int del_users[max_users];
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
                
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("username is: %s\n", token);
                #endif
                strcpy(pnt_user->username, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("name is: %s\n", token);
                #endif
                strcpy(pnt_user->name, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("surname is: %s\n", token);
                #endif
                strcpy(pnt_user->surname, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                aux = atoi(token);
                #ifdef DEBUG_MODE
                printf("uid is: %d\n", aux);
                #endif
                pnt_user->uid = aux;
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                aux = atoi(token);
                #ifdef DEBUG_MODE
                printf("gid is: %d\n", aux);
                #endif
                pnt_user->gid = aux;
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("group_names is: %s\n", token);
                #endif
                strcpy(pnt_user->group_names, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("creation_date is: %s\n", token);
                #endif
                strcpy(pnt_user->creation_date, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("expiration_date is: %s\n", token);
                #endif
                strcpy(pnt_user->expiration_date, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("vpn_expiration_date is: %s\n", token);
                #endif
                strcpy(pnt_user->vpn_expiration_date, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("email is: %s\n", token);
                #endif
                strcpy(pnt_user->email, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
		//*(token2-1) = '\0';
                #ifdef DEBUG_MODE
                printf("closing_date is: %s\n", token);
                // printf("its strlen: %s\n", strlen(token));
		#endif
                strcpy(pnt_user->closing_date, token);
		token = token2+1;
		for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
		*token2 = '\0'; // skip password field
		#ifdef DEBUG_MODE
		printf("password field: %s\n", token);
		#endif
		//
		token = token2+1;
		for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0' && *token2 != '\n'; ++token2);
		pnt_user->is_mach_user = token2 != token;
		#ifdef DEBUG_MODE
		printf("p_is_mach_user: %d\n", pnt_user->is_mach_user);
		#endif
		*token2 = '\0';
		#ifdef DEBUG_MODE
		printf("p_mach: %s\n\n", token);
		#endif
	}
        
        fclose(fp);
        
	printf("[%s] Total line num: %d\n", gettime(NULL), line_num);
        
        int i, j;
        int rows;
        int result;
        
        idm_user_t users_db[max_users];
        
        for(rows=0; !(result = mysql_stmt_fetch(select_stmt)); ++rows)
        {
                pnt_user = &users_db[rows]; // &(users_db[rows].idm_user);
                
                #ifdef DEBUG_MODE
                printf("PRE %d, %s, %s, %s, %d, %d, %s, %s, %s, %s, %s, %s\n", p_iduser_idm, p_username, p_name, p_surname, p_uid, p_gid, p_group_names, p_creation_date, is_null_expiration_date ? NULL_IDENTIFIER : p_expiration_date, is_null_vpn_expiration_date ? NULL_IDENTIFIER : p_vpn_expiration_date, is_null_email ? NULL_IDENTIFIER : p_email, is_null_closing_date ? NULL_IDENTIFIER : p_closing_date);
                #endif
               
                pnt_user->iduser_idm = p_iduser_idm; // users_db[rows].iduser_idm = p_iduser_idm; // pnt_user->iduser_idm = p_iduser_idm;
                strcpy(pnt_user->username, p_username);
                strcpy(pnt_user->name, p_name);
                strcpy(pnt_user->surname, p_surname);
                pnt_user->uid = p_uid;
                pnt_user->gid = p_gid;
                strcpy(pnt_user->group_names, p_group_names);
                // strcpy(pnt_user->division, p_division);
                strcpy(pnt_user->creation_date, is_null_creation_date ? NULL_IDENTIFIER : p_creation_date);
                strcpy(pnt_user->expiration_date, is_null_expiration_date ? NULL_IDENTIFIER : p_expiration_date);
                strcpy(pnt_user->vpn_expiration_date, is_null_vpn_expiration_date ? NULL_IDENTIFIER : p_vpn_expiration_date);
                strcpy(pnt_user->email, is_null_email ? NULL_IDENTIFIER : p_email);
                strcpy(pnt_user->closing_date, is_null_closing_date ? NULL_IDENTIFIER : p_closing_date);
                
                #ifdef DEBUG_MODE
                printf("POST %d, %s, %s, %s, %d, %d, %s, %s, %s, %s, %s, %s\n\n", pnt_user->iduser_idm, pnt_user->username, pnt_user->name, pnt_user->surname, pnt_user->uid, pnt_user->gid, pnt_user->group_names, pnt_user->creation_date, pnt_user->expiration_date, pnt_user->vpn_expiration_date, pnt_user->email, pnt_user->closing_date);
                #endif
                
        }
        
        for(; mysql_next_result(conn) == 0;);
        mysql_stmt_close(select_stmt); // close select_stmt
        
        #ifdef DEBUG_MODE
        printf("mysql_stmt_fetch: %d\n", result);
        #endif
        
        printf("[%s] Total rows num: %d\n", gettime(NULL), rows);
        
        idm_user_t * pnt_user_db = NULL;
        
        /*
        id_idm_user_t users_idm_to_update[max_users];
        idm_user_t users_idm_to_insert[max_users];
        */
        
        // COMPARISON CODE
        
	printf("[%s] line_num: %d\n\n", gettime(NULL), line_num);
        
        for(i=0; i<line_num; ++i)
        {
                pnt_user = &users[i];
                for(j=0; j<rows; ++j)
                {
                        pnt_user_db = &users_db[j];
                        // printf("idm_uid: %d, db_uid: %d\n", pnt_user->uid, pnt_user_db->uid);
                        if(!strcmp(pnt_user->username, pnt_user_db->username)) // pnt_user->uid == pnt_user_db->uid)
                        {
                                
				// printf("username: %s\n", pnt_user->username);
                                //#ifdef DEBUG_MODE
                                //printf("Hallelujah\n");
				//#endif
				
				//#ifdef DEBUG_MODE

				// printf("-----------\n");
				if(strcmp(pnt_user->closing_date, pnt_user_db->closing_date))
					printf("closing_date differs\n");

				/*
				if(strcmp(pnt_user->username, pnt_user_db->username))
					printf("username differs\n");
				*/

				if(strcmp(pnt_user->name, pnt_user_db->name))
					printf("name differs\n");

				if(strcmp(pnt_user->surname, pnt_user_db->surname))
                                        printf("surname differs\n");

				if(pnt_user->uid != pnt_user_db->uid) // strcmp(pnt_user->group_name, pnt_user_db->group_name))
                                        printf("uid differs\n");

				if((!ignore_groups) && pnt_user->gid != pnt_user_db->gid) // strcmp(pnt_user->group_name, pnt_user_db->group_name))
					printf("gid differs\n");

				if((!ignore_groups) && strcmp(pnt_user->username,"forcepoint") && !are_groups_same(pnt_user->group_names, pnt_user_db->group_names)) // strcmp(pnt_user->group_names, pnt_user_db->group_names))
					printf("group_names differs\n");			

				if(strcmp(pnt_user->creation_date, pnt_user_db->creation_date))
					printf("creation date differs\n");

				if(strcmp(pnt_user->expiration_date, pnt_user_db->expiration_date))
					printf("expiration date differs\n");

				if(strcmp(pnt_user->vpn_expiration_date, pnt_user_db->vpn_expiration_date))
					printf("vpn_expiration_date differs\n");
				
				if(strcmp(pnt_user->email, pnt_user_db->email))
				{
					printf("IDM Email: %s\n", pnt_user->email);
					printf("DB Email: %s\n", pnt_user_db->email);
					printf("email differs\n");
				}

				//#endif

                                if((!strcmp(pnt_user_db->closing_date, NULL_IDENTIFIER)) && (strcmp(pnt_user->closing_date, pnt_user_db->closing_date) || strcmp(pnt_user->name, pnt_user_db->name) || strcmp(pnt_user->surname, pnt_user_db->surname)|| pnt_user->uid != pnt_user_db->uid || ((!ignore_groups) && strcmp(pnt_user->username,"forcepoint") && (pnt_user->gid != pnt_user_db->gid || !are_groups_same(pnt_user->group_names, pnt_user_db->group_names))) || //strcmp(pnt_user->group_names, pnt_user_db->group_names)))  ||
                                   strcmp(pnt_user->creation_date, pnt_user_db->creation_date) || strcmp(pnt_user->expiration_date, pnt_user_db->expiration_date) ||
                                   strcmp(pnt_user->vpn_expiration_date, pnt_user_db->vpn_expiration_date) || strcmp(pnt_user->email, pnt_user_db->email)))
				{
                                        pnt_user->iduser_idm = pnt_user_db->iduser_idm; // update record on DB with IDM's record values, UPDATE_CODE.
					printf("username: %s\n", pnt_user->username);
					// printf("-----------\n");
				}
                                break; // do it in any case, if iduser_idm field of pnt_user structure still hasn't been valued from there, however it should have the very initial value, STAY_CODE.
                        }    
                }
                
		#ifdef DEBUG_MODE
		printf("j is %d, closing_date: %s\n", j, pnt_user->closing_date);
		#endif

                if(j == rows && !strcmp(pnt_user->closing_date, NULL_IDENTIFIER))
		{
			// (j == rows) // insert only if it has a non-null closing_date.
			#ifdef DEBUG_MDOE
			printf("insert code for %s\n", pnt_user->username);
			#endif
			-- pnt_user->iduser_idm; // from STAY_CODE to INSERT_CODE.
		}
        }
        
        int local_del_idx = 0;
        
        for(j=0; j<rows; ++j)
        {
                pnt_user_db = &users_db[j];
                for(i=0; i<line_num; ++i)
                {
                        pnt_user = &users[i];
                        if(!strcmp(pnt_user->username, pnt_user_db->username)) // pnt_user->uid == pnt_user_db->uid)
                                break;
                }
                
                if(i == line_num)
                {
                        del_users[local_del_idx] = pnt_user_db->iduser_idm;
                        ++ local_del_idx;
                        pnt_user_db->iduser_idm = DELETE_CODE;
                }
        }

        int to_insert = 0;
        register unsigned char headered = 0;
        const char * padding = PADDING; //  "#####################################################";
        const char * border_padding = BORDER; 
        char p_numbers[2][33]; // for uid and gid, respectively
        char buffer[MAX_BUFLINE_LEN];
        char border_buffer[MAX_BUFLINE_LEN];
        char mail_buffer[MAX_LOGBUF_LEN];
        int padLens[10];
        int numbersPadLens[2];
        
        // char mail_format_buf[_MAX_MAIL_LEN];
        
        sprintf(buffer, "| username%*.*s| name%*.*s| surname%*.*s| uid%*.*s| gid%*.*s| group_names%*.*s| creation_date%*.*s| exp_date%*.*s| vpn_exp_date%*.*s| email%*.*s| closing_date%*.*s|\n", 7, 7, padding, 11, 11, padding, 8, 8, padding, 12, 12, padding, 12, 12, padding, 4, 4, padding, 2, 2, padding, 7, 7, padding, 3, 3, padding, 10, 10, padding, 3, 3, padding);
        sprintf(border_buffer, "+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s\n", MAX_USERNAME_LEN, MAX_USERNAME_LEN, border_padding, _MAX_NAME_LEN, _MAX_NAME_LEN, border_padding, _MAX_SURNAME_LEN, _MAX_SURNAME_LEN, border_padding, NUMBERS_FIXED_LEN, NUMBERS_FIXED_LEN, border_padding, NUMBERS_FIXED_LEN, NUMBERS_FIXED_LEN, border_padding, _MAX_GROUP_NAMES_LEN, _MAX_GROUP_NAMES_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding, _MAX_EMAIL_LEN, _MAX_EMAIL_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding);
         
	strcpy(mail_buffer, "");

        for(i=0; i<line_num; ++i)
        {

                pnt_user = &users[i];
                // printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                if(pnt_user->iduser_idm == INSERT_CODE) // INSERT_CODE
                {
                        if(!headered)
                        {
                                sprintf(mail_buffer, "<table style=\"background-color: black; color: #adff29;\"><tr style=\"color: red; font-weight: bold;\"><th>username</th><th>name</th><th>surname</th><th>uid</th><th>gid</th><th>group_names</th><th>creation_date</th><th>expiration_date</th><th>vpn_expiration_date</th><th>email</th><th>closing_date</th></tr>\n");
                                headered = 1;
                                printf(border_buffer);
                                printf(buffer);
                                printf(border_buffer);
                        }
                
                        ++ to_insert;
                        padLens[0] = MAX_USERNAME_LEN - strlen(pnt_user->username) -1;
                        padLens[1] = _MAX_NAME_LEN - strlen(pnt_user->name) -1;
                        
                        if(padLens[1] <= 0)
                                padLens[1] = 0;
                        
                        padLens[2] = _MAX_SURNAME_LEN - strlen(pnt_user->surname) -1;
                        
                        if(padLens[2] <= 0)
                                padLens[2] = 0;
                        
                        padLens[3] = _MAX_GROUP_NAMES_LEN - strlen(pnt_user->group_names) -1;
                        
                        if(padLens[3] <= 0)
                                padLens[3] = 0;
                        
                        padLens[4] = _MAX_DATE_LEN - strlen(pnt_user->creation_date) -1;
                        padLens[5] = _MAX_DATE_LEN - strlen(pnt_user->expiration_date) -1;
                        padLens[6] = _MAX_DATE_LEN - strlen(pnt_user->vpn_expiration_date) -1;

                        padLens[7] = _MAX_EMAIL_LEN - strlen(pnt_user->email) -1;
                        
                        padLens[8] = _MAX_DATE_LEN - strlen(pnt_user->closing_date) -1;
                        
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
                                                
                       	if(padLens[7] <= 0)
                                padLens[7] = 0;
                      
                    
                                printf("| %s%*.*s| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s|\n", pnt_user->username, padLens[0], padLens[0], padding, pnt_user->name, padLens[1], padLens[1], padding, pnt_user->surname, padLens[2], padLens[2], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_user->group_names, padLens[3], padLens[3], padding, pnt_user->creation_date, padLens[4], padLens[4], padding, pnt_user->expiration_date, padLens[5], padLens[5], padding, pnt_user->vpn_expiration_date, padLens[6], padLens[6], padding, pnt_user->email, padLens[7], padLens[7], padding, pnt_user->closing_date, padLens[8], padLens[8], padding);
                                
                        sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td></tr>\n", mail_buffer, pnt_user->username, pnt_user->name, pnt_user->surname, pnt_user->uid, pnt_user->gid, pnt_user->group_names, pnt_user->creation_date, pnt_user->expiration_date, pnt_user->vpn_expiration_date, pnt_user->email, pnt_user->closing_date);
                        
                } 
        }
        
        if(headered)
                printf(border_buffer);
        
        headered = 0;
        
        char ch;
        register unsigned char prompted_to_insert = 0;
        
        if(to_insert)
        {
                sprintf(mail_buffer, "%s</table>\n", mail_buffer);
		printf("\n\nThe following %d records have to be INSERTED into the %s DB. ", to_insert, database);
                
                if(prompt_on_insert)
                {
                        printf("Would you like to proceed? [Y/n]\n");
                        
                        // char ch;
                        
                        // printf("PRESS KEY: %c\n", ch);
                        
                        // (void) getchar();
                        prompted_to_insert = 1;
                        
                        if((ch = getchar()) == 'n')
                        {
                                sprintf(mail_buffer, "%s<p>%d results NOT INSERTED into the %s DB.</p><br>\n", mail_buffer, to_insert, database);
                                to_insert = 0;
                        }
                        else
                                sprintf(mail_buffer, "%s<p>%d results INSERTED into the %s DB.</p><br>\n", mail_buffer, to_insert, database);
                                       
                        #ifdef DEBUG_MODE 
                        printf("PRESSED KEY: %c\n", ch);
                        #endif
                }
		else
			sprintf(mail_buffer, "%s<p>%d results INSERTED into the %s DB.</p><br>\n", mail_buffer, to_insert, database);

                printf("\n\n");
        }
        
        #ifdef DEBUG_MODE
        printf("chinsert: %c\n", ch);
        #endif
              
        // sprintf(mail_buffer, "%s</table>\n<p>%d results %s INSERTED into the %s DB.</p><br>\n", mail_buffer, to_insert, to_insert ? "" : "NOT", database);      
                
        int to_update = 0;
        
        for(i=0; i<line_num; ++i)
        {
                pnt_user = &users[i];
                // printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                if(pnt_user->iduser_idm >= 0) // UPDATE_CODE
                {
                        if(!headered)
                        {
                                sprintf(mail_buffer, "%s<table style=\"background-color: black; color: #adff29;\"><tr style=\"color: red; font-weight: bold;\"><th>username</th><th>name</th><th>surname</th><th>uid</th><th>gid</th><th>group_names</th><th>creation_date</th><th>expiration_date</th><th>vpn_expiration_date</th><th>email</th><th>closing_date</th></tr>\n", mail_buffer);
                                headered = 1;
                                printf(border_buffer);
                                printf(buffer);
                                printf(border_buffer);
                        }
                        
                        ++ to_update;
                        padLens[0] = MAX_USERNAME_LEN - strlen(pnt_user->username) -1;
                        padLens[1] = _MAX_NAME_LEN - strlen(pnt_user->name) -1;
                        
                        if(padLens[1] <= 0)
                                padLens[1] = 0;
                        
                        padLens[2] = _MAX_SURNAME_LEN - strlen(pnt_user->surname) -1;
                        
                        if(padLens[2] <= 0)
                                padLens[2] = 0;
                        
                        padLens[3] = _MAX_GROUP_NAMES_LEN - strlen(pnt_user->group_names) -1;
                        
                        if(padLens[3] <= 0)
                                padLens[3] = 0;
                        
                        padLens[4] = _MAX_DATE_LEN - strlen(pnt_user->creation_date) -1;
                        padLens[5] = _MAX_DATE_LEN - strlen(pnt_user->expiration_date) -1;
                        padLens[6] = _MAX_DATE_LEN - strlen(pnt_user->vpn_expiration_date) -1;

                        padLens[7] = _MAX_EMAIL_LEN - strlen(pnt_user->email) -1;
                        
                        padLens[8] = _MAX_DATE_LEN - strlen(pnt_user->closing_date) -1;
                        
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
                                                
                        if(padLens[7] <= 0)
                                padLens[7] = 0;   
                    
                                printf("| %s%*.*s| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s|\n", pnt_user->username, padLens[0], padLens[0], padding, pnt_user->name, padLens[1], padLens[1], padding, pnt_user->surname, padLens[2], padLens[2], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_user->group_names, padLens[3], padLens[3], padding, pnt_user->creation_date, padLens[4], padLens[4], padding, pnt_user->expiration_date, padLens[5], padLens[5], padding, pnt_user->vpn_expiration_date, padLens[6], padLens[6], padding, pnt_user->email, padLens[7], padLens[7], padding, pnt_user->closing_date, padLens[8], padLens[8], padding);
                                
                        sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td></tr>\n", mail_buffer, pnt_user->username, pnt_user->name, pnt_user->surname, pnt_user->uid, pnt_user->gid, pnt_user->group_names, pnt_user->creation_date, pnt_user->expiration_date, pnt_user->vpn_expiration_date, pnt_user->email, pnt_user->closing_date);
                        
                } 
        }
        
        if(headered)
                printf(border_buffer);
                
        headered = 0;
        
        if(to_update)
        {
		sprintf(mail_buffer, "%s</table>\n", mail_buffer);
                printf("\n\nThe following %d records have to be UPDATED into the %s DB. ", to_update, database);
                
                if(prompt_on_update)
                {
                        printf("Would you like to proceed? [Y/n]\n");
                        
                        // char ch;
                        
                        // printf("PRESS KEY: %c\n", ch);
                        
                        if(prompted_to_insert)
                                (void) getchar();
                        
                        prompted_to_insert = 1;
                        
                        if((ch = getchar()) == 'n')
                        {
                                sprintf(mail_buffer, "%s<p>%d results NOT UPDATED into the %s DB.</p><br>", mail_buffer, to_update, database); 
                                to_update = 0;
                        }
                        else
                                sprintf(mail_buffer, "%s<p>%d results UPDATED into the %s DB.</p><br>", mail_buffer, to_update, database); 
                                
                        #ifdef DEBUG_MODE 
                        printf("PRESSED KEY: %c\n", ch);
                        #endif
                }
		else
			sprintf(mail_buffer, "%s<p>%d results UPDATED into the %s DB.</p><br>", mail_buffer, to_update, database);

                printf("\n\n");
        }
        
        // sprintf(mail_buffer, "%s</table>\n<p>%d results %s UPDATED into the %s DB.</p><br>", mail_buffer, to_update, to_update ? "" : "NOT", database); 
        
        int to_delete = 0;
        
        if(prompt_on_delete < NO_DELETE_CODE)
        {
                for(j=0; j<rows; ++j)
                {
                        pnt_user_db = &users_db[j];
                        // printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                        if(pnt_user_db->iduser_idm == DELETE_CODE) // UPDATE_CODE
                        {
                                if(!headered)
                                {
                                        sprintf(mail_buffer, "%s<table style=\"background-color: black; color: #adff29;\"><tr style=\"color: red; font-weight: bold;\"><th>username</th><th>name</th><th>surname</th><th>uid</th><th>gid</th><th>group_names</th><th>creation_date</th><th>expiration_date</th><th>vpn_expiration_date</th><th>email</th><th>closing_date</th></tr>\n", mail_buffer);
                                        headered = 1;
                                        printf(border_buffer);
                                        printf(buffer);
                                        printf(border_buffer);
                                }
                                
                                ++ to_delete;
                                padLens[0] = MAX_USERNAME_LEN - strlen(pnt_user_db->username) -1;
                                padLens[1] = _MAX_NAME_LEN - strlen(pnt_user_db->name) -1;
                                
                                if(padLens[1] <= 0)
                                        padLens[1] = 0;
                                
                                padLens[2] = _MAX_SURNAME_LEN - strlen(pnt_user_db->surname) -1;
                                
                                if(padLens[2] <= 0)
                                        padLens[2] = 0;
                                
                                padLens[3] = _MAX_GROUP_NAMES_LEN - strlen(pnt_user_db->group_names) -1;
                                
                                if(padLens[3] <= 0)
                                        padLens[3] = 0; 
                                
                                padLens[4] = _MAX_DATE_LEN - strlen(pnt_user_db->creation_date) -1;
                                padLens[5] = _MAX_DATE_LEN - strlen(pnt_user_db->expiration_date) -1;
                                padLens[6] = _MAX_DATE_LEN - strlen(pnt_user_db->vpn_expiration_date) -1;

                                padLens[7] = _MAX_EMAIL_LEN - strlen(pnt_user_db->email) -1;
                                
                                padLens[8] = _MAX_DATE_LEN - strlen(pnt_user_db->closing_date) -1;
                                
                                sprintf(p_numbers[0], "%d", pnt_user_db->uid);
                                numbersPadLens[0] = NUMBERS_FIXED_LEN - strlen(p_numbers[0]);
                                sprintf(p_numbers[1], "%d", pnt_user_db->gid);
                                numbersPadLens[1] = NUMBERS_FIXED_LEN - strlen(p_numbers[1]);
                                
                                /*
                                #ifdef DEBUG_MODE
                                printf("strlen username: %d, padlens 0: %d\n", strlen(pnt_user_db->username), padLens[0]);
                                printf("strlen mail: %d, padlens 8: %d\n", mail_len, padLens[8]);
                                #endif
                                */
                                                        
                                if(padLens[7] <= 0)
                                        padLens[7] = 0;   
                            
                                        printf("| %s%*.*s| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s| %.15s%*.*s|\n", pnt_user_db->username, padLens[0], padLens[0], padding, pnt_user_db->name, padLens[1], padLens[1], padding, pnt_user_db->surname, padLens[2], padLens[2], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_user_db->group_names, padLens[3], padLens[3], padding, pnt_user_db->creation_date, padLens[4], padLens[4], padding, pnt_user_db->expiration_date, padLens[5], padLens[5], padding, pnt_user_db->vpn_expiration_date, padLens[6], padLens[6], padding, pnt_user_db->email, padLens[7], padLens[7], padding, pnt_user_db->closing_date, padLens[8], padLens[8], padding);
                                        
                                sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td><td>%.15s</td></tr>\n", mail_buffer, pnt_user_db->username, pnt_user_db->name, pnt_user_db->surname, pnt_user_db->uid, pnt_user_db->gid, pnt_user_db->group_names, pnt_user_db->creation_date, pnt_user_db->expiration_date, pnt_user_db->vpn_expiration_date, pnt_user_db->email, pnt_user_db->closing_date);
                                
                        } 
                }
                
                if(headered)
                        printf(border_buffer);
                
                if(to_delete)
                {
			sprintf(mail_buffer, "%s</table>\n", mail_buffer);
                        printf("\n\nThe following %d records have to be DELETED from the %s DB. ", to_delete, database);
                        
                        if(prompt_on_delete)
                        {
                                printf("Would you like to proceed? [Y/n]\n");
                                
                                // char ch;
                                
                                // printf("PRESS KEY: %c\n", ch);
                                
                                if(prompted_to_insert)
                                        (void) getchar();
                                
                                if((ch = getchar()) == 'n')
                                {
                                        sprintf(mail_buffer, "%s<p>%d results NOT DELETED from the %s DB.</p><br>", mail_buffer, to_delete, database); 
                                        to_delete = 0;
                                }
                                else
                                        sprintf(mail_buffer, "%s<p>%d results DELETED from the %s DB.</p><br>", mail_buffer, to_delete, database); 
                                        
                                #ifdef DEBUG_MODE 
                                printf("PRESSED KEY: %c\n", ch);
                                #endif
                        }
			else
				sprintf(mail_buffer, "%s<p>%d results DELETED from the %s DB.</p><br>", mail_buffer, to_delete, database);

                        printf("\n\n");
                }
        }
        
        // sprintf(mail_buffer, "%s</table>\n<p>%d results %s DELETED from the %s DB.</p><br>", mail_buffer, to_delete, to_delete ? "" : "NOT", database); 
          
        register unsigned char _to_insert = 0;
        register unsigned char _to_update = 0;
        
        register unsigned char inserted = 0;
        register unsigned char updated = 0;
        
        MYSQL_STMT * this_stmt = NULL;
        
        #ifdef DEBUG_MODE
        printf("to_insert: %d\nto_update: %d, to_delete: %d, ch: %c\n", to_insert, to_update, to_delete, ch);
        #endif
        
        if(to_insert || to_update)
        {
                for(i=0; i<line_num; ++i)
                {
                        pnt_user = &users[i];
                        
                        _to_insert = to_insert && pnt_user->iduser_idm == INSERT_CODE;
                        _to_update = to_update && pnt_user->iduser_idm >= 0;
                        
                        if(_to_insert || _to_update)
                        {
                        
                                #ifdef DEBUG_MODE
                                printf("i: %d\n", i);
                                printf("pnt_user->username: %s\n", pnt_user->username);
                                #endif
                                strcpy(p_username, pnt_user->username);
                                p_username_length = strlen(p_username);
                                #ifdef DEBUG_MODE
                                printf("pnt_user->name: %s\n", pnt_user->name);
                                #endif
                                strcpy(p_name, pnt_user->name);
                                p_name_length = strlen(p_name);
                                #ifdef DEBUG_MODE
                                printf("pnt_user->surname: %s\n", pnt_user->surname);
                                #endif
                                strcpy(p_surname, pnt_user->surname);
                                p_surname_length = strlen(p_surname);
                                #ifdef DEBUG_MODE
                                printf("pnt_user->uid: %d\n", pnt_user->uid);
                                #endif
                                p_uid = pnt_user->uid;
                                #ifdef DEBUG_MODE
                                printf("pnt_user->gid: %d\n", pnt_user->gid);
                                #endif
                                p_gid = pnt_user->gid;
                                #ifdef DEBUG_MODE
                                printf("pnt_user->group_names: %s\n", pnt_user->group_names);
                                #endif
                                strcpy(p_group_names, pnt_user->group_names);
                                p_group_names_length = strlen(p_group_names);

				#ifdef DEBUG_MODE
				printf("pnt_user->is_mach_user: %s\n", pnt_user->is_mach_user);
				#endif

				p_is_mach_user = pnt_user->is_mach_user;
                                
				if(pnt_user->creation_date && strcmp(pnt_user->creation_date, NULL_IDENTIFIER) && strlen(pnt_user->creation_date))
                                {

                                        is_null_creation_date = 0;
                                        #ifdef DEBUG_MODE
                                        printf("creation_date not null\n");
                                        printf("pnt_user->creation_date: %s\n", pnt_user->creation_date);
                                        #endif
                                        strcpy(p_creation_date, pnt_user->creation_date);
                                        p_creation_date_length = strlen(p_creation_date);
                                }
                                else
                                {
                                        #ifdef DEBUG_MODE
                                        printf("creation_date null\n");
                                        #endif
                                        is_null_creation_date = 1;
                                        p_creation_date_length = 0;
                                }

                                if(pnt_user->expiration_date && strcmp(pnt_user->expiration_date, NULL_IDENTIFIER) && strlen(pnt_user->expiration_date))
                                {
                                        
                                        is_null_expiration_date = 0;
                                        #ifdef DEBUG_MODE
                                        printf("exp not null\n");
                                        printf("pnt_user->expiration_date: %s\n", pnt_user->expiration_date);
                                        #endif
                                        strcpy(p_expiration_date, pnt_user->expiration_date);
                                        p_expiration_date_length = strlen(p_expiration_date);
                                }
                                else
                                {
                                        #ifdef DEBUG_MODE
                                        printf("exp null\n");
                                        #endif
                                        is_null_expiration_date = 1;
                                        p_expiration_date_length = 0;
                                }
                                
                                if(pnt_user->vpn_expiration_date && strcmp(pnt_user->vpn_expiration_date, NULL_IDENTIFIER) && strlen(pnt_user->vpn_expiration_date))
                                {
                                        is_null_vpn_expiration_date = 0;
                                        #ifdef DEBUG_MODE
                                        printf("vpn_exp not null\n");
                                        printf("pnt_user->vpn_expiration_date: %s\n", pnt_user->vpn_expiration_date);
                                        #endif
                                        strcpy(p_vpn_expiration_date, pnt_user->vpn_expiration_date);
                                        p_vpn_expiration_date_length = strlen(p_vpn_expiration_date);
                                }
                                else
                                {
                                        #ifdef DEBUG_MODE
                                        printf("vpn_exp null\n");
                                        #endif
                                        is_null_vpn_expiration_date = 1;
                                        p_vpn_expiration_date_length = 0;
                                }
                                
                                if(strcmp(pnt_user->email, NULL_IDENTIFIER))
                                {
                                        is_null_email = 0;
                                        #ifdef DEBUG_MODE
                                        printf("mail not null\n");
                                        printf("pnt_user->email: %s\n", pnt_user->email);
                                        #endif
                                        strcpy(p_email, pnt_user->email);
                                        p_email_length = strlen(p_email);
                                }
                                else
                                {
                                        #ifdef DEBUG_MODE
                                        printf("mail null\n");
                                        #endif
                                        is_null_email = 1;
                                        p_email_length = 0;
                                }
                               
                                if(strcmp(pnt_user->closing_date, NULL_IDENTIFIER))
                                {
                                        
                                        is_null_closing_date = 0;
                                        #ifdef DEBUG_MODE
                                        printf("closing_date not null\n");
                                        printf("pnt_user->closing_date: %s\n", pnt_user->closing_date);
                                        #endif
                                        strcpy(p_closing_date, pnt_user->closing_date);
                                        p_closing_date_length = strlen(p_closing_date);
                                }
                                else
                                {
                                        #ifdef DEBUG_MODE
                                        printf("closing_date null\n");
                                        #endif
                                        is_null_closing_date = 1;
                                        p_closing_date_length = 0;
                                }
                        
                                if(_to_update)
                                {
                                        #ifdef DEBUG_MODE
                                        printf("i: %d\n", i);
                                        printf("iduser_idm: %d\n", pnt_user->iduser_idm);
                                        #endif
                                        p_iduser_idm = pnt_user->iduser_idm; // 
                                        this_stmt = is_null_closing_date ? update_stmt : update_pres_stmt;
                                }
                                else
                                        this_stmt = insert_stmt;
                            
                                if((status = mysql_stmt_execute(this_stmt)))
                                {             
                                        fprintf(stderr, "Error on mysql_stmt_execute: %d\n", status);
                                        fprintf(stderr, "Error: %s\n", mysql_stmt_error(this_stmt));
                                        exit(1);
                                }
                                
                                if(_to_insert)
                                        inserted = 1;
                                else
                                        updated = 1;
                        
                        }
                        
                        
                }
                
                if(inserted || updated)
                {
                    if(inserted)
                        printf("[%s] %d records have been successfully INSERTED into the %s DB.\n\n", gettime(NULL), to_insert, database);
                    if(updated)
                        printf("[%s] %d records have been successfully UPDATED into the %s DB.\n\n", gettime(NULL), to_update, database);
                        
                    /*
                    if(is_mail_active)
                    {
                        sendmail(from_mail, to_mail, mail_cmd_to, mail_buffer);
                        
                        #ifdef DEBUG_MODE
                            printf("mail_buffer length: %ld\n", strlen(mail_buffer));
                        #endif
                    }
                    */
                }
        }
        
        if(to_delete)
        {
                for(j=0; j<local_del_idx; ++j)
                {
                        p_iduser_idm = del_users[j];
                        
                        if((status = mysql_stmt_execute(delete_stmt)))
                        {             
                                fprintf(stderr, "Error on mysql_stmt_execute: %d\n", status);
                                fprintf(stderr, "Error: %s\n", mysql_stmt_error(this_stmt));
                                exit(1);
                        }
                }
                
                printf("[%s] %d records have been successfully DELETED from the %s DB.\n\n", gettime(NULL), to_delete, database);
                
        }
        
        if((inserted || updated || to_delete) && is_mail_active)
        {
                sendmail(from_mail, to_mail, mail_cmd_to, mail_buffer);
                
                #ifdef DEBUG_MODE
                    printf("mail_buffer length: %ld\n", strlen(mail_buffer));
                #endif
        }
           
        mysql_stmt_close(insert_stmt);
        mysql_stmt_close(update_stmt);
        mysql_stmt_close(update_pres_stmt);
        mysql_stmt_close(delete_stmt);
        mysql_close(conn); 
        
        
        
        #undef is_mail_active 
                        
        return 0;
}


