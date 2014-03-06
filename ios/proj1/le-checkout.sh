#!/bin/sh
if [  $# -ne 1 ] ; then
  echo "Parametr nebyl zadan spravne"
  exit 1
elif [ ! -d "$1" ] ; then
  echo "Adresar neexistuje"
  exit 1
fi
if [ -d "./.le" ] ; then
  cd "./.le"
  rm * >&/dev/null
  cd ".."
else
  mkdir ".le"
fi
if [ -f ./.le/.config ] ; then
  grep '^projdir' ./.le/.config > /dev/null
  if [ $? -eq 0 ] ; then
    sed "s|projdir.*|projdir $1|" -i ./.le/.config
  else
    echo "projdir $1" >> ./.le/.config
  fi
else
  echo "projdir $1" > ./.le/.config
fi
if [ -n "`grep '^ignore' ./.le/.config`" ] ; then
  ign_lst="`grep '^ignore' ./.le/.config | sed "s/ignore //" | tr '\n' '|' | sed 's/.$//'`"
fi
if [ -z "$ign_lst" ] ; then
  cp $1/* ./ >&/dev/null
  cp $1/* ./.le/ >&/dev/null
else
  cpy_arr=(`find $1/ -maxdepth 1 -type f 2>/dev/null | sed 's/^.*\///' | grep -vE '^\..*' | grep -vE "$ign_lst"`)
  for i in ${cpy_arr[@]} ; do
    cp $1/$i ./ >&/dev/null
    cp $1/$i ./.le >&/dev/null
  done
fi
