#!/bin/bash
git branch | grep -e "^*" | cut -d ' ' -f 2