
IF EXIST zrus.to del zrus.to

:START

IF EXIST zrus.to GOTO END
IF EXIST test001.log GOTO SKIPTEST001
PathFinding.exe test001.txt > test001.log
:SKIPTEST001

IF EXIST zrus.to GOTO END
IF EXIST test002.log GOTO SKIPTEST002
PathFinding.exe test002.txt > test002.log
:SKIPTEST002

:END

