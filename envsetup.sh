#!/bin/bash

export MCR_HOME=$( cd "$( dirname "${BASH_SOURCE[0]}" )" && pwd )
echo "MCR_HOME set to ${MCR_HOME}"
alias mcd="cd \"${MCR_HOME}\""
alias ubu="ubuntu-sdk"
alias pane="tmux resize-pane"
alias a="git add --all ; status"
alias c="git commit -sm"
alias t="tig --all"
alias status="git status"
alias push="git push"
alias pull="git pull"
alias fetch="git fetch --all"
alias q="qmake -o Makefile *.pro"
alias v="vim -S \"${MCR_HOME}/vimrc\" -p"
alias m="make"

alias msed="sed -i''"
# Adding space
alias refs="msed \"/\&=/! s/\&/ \& /g\""
alias andeq="msed \"s/\&=/ \&= /g\""
alias pts="msed -E \"/(\*=)|(\*\!)|(\/\*)|(\*\/)/! s/\*/ \* /g\""
alias multeq="msed \"s/\*=/ \*= /g\""
alias cospace="msed \"/^#/! s/\,/, /g\""
alias leftpar="msed \"/^#/! s/(/ ( /g\""
alias rightpar="msed \"/^#/! s/)/ ) /g\""
alias leftbr="msed \"s/\[/ \[ /g\""
alias rightbr="msed \"s/\]/ \] /g\""
alias excl="msed -E \"/(\/!)|(\!=)|(\*\!)/! s/\!/ \! /g\""
alias inspace="msed \"s/\!=/ \!= /g\""
alias semicol="msed \"s/;/ ; /g\""
# Removing extra space
alias tabs="msed \"/\t \+\*/! s/\t \+/\t/g\""
alias comms="msed \"s/ \+,/,/g\""
alias dots="msed \"s/ \+\./\./g\""
alias derefs="msed \"s/ \+->/->/g\""
alias lines="msed \"/^ \+\*/! s/^ \+//g\""
alias linesend="msed \"s/\t* *$//g\""
alias trapap="msed -E \"/(^ *\* *\*\/)|(\*=)/! s/\* *\*/\*\*/g\""
alias trapref="msed \"/\&=/! s/\& *\&/\&\&/g\""
alias single="msed \"s/ \+/ /g\""
# Recover block comment spacing
# alias recomment="msed \"s/^\*/ \*/g\""
# alias commbeg="msed \"s,/ \+\*,/\*,g\""
# alias commend="msed \"s,\* \+/,\*/,g\""
# alias commdoxy="msed \"s,\* \+\!,\*\!,g\""
# Clean out backup files
alias cleanbak="find \"${MCR_HOME}\" -name '*.bak' -exec rm '{}' \;"

function sedall() {
  cp "$1" "$1.bak"
  echo >> "$1"
  msed -e :a -e '/^\n*$/{$d;N;ba' -e '}' "$1"
  refs "$1"
  andeq "$1"
  pts "$1"
  multeq "$1"
  cospace "$1"
  leftpar "$1"
  rightpar "$1"
  leftbr "$1"
  rightbr "$1"
  excl "$1"
  inspace "$1"
  semicol "$1"

  tabs "$1"
  comms "$1"
  dots "$1"
  derefs "$1"
  lines "$1"
  linesend "$1"
  trapap "$1"
  trapap "$1"
  trapref "$1"
  trapref "$1"
  single "$1"

#  recomment "$1"
#  commbeg "$1"
#  commend "$1"
#  commdoxy "$1"
}

# easy sedall for all source files
function sourcesed() {
  for i in $(find . -name "$1")
  do
    sedall $i
  done
}

function sourcesedall() {
  sourcesed '*.h'
  sourcesed '*.hpp'
  sourcesed '*.hxx'
  sourcesed '*.c'
  sourcesed '*.cpp'
}
alias ssa='sourcesedall'

