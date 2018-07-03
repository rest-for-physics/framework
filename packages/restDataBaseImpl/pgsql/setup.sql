drop table if exists rest_files;
drop table if exists rest_runs;

-- drop and recreate with different number in later scripts.
drop function if exists daq_db_version();
create function daq_db_version() returns integer as 'select 1;' language sql;

-- create types.
drop type if exists run_type cascade;
create type run_type as enum (
         'SW_DEBUG',
         'HW_DEBUG',
         'CALIBRATION',
         'OTHER',
         'PHYSICS_DBD'
);

create table rest_runs (
       run_id serial not null,
       subrun_id serial not null,
       primary key (run_id, subrun_id),
       type run_type not null default 'OTHER',
       usr text not null default user,
       tag text not null default '',
       description text not null default '',
       version text not null default '',
       run_start timestamp with time zone default now(),
       run_end timestamp with time zone
);

create table rest_files (
       run_id serial not null,
       subrun_id serial not null,
       file_id integer not null,
       constraint runids foreign key (run_id, subrun_id) REFERENCES rest_runs (run_id, subrun_id),
       primary key (run_id, subrun_id, file_id),
       file_name varchar(256) not null default '',
       file_size bigint, -- in bytes
       start_time timestamp with time zone default now(),
       stop_time timestamp with time zone,
       event_rate real,
       sha1sum text,
       quality bool
);



