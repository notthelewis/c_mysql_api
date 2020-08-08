#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <mysql/mysql.h>
#include <stdbool.h>

#define STRING_SIZE 60
#define INSERT_NEW "INSERT INTO analytics.new_live (state, recv, sent, machine_id, foreign_addr, port, process) VALUES (?,?,?,?,?,?,?)"

// For the next version
#define CHECKEXISTS "SELECT EXISTS(SELECT * FROM new_live WHERE port = ? AND process = ?)"

/*      Compile with:
                gcc db.connect.c `mysql_config --libs` -O1
                for the best results
*/

void send_help(char ** argv)
{
        printf("\n%s inputs data into MySQL, and takes up to 4 paramaters.\n", argv[0]);
        printf("The first paramater must be the data_sent row, which is put into the Database, acompanied by the other rows thereafter.\n");
        printf("There are two exceptions to this, the case of sending the help command (-help, --help, -h)  \n");
        printf("And the -s flag.\n");
        printf("If the -s flag is sent, the program will send the next argument directly to the database. \n");
        printf("Otherwise, the program will attempt to fill in the same Query defined in the source code. \n\n");
        printf("The prewritten INSERT REQUIRES 5 FIELDS: \n \n");
        printf("(data_sent INT, machine_id VARCHAR(33), foreign_addres VARCHAR(15), port INT, connection_state VARCHAR(50))\n");
        printf("Without these parameters, the program will fail. \n\n");
}

int main(int argc, char ** argv)
{
        MYSQL *mysql=  mysql_init(NULL);
        MYSQL_STMT     *stmt;
        MYSQL_BIND     bind[7];
        int                param_count;

        /* Declerations for the each row's column
         * char arrays require a length to bind.
         * machine_id will always be 33, the other two will vary.
         */

        char               state[12];
        unsigned long  state_len;

        int                recv;
        int                sent;

        char           machine_id[33];
        unsigned long  machine_id_len = 33;

        char           foreign_addr[15];
        unsigned long  foreign_addr_len;

        int            port;
        int            process;

        if ((strcmp(argv[1], "-help") == 0) || strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0){
                send_help(argv);
                return 0;
        }


        // Single query mode, designed to send a single query, extraneous to the rest of the prog
        if ((strcmp(argv[1], "-s") == 0)){
                if (mysql_real_connect(mysql, "localhost", "root", "Slapmyassandcallmejulie99",
                   NULL, 0, NULL, 0) == NULL){
                        fprintf(stderr, "%s\n", mysql_error(mysql));
                        mysql_close(mysql);
                        exit(1);
                }
                if (mysql_query(mysql, argv[2])) {
                        fprintf(stderr, "%s\n", mysql_error(mysql));
                        mysql_close(mysql);
                        exit(1);
                }
                return 0;
        }

        // If single query mode isn't selected, --help hasn't been passed and there's not enough (or too many) arguments passed- fail with err
        if (argc < 7 || argc > 8) {
                printf("Incorrect number of parameters\n");
                return 0;
        }
        if (mysql_real_connect(mysql, "localhost", "root", "Slapmyassandcallmejulie99",
           NULL, 0, NULL, 0) == NULL){
                fprintf(stderr, "%s\n", mysql_error(mysql));
                mysql_close(mysql);
                exit(1);
        }


        stmt = mysql_stmt_init(mysql);
        if (!stmt){
                fprintf(stderr, " mysql_stmt_init(), out of memory\n");
                return 0;
        }

        /* Prepare an INSERT query with 7 parameters */
        if (mysql_stmt_prepare(stmt, INSERT_NEW, strlen(INSERT_NEW))){
                fprintf(stderr, " mysql_stmt_prepare(), INSERT failed\n");
                fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
                return 0;
        }


        /* Bind data for all 7 parameters */
        memset(bind, 0, sizeof(bind));


        /* STRING PARAM */
        bind[0].buffer_type= MYSQL_TYPE_STRING;
        bind[0].buffer= (char *)state;
        bind[0].buffer_length= STRING_SIZE;
        bind[0].is_null= 0;
        bind[0].length= &state_len;


        /* INTEGER PARAM */
        bind[1].buffer_type= MYSQL_TYPE_LONG;
        bind[1].buffer= (char *)&recv;
        bind[1].is_null= 0;
        bind[1].length= 0;

        /* INTEGER PARAM */
        bind[2].buffer_type= MYSQL_TYPE_LONG;
        bind[2].buffer= (char *)&sent;
        bind[2].is_null= 0;
        bind[2].length= 0;

        /* STRING PARAM */
        bind[3].buffer_type= MYSQL_TYPE_STRING;
        bind[3].buffer= (char *)machine_id;
        bind[3].buffer_length= STRING_SIZE;
        bind[3].is_null= 0;
        bind[3].length= &machine_id_len;

        /* STRING PARAM */
        bind[4].buffer_type= MYSQL_TYPE_STRING;
        bind[4].buffer= (char *)foreign_addr;
        bind[4].buffer_length= STRING_SIZE;
        bind[4].is_null= 0;
        bind[4].length= &foreign_addr_len;

        /* INTEGER PARAM */
        bind[5].buffer_type= MYSQL_TYPE_LONG;
        bind[5].buffer= (char *)&port;
        bind[5].is_null= 0;
        bind[5].length= 0;

        /* INTEGER PARAM */
        bind[6].buffer_type= MYSQL_TYPE_LONG;
        bind[6].buffer= (char *)&process;
        bind[6].is_null= 0;
        bind[6].length= 0;

        if (mysql_stmt_bind_param(stmt, bind)){
                fprintf(stderr, " mysql_stmt_bind_param() failed\n");
                fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
                return 0;
        }

        /* Specify the data values */

        /* string */
        strncpy(state, argv[1], 12);
        state_len= 12;

        recv= atoi(argv[2]);    /* integer */
        sent= atoi(argv[3]);    /* integer */

        /* string */
        strncpy(machine_id, argv[4], 33);
        machine_id_len= strlen(machine_id);

        /* string */
        strncpy(foreign_addr, argv[5], 15);
        foreign_addr_len= strlen(foreign_addr);

        port= atoi(argv[6]);    /* integer */
        process= atoi(argv[7]); /* integer */



        /* Execute the INSERT statement */
        if (mysql_stmt_execute(stmt))
        {
                fprintf(stderr, " mysql_stmt_execute(), 1 failed\n");
                fprintf(stderr, " %s\n", mysql_stmt_error(stmt));
                return 0;
        }

        if (mysql_stmt_close(stmt)){
                  fprintf(stderr, " failed while closing the statement\n");
                  fprintf(stderr, " %s\n", mysql_error(mysql));
                  return 0;
        }

        return 0;

}
