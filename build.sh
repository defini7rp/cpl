#!/bin/bash

gcc -I include/ src/cpl_hashtable.c src/cpl_utils.c src/cpl_lexer.c src/cpl_parser.c src/cpl_interpreter.c src/cpl.c -o cpl