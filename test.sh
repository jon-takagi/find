#!/usr/bin/env bash

g++-8 -O3 -Werror -Wall -Wextra -pedantic  -std=c++17 myfind.cc -o test_find -lstdc++fs || exit

for bin in find ./test_find
do
  echo Running tests with ${bin}
  mkdir ${bin}.out &> /dev/null
  rm ${bin}.out/*

  (cd tstdir; ${bin}; cd ..) &> ${bin}.out/no-args.out
  ${bin} tstdir &> ${bin}.out/one-arg.out
  ${bin} tstdir tstdir &> ${bin}.out/one-arg-dup.out
  ${bin} tstdir tstdir/dir1 &> ${bin}.out/two-args.out
  ${bin} nodir &> ${bin}.out/nodir.out
done

###############

echo "Comparing test outputs"

for out in find.out/*.out
do
  echo TESTING: $out ...
  diff $out ${bin}.out
done
