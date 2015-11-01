select avg(cycles) as c, stddev(cycles), alg, vertcount into outfile 'D:/
data.txt' from result group by alg,vertcount order by c;