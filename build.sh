#!/bin/sh

gcc -lm -g -Wall -pedantic -std=c99 mt.c lexer.c asdr_envelope.c parser.c expression.c util.c
