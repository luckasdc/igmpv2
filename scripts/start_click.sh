#!/bin/sh

cd /home/student/click/scripts/

/home/student/click/userlevel/click glue.click &
/home/student/click/userlevel/click -p 10001 router.click &
/home/student/click/userlevel/click -p 10002 server.click &
/home/student/click/userlevel/click -p 10003 client21.click &
#/home/student/click/userlevel/click -p 10004 client22.click &
#/home/student/click/userlevel/click -p 10005 client31.click &
#/home/student/click/userlevel/click -p 10006 client32.click &

wait
