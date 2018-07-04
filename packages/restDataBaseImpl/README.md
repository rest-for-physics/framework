====================================

Author : Ni Kaixiang
Date : 27-June-2018
Description : Implementation of REST database accessor: TRestDataBase. 

===================================

#Introduction

REST classes will try to access certain database through the interface class TRestDataBase. 
Such database accessibility is disabled when we install REST, since the interface lacks implementation.
So We need to install this package before we can use database.
The target database of this package is PostgreSQL. Initial code is from Chen Xun.

We also provide an sql file instructing how to set up tables in database. There are two tables:

table1(run table): **run_id**, **subrun_id**, type, usr, tag, description, version, run_start, run_end

table1(file table): **run_id**, **subrun_id**, **file_id**, file_name, file_size, start_time, stop_time, event_rate, sha1sum, quality

The bold items are primary keys, which means their combination should be unique in each tables.

A so called "run" in REST can either be a data taking run or a simulation run. Its id is unique and
is specified in column "run_id". Based on this run, multiple analysis could be commenced. Their id 
are also unique and are specified in column "subrun_id". subrun_id is 0 for data taking run and 
simulation run. run_id is 0 for analysis run when input file is not recorded in database. 

It is better to set a unique file name in the table. 

#How to use

First switch to root directory of this package. Then type:

`mkdir build`  
`cd build`  
`cmake ..`  
`make install`  

The compiled library will automatically be installed to $REST_PATH/lib.

