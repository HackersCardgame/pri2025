while [ 1 ]
do
  file=~/$(date +%y%m%d%H%M).log
  ./phaseServer | tee $file
  echo KILLING remaining processes
  sleep 1
  pgrep phaseServer | xargs kill -9

  echo RESTARTING SERVER
  sleep 1
done

