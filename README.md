# CCH

This Customizable Contraction Hierarchy is calculating shortes pathes on graphs, usually representing maps.

It is an implementation done together with a friend at University.


It has two pre-computation steps in order to let shortes-distance queries on graphes run faster.

The first pre-computation step takes relatively long but has to be done only once on a certain graph, as long as the vertices and edges do not change.

The second pre-computation step should take maximum a minute on a graph, the size of Europe.
In this step you can customize the weights of the graph in order to fit e.g. changing traffic-situations and shorter or longer traveling times.
