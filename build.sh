#!/bin/sh

gcc -lm -g -O2 -Wall -Wextra -Wdouble-promotion -Wfloat-conversion -pedantic -std=c99 mt.c lexer.c adsr_envelope.c parser.c expression.c util.c audio_system.c event_bus.c -lraylib
