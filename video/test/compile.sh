#!/bin/bash

gcc -o avcodec_sample.0.5.0 avcodec_sample.0.5.0.c -lavformat -lavcodec -lavutil -lswscale -lz -lbz2
