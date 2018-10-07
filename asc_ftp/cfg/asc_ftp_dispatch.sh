DYNAMIC_IP="10.161.1.161"

. /pkg01/config/com_profile

if netstat -rn | grep -v grep | grep -q  $DYNAMIC_IP
then

######¡ã¨¹???¡ã¦Ì£¤¡¤??e ##########
PWD_FILE=/pkg01/config/asc_collect/asc_ftp_1to1.pwd
CFG_FILE=/pkg01/config/asc_collect/asc_ftp_dispatch.cfg
TMP_FILE=/datasource101/collect_tmp/list/asc_ftp_dispatch.list
RUN_LOG=/dataanalys101/log/asc_collect/runlog/asc_ftp_dispatch.runlog

BIN_DIR=/pkg01/bin/asc_collect
EXEC=asc_ftp_dispatch
if ps -efx | grep "$BIN_DIR/$EXEC" | grep -v grep | read nouse
then
    echo asc_ftp_dispatch exist!
else
        echo $EXEC run...
        nohup  $BIN_DIR/$EXEC $PWD_FILE $CFG_FILE $TMP_FILE $RUN_LOG 2>&1 > /dev/null &
##       $BIN_DIR/$EXEC $PWD_FILE $CFG_FILE $TMP_FILE $RUN_LOG  
fi

fi