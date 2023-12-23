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
#define INSERT_GROUP "CALL insert_group(?,?,?,?,?)"
#define UPDATE_GROUP "CALL update_group(?,?,?,?,?,?)"
#define DELETE_GROUP "CALL delete_group(?)"
#define SELECT_ALL_GROUPS "CALL select_all_groups( )"

// typedef bool my_bool;

enum
{
   
    P_IN_NAME,
    P_IN_DESC,
    P_IN_STATUS,
    P_IN_GID,
    P_IN_CREATION_DATE,
    MAX_IN_GROUP_IDM_PARAMS
} insert_in_group_idm_enum;

#define MAX_INSERT_GROUP_PARAMS MAX_IN_GROUP_IDM_PARAMS

enum
{
    P_INOUT_IDGROUP_IDM,
    P_INOUT_NAME,
    P_INOUT_DESC,
    P_INOUT_STATUS,
    P_INOUT_GID,
    P_INOUT_CREATION_DATE,
    MAX_INOUT_GROUP_IDM_PARAMS
} insert_up_group_idm_enum;

#define MAX_UPDATE_GROUP_PARAMS MAX_INOUT_GROUP_IDM_PARAMS
#define MAX_DELETE_GROUP_PARAMS (P_INOUT_IDGROUP_IDM+1)
#define MAX_SELECT_ALL_GROUPS_PARAMS MAX_INOUT_GROUP_IDM_PARAMS

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

#define GROUPSIDM_SERVER "127.0.0.1"
#define GROUPSIDM_GROUP "root"
#define GROUPSIDM_PASSWORD "root"
#define GROUPSIDM_DATABASE "database"

#define DEFAULT_INPUT_FILE "group_idm.csv"

#define MAX_BUFTIME_LENGTH 160

#define MAX_PASSWORD_LEN 512


#define MAX_NAME_LEN 32
#define MAX_DESC_LEN 512
#define MAX_DATE_LEN 100

#define _MAX_NAME_LEN 16
#define _MAX_DESC_LEN 16
#define _MAX_DATE_LEN 16

#define MAX_LINE_LEN 2*(MAX_NAME_LEN+MAX_DESC_LEN+MAX_DATE_LEN+6) // added 2* for safety
#define MAX_BUFLINE_LEN MAX_LINE_LEN
#define NUMBERS_FIXED_LEN 16 // 21 // 16

#define MAX_GROUPS 128

#define NULL_IDENTIFIER "None" // python-like, "NULL" is C-like, but it doesn't preserve consistency between Python-generated .csv input_file and group_idm DB's table.
#define INSERT_CODE -99
#define STAY_CODE -98
#define DELETE_CODE -97
#define UPDATE_PRES_CODE -96

#define NO_DELETE_CODE 2

typedef struct _idm_group_t
{
        int idgroup_idm; //  
        char name[MAX_NAME_LEN];
        char desc[MAX_DESC_LEN];
	char creation_date[MAX_DATE_LEN];
        char status;
        int gid;
} idm_group_t;

/*
typedef struct _id_idm_group_t
{
        int idgroup_idm;
        idm_group_t idm_group;
} id_idm_group_t;
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

int main(int argc, char *argv[])
{
        const unsigned char prompt_on_insert = argc > 1 ? atoi(argv[1]): DEFAULT_PROMPT_ON_INSERT;
        const unsigned char prompt_on_update = argc > 2 ? atoi(argv[2]): DEFAULT_PROMPT_ON_UPDATE;
        const unsigned char prompt_on_delete = argc > 3 ? atoi(argv[3]): DEFAULT_PROMPT_ON_DELETE;
	const int max_groups = argc > 4 ? atoi(argv[4]) : MAX_GROUPS;
        const char * input_file = argc > 5 ? argv[5] : DEFAULT_INPUT_FILE;
        const char * server = argc > 6 ? argv[6] : GROUPSIDM_SERVER;
        const char * group = argc > 7 ? argv[7] : GROUPSIDM_GROUP;
        const char * password = argc > 8 ? argv[8] : GROUPSIDM_PASSWORD;
        const char * database = argc > 9 ? argv[9] : GROUPSIDM_DATABASE;
        const char * mail_cmd = argc > 10 ? argv[10] : DEFAULT_MAIL_COMMAND;
        const char * from_mail = argc > 11 ? argv[11] : DEFAULT_FROM_MAIL;
        const char * to_mail = argc > 12 ? argv[12] : NULL;

        // char * token;

        #ifdef DEBUG_MODE
        char psw_asterisks[MAX_PASSWORD_LEN];
        char *token_aux = psw_asterisks;
        for(const char * token = password; *token != '\0'; ++token, ++token_aux)
            *token_aux = '*';
        *token_aux = '\0';

        printf("prompt_on_insert: %u\nprompt_on_update: %u\nprompt_on_delete: %u\nmax_groups: %d\ninput_file: %s\nserver: %s\ngroup: %s\npassword: %s\ndatabase: %s\nmail_cmd: %s\nfrom_mail: %s\nto_mail: %s\n",
                prompt_on_insert, prompt_on_update, prompt_on_delete, max_groups, input_file, server, group, psw_asterisks, database, mail_cmd, from_mail, to_mail);
       
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
        if (!mysql_real_connect(conn, server, group, password, database, 0, NULL, 0))
        {
                fprintf(stderr, "Error on mysql_real_connect!\n");
                exit(1);
        }

        MYSQL_STMT * insert_stmt;
        MYSQL_STMT * update_stmt; 
        MYSQL_STMT * delete_stmt;
        MYSQL_STMT * select_stmt;
        MYSQL_RES  *prepare_meta_result;
        MYSQL_BIND ps_insert_group_params[MAX_INSERT_GROUP_PARAMS];
        MYSQL_BIND ps_update_group_params[MAX_UPDATE_GROUP_PARAMS]; 
        MYSQL_BIND ps_delete_group_params[MAX_DELETE_GROUP_PARAMS];
        // MYSQL_BIND out_params[MAX_INSERT_GROUP_PARAMS];

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

        int status = mysql_stmt_prepare(insert_stmt, INSERT_GROUP, strlen(INSERT_GROUP));

        #ifdef DEBUG_MODE
        printf("\nstatus of insert_stmt prepare: %d\n", status);
        #endif

        status = mysql_stmt_prepare(update_stmt, UPDATE_GROUP, strlen(UPDATE_GROUP));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_stmt prepare: %d\n", status);
        #endif
         
        status = mysql_stmt_prepare(delete_stmt, DELETE_GROUP, strlen(DELETE_GROUP));
        
        #ifdef DEBUG_MODE
        printf("\nstatus of delete_stmt prepare: %d\n", status);
        #endif
        
        status = mysql_stmt_prepare(select_stmt, SELECT_ALL_GROUPS, strlen(SELECT_ALL_GROUPS));

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
        memset(ps_insert_group_params, 0, sizeof(ps_insert_group_params));
        memset(ps_update_group_params, 0, sizeof(ps_update_group_params));
        memset(ps_delete_group_params, 0, sizeof(ps_delete_group_params));
        // memset(out_params, 0, sizeof(out_params));

        #ifdef DEBUG_MODE
        printf("\n\n *** AFTER MEMSET AND BEFORE ALL PS_PARAMS STUFFS *** \n\n");
        #endif
        
        unsigned long ul_zero_value = 0;
        
        // idgroup_idm
        int p_idgroup_idm = 0;
        ps_update_group_params[P_INOUT_IDGROUP_IDM].buffer_type = MYSQL_TYPE_LONG;
        ps_update_group_params[P_INOUT_IDGROUP_IDM].buffer = (int *) &p_idgroup_idm;
        ps_update_group_params[P_INOUT_IDGROUP_IDM].length = &ul_zero_value;
        ps_update_group_params[P_INOUT_IDGROUP_IDM].is_null = 0;
         
        ps_delete_group_params[P_INOUT_IDGROUP_IDM].buffer_type = MYSQL_TYPE_LONG;
        ps_delete_group_params[P_INOUT_IDGROUP_IDM].buffer = (int *) &p_idgroup_idm;
        ps_delete_group_params[P_INOUT_IDGROUP_IDM].length = &ul_zero_value;
        ps_delete_group_params[P_INOUT_IDGROUP_IDM].is_null = 0;
         
        // name
        unsigned long p_name_length = 0;
        char p_name[MAX_NAME_LEN];
        ps_insert_group_params[P_IN_NAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_group_params[P_IN_NAME].buffer = (char *) p_name; 
        ps_insert_group_params[P_IN_NAME].buffer_length = MAX_NAME_LEN;
        ps_insert_group_params[P_IN_NAME].is_null = 0;
        ps_insert_group_params[P_IN_NAME].length = &p_name_length;
        
        ps_update_group_params[P_INOUT_NAME].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_group_params[P_INOUT_NAME].buffer = (char *) p_name; 
        ps_update_group_params[P_INOUT_NAME].buffer_length = MAX_NAME_LEN;
        ps_update_group_params[P_INOUT_NAME].is_null = 0;
        ps_update_group_params[P_INOUT_NAME].length = &p_name_length;
        
        // desc
        unsigned long p_desc_length = 0;
        char p_desc[MAX_DESC_LEN];
        ps_insert_group_params[P_IN_DESC].buffer_type = MYSQL_TYPE_STRING; 
        ps_insert_group_params[P_IN_DESC].buffer = (char *) p_desc; 
        ps_insert_group_params[P_IN_DESC].buffer_length = MAX_DESC_LEN;
        ps_insert_group_params[P_IN_DESC].is_null = 0;
        ps_insert_group_params[P_IN_DESC].length = &p_desc_length;
        
        ps_update_group_params[P_INOUT_DESC].buffer_type = MYSQL_TYPE_STRING; 
        ps_update_group_params[P_INOUT_DESC].buffer = (char *) p_desc; 
        ps_update_group_params[P_INOUT_DESC].buffer_length = MAX_DESC_LEN;
        ps_update_group_params[P_INOUT_DESC].is_null = 0;
        ps_update_group_params[P_INOUT_DESC].length = &p_desc_length;

	// status
        char p_status = 0;
        ps_insert_group_params[P_IN_STATUS].buffer_type = MYSQL_TYPE_TINY;
        ps_insert_group_params[P_IN_STATUS].buffer = (char *) &p_status;
        ps_insert_group_params[P_IN_STATUS].length = &ul_zero_value;
        ps_insert_group_params[P_IN_STATUS].is_null = 0;

        ps_update_group_params[P_INOUT_STATUS].buffer_type = MYSQL_TYPE_TINY;
        ps_update_group_params[P_INOUT_STATUS].buffer = (int *) &p_status;
        ps_update_group_params[P_INOUT_STATUS].length = &ul_zero_value;
        ps_update_group_params[P_INOUT_STATUS].is_null = 0;

	// gid
        int p_gid = 0;
        ps_insert_group_params[P_IN_GID].buffer_type = MYSQL_TYPE_LONG;
        ps_insert_group_params[P_IN_GID].buffer = (int *) &p_gid;
        ps_insert_group_params[P_IN_GID].length = &ul_zero_value;
        ps_insert_group_params[P_IN_GID].is_null = 0;

        ps_update_group_params[P_INOUT_GID].buffer_type = MYSQL_TYPE_LONG;
        ps_update_group_params[P_INOUT_GID].buffer = (int *) &p_gid;
        ps_update_group_params[P_INOUT_GID].length = &ul_zero_value;
        ps_update_group_params[P_INOUT_GID].is_null = 0;

	// creation_date
        my_bool is_null_creation_date;
        unsigned long p_creation_date_length = 0;
        char p_creation_date[MAX_DATE_LEN];
        ps_insert_group_params[P_IN_CREATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_insert_group_params[P_IN_CREATION_DATE].buffer = (char *) p_creation_date;
        ps_insert_group_params[P_IN_CREATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_insert_group_params[P_IN_CREATION_DATE].length = &p_creation_date_length;
        ps_insert_group_params[P_IN_CREATION_DATE].is_null = &is_null_creation_date;

	ps_update_group_params[P_INOUT_CREATION_DATE].buffer_type = MYSQL_TYPE_STRING;
        ps_update_group_params[P_INOUT_CREATION_DATE].buffer = (char *) p_creation_date;
        ps_update_group_params[P_INOUT_CREATION_DATE].buffer_length = MAX_DATE_LEN;
        ps_update_group_params[P_INOUT_CREATION_DATE].length = &p_creation_date_length;
        ps_update_group_params[P_INOUT_CREATION_DATE].is_null = &is_null_creation_date;



        #ifdef DEBUG_MODE
        printf("\nbefore mysql_stmt_bind_param.\n");
        #endif
        
        if((status = mysql_stmt_bind_param(insert_stmt, ps_insert_group_params)))
        {
                fprintf(stderr, "insert_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of insert_stmt mysql_stmt_bind_param: %d\n", status);
        #endif
        
        if((status = mysql_stmt_bind_param(update_stmt, ps_update_group_params)))
        {
                fprintf(stderr, "update_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of update_stmt mysql_stmt_bind_param: %d\n", status);
        #endif

        if((status = mysql_stmt_bind_param(delete_stmt, ps_delete_group_params)))
        {
                fprintf(stderr, "delete_stmt mysql_stmt_bind_param failed\n");
                exit(1);
        }
        
        #ifdef DEBUG_MODE
        printf("\nstatus of delete_stmt mysql_stmt_bind_param: %d\n", status);
        #endif
        
        if((status = mysql_stmt_bind_result(select_stmt, ps_update_group_params)))
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
        int del_groups[max_groups];
        idm_group_t groups[max_groups];
        idm_group_t * pnt_group = NULL;
        
        printf("\n");
        
        (void) fgets(buf, MAX_LINE_LEN, fp); // exclude header line
        
        for(line_num=0; fgets(buf, MAX_LINE_LEN, fp); ++line_num)
        {
                pnt_group = &groups[line_num];
                
                pnt_group->idgroup_idm = STAY_CODE;
                // printf("[%d]: %s", line_num, buf);
                token = buf;           
          
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("name is: %s\n", token);
                #endif
                strcpy(pnt_group->name, token);
                token = token2+1;
                for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                #ifdef DEBUG_MODE
                printf("desc is: %s\n", token);
                #endif
                strcpy(pnt_group->desc, token);
                token = token2+1; 
        
		for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0' && *token2 != '\n'; ++token2);
                *token2 = '\0';
		aux = atoi(token);
                #ifdef DEBUG_MODE
                printf("status: %d\n", pnt_group->status);
                #endif
		pnt_group->status = aux;
                token = token2+1;

		for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *token2 = '\0';
                aux = atoi(token);
                #ifdef DEBUG_MODE
                printf("gid is: %d\n", aux);
                #endif
                pnt_group->gid = aux;
                token = token2+1;

		for(token2 = token; *token2 != CSV_SEPARATOR && *token2 != '\0'; ++token2);
                *(token2-1) = '\0';
                #ifdef DEBUG_MODE
                printf("creation_date is: %s\n", token);
                #endif
                strcpy(pnt_group->creation_date, token);
                token = token2+1;


	}
        
        fclose(fp);
        printf("[%s] Total line num: %d\n", gettime(NULL), line_num);
        
        int i, j;
        int rows;
        int result;
        
        idm_group_t groups_db[max_groups];
        
        for(rows=0; !(result = mysql_stmt_fetch(select_stmt)); ++rows)
        {
                pnt_group = &groups_db[rows]; // &(groups_db[rows].idm_group);
                
                #ifdef DEBUG_MODE
                printf("PRE %d, %s, %s, %d, %d, %s\n", p_idgroup_idm, p_name, p_desc, p_status, p_gid, p_creation_date);
                #endif
               
                pnt_group->idgroup_idm = p_idgroup_idm; // groups_db[rows].idgroup_idm = p_idgroup_idm; // pnt_group->idgroup_idm = p_idgroup_idm;
                strcpy(pnt_group->name, p_name);
                strcpy(pnt_group->desc, p_desc);
                pnt_group->status = p_status;
                pnt_group->gid = p_gid;
		strcpy(pnt_group->creation_date, is_null_creation_date ? NULL_IDENTIFIER : p_creation_date);
                              
                #ifdef DEBUG_MODE
                printf("POST %d, %s, %s, %d, %d\n\n", pnt_group->idgroup_idm, pnt_group->name, pnt_group->desc, pnt_group->status, pnt_group->gid, pnt_group->creation_date);
                #endif
                
        }
        
        for(; mysql_next_result(conn) == 0;);
        mysql_stmt_close(select_stmt); // close select_stmt
        
        #ifdef DEBUG_MODE
        printf("mysql_stmt_fetch: %d\n", result);
        #endif
        
        printf("[%s] Total rows num: %d\n", gettime(NULL), rows);
        
        idm_group_t * pnt_group_db = NULL;
        
        /*
        id_idm_group_t groups_idm_to_update[max_groups];
        idm_group_t groups_idm_to_insert[max_groups];
        */
        
        // COMPARISON CODE
        printf("[%s] line_num: %d\n\n", gettime(NULL), line_num);
        
        for(i=0; i<line_num; ++i)
        {
                pnt_group = &groups[i];
                for(j=0; j<rows; ++j)
                {
                        pnt_group_db = &groups_db[j];
                        // printf("idm_gid: %d, db_gid: %d\n", pnt_group->uid, pnt_group_db->uid);
                        if(!strcmp(pnt_group->name, pnt_group_db->name)) // pnt_group->gid == pnt_group_db->gid)
                        {
                                
				// printf("groupname: %s\n", pnt_group->groupname);
                                //#ifdef DEBUG_MODE
                                //printf("Hallelujah\n");
				//#endif
				
				//#ifdef DEBUG_MODE

				// printf("-----------\n");

				/*
				if(strcmp(pnt_group->name, pnt_group_db->name))
					printf("name differs\n");
				*/

				if(strcmp(pnt_group->desc, pnt_group_db->desc))
				{
					pnt_group->idgroup_idm = pnt_group_db->idgroup_idm;
                                        printf("[%s] desc differs\n", pnt_group->name);
					printf("IDM: %s, DB: %s\n", pnt_group->desc, pnt_group_db->desc);
				}

				if(pnt_group->status != pnt_group_db->status)
				{
					pnt_group->idgroup_idm = pnt_group_db->idgroup_idm;
					printf("[%s] status differs\n", pnt_group->name);
                                        printf("IDM: %d, DB: %d\n", pnt_group->status, pnt_group_db->status);
				}

				if(pnt_group->gid != pnt_group_db->gid)
				{
					pnt_group->idgroup_idm = pnt_group_db->idgroup_idm;
					printf("[%s] gid differs\n", pnt_group->name);
					printf("IDM: %d, DB: %d\n", pnt_group->gid, pnt_group_db->gid);
				}

				if(strcmp(pnt_group->creation_date, pnt_group_db->creation_date))
                                {
                                        pnt_group->idgroup_idm = pnt_group_db->idgroup_idm;
                                        printf("[%s] creation_date differs\n", pnt_group->name);
                                        printf("IDM: %s, DB: %s\n", pnt_group->creation_date, pnt_group_db->creation_date);
                                }

                                break; // do it in any case, if idgroup_idm field of pnt_group structure still hasn't been valued from there, however it should have the very initial value, STAY_CODE.
                        }    
                }
              
                if(j == rows)
		{
			// (j == rows) // insert only if it has a non-null closing_date.
			#ifdef DEBUG_MDOE
			printf("insert code for %s\n", pnt_group->groupname);
			#endif
			-- pnt_group->idgroup_idm; // from STAY_CODE to INSERT_CODE.
		}
        }
        
        int local_del_idx = 0;
        
        for(j=0; j<rows; ++j)
        {
                pnt_group_db = &groups_db[j];
                for(i=0; i<line_num; ++i)
                {
                        pnt_group = &groups[i];
                        if(!strcmp(pnt_group->name, pnt_group_db->name))
                                break;
                }
                
                if(i == line_num)
                {
                        del_groups[local_del_idx] = pnt_group_db->idgroup_idm;
                        ++ local_del_idx;
                        pnt_group_db->idgroup_idm = DELETE_CODE;
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
        int padLens[3];
        int numbersPadLens[2];
        
        // char mail_format_buf[_MAX_MAIL_LEN];
        
	strcpy(mail_buffer, "");

        sprintf(buffer, "| name%*.*s| desc%*.*s| status%*.*s| gid%*.*s| creation_date%*.*s|\n", 11, 11, padding, 11, 11, padding, 9, 9, padding, 12, 12, padding, 2, 2, padding);
        sprintf(border_buffer, "+%*.*s+%*.*s+%*.*s+%*.*s+%*.*s+\n", _MAX_NAME_LEN, _MAX_NAME_LEN, border_padding, _MAX_DESC_LEN, _MAX_DESC_LEN, border_padding, NUMBERS_FIXED_LEN, NUMBERS_FIXED_LEN, border_padding, NUMBERS_FIXED_LEN, NUMBERS_FIXED_LEN, border_padding, _MAX_DATE_LEN, _MAX_DATE_LEN, border_padding);

        for(i=0; i<line_num; ++i)
        {

                pnt_group = &groups[i];
                // printf("idgroup_idm: %d\n", pnt_group->idgroup_idm);
                if(pnt_group->idgroup_idm == INSERT_CODE) // INSERT_CODE
                {
                        if(!headered)
                        {
                                sprintf(mail_buffer, "<table style=\"background-color: black; color: #adff29;\"><tr style=\"color: red; font-weight: bold;\"><th>name</th><th>desc</th><th>status</th><th>gid</th><th>creation_date</th></tr>\n");
                                headered = 1;
                                printf(border_buffer);
                                printf(buffer);
                                printf(border_buffer);
                        }
                
                        ++ to_insert;
                       
                        padLens[0] = _MAX_NAME_LEN - strlen(pnt_group->name) -1;
                        
                        if(padLens[0] <= 0)
                                padLens[0] = 0;
                        
                        padLens[1] = _MAX_DESC_LEN - strlen(pnt_group->desc) -1;
                        
                        if(padLens[1] <= 0)
                                padLens[1] = 0;           

			padLens[2] = _MAX_DATE_LEN - strlen(pnt_group->creation_date) -1;
              
                        sprintf(p_numbers[0], "%d", pnt_group->status);
                        numbersPadLens[0] = NUMBERS_FIXED_LEN - strlen(p_numbers[0]);
                        sprintf(p_numbers[1], "%d", pnt_group->gid);
                        numbersPadLens[1] = NUMBERS_FIXED_LEN - strlen(p_numbers[1]);
                       
                        printf("| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s|%s%*.*s|\n", pnt_group->name, padLens[0], padLens[0], padding, pnt_group->desc, padLens[1], padLens[1], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_group->creation_date, padLens[2], padLens[2], padding); 	
                        
                        sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%s</td></tr>\n", mail_buffer, pnt_group->name, pnt_group->desc, pnt_group->status, pnt_group->gid, pnt_group->creation_date);
                        
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
                pnt_group = &groups[i];
                // printf("idgroup_idm: %d\n", pnt_group->idgroup_idm);
                if(pnt_group->idgroup_idm >= 0) // UPDATE_CODE
                {
			if(!headered)
                        {
                                sprintf(mail_buffer, "%s<table style=\"background-color: black; color: #adff29;\"><tr style=\"color: red; font-weight: bold;\"><th>name</th><th>desc</th><th>status</th><th>gid</th><th>creation_date</th></tr>\n", mail_buffer);
                                headered = 1;
                                printf(border_buffer);
                                printf(buffer);
                                printf(border_buffer);
                        }

                        ++ to_update;
                        
                        padLens[0] = _MAX_NAME_LEN - strlen(pnt_group->name) -1;

                        if(padLens[0] <= 0)
                                padLens[0] = 0;

                        padLens[1] = _MAX_DESC_LEN - strlen(pnt_group->desc) -1;

                        if(padLens[1] <= 0)
                                padLens[1] = 0;

			padLens[2] = _MAX_DATE_LEN - strlen(pnt_group->creation_date) -1;

                        sprintf(p_numbers[0], "%d", pnt_group->status);
                        numbersPadLens[0] = NUMBERS_FIXED_LEN - strlen(p_numbers[0]);
                        sprintf(p_numbers[1], "%d", pnt_group->gid);
                        numbersPadLens[1] = NUMBERS_FIXED_LEN - strlen(p_numbers[1]);

                        printf("| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s|%s%*.*s|\n", pnt_group->name, padLens[0], padLens[0], padding, pnt_group->desc, padLens[1], padLens[1], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_group->creation_date, padLens[2], padLens[2], padding);

                        sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td><td>%s</td></tr>\n", mail_buffer, pnt_group->name, pnt_group->desc, pnt_group->status, pnt_group->gid, pnt_group->creation_date);

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
                        pnt_group_db = &groups_db[j];
                        // printf("idgroup_idm: %d\n", pnt_group->idgroup_idm);
                        if(pnt_group_db->idgroup_idm == DELETE_CODE) // UPDATE_CODE
                        {
                               
				if(!headered)
                        	{
                             		sprintf(mail_buffer, "%s<table style=\"background-color: black; color: #adff29;\"><tr style=\"color: red; font-weight: bold;\"><th>name</th><th>desc</th><th>status</th><th>gid</th><th>creation_date</th></tr>\n", mail_buffer);
                             		headered = 1;
                           	        printf(border_buffer);
                          		printf(buffer);
                        		printf(border_buffer);
                       		}

                        	++ to_delete;
                        
                        	padLens[0] = _MAX_NAME_LEN - strlen(pnt_group->name) -1;

                    	    	if(padLens[0] <= 0)
                        	        padLens[0] = 0;

                        	padLens[1] = _MAX_DESC_LEN - strlen(pnt_group->desc) -1;

                        	if(padLens[1] <= 0)
                                	padLens[1] = 0;

				padLens[2] = _MAX_DATE_LEN - strlen(pnt_group->creation_date) -1;

                        	sprintf(p_numbers[0], "%d", pnt_group->status);
                        	numbersPadLens[0] = NUMBERS_FIXED_LEN - strlen(p_numbers[0]);
                        	sprintf(p_numbers[1], "%d", pnt_group->gid);
                        	numbersPadLens[1] = NUMBERS_FIXED_LEN - strlen(p_numbers[1]);

                        	printf("| %.15s%*.*s| %.15s%*.*s|%*.*s%s|%*.*s%s|%*.*s%s|\n", pnt_group->name, padLens[0], padLens[0], padding, pnt_group->desc, padLens[1], padLens[1], padding, numbersPadLens[0], numbersPadLens[0], padding, p_numbers[0], numbersPadLens[1], numbersPadLens[1], padding, p_numbers[1], pnt_group->creation_date, padLens[2], padLens[2], padding);

                        	sprintf(mail_buffer, "%s<tr><td>%.15s</td><td>%.15s</td><td>%d</td><td>%d</td></tr>\n", mail_buffer, pnt_group->name, pnt_group->desc, pnt_group->status, pnt_group->gid);
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
                        pnt_group = &groups[i];
                        
                        _to_insert = to_insert && pnt_group->idgroup_idm == INSERT_CODE;
                        _to_update = to_update && pnt_group->idgroup_idm >= 0;
                        
                        if(_to_insert || _to_update)
                        {
                       
                                #ifdef DEBUG_MODE
                                printf("pnt_group->name: %s\n", pnt_group->name);
                                #endif
                                strcpy(p_name, pnt_group->name);
                                p_name_length = strlen(p_name);
                                #ifdef DEBUG_MODE
                                printf("pnt_group->desc: %s\n", pnt_group->desc);
                                #endif
                                strcpy(p_desc, pnt_group->desc);
                                p_desc_length = strlen(p_desc);
                                #ifdef DEBUG_MODE
                                printf("pnt_group->status: %d\n", pnt_group->status);
                                #endif
                                p_status = pnt_group->status;
                                #ifdef DEBUG_MODE
                                printf("pnt_group->gid: %d\n", pnt_group->gid);
                                #endif
                                p_gid = pnt_group->gid;
                               
				if(pnt_group->creation_date && strcmp(pnt_group->creation_date, NULL_IDENTIFIER) && strlen(pnt_group->creation_date))
                                {

                                        is_null_creation_date = 0;
                                        #ifdef DEBUG_MODE
                                        printf("creation_date not null\n");
                                        printf("pnt_group->creation_date: %s\n", pnt_group->creation_date);
                                        #endif
                                        strcpy(p_creation_date, pnt_group->creation_date);
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

                                if(_to_update)
                                {
                                        #ifdef DEBUG_MODE
                                        printf("i: %d\n", i);
                                        printf("idgroup_idm: %d\n", pnt_group->idgroup_idm);
                                        #endif
                                        p_idgroup_idm = pnt_group->idgroup_idm; // 
                                        this_stmt = update_stmt;
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
                        p_idgroup_idm = del_groups[j];
                        
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
        mysql_stmt_close(delete_stmt);
        mysql_close(conn); 
        
        
        
        #undef is_mail_active 
                        
        return 0;
}


