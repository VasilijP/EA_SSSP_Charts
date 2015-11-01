select avg(cycles) as c, stddev(cycles), alg, vertcount, conn into outfil
e 'D:/data.txt' from result where batchname like '%FLAT%' and batchname like '%N
EAREST%' and batchname like '%PEREDGECONN%' group by alg, vertcount, conn order
by c;