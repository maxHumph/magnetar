#!/bin/bash

echo "./core" > /proc/sys/kernel/core_pattern 
cat /proc/sys/kernel/core_pattern 
