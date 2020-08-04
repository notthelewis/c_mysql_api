#!/usr/bin/env sh

# Format of file:
# Proto,Recv-Q,Send-Q,Local,Address,Foreign,Address,State

# MySQL CREATE COMMANDS:
# create table analytics.live (dat_sent int, machine_id VARCHAR(33), foreign_addr VARCHAR(15), port INT, con_state VARCHAR(50))ENGINE=MEMORY;
# create table analytics.history (dat_sent int, machine_id VARCHAR(33), foreign_addr VARCHAR(15), port INT, con_state VARCHAR(50));

# CREATE TABLE analytics.new_live (state VARCHAR(12), recv INT, sent INT, machine_id VARCHAR(15), foreign_addr VARCHAR(15), port INT, process VARCHAR(10))ENGINE=MEMORY;
# CREATE TABLE analytics.new_history (state VARCHAR(12), recv INT, sent INT, machine_id VARCHAR(15), foreign_addr VARCHAR(15), port INT, process VARCHAR(10))ENGINE=MEMORY;


# The sed command deletes the two header lines
# netstat -atu -4  --numeric-hosts --numeric-ports|sed '1,2d'  > example_data
ss -antlp4 | sed '1,2d' > example_data


# Replaces all whitespace in the file with a '|' which is to be used as a delimiter
cat example_data | sed -e 's/[[:space:]]\+/|/g' > new_data
machine_id=`cat /etc/machine-id`

epoch_time=$(date +%s)
previous=`cat previous_epoch`
time_passed=$(($epoch_time - $previous))

# If it's been longer than a minute
#   Copy over analytics.live into analytics.history
#   Truncate analytics.live

if [ $time_passed -gt 60 ];
then
    echo $epoch_time > previous_epoch
    query="INSERT INTO analytics.new_history (state, recv, sent, machine_id, foreign_addr, port, process) SELECT * FROM analytics.new_live"
    ./a.out -s "$query"

    query="TRUNCATE analytics.new_live"
    ./a.out -s "$query"
fi


# CREATE TABLE analytics.new_live (state VARCHAR(12), recv INT, sent INT, machine_id VARCHAR(15), foreign_addr VARCHAR(15), port INT, process VARCHAR(250))ENGINE=MEMORY;
file=new_data
IFS='|'
while read -r state recv sent lcl_addr frn_addr proc
do
    port=`echo $lcl_addr| cut -d ':' -f2`
    frn_ip=`echo $frn_addr| cut -d ':' -f1`
        pid=`echo $proc|awk -F, '{print $2}'| cut -d'=' -f2`

    ./a.out $state $recv $sent $machine_id $frn_ip $port $pid


done < "$file"
