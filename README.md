On enterprise Linux distros, the SS command should be installed by default... If not, the iproute2 package is the one to look for.  

The scripy must be ran as sudo, as the command: ss -antlp   will require sudo privileges to get the PID of the current processes. 

To run this software, you'll need to have a few dependencies installed on the system. There is a bash script provided which will automatically install them, and it is called envCheck.sh. 

The source code of the C file will need to be amended, as it hardcodes the mysql connection string inside the file. This is by design, as it means that I can call the compiled code without checking for passwords every time it's being run. 

You will need to ammend this connection string two times: 

```
   if ((strcmp(argv[1], "-s") == 0)){
        if (mysql_real_connect(mysql, "localhost", "username", "password",
           NULL, 0, NULL, 0) == NULL){
```



To do: 

  Write a make file
  Create SQL stored procedures
  Implement this API as part of a larger network monitoring suite- including ifstat
  
