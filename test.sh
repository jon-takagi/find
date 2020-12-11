#!/usr/bin/env bash

g++-8 -O3 -Werror -Wall -Wextra -pedantic  -std=c++17 simplifind.cpp -o test_find -lstdc++fs || exit

for bin in find /vagrant/os/find/test_find
do
  echo Running tests with ${bin}
  mkdir ${bin}.out &> /dev/null
  rm ${bin}.out/*

  # (cd tstdir; ${bin}; cd ..) &> ${bin}.out/no-args.out
  # ${bin} tstdir &> ${bin}.out/one-arg.out
  # ${bin} tstdir tstdir &> ${bin}.out/one-arg-dup.out
  # ${bin} tstdir/b tstdir/a &> ${bin}.out/two-args.out
  # ${bin} tstdir/dir &> ${bin}.out/missing-dir.out
  # ${bin} tstdir -name &> ${bin}.out/missing-name.out
  # ${bin} tstdir -name "*.txt" &> ${bin}.out/name.out
  # ${bin} tstdir -name *RE?D* &> ${bin}.out/regex.out
  # ${bin} tstdir -type f -exec file '{}' \; &> ${bin}.out/file-exec.out
  # ${bin} tstdir -exec &> ${bin}.out/exec_noargs.out
  ${bin} tstdir -exec ls '{}' \; &> ${bin}.out/exec_ls.out
  # ${bin} tstdir -exec ls '{}' \; -print &> ${bin}.out/exec_ls_print.out
  # ${bin} tstdir -print -exec ls '{}' \; &> ${bin}.out/print_exec_ls.out
  # ${bin} tstdir -print -print -print &> ${bin}.out/multiprint.out
  # ${bin} tstdir -exec cat '{}' \; &> ${bin}.out/exec_cat.out
done

###############

echo "Comparing test outputs"

for out in find.out/*.out
do
  echo TESTING: $out ...
  diff $out ${bin}.out
done
