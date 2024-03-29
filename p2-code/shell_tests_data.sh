#!/bin/bash

function tick() {               # command to pause input
    sleep $ticktime
}

function server_spawn () {
    server="${tid}-serv"                   # set server name
    server_out="${server}.${valg}out"      # set server output name
    if [ ! "$valg" = "" ]; then
        outfiles+=("$server_out")
    fi
    $valg ${valg_opts} ./bin/bl-server $server >& ${server_out} & server_pid=$!
}

function server_close () {
    kill $server_pid
    wait $server_pid
}

function client_spawn () {
  client=$1
#  echo spawning "$client"
  client_out="${tid}-${client}.${valg}out"
  client_fifo="${tid}-${client}.fifo"
  outfiles+=("$client_out")
  rm -f ${client_fifo}
  mkfifo ${client_fifo}
  ./cat-sig.sh <> $client_fifo > \
    >($valg ${valg_opts} ./bin/bl-client $server $client |& ./normalize.awk >& \
    ${client_out} ) &  pid=$!
  eval ${client}_pid=$pid
#  echo client "$client $pid" spawned
}

function client_print () {
  client=$1
  mesg="$2"
  echo -e "$mesg" > ${tid}-${client}.fifo
#  echo -e "$mesg" ${tid}-${client}.fifo
  # printf "%s\n" "$mesg" > ${client}.fifo
}

function client_close () {
  client=$1
#  echo closing "$client"
  client_pid="${client}_pid"
#  echo "$client_pid" ${!client_pid}
  kill "${!client_pid}"
  wait "${!client_pid}"
  rm -f ${tid}-${client}.fifo
#  echo client "$client" closed
}


T=0                             # global test number

# Server start/stop
((T++))
tnames[T]="server-only"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
tick
tick
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
EOF

# Server and single client, no messages
((T++))
tnames[T]="1client-1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_close Bruce
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --
Bruce>> 
EOF

# # Server and single client, single message
# ((T++))
# tnames[T]="1client-2"
# read -r -d '' setup[$T] <<"EOF"
# EOF
# read -r -d '' actions[$T] <<"EOF"
# server_spawn
# client_spawn Bruce
# client_print Bruce "This is a test\n"
# client_close Bruce
# server_close
# EOF
# read -r -d '' teardown[$T] <<"EOF"
# EOF
# read -r -d '' expect_outs[$T] <<"EOF"
# -- Bruce JOINED --
# [Bruce] : This is a test
# Bruce>> 
# EOF

# Server and single client, multiple messages
((T++))
tnames[T]="1client-2"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_print Bruce "This is a test\n"
client_print Bruce "Is anyone there\n"
client_print Bruce "Alfreeeed!\n"
client_close Bruce
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --
[Bruce] : This is a test
[Bruce] : Is anyone there
[Bruce] : Alfreeeed!
Bruce>> 
EOF

# Server and two clients, no messages
((T++))
tnames[T]="2client-1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_close Bruce
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --
-- Clark JOINED --	-- Bruce DEPARTED --
Bruce>> 	Clark>> 
EOF

# Server and two clients, single message
((T++))
tnames[T]="2client-2"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Bruce "Hey big guy\n" 
client_close Bruce
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --
-- Clark JOINED --	[Bruce] : Hey big guy
[Bruce] : Hey big guy	-- Bruce DEPARTED --
Bruce>> 	Clark>> 
EOF

# Server and two clients, multiple messages
((T++))
tnames[T]="2client-3"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Bruce "Hey big guy\n" 
client_print Bruce "What's happen'?\n" 
client_print Clark "Not much, how about you?\n" 
client_print Bruce "Hangin' out..." 
client_print Bruce "In the BATCAVE" 
client_print Clark "I figured..." 
client_print Clark "I'm out" 
client_close Clark
client_close Bruce
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --
-- Clark JOINED --	[Bruce] : Hey big guy
[Bruce] : Hey big guy	[Bruce] : What's happen'?
[Bruce] : What's happen'?	[Clark] : Not much, how about you?
[Clark] : Not much, how about you?	[Bruce] : Hangin' out...
[Bruce] : Hangin' out...	[Bruce] : In the BATCAVE
[Bruce] : In the BATCAVE	[Clark] : I figured...
[Clark] : I figured...	[Clark] : I'm out
[Clark] : I'm out	Clark>> 
-- Clark DEPARTED --	
Bruce>> 	
EOF

# Server and two clients, multiple messages, one client remains
((T++))
tnames[T]="2client-4"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Bruce "Hello\n" 
client_print Clark "Yo\n"
client_print Bruce "bye\n" 
client_close Bruce
client_print Clark "guess I'm all alone\n"
client_print Clark "this is lame\n"
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --
-- Clark JOINED --	[Bruce] : Hello
[Bruce] : Hello	[Clark] : Yo
[Clark] : Yo	[Bruce] : bye
[Bruce] : bye	-- Bruce DEPARTED --
Bruce>> 	[Clark] : guess I'm all alone
	[Clark] : this is lame
	Clark>> 
EOF


# Server and two clients, multiple messages, one client remains
((T++))
tnames[T]="2client-5"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Bruce "Hello\n" 
client_print Clark "Yo\n"
client_print Bruce "bye\n" 
client_print Clark "No\n"
client_print Bruce "Yep\n" 
client_print Clark "Wrong\n"
client_print Bruce "Hah\n" 
client_close Clark
client_print Bruce "grrr\n"
client_print Bruce "grrr\n"
client_close Bruce
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --
-- Clark JOINED --	[Bruce] : Hello
[Bruce] : Hello	[Clark] : Yo
[Clark] : Yo	[Bruce] : bye
[Bruce] : bye	[Clark] : No
[Clark] : No	[Bruce] : Yep
[Bruce] : Yep	[Clark] : Wrong
[Clark] : Wrong	[Bruce] : Hah
[Bruce] : Hah	Clark>> 
-- Clark DEPARTED --	
[Bruce] : grrr	
[Bruce] : grrr	
Bruce>> 	
EOF

# Server and two clients, multiple messages, one client remains
((T++))
tnames[T]="2client-6"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Bruce "Hello\n" 
client_print Clark "Yo\n"
client_print Bruce "bye\n" 
client_print Clark "No\n"
client_print Bruce "Yep\n" 
client_print Clark "Wrong\n"
client_print Bruce "Hah\n" 
client_print Bruce "grrr\n"
client_print Bruce "grrr\n"
client_close Bruce
client_print Clark "Ugh\n"
client_print Clark "Ugh\n"
client_print Clark "sigh..\n"
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --
-- Clark JOINED --	[Bruce] : Hello
[Bruce] : Hello	[Clark] : Yo
[Clark] : Yo	[Bruce] : bye
[Bruce] : bye	[Clark] : No
[Clark] : No	[Bruce] : Yep
[Bruce] : Yep	[Clark] : Wrong
[Clark] : Wrong	[Bruce] : Hah
[Bruce] : Hah	[Bruce] : grrr
[Bruce] : grrr	[Bruce] : grrr
[Bruce] : grrr	-- Bruce DEPARTED --
Bruce>> 	[Clark] : Ugh
	[Clark] : Ugh
	[Clark] : sigh..
	Clark>> 
EOF

# Server and two clients, multiple messages, one client rejoins
((T++))
tnames[T]="2client-rejoin1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Bruce "Hello\n" 
client_print Clark "Yo\n"
client_print Bruce "bye\n" 
client_close Bruce
client_print Clark "Ugh\n"
client_spawn Bruce
client_print Bruce "back\n" 
client_print Clark "great\n"
client_print Clark "gone\n"
client_print Bruce "ack\n" 
client_close Clark
client_print Bruce "alone\n" 
client_close Bruce
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --	-- Bruce JOINED --
[Bruce] : back	[Bruce] : Hello	[Bruce] : back
[Clark] : great	[Clark] : Yo	[Clark] : great
[Clark] : gone	[Bruce] : bye	[Clark] : gone
[Bruce] : ack	-- Bruce DEPARTED --	[Bruce] : ack
-- Clark DEPARTED --	[Clark] : Ugh	-- Clark DEPARTED --
[Bruce] : alone	-- Bruce JOINED --	[Bruce] : alone
Bruce>> 	[Bruce] : back	Bruce>> 
	[Clark] : great	
	[Clark] : gone	
	[Bruce] : ack	
	Clark>> 	
EOF

# # Server and two clients, multiple messages, two clients rejoins
# ((T++))
# tnames[T]="2client-rejoin2"
# read -r -d '' setup[$T] <<"EOF"
# EOF
# read -r -d '' actions[$T] <<"EOF"
# server_spawn
# client_spawn Bruce
# client_print Bruce "alone\n" 
# client_spawn Clark
# client_print Bruce "Hello\n" 
# client_print Clark "Yo\n"
# client_print Bruce "bye\n" 
# client_close Bruce
# client_print Clark "Ugh\n"
# client_spawn Bruce
# client_print Bruce "back\n" 
# client_print Clark "great\n"
# client_print Clark "gone\n"
# client_print Bruce "ack\n" 
# client_close Clark
# client_print Bruce "alone\n" 
# client_spawn Clark
# client_print Bruce "joy\n" 
# client_close Bruce
# client_close Clark
# server_close
# EOF
# read -r -d '' teardown[$T] <<"EOF"
# EOF
# read -r -d '' expect_outs[$T] <<"EOF"
# -- Bruce JOINED --	-- Clark JOINED --	-- Bruce JOINED --	-- Clark JOINED --
# [Bruce] : back	[Bruce] : joy	[Bruce] : back	[Bruce] : joy
# [Clark] : great	-- Bruce DEPARTED --	[Clark] : great	-- Bruce DEPARTED --
# [Clark] : gone	Clark>> 	[Clark] : gone	Clark>> 
# [Bruce] : ack		[Bruce] : ack	
# -- Clark DEPARTED --		-- Clark DEPARTED --	
# [Bruce] : alone		[Bruce] : alone	
# -- Clark JOINED --		-- Clark JOINED --	
# [Bruce] : joy		[Bruce] : joy	
# Bruce>> 		Bruce>> 	
# EOF

# Three clients, no messages
((T++))
tnames[T]="3clients-1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
client_spawn Bruce
client_spawn Lois
client_close Bruce
client_close Lois
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --	-- Bruce JOINED --	-- Lois JOINED --
-- Bruce JOINED --	-- Lois JOINED --	-- Bruce DEPARTED --
-- Lois JOINED --	Bruce>> 	Lois>> 
-- Bruce DEPARTED --		
-- Lois DEPARTED --		
Clark>> 		
EOF

# Three clients, one message
((T++))
tnames[T]="3clients-2"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
client_spawn Bruce
client_spawn Lois
client_print Bruce "Batman!\n"
client_close Bruce
client_close Lois
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --	-- Bruce JOINED --	-- Lois JOINED --
-- Bruce JOINED --	-- Lois JOINED --	[Bruce] : Batman!
-- Lois JOINED --	[Bruce] : Batman!	-- Bruce DEPARTED --
[Bruce] : Batman!	Bruce>> 	Lois>> 
-- Bruce DEPARTED --		
-- Lois DEPARTED --		
Clark>> 		
EOF

# Three clients, several messages
((T++))
tnames[T]="3clients-3"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
client_spawn Bruce
client_spawn Lois
client_print Bruce "Batman!\n"
client_print Clark "Superman!\n"
client_print Lois "Reporter?\n"
client_print Bruce "Ha\n"
client_print Clark "He he\n"
client_print Lois "bite me\n"
client_close Lois
client_close Clark
client_close Bruce
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --	-- Bruce JOINED --	-- Lois JOINED --
-- Bruce JOINED --	-- Lois JOINED --	[Bruce] : Batman!
-- Lois JOINED --	[Bruce] : Batman!	[Clark] : Superman!
[Bruce] : Batman!	[Clark] : Superman!	[Lois] : Reporter?
[Clark] : Superman!	[Lois] : Reporter?	[Bruce] : Ha
[Lois] : Reporter?	[Bruce] : Ha	[Clark] : He he
[Bruce] : Ha	[Clark] : He he	[Lois] : bite me
[Clark] : He he	[Lois] : bite me	Lois>> 
[Lois] : bite me	-- Lois DEPARTED --	
-- Lois DEPARTED --	-- Clark DEPARTED --	
Clark>> 	Bruce>> 	
EOF

# Three clients, several messages before/after full join
((T++))
tnames[T]="3clients-4"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
client_print Clark "Superman!\n"
client_spawn Bruce
client_print Bruce "Hey!\n"
client_print Clark "Superman!\n"
client_spawn Lois
client_print Bruce "Batman!\n"
client_print Clark "Superman!\n"
client_print Lois "Not again\n"
client_print Bruce "Vanish!\n"
client_close Bruce
client_print Clark "He he\n"
client_print Lois "Lame\n"
client_print Clark "Yup\n"
client_close Lois
client_print Clark "Fortress of solitude\n"
client_close Clark
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --	-- Bruce JOINED --	-- Lois JOINED --
[Clark] : Superman!	[Bruce] : Hey!	[Bruce] : Batman!
-- Bruce JOINED --	[Clark] : Superman!	[Clark] : Superman!
[Bruce] : Hey!	-- Lois JOINED --	[Lois] : Not again
[Clark] : Superman!	[Bruce] : Batman!	[Bruce] : Vanish!
-- Lois JOINED --	[Clark] : Superman!	-- Bruce DEPARTED --
[Bruce] : Batman!	[Lois] : Not again	[Clark] : He he
[Clark] : Superman!	[Bruce] : Vanish!	[Lois] : Lame
[Lois] : Not again	Bruce>> 	[Clark] : Yup
[Bruce] : Vanish!		Lois>> 
-- Bruce DEPARTED --		
[Clark] : He he		
[Lois] : Lame		
[Clark] : Yup		
-- Lois DEPARTED --		
[Clark] : Fortress of solitude		
Clark>> 		
EOF

# Three clients with rejoins and messages
((T++))
tnames[T]="3clients-rejoin1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark1
client_print Clark1 "Superman!\n"
client_spawn Bruce1
client_print Bruce1 "Batman!\n"
client_print Clark1 "Superman!\n"
client_close Bruce1
client_spawn Lois1
client_print Clark1 "Superman!\n"
client_print Lois1 "Not again\n"
client_spawn Bruce2
client_print Bruce2 "Vanish!\n"
client_close Bruce2
client_print Clark1 "He he\n"
client_print Clark1 "Super speed\n"
client_close Clark1
client_print Lois1 "Lame\n"
client_spawn Clark2
client_print Clark2 "Yup - super hearing\n"
client_close Lois1
client_print Clark2 "Fortress of solitude\n"
client_spawn Bruce3
client_print Bruce3 "From the shadows!\n"
client_print Bruce3 "Kryptonite!\n"
client_close Clark2
client_print Bruce3 "Batman!\n"
client_close Bruce3
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark1 JOINED --	-- Bruce1 JOINED --	-- Lois1 JOINED --	-- Bruce2 JOINED --	-- Clark2 JOINED --	-- Bruce3 JOINED --
[Clark1] : Superman!	[Bruce1] : Batman!	[Clark1] : Superman!	[Bruce2] : Vanish!	[Clark2] : Yup - super hearing	[Bruce3] : From the shadows!
-- Bruce1 JOINED --	[Clark1] : Superman!	[Lois1] : Not again	Bruce2>> 	-- Lois1 DEPARTED --	[Bruce3] : Kryptonite!
[Bruce1] : Batman!	Bruce1>> 	-- Bruce2 JOINED --		[Clark2] : Fortress of solitude	-- Clark2 DEPARTED --
[Clark1] : Superman!		[Bruce2] : Vanish!		-- Bruce3 JOINED --	[Bruce3] : Batman!
-- Bruce1 DEPARTED --		-- Bruce2 DEPARTED --		[Bruce3] : From the shadows!	Bruce3>> 
-- Lois1 JOINED --		[Clark1] : He he		[Bruce3] : Kryptonite!	
[Clark1] : Superman!		[Clark1] : Super speed		Clark2>> 	
[Lois1] : Not again		-- Clark1 DEPARTED --			
-- Bruce2 JOINED --		[Lois1] : Lame			
[Bruce2] : Vanish!		-- Clark2 JOINED --			
-- Bruce2 DEPARTED --		[Clark2] : Yup - super hearing			
[Clark1] : He he		Lois1>> 			
[Clark1] : Super speed					
Clark1>> 					
EOF

# Four clients messages
((T++))
tnames[T]="4clients-1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Bruce
client_spawn Clark
client_print Clark "Superman!\n"
client_print Bruce "Batman!\n"
client_print Clark "Superman v Batman\n"
client_print Bruce "No: Batman v Superman\n"
client_print Clark "It would set up Super Friends"
client_spawn Lois
client_print Lois "Gentlemen"
client_print Clark "Lois, what's up\n"
client_print Bruce "I am the night!\n"
client_spawn Barbara 
client_print Lois "Can anyone control him"
client_print Barbara "nope\n"
client_print Clark "not me, keeps throwing green rocks at me\n"
client_print Bruce "because..."
client_print Barbara "no no no...\n"
client_print Bruce "I'M BATMAN"
client_print Clark "nice"
client_print Lois "nope, nope, neeey-ooope"
client_close Lois
client_print Barbara "Keep antagonizing me and watch what happens\n"
client_print Clark "You end up in a wheel chair?\n"
client_print Barbara ">:-(\n"
client_print Bruce "Whoa, that's insensitive"
client_print Clark "Ummm, sorry?"
client_close Clark 
client_print Barbara "And I thought you were rude"
client_print Bruce "I am rude"
client_print Bruce "I'm whatever this city needs me to be"
client_print Barbara "cripes"
client_print Bruce "Because I'm BATMAN"
client_close Bruce
client_print Barbara "I've got to find a different server"
client_close Barbara
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Bruce JOINED --	-- Clark JOINED --	-- Lois JOINED --	-- Barbara JOINED --
-- Clark JOINED --	[Clark] : Superman!	[Lois] : Gentlemen	[Lois] : Can anyone control him
[Clark] : Superman!	[Bruce] : Batman!	[Clark] : Lois, what's up	[Barbara] : nope
[Bruce] : Batman!	[Clark] : Superman v Batman	[Bruce] : I am the night!	[Clark] : not me, keeps throwing green rocks at me
[Clark] : Superman v Batman	[Bruce] : No: Batman v Superman	-- Barbara JOINED --	[Bruce] : because...
[Bruce] : No: Batman v Superman	[Clark] : It would set up Super Friends	[Lois] : Can anyone control him	[Barbara] : no no no...
[Clark] : It would set up Super Friends	-- Lois JOINED --	[Barbara] : nope	[Bruce] : I'M BATMAN
-- Lois JOINED --	[Lois] : Gentlemen	[Clark] : not me, keeps throwing green rocks at me	[Clark] : nice
[Lois] : Gentlemen	[Clark] : Lois, what's up	[Bruce] : because...	[Lois] : nope, nope, neeey-ooope
[Clark] : Lois, what's up	[Bruce] : I am the night!	[Barbara] : no no no...	-- Lois DEPARTED --
[Bruce] : I am the night!	-- Barbara JOINED --	[Bruce] : I'M BATMAN	[Barbara] : Keep antagonizing me and watch what happens
-- Barbara JOINED --	[Lois] : Can anyone control him	[Clark] : nice	[Clark] : You end up in a wheel chair?
[Lois] : Can anyone control him	[Barbara] : nope	[Lois] : nope, nope, neeey-ooope	[Barbara] : >:-(
[Barbara] : nope	[Clark] : not me, keeps throwing green rocks at me	Lois>> 	[Bruce] : Whoa, that's insensitive
[Clark] : not me, keeps throwing green rocks at me	[Bruce] : because...		[Clark] : Ummm, sorry?
[Bruce] : because...	[Barbara] : no no no...		-- Clark DEPARTED --
[Barbara] : no no no...	[Bruce] : I'M BATMAN		[Barbara] : And I thought you were rude
[Bruce] : I'M BATMAN	[Clark] : nice		[Bruce] : I am rude
[Clark] : nice	[Lois] : nope, nope, neeey-ooope		[Bruce] : I'm whatever this city needs me to be
[Lois] : nope, nope, neeey-ooope	-- Lois DEPARTED --		[Barbara] : cripes
-- Lois DEPARTED --	[Barbara] : Keep antagonizing me and watch what happens		[Bruce] : Because I'm BATMAN
[Barbara] : Keep antagonizing me and watch what happens	[Clark] : You end up in a wheel chair?		-- Bruce DEPARTED --
[Clark] : You end up in a wheel chair?	[Barbara] : >:-(		[Barbara] : I've got to find a different server
[Barbara] : >:-(	[Bruce] : Whoa, that's insensitive		Barbara>> 
[Bruce] : Whoa, that's insensitive	[Clark] : Ummm, sorry?		
[Clark] : Ummm, sorry?	Clark>> 		
-- Clark DEPARTED --			
[Barbara] : And I thought you were rude			
[Bruce] : I am rude			
[Bruce] : I'm whatever this city needs me to be			
[Barbara] : cripes			
[Bruce] : Because I'm BATMAN			
Bruce>> 			
EOF


# Shutdown message from server
((T++))
tnames[T]="shutdown-1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
tick
server_close
client_close Clark
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --
!!! server is shutting down !!!
Clark>> 
EOF

# Shutdown message from server
((T++))
tnames[T]="shutdown-2"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
client_spawn Bruce
client_spawn Lois
server_close
client_close Bruce
client_close Clark
client_close Lois
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --	-- Bruce JOINED --	-- Lois JOINED --
-- Bruce JOINED --	-- Lois JOINED --	!!! server is shutting down !!!
-- Lois JOINED --	!!! server is shutting down !!!	Lois>> 
!!! server is shutting down !!!	Bruce>> 	
Clark>> 		
EOF


# Shutdown message from server
((T++))
tnames[T]="shutdown-3"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn Clark
client_spawn Bruce
client_print Bruce "Batman!\n"
client_spawn Lois
client_print Clark "Superman!\n"
server_close
client_close Bruce
client_close Lois
client_close Clark
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- Clark JOINED --	-- Bruce JOINED --	-- Lois JOINED --
-- Bruce JOINED --	[Bruce] : Batman!	[Clark] : Superman!
[Bruce] : Batman!	-- Lois JOINED --	!!! server is shutting down !!!
-- Lois JOINED --	[Clark] : Superman!	Lois>> 
[Clark] : Superman!	!!! server is shutting down !!!	
!!! server is shutting down !!!	Bruce>> 	
Clark>> 		
EOF

# Many clients messages
((T++))
tnames[T]="stress-1"
read -r -d '' setup[$T] <<"EOF"
EOF
read -r -d '' actions[$T] <<"EOF"
server_spawn
client_spawn A
client_spawn B
client_spawn C
client_print C "1\n"
client_print B "2\n"
client_print B "3\n"
client_print A "4\n"
client_spawn D
client_spawn E
client_print A "5\n"
client_print D "6\n"
client_print E "7\n"
client_print C "8\n"
client_spawn F
client_spawn G
client_spawn H
client_print A "9\n"
client_print D "10\n"
client_print E "11\n"
client_print C "12\n"
client_print B "13\n"
client_close C
client_print F "14\n"
client_print G "15\n"
client_print D "16\n"
client_print E "17\n"
client_print B "18\n"
client_close D
client_print F "19\n"
client_print F "20\n"
client_print E "21\n"
client_print A "22\n"
client_close G
client_print B "23\n"
client_close A
client_close B
client_print F "24\n"
client_close E
client_close F
client_print H "25\n"
client_close H
server_close
EOF
read -r -d '' teardown[$T] <<"EOF"
EOF
read -r -d '' expect_outs[$T] <<"EOF"
-- A JOINED --	-- B JOINED --	-- C JOINED --	-- D JOINED --	-- E JOINED --	-- F JOINED --	-- G JOINED --	-- H JOINED --
-- B JOINED --	-- C JOINED --	[C] : 1	-- E JOINED --	[A] : 5	-- G JOINED --	-- H JOINED --	[A] : 9
-- C JOINED --	[C] : 1	[B] : 2	[A] : 5	[D] : 6	-- H JOINED --	[A] : 9	[D] : 10
[C] : 1	[B] : 2	[B] : 3	[D] : 6	[E] : 7	[A] : 9	[D] : 10	[E] : 11
[B] : 2	[B] : 3	[A] : 4	[E] : 7	[C] : 8	[D] : 10	[E] : 11	[C] : 12
[B] : 3	[A] : 4	-- D JOINED --	[C] : 8	-- F JOINED --	[E] : 11	[C] : 12	[B] : 13
[A] : 4	-- D JOINED --	-- E JOINED --	-- F JOINED --	-- G JOINED --	[C] : 12	[B] : 13	-- C DEPARTED --
-- D JOINED --	-- E JOINED --	[A] : 5	-- G JOINED --	-- H JOINED --	[B] : 13	-- C DEPARTED --	[F] : 14
-- E JOINED --	[A] : 5	[D] : 6	-- H JOINED --	[A] : 9	-- C DEPARTED --	[F] : 14	[G] : 15
[A] : 5	[D] : 6	[E] : 7	[A] : 9	[D] : 10	[F] : 14	[G] : 15	[D] : 16
[D] : 6	[E] : 7	[C] : 8	[D] : 10	[E] : 11	[G] : 15	[D] : 16	[E] : 17
[E] : 7	[C] : 8	-- F JOINED --	[E] : 11	[C] : 12	[D] : 16	[E] : 17	[B] : 18
[C] : 8	-- F JOINED --	-- G JOINED --	[C] : 12	[B] : 13	[E] : 17	[B] : 18	-- D DEPARTED --
-- F JOINED --	-- G JOINED --	-- H JOINED --	[B] : 13	-- C DEPARTED --	[B] : 18	-- D DEPARTED --	[F] : 19
-- G JOINED --	-- H JOINED --	[A] : 9	-- C DEPARTED --	[F] : 14	-- D DEPARTED --	[F] : 19	[F] : 20
-- H JOINED --	[A] : 9	[D] : 10	[F] : 14	[G] : 15	[F] : 19	[F] : 20	[E] : 21
[A] : 9	[D] : 10	[E] : 11	[G] : 15	[D] : 16	[F] : 20	[E] : 21	[A] : 22
[D] : 10	[E] : 11	[C] : 12	[D] : 16	[E] : 17	[E] : 21	[A] : 22	-- G DEPARTED --
[E] : 11	[C] : 12	[B] : 13	[E] : 17	[B] : 18	[A] : 22	G>> 	[B] : 23
[C] : 12	[B] : 13	C>> 	[B] : 18	-- D DEPARTED --	-- G DEPARTED --		-- A DEPARTED --
[B] : 13	-- C DEPARTED --		D>> 	[F] : 19	[B] : 23		-- B DEPARTED --
-- C DEPARTED --	[F] : 14			[F] : 20	-- A DEPARTED --		[F] : 24
[F] : 14	[G] : 15			[E] : 21	-- B DEPARTED --		-- E DEPARTED --
[G] : 15	[D] : 16			[A] : 22	[F] : 24		-- F DEPARTED --
[D] : 16	[E] : 17			-- G DEPARTED --	-- E DEPARTED --		[H] : 25
[E] : 17	[B] : 18			[B] : 23	F>> 		H>> 
[B] : 18	-- D DEPARTED --			-- A DEPARTED --			
-- D DEPARTED --	[F] : 19			-- B DEPARTED --			
[F] : 19	[F] : 20			[F] : 24			
[F] : 20	[E] : 21			E>> 			
[E] : 21	[A] : 22						
[A] : 22	-- G DEPARTED --						
-- G DEPARTED --	[B] : 23						
[B] : 23	-- A DEPARTED --						
A>> 	B>> 						
EOF
