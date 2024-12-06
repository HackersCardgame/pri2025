bind pubm - "*" main
#bind pub - !sites sites

#########################

proc putfast {arg} {
  append arg "\n"
  putnow $arg
}

proc main {nick uhost handle chan arg} {
#  set command "site $arg"
  set line_list [split $arg \n]
  foreach line $line_list {
	  if {[string index $arg 0] == "!" } {
			if {[catch {exec ../bin/phaseIrc $arg $nick } result] == 0} {
				set result_list [split $result \n]
				foreach row $result_list {
				  putserv "PRIVMSG $chan : $row"
				}
			} else {
				putserv "PRIVMSG $chan : ERROR: Something went wrong..."
				putserv "PRIVMSG $chan : Information about it: $::errorInfo"
			}
	  }
  }
}

putlog "pHASE rELEASE iNTERFACE loaded..."
