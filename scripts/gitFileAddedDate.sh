#!/bin/bash
git log --format=%aD $1 | tail -1
