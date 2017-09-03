#!/usr/bin/env sh

build () {
	source_code="source_code"
	build_output="build_output"

	mkdir -p "$build_output"
	
	flags="$flags -Wall" # show all errors. TODO(oliver): Find out why, there are still disabled warnings despite setting "-Wall", apparently...
	flags="$flags -O0" # do not optimize. faster and more predictable compile during development
	flags="$flags -pedantic-errors" # treat "minor" issues as errors. let us be strict while learning C. 
	flags="$flags -Wextra" # show more warnings than usual ?
	flags="$flags -Werror" # turn warnings into errors. scared while learning C. TODO(oliver): Are those other Warning flags redundant now?
	flags="$flags -std=c89" # as long as I do not know about the differences in the various C standards, I will stick to the (presumably) most portable.

	gcc $flags "$source_code/main.c" -o "$build_output/main"
}

./clean.sh
build
