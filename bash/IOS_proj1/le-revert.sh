#!/bin/sh
if [ -f ./.le/.config ] ; then
  if [ -n "`grep '^ignore' ./.le/.config`" ] ; then
    ign_lst="`grep '^ignore' ./.le/.config | sed "s/ignore //" | tr '\n' '|' | sed 's/.$//'`"
  fi
fi
cd "./.le/"
if [ $# -eq 0 ] ; then
  if [ -z "$ign_lst" ] ; then
    fls_arr=$(find -maxdepth 1 -type f 2>/dev/null | sed 's/^.*\///' | grep -vE '^\..*')
  else
    fls_arr=$(find -maxdepth 1 -type f 2>/dev/null | sed 's/^.*\///' | grep -vE '^\..*' | grep -vE "$ign_lst")
  fi
else
  if [ -z "$ign_lst" ] ; then
    fls_arr=$(find "$@" 2>/dev/null | grep -vE '^\..*')
  else
    fls_arr=$(find "$@" 2>/dev/null | grep -vE '^\..*' | grep -vE "$ign_lst")
  fi
fi
cd ".."
for i in ${fls_arr[@]} ; do
  cp ./.le/$i ./
done
