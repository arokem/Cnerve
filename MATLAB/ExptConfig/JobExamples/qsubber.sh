#!/bin/bash
#
module load parallel_sql
cd /gscratch/stf/jresnick/NewData/NormSupra/
psu --del --sql-set NormSupra -y
cat commands | psu --load --sql-set NormSupra
#
for job in $(seq 1 1); do SimJobs+="$(qsub -q bfwait ./parallel_sql_job.pbs):"; done
SimJobs=$(echo -n $SimJobs | head -c -1)
echo $SimJobs
qsub -q bf -W depend=afterok:$SimJobs ./retrieve.pbs
exit 0