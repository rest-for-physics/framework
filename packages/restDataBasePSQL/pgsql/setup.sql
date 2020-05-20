drop table if exists rest_files;
drop table if exists rest_metadata;
drop table if exists rest_runs;

-- drop and recreate with different number in later scripts.
drop function if exists daq_db_version();
create function daq_db_version() returns integer as 'select 1;' language sql;

-- create types.
drop type if exists run_type cascade;
create type run_type as enum (
         'SW_DEBUG',
         'HW_DEBUG',
		 'SIMULATION',
         'CALIBRATION',
		 'PHYSICS_DBD',
         'OTHER'
);

create table rest_runs (
       run_id serial not null,
       primary key (run_id),
       type run_type not null default 'OTHER',
       tag text not null default '',
       description text not null default '',
       version text not null default '',
       run_start timestamp with time zone default now(),
       run_end timestamp with time zone
);

create table rest_files (
       run_id serial not null,
       file_id integer not null,
       constraint runids foreign key (run_id) REFERENCES rest_runs (run_id),
       primary key (run_id, file_id),
       file_name varchar(256) not null default '',
       file_size bigint, -- in bytes
       start_time timestamp with time zone default now(),
       stop_time timestamp with time zone,
       event_rate real,
       sha1sum text,
       quality bool
);

create table rest_metadata (
       run_id serial not null,
       constraint runids foreign key (run_id) REFERENCES rest_runs (run_id),
       primary key (run_id),
       MMs_layout text,              -- Prototype_7MM (name of the readout plane)
       MMs_id int[],                 -- 0 2 3 4 6 8 9 
       MMs_active bool[],            -- 1 0 1 1 1 1 1 (bool)
       MMs_board int[],              -- 2 1 2 1 0 3 0
       MMs_slot int[],               -- 3(01) 12(23) 12(23) 3(01) 12(23) 6(12) 3(01)
       DAQ_HVmesh int[],             -- 340 340 360 360 370 370 360 (V)
       DAQ_HVinternalrim int[],      -- 700 700 700 700 700 700 700 (V)
       DAQ_HVexternalrim int[],      -- 0 0 0 0 0 0 0
       DAQ_dynamicrange int[],       -- 120 120 120 120 120 120 120 (fc)
       DAQ_threshold text,           -- 2-0
       DAQ_samplingrate integer,     -- 5 (MHz)
       DAQ_triggerdelay integer,     -- 361
       DAQ_shappingtime integer,     -- 1 (us)
       DAQ_baseline integer          -- 380+20
       DAQ_readoutmode text          -- full readout / partial readout
       Gas_material text,            -- xe:0.99, n(ch3)3:0.01
       Gas_pressure FLOAT4,          -- 8.5 (bar)
       Gas_temperature FLOAT4,       -- 300 (K)
       Gas_flowrate FLOAT4,          -- 20 (slpm)
       Gas_driftvoltage FLOAT4,      -- 40 (KV)
       Gas_elifetime FLOAT4,         -- 700000 (0s)
       elog text                    -- https://elog.pandax.sjtu.edu.cn/PandaX-3/302
);


-- migrate from pandax database

-- insert into rest_runs (run_id, subrun_id, version, type, description, run_start, run_end) select runs.run_id, 0, 'OLD_Version', 'OTHER', runs.description, files.start_time, files.stop_time from (files RIGHT JOIN runs ON (files.run_id=runs.run_id and files.file_id=0));

-- insert into rest_files (run_id, subrun_id, file_id, file_name,file_size, start_time, stop_time) select run_id, 0, file_id, file_name,file_size, start_time, stop_time from files;

insert into rest_runs (run_id) values (0);
insert into rest_files (run_id, file_id) values (0,0);
insert into rest_metadata (run_id, MMs_tag) values (0,'Prototype_7MM');
update rest_metadata set MMS_idlist = '{0, 2, 3, 4, 6, 8, 9}';
update rest_metadata set MMs_active = '{1, 0, 1, 1, 1, 1, 1}';
update rest_metadata set MMs_HV_mesh = '{340, 340, 360, 360, 370, 370, 360}';
update rest_metadata set MMs_HV_internalrim = '{700, 700, 700, 700, 700, 700, 700}';