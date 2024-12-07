# pri2025

### updating to debian bookworm

## DISCLAMER:
i dont pirate software, movies or music, i just found it funny to programm such a big software. i mean if pirates use Microsoft Windows nobody would sue Microsoft because of that i guess

## ABOVE ABOVE TOP SECRET
first busts of WAREZ and Trader ScENE was about 2010. It's not about poor people in hollywood that were betrayed because of lincese fees. ThE ScENE is a large content distribution network with tech savy people. Reasion is in my SUBJECTIVE opinion that miliaries all over the globe not just have a global assessionation grid but also a global censorship grid. if militaries kill inocent people all over the globe it implies that there is also some sort of censorship infrastructure. often privatized, eg insurance companies that also have insurance for protecting people against legitimate lawsutes.

this censorship infrastructure seems to work based on hash valuse of files that were already indexed. and as soon as one tries to copy, analyze or send such a file to officials (z.B FEDPOL in Switzerland) it automatically enables functions like ICD10 G40.2 on different strenghts. So the reason why autistic people have autistic helmets. maybe this infrastructure also targets federal officers.

And it is likely that it has a quite easy to use API / GUI for violent but not that intelligent people, eg. 

* has an officers degree in Swiss Army
* throws the file in the recycle bin on his windows

-> what would activate global destruction of that file based on hash value

also likely that since the INTEL CPU with built in WiMax that there is some sort of secont network, so if you computer is completely air gapped they can still censor things the would bring murderer with officers degree of local army into civil curt and into prison


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
