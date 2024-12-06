# pri2022



As root
1. run Prerequisites, tested on debian buster, bullseye ... no guarantee that you dont have to fix things since things change
2. run ./tools/install/setup.sh
3. run make in ./
4. run unittests.sh in ./
5. have a look at the code
6. make 
7. make install-client installs the client to the system
8. make install-server installs the server to the system (needs 6+ Core!) as a service you can run with systemctl
9. to get the first user with password see server/hashPassword_unittest.cpp and use mysql command to create one (puzzle)

you need the modified cb-ftp: https://github.com/HackersCardgame/cbftp-r983, needs also to be compilet, has also a prerequisistes.sh, the repo is somehow broken, so maybe you need to use the https://github.com/HackersCardgame/cbftp and apply the patch from r983 if the patch still works or then you need to modify manually (C/C++)

initial User ist user02 with password 123456, password is hashed in the database, you can have a look at ./pri2022/src/server/hashPassword_unittest.cpp to see how you need to create the right hash

in the /bin directory run
./phaseClient user02 123456

but first you need to start the server also in the bin directory or you can install the server as systemD startup thingie with make command [1]
./pri2022/bin/startServer.sh 


[1] https://github.com/HackersCardgame/pri2022/blob/main/Makefile#L204-L219
